/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef COPSE_CPS_H
#define COPSE_CPS_H

#include <libcork/core.h>


struct cps_cont;

typedef int
(*cps_cont_resume_f)(void *user_data, struct cps_cont *next);

struct cps_cont {
    void  *user_data;
    cork_free_f  free_user_data;
    cps_cont_resume_f  resume;
};

struct cps_cont *
cps_cont_new(void);

void
cps_cont_free(struct cps_cont *cont);

void
cps_cont_set(struct cps_cont *cont,
             void *user_data, cork_free_f free_user_data,
             cps_cont_resume_f resume);

/* Keeps the same user_data */
void
cps_cont_set_resume(struct cps_cont *cont, cps_cont_resume_f resume);

#define cps_resume(c, n) ((c)->resume((c)->user_data, (n)))


extern struct cps_cont * const  cps_done_cont;

#define cps_done       (cps_done_cont)
#define cps_return(n)  (cps_resume((n), cps_done))
#define cps_run(n)     (cps_resume((n), cps_done))


#endif /* COPSE_CPS_H */
