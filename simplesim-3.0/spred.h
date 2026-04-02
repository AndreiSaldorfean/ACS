/*
 * bpred.c - branch predictor routines
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 *
 * Copyright (C) 1994, 1995, 1996, 1997, 1998 by Todd M. Austin
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful.  The tool set comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use.
 *
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 *
 *    This source code is distributed for non-commercial use only.
 *    Please contact the maintainer for restrictions applying to
 *    commercial use.
 *
 *    Permission is granted to anyone to make or distribute copies
 *    of this source code, either as received or modified, in any
 *    medium, provided that all copyright notices, permission and
 *    nonwarranty notices are preserved, and that the distributor
 *    grants the recipient permission for further redistribution as
 *    permitted by this document.
 *
 *    Permission is granted to distribute this file in compiled
 *    or executable form under the same conditions that apply for
 *    source code, provided that either:
 *
 *    A. it is accompanied by the corresponding machine-readable
 *       source code,
 *    B. it is accompanied by a written offer, with no time limit,
 *       to give anyone a machine-readable copy of the corresponding
 *       source code in return for reimbursement of the cost of
 *       distribution.  This written offer must permit verbatim
 *       duplication by anyone, or
 *    C. it is distributed by someone who received only the
 *       executable form, and is accompanied by a copy of the
 *       written offer of source code that they received concurrently.
 *
 * In other words, you are welcome to use, share and improve this
 * source file.  You are forbidden to forbid anyone else to use, share
 * and improve what you give them.
 *
 * INTERNET: dburger@cs.wisc.edu
 * US Mail:  1210 W. Dayton Street, Madison, WI 53706
 *
 * $Id: bpred.c,v 1.1.1.1 2000/05/26 15:18:57 taustin Exp $
 *
 * $Log: bpred.c,v $
 * Revision 1.1.1.1  2000/05/26 15:18:57  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.4  1998/08/27 07:56:23  taustin
 * implemented host interface description in host.h
 * return address stack (RAS) performance stats improved
 * explicit BTB sizing option added to branch predictors, use
 *       "-btb" option to configure BTB
 * added target configuration parameters to control branch
 *       predictor indexing
 * L2 index computation is more "compatible" to McFarling's
 *       verison of it, i.e., if the PC xor address component is only
 *       part of the index, take the lower order address bits for the
 *       other part of the index, rather than the higher order ones
 * return address stack (RAS) bug fixed (improves pred perf)
 *
 * Revision 1.1.1.1  1997/05/22 00:33:18  aklauser
 *
 * Revision 1.11  1997/05/01 20:23:00  skadron
 * BTB bug fixes; jumps no longer update direction state; non-taken
 *    branches non longer update BTB
 *
 * Revision 1.10  1997/05/01 00:05:42  skadron
 * Separated BTB from direction-predictor
 *
 * Revision 1.9  1997/04/30  01:42:42  skadron
 * 1. Not aggressively returning the BTB target regardless of hit on jump's,
 *    but instead returning just "taken" when it's a BTB miss yields an
 *    apparent epsilon performance improvement for cc1 and perl.
 * 2. Bug fix: if no retstack, treat return's as any other jump
 *
 * Revision 1.8  1997/04/29  23:50:33  skadron
 * Added r31 info to distinguish between return-JRs and other JRs for bpred
 *
 * Revision 1.7  1997/04/29  22:53:04  skadron
 * Hopefully bpred is now right: bpred now allocates entries only for
 *    branches; on a BTB miss it still returns a direction; and it uses a
 *    return-address stack.  Returns are not yet distinguished among JR's
 *
 * Revision 1.6  1997/04/28  17:37:02  skadron
 * Bpred now allocates entries for any instruction instead of only
 *    branches; also added return-address stack
 *
 * Revision 1.5  1997/04/24  16:57:21  skadron
 * Bpred used to return no prediction if the indexing branch didn't match
 *    in the BTB.  Now it can predict a direction even on a BTB address
 *    conflict
 *
 * Revision 1.4  1997/03/27  16:31:52  skadron
 * Fixed bug: sim-outorder calls bpred_after_priming(), even if no bpred
 *    exists.  Now we check for a null ptr.
 *
 * Revision 1.3  1997/03/25  16:16:33  skadron
 * Statistics now take account of priming: statistics report only
 *    post-prime info.
 *
 * Revision 1.2  1997/02/24  18:02:41  skadron
 * Fixed output format of a formula stat
 *
 * Revision 1.1  1997/02/16  22:23:54  skadron
 * Initial revision
 *
 *
 */
#ifndef SPRED_H
#define SPRED_H

/* ================================================ INCLUDES =============================================== */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "machine.h"
#include "misc.h"

/* ================================================= MACROS ================================================ */
/* turn this on to enable the SimpleScalar 2.0 RAS bug */
/* #define RAS_BUG_COMPATIBLE */

#define BIMOD_HASH(PRED, ADDR)                                                 \
    ((((ADDR) >> 19) ^ ((ADDR) >> MD_BR_SHIFT)) &                              \
     ((PRED)->config.bimod.size - 1))
/* was: ((baddr >> 16) ^ baddr) & (pred->dirpred.bimod.size-1) */

/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
typedef struct element *valueList;

struct element
{
    sword_t value;
    valueList nextValue;
};

typedef struct location *addrList;

struct location
{
    md_addr_t addr;
    addrList nextAddress;
    valueList values;
};

