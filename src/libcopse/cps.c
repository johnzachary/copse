/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include "copse/cps.h"

static int
cps_done__resume(struct cps_cont *cont, struct cps_cont *next);

struct cps_cont   cps_done_cont = {
    cps_done__resume
};

static int
cps_done__resume(struct cps_cont *cont, struct cps_cont *next)
{
    if (next == cps_done) {
        return 0;
    } else {
        return cps_return(next);
    }
}
