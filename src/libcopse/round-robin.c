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

struct cps_rr_priv {
    struct cps_rr  public;
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

#define queue_used_size(self) \
    (((self)->tail - (self)->head) & (self)->size_mask)


static int
cps_rr__start(struct cps_cont *cont, struct cps_cont *next);

static int
cps_rr__yield(struct cps_cont *cont, struct cps_cont *next);


struct cps_rr *
cps_rr_new(void)
{
    struct cps_rr_priv  *self = malloc(sizeof(struct cps_rr_priv));
    DEBUG("[%p] Allocated new round-robin scheduler\n", self);
    self->public.start.resume = cps_rr__start;
    self->public.yield.resume = cps_rr__yield;
    self->queue = malloc(INITIAL_QUEUE_SIZE * sizeof(struct cps_cont *));
    self->size_mask = INITIAL_QUEUE_SIZE - 1;
    self->head = 0;
    self->tail = 0;
    return &self->public;
}

void
cps_rr_free(struct cps_rr *pself)
{
    struct cps_rr_priv  *self =
        cps_container_of(pself, struct cps_rr_priv, public);
    DEBUG("[%p] Freeing round-robin scheduler\n", self);
    free(self->queue);
    free(self);
}

void
cps_rr_add(struct cps_rr *pself, struct cps_cont *cont)
{
    struct cps_rr_priv  *self =
        cps_container_of(pself, struct cps_rr_priv, public);
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


static int
cps_rr__yield(struct cps_cont *cont, struct cps_cont *next)
{
    struct cps_rr_priv  *self =
        cps_container_of(cont, struct cps_rr_priv, public.yield);
    struct cps_cont  *head_cont;

    /* Add `next` to the work queue if it represents a real continuation. */
    if (next != cps_done) {
        DEBUG("[%p] Adding continuation %p to end of queue\n", self, next);
        self->queue[self->tail] = next;
        self->tail = (self->tail + 1) & self->size_mask;
    }

    if (self->head == self->tail) {
        /* All of the entries in the work queue have completed.  Return from the
         * round-robin scheduler. */
        DEBUG("[%p] All continuations finished\n", self);
        return cps_return(self->done);
    } else {
        /* There's something in the work queue to pass control to. */
        head_cont = self->queue[self->head];
        self->head = (self->head + 1) & self->size_mask;
        DEBUG("[%p] Yielding to continuation %p\n", self, head_cont);
        return cps_resume(head_cont, cont);
    }
}

static int
cps_rr__start(struct cps_cont *cont, struct cps_cont *next)
{
    struct cps_rr_priv  *self =
        cps_container_of(cont, struct cps_rr_priv, public.start);
    self->done = next;
    return cps_rr__yield(&self->public.yield, cps_done);
}