/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* =============================================== MODULE API ============================================== */
addrList news();
addrList pushAddress(addrList p, md_addr_t addr, sword_t value);
void pushValue(addrList p, sword_t value);
int foundValue(addrList l, sword_t value, int history);
int foundAddress(addrList l, md_addr_t addr, sword_t value, int history);

// /* create a branch predictor */
// struct bpred_t *			/* branch predictory instance */
// bpred_create(enum bpred_class class,	/* type of predictor to create */
// 	     unsigned int bimod_size,	/* bimod table size */
// 	     unsigned int l1size,	/* 2lev l1 table size */
// 	     unsigned int l2size,	/* 2lev l2 table size */
// 	     unsigned int meta_size,	/* meta table size */
// 	     unsigned int shift_width,	/* history register width */
// 	     unsigned int xor,  	/* history xor address flag */
// 	     unsigned int btb_sets,	/* number of sets in BTB */
// 	     unsigned int btb_assoc,	/* BTB associativity */
// 	     unsigned int retstack_size); /* num entries in ret-addr stack */
//
// /* create a branch direction predictor */
// struct bpred_dir_t *		/* branch direction predictor instance */
// bpred_dir_create (
//   enum bpred_class class,	/* type of predictor to create */
//   unsigned int l1size,	 	/* level-1 table size */
//   unsigned int l2size,	 	/* level-2 table size (if relevant) */
//   unsigned int shift_width,	/* history register width */
//   unsigned int xor);	    	/* history xor address flag */
//
//
// /* print branch direction predictor configuration */
// void
// bpred_dir_config(
//   struct bpred_dir_t *pred_dir,	/* branch direction predictor instance */
//   char name[],			/* predictor name */
//   FILE *stream);			/* output stream */
//
//
// /* print branch predictor configuration */
// void
// bpred_config(struct bpred_t *pred,	/* branch predictor instance */
// 	     FILE *stream);		/* output stream */
//
// /* print predictor stats */
// void
// bpred_stats(struct bpred_t *pred,	/* branch predictor instance */
// 	    FILE *stream);		/* output stream */
//
// /* register branch predictor stats */
// void
// bpred_reg_stats(struct bpred_t *pred,	/* branch predictor instance */
// 		struct stat_sdb_t *sdb);	/* stats database */
//
//
// void
// bpred_after_priming(struct bpred_t *bpred);
//
//
//
// /* predicts a branch direction */
// char *						/* pointer to counter */
// bpred_dir_lookup(struct bpred_dir_t *pred_dir,	/* branch dir predictor inst */
// 		 md_addr_t baddr);		/* branch address */
//
//
// /* probe a predictor for a next fetch address, the predictor is probed
//    with branch address BADDR, the branch target is BTARGET (used for
//    static predictors), and OP is the instruction opcode (used to simulate
//    predecode bits; a pointer to the predictor state entry (or null for jumps)
//    is returned in *DIR_UPDATE_PTR (used for updating predictor state),
//    and the non-speculative top-of-stack is returned in stack_recover_idx
//    (used for recovering ret-addr stack after mis-predict).  */
// md_addr_t				/* predicted branch target addr */
// bpred_lookup(struct bpred_t *pred,	/* branch predictor instance */
// 	     md_addr_t baddr,		/* branch address */
// 	     md_addr_t btarget,		/* branch target if taken */
// 	     enum md_opcode op,		/* opcode of instruction */
// 	     int is_call,		/* non-zero if inst is fn call */
// 	     int is_return,		/* non-zero if inst is fn return */
// 	     struct bpred_update_t *dir_update_ptr, /* pred state pointer */
// 	     int *stack_recover_idx);	/* Non-speculative top-of-stack;
// 					 * used on mispredict recovery */
//
//
// /* Speculative execution can corrupt the ret-addr stack.  So for each
//  * lookup we return the top-of-stack (TOS) at that point; a mispredicted
//  * branch, as part of its recovery, restores the TOS using this value --
//  * hopefully this uncorrupts the stack. */
// void
// bpred_recover(struct bpred_t *pred,	/* branch predictor instance */
// 	      md_addr_t baddr,		/* branch address */
// 	      int stack_recover_idx);	/* Non-speculative top-of-stack;
// 					 * used on mispredict recovery */
//
// /* update the branch predictor, only useful for stateful predictors; updates
//    entry for instruction type OP at address BADDR.  BTB only gets updated
//    for branches which are taken.  Inst was determined to jump to
//    address BTARGET and was taken if TAKEN is non-zero.  Predictor
//    statistics are updated with result of prediction, indicated by CORRECT and
//    PRED_TAKEN, predictor state to be updated is indicated by *DIR_UPDATE_PTR
//    (may be NULL for jumps, which shouldn't modify state bits).  Note if
//    bpred_update is done speculatively, branch-prediction may get polluted. */
// void
// bpred_update(struct bpred_t *pred,	/* branch predictor instance */
// 	     md_addr_t baddr,		/* branch address */
// 	     md_addr_t btarget,		/* resolved branch target */
// 	     int taken,			/* non-zero if branch was taken */
// 	     int pred_taken,		/* non-zero if branch was pred taken */
// 	     int correct,		/* was earlier addr prediction ok? */
// 	     enum md_opcode op,		/* opcode of instruction */
// 	     struct bpred_update_t *dir_update_ptr);/* pred state pointer */

#endif SPRED_H
