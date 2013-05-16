/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include "copse/cps.h"
#include "copse/round-robin.h"


#if !defined(CPS_DEBUG_RR)
#define CPS_DEBUG_RR  0
#endif

#if CPS_DEBUG_RR
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


#define INITIAL_QUEUE_SIZE  16

struct cps_rr {
    struct cps_cont  *yield;
    struct cps_cont  *done;

    /* The work queue.  This is a ring buffer of continuation pointers.  The
     * size of the ring buffer will always be a power of 2, allowing us to
     * module by the queue size with a & operation instead of a %.
     *
     * The head is the index into the work queue of the next continuation to
     * pass control to.  The tail is the index of the next empty element of the
     * work queue.
     *
     * We always leave at least one element of the queue empty.  This means that
     * if head == tail, the queue is empty. */
    struct cps_cont  **queue;
    size_t  size_mask;  /* == allocated_count - 1 */
    size_t  head;
    size_t  tail;
};

#define queue_is_empty(self)  ((self)->head == (self)->tail)
#define queue_used_size(self) \
    (((self)->tail - (self)->head) & (self)->size_mask)


static int
cps_rr__yield(void *user_data, struct cps_cont *next);


struct cps_rr *
cps_rr_new(void)
{
    struct cps_rr  *self = cork_new(struct cps_rr);
    DEBUG("[%p] Allocated new round-robin scheduler\n", self);
    self->yield = cps_cont_new();
    cps_cont_set(self->yield, self, NULL, cps_rr__yield);
    self->queue = malloc(INITIAL_QUEUE_SIZE * sizeof(struct cps_cont *));
    self->size_mask = INITIAL_QUEUE_SIZE - 1;
    self->head = 0;
    self->tail = 0;
    return self;
}

void
cps_rr_free(struct cps_rr *self)
{
    DEBUG("[%p] Freeing round-robin scheduler\n", self);
    cps_cont_free(self->yield);
    free(self->queue);
    free(self);
}

void
cps_rr_add(struct cps_rr *self, struct cps_cont *cont)
{
    size_t  old_used_size = queue_used_size(self);

    DEBUG("[%p] Adding continuation %p\n", self, cont);
    if (old_used_size == self->size_mask) {
        /* The queue is full.  Resize! */
        size_t  old_size = self->size_mask + 1;
        size_t  new_size = old_size * 2;
        size_t  pre_size;
        struct cps_cont  **queue = malloc(new_size * sizeof(struct cps_cont *));
        DEBUG("[%p]   Resizing work queue to %zu elements\n", self, new_size);

        /* Copy the existing continuations into the beginning of the work queue.
         * (We can't reuse the old queue directly because the wrap-around point
         * might have changed.  This code path will be executed infrequently
         * enough that we don't need to over-optimize it.) */

        /* The number of elements in the old queue that appear before the
         * wrap-around point of the ring buffer. */
        pre_size = old_size - self->head;

        DEBUG("[%p]   Moving %zu elements to beginning of new queue\n",
              self, pre_size);
        memcpy(queue, self->queue + self->head,
               pre_size * sizeof(struct cps_cont *));
        if (self->head > 0) {
            DEBUG("[%p]   Moving %zu elements to end of new queue\n",
                  self, self->head);
            memcpy(queue + pre_size, self->queue,
                   self->head * sizeof(struct cps_cont *));
        }

        self->queue = queue;
        self->head = 0;
        self->tail = old_used_size;
        self->size_mask = new_size - 1;
    }

    self->queue[self->tail] = cont;
    self->tail = (self->tail + 1) & self->size_mask;
}

struct cps_cont *
cps_rr_get_yield(struct cps_rr *rr)
{
    return rr->yield;
}

static int
cps_rr__yield(void *user_data, struct cps_cont *next)
{
    struct cps_rr  *self = user_data;
    struct cps_cont  *head_cont;

    /* Add `next` to the work queue. */
    DEBUG("[%p] Adding continuation %p to end of queue\n", self, next);
    self->queue[self->tail] = next;
    self->tail = (self->tail + 1) & self->size_mask;

    /* There must be something in the work queue to pass control to, since we
     * just added an element. */
    head_cont = self->queue[self->head];
    self->head = (self->head + 1) & self->size_mask;
    DEBUG("[%p] Yielding to continuation %p\n", self, head_cont);
    return cps_resume(head_cont, self->yield);
}

int
cps_rr_drain(struct cps_rr *self)
{
    while (!queue_is_empty(self)) {
        int  rc;
        struct cps_cont  *head_cont = self->queue[self->head];
        self->head = (self->head + 1) & self->size_mask;
        DEBUG("[%p] Yielding to continuation %p\n", self, head_cont);
        rc = cps_resume(head_cont, self->yield);
        if (rc != 0) {
            return rc;
        }
    }
    DEBUG("[%p] All continuations finished\n", self);
    return 0;
}

int
cps_rr_resume(struct cps_rr *rr, struct cps_cont *next)
{
    return cps_resume(rr->yield, next);
}

int
cps_rr_run_one_lap(struct cps_rr *rr)
{
    return cps_run(rr->yield);
}
