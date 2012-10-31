/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

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


struct cps_rr_entry {
    struct cps_cont  *cont;
    struct cps_rr_entry  *next;
};

struct cps_rr_priv {
    struct cps_rr  public;
    struct cps_cont  *done;
    /* Invariant:
     *   If the work queue is empty, head == tail == NULL.
     *   Otherwise, head and tail encode a circular singly-linked list.
     */
    struct cps_rr_entry  *head;
    struct cps_rr_entry  *tail;
    /* cpr_rr_entry instances that we've allocated but aren't currently being
     * used in the work queue. */
    struct cps_rr_entry  *unused;
};


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
    self->head = NULL;
    self->tail = NULL;
    self->unused = NULL;
    return &self->public;
}

void
cps_rr_free(struct cps_rr *pself)
{
    struct cps_rr_priv  *self =
        cps_container_of(pself, struct cps_rr_priv, public);
    struct cps_rr_entry  *curr;
    struct cps_rr_entry  *next;

    DEBUG("[%p] Freeing round-robin scheduler\n", self);

    for (curr = self->head; curr != NULL; curr = next) {
        DEBUG("[%p] Freeing head entry %p\n", self, curr);
        next = curr->next;
        free(curr);
    }

    for (curr = self->unused; curr != NULL; curr = next) {
        DEBUG("[%p] Freeing unused entry %p\n", self, curr);
        next = curr->next;
        free(curr);
    }

    free(self);
}

void
cps_rr_add(struct cps_rr *pself, struct cps_cont *cont)
{
    struct cps_rr_priv  *self =
        cps_container_of(pself, struct cps_rr_priv, public);
    struct cps_rr_entry  *entry;

    DEBUG("[%p] Adding continuation %p\n", self, cont);

    if (self->unused == NULL) {
        entry = malloc(sizeof(struct cps_rr_entry));
        DEBUG("[%p]   New entry %p\n", self, entry);
    } else {
        entry = self->unused;
        self->unused = entry->next;
        DEBUG("[%p]   Reusing entry %p\n", self, entry);
    }

    if (self->head == NULL) {
        self->head = entry;
    } else {
        self->tail->next = entry;
    }

    self->tail = entry;
    entry->cont = cont;
    entry->next = self->head;
    DEBUG("[%p]   head: %p -> %p\n", self, self->head, self->head->next);
    DEBUG("[%p]   tail: %p -> %p\n", self, self->tail, self->tail->next);
}


static int
cps_rr__yield(struct cps_cont *cont, struct cps_cont *next)
{
    struct cps_rr_priv  *self =
        cps_container_of(cont, struct cps_rr_priv, public.yield);
    struct cps_rr_entry  *head = self->head;
    struct cps_cont  *head_cont;

    /* Fast path: If there isn't anything in the work queue, immediately pass
     * control to `next`. */
    if (head == NULL) {
        if (next == cps_done) {
            /* If `next` is cps_done, then all of the entries in the work queue
             * have completed.  Return from the round-robin scheduler. */
            DEBUG("[%p] All continuations finished\n", self);
            return cps_return(self->done);
        } else {
            /* Otherwise, `next` is probably the final continuation we need to
             * run.  However, it might pass control back to us with *another*
             * continuation to run, so we need to tell it to return to the
             * scheduler. */
            DEBUG("[%p] Yielding directly to new continuation %p\n", self, next);
            return cps_resume(next, cont);
        }
    }

    /* There's something in the work queue to pass control to. */
    head_cont = head->cont;

    if (next != &cps_done_cont) {
        /* If the `next` continuation is not cps_done, we reuse the current head
         * of the work queue to store `next`, move that entry to the end of the
         * work queue. */
        DEBUG("[%p] Reusing entry %p for new continuation %p\n",
              self, head, next);
        head->cont = next;
        self->head = head->next;
        self->tail = head; /* == self->tail->next */
    } else {
        /* If `next` is cps_done, then we remove the current entry from the work
         * queue entirely. */
        DEBUG("[%p] Removing entry %p from work queue\n", self, head);
        DEBUG("[%p]   head: %p -> %p\n", self, self->head, self->head->next);
        DEBUG("[%p]   tail: %p -> %p\n", self, self->tail, self->tail->next);
        if (head->next == self->head) {
            /* This is the last entry in the work queue. */
            DEBUG("[%p]   Final entry in queue\n", self);
            self->head = NULL;
            self->tail = NULL;
        } else {
            self->head = head->next;
            self->tail->next = head->next;
        }

        head->next = self->unused;
        self->unused = head;
    }

    /* And then pass control to the continuation at the head of the work queue. */
    DEBUG("[%p] Yielding to continuation %p\n", self, head_cont);
    return cps_resume(head_cont, cont);
}

static int
cps_rr__start(struct cps_cont *cont, struct cps_cont *next)
{
    struct cps_rr_priv  *self =
        cps_container_of(cont, struct cps_rr_priv, public.start);
    self->done = next;
    return cps_rr__yield(&self->public.yield, cps_done);
}
