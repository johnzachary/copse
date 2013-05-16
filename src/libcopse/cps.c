/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>

#include "copse/cps.h"


struct cps_cont *
cps_cont_new(void)
{
    struct cps_cont  *cont = cork_new(struct cps_cont);
    cont->user_data = NULL;
    cont->resume = NULL;
    cont->free_user_data = NULL;
    return cont;
}

void
cps_cont_free(struct cps_cont *cont)
{
    cork_free_user_data(cont);
    free(cont);
}

void
cps_cont_set(struct cps_cont *cont,
             void *user_data, cork_free_f free_user_data,
             cps_cont_resume_f resume)
{
    cork_free_user_data(cont);
    cont->user_data = user_data;
    cont->free_user_data = free_user_data;
    cont->resume = resume;
}

void
cps_cont_set_resume(struct cps_cont *cont, cps_cont_resume_f resume)
{
    cont->resume = resume;
}

static int
cps_done__resume(void *user_data, struct cps_cont *next)
{
    return 0;
}

static struct cps_cont  cps_done_cont__instance = {
    NULL, NULL, cps_done__resume
};

struct cps_cont * const  cps_done_cont = &cps_done_cont__instance;
