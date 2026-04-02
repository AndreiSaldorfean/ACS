/*
 * sim-bpred.c - sample branch predictor simulator implementation
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
 * $Id: sim-bpred.c,v 1.1.1.1 2000/05/26 15:19:03 taustin Exp $
 *
 * $Log: sim-bpred.c,v $
 * Revision 1.1.1.1  2000/05/26 15:19:03  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.3  1999/12/31 18:46:55  taustin
 * quad_t naming conflicts removed
 *
 * Revision 1.2  1999/12/13 18:45:29  taustin
 * cross endian execution support added
 *
 * Revision 1.1  1998/08/27 15:54:35  taustin
 * Initial revision
 *
 *
 */

/* ================================================ INCLUDES =============================================== */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include <stdbool.h>

/////////////////
/*
 * This file implements a branch predictor analyzer.
 */
#include "spred.h"
/////////////////
#include "sim.h"

/* ================================================= MACROS ================================================ */
/* turn this on to enable the SimpleScalar 2.0 RAS bug */
/* #define RAS_BUG_COMPATIBLE */

/* next program counter */
#define SET_NPC(EXPR)		(regs.regs_NPC = (EXPR))

/* target program counter */
#undef  SET_TPC
#define SET_TPC(EXPR)		(target_PC = (EXPR))

/* current program counter */
#define CPC			(regs.regs_PC)

/* general purpose registers */
#define GPR(N)			(regs.regs_R[N])
#define SET_GPR(N,EXPR)		(regs.regs_R[N] = (EXPR))

#if defined(TARGET_PISA)
////////////////////////////////////////
/* general register dependence decoders */
#define DGPR(N)			(N)
#define DGPR_D(N)		((N) &~1)

/* floating point register dependence decoders */
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)

/* miscellaneous register dependence decoders */
#define DNA			(0)
#define DHI			(0+32+32)
#define DLO			(1+32+32)
#define DFCC			(2+32+32)
#define DTMP			(3+32+32)
///////////////////////////////////////////////////
/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_L(N)		(regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)	(regs.regs_F.l[(N)] = (EXPR))
#define FPR_F(N)		(regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)	(regs.regs_F.f[(N)] = (EXPR))
#define FPR_D(N)		(regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)	(regs.regs_F.d[(N) >> 1] = (EXPR))

/* miscellaneous register accessors */
#define SET_HI(EXPR)		(regs.regs_C.hi = (EXPR))
#define HI			(regs.regs_C.hi)
#define SET_LO(EXPR)		(regs.regs_C.lo = (EXPR))
#define LO			(regs.regs_C.lo)
#define FCC			(regs.regs_C.fcc)
#define SET_FCC(EXPR)		(regs.regs_C.fcc = (EXPR))

#elif defined(TARGET_ALPHA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)		(regs.regs_F.q[N])
#define SET_FPR_Q(N,EXPR)	(regs.regs_F.q[N] = (EXPR))
#define FPR(N)			(regs.regs_F.d[N])
#define SET_FPR(N,EXPR)		(regs.regs_F.d[N] = (EXPR))

/* miscellaneous register accessors */
#define FPCR			(regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(regs.regs_C.fpcr = (EXPR))
#define UNIQ			(regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(regs.regs_C.uniq = (EXPR))

#else
#error No ISA target defined...
#endif

/* precise architected memory state help functions */
#define READ_BYTE(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_BYTE(mem, addr = (SRC)))
#define READ_HALF(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_HALF(mem, addr = (SRC)))
#define READ_WORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_WORD(mem, addr = (SRC)))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_QWORD(mem, addr = (SRC)))
#endif /* HOST_HAS_QWORD */

#define WRITE_BYTE(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_BYTE(mem, addr = (DST), (SRC)))
#define WRITE_HALF(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_HALF(mem, addr = (DST), (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_WORD(mem, addr = (DST), (SRC)))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_QWORD(mem, addr = (DST), (SRC)))
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST)	sys_syscall(&regs, mem_access, mem, INST, TRUE)

#if 1 /* NOTE: THESE SHOUDLN'T BE HERE */
typedef struct LVPTelement *LVPTvalueList;
struct LVPTelement
{
  sword_t value;
  LVPTvalueList nextValue;
  int count;	// it's used only by the contextual predictor
};

typedef struct LVPTlocation *LVPTaddrList;
struct LVPTlocation
{
  md_addr_t addr;
  LVPTaddrList nextAddress;
  LVPTvalueList values;
  int automat;
  sword_t stride[2];  // it's used only by the stride predictor
};
#endif

enum md_fault_type {
  md_fault_none = 0,		/* no fault */
  md_fault_access,		/* storage access fault */
  md_fault_alignment,		/* storage alignment fault */
  md_fault_overflow,		/* signed arithmetic overflow fault */
  md_fault_div0,		/* division by zero fault */
  md_fault_break,		/* BREAK instruction fault */
  md_fault_unimpl,		/* unimplemented instruction fault */
  md_fault_internal		/* internal S/W fault */
};

/* ============================================ LOCAL VARIABLES ============================================ */
/* Total number of correctly predicted loads */
static counter_t valuePrediction = 0;

/* Total number of loads classified as predictable */
static counter_t classifiedPred = 0;

/* Total number of loads classified as unpredictable */
static counter_t classifiedUnpred = 0;

/* Total number of correctly classified predictable loads */
static counter_t predictable = 0;

/* Total number of correctly classified unpredictable loads */
static counter_t unpredictable = 0;
static counter_t T1 = 0;
static counter_t T2 = 0;


/* dimension of LVPT */
static int LVPTdim;

/* type of predictor */
static int contextual;

/* size of pattern */
static int pattern;

static counter_t npenload=1;
static counter_t Tacces_DRAM=0;
static counter_t foundA=0;

static counter_t foundA_miss=0;
static counter_t notfoundA=0;
float S;

int Factor_Complexitate_Arhitectura = 1;
static int Tacces_incremental;
static int Tacces_contextual;
static int Tacces_hibrid;

static int Tacces_predictor;


/* what to do? */
static int predict;	/* 1-prediction; 0-determining load value locality */

/* type of LVPT */
static int isAssoc;		/* 1-associative; 0-dirrect mapped */

/* load value history */
static int history;

/* which address? */
static int memAddress;  /* 1-memory address; 0-instruction address (PC); */

///////////////////////////////////////////////////////////////////////////////

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

/* maximum number of inst's to execute */
static unsigned int max_insts;

/* branch predictor type {nottaken|taken|perfect|bimod|2lev} */
static char *pred_type;

/* bimodal predictor config (<table_size>) */
static int bimod_nelt = 1;
static int bimod_config[1] =
  { /* bimod tbl size */2048 };

/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
static int twolev_nelt = 4;
static int twolev_config[4] =
  { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE};

/* combining predictor config (<meta_table_size> */
static int comb_nelt = 1;
static int comb_config[1] =
  { /* meta_table_size */1024 };

/* return address stack (RAS) size */
static int ras_size = 8;

/* BTB predictor config (<num_sets> <associativity>) */
static int btb_nelt = 2;
static int btb_config[2] =
  { /* nsets */512, /* assoc */4 };

/* branch predictor */
static struct bpred_t *pred;

/* track number of insn and refs */
static counter_t sim_num_refs = 0;

/* total number of branches executed */
static counter_t sim_num_branches = 0;
////////////////////////////////////////////
/* total number of loads executed */
static counter_t sim_num_loads = 0;
////////////////////////////////////////////

/* ============================================ GLOBAL VARIABLES =========================================== */
/* declaration of list of addresses for load value locality */
addrList l;

/* declaration of list */
LVPTaddrList lvpt;
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
LVPTaddrList pushLVPTAddress(LVPTaddrList p, md_addr_t addr)
{
  LVPTaddrList q;
  q=(LVPTaddrList)malloc(sizeof(struct LVPTlocation));
  q->addr=addr;
  q->nextAddress=p;
  q->values = NULL;
  q->automat = 0;
  return q;
}

void insertLVPTValue(LVPTaddrList ad, sword_t value, int history)
{
  int i;
  LVPTvalueList q = ad->values;
  LVPTvalueList p;
  LVPTvalueList newValue;
  if(history == 1)
  {
	  if(ad->values != NULL)
	  {
		  if(!contextual)
		  {
			  ad->stride[1] = ad->stride[0];
			  ad->stride[0] = value - q->value;
		  }
		  q->value = value;
			return;
	  }
  }

  else if(q != NULL)
  {
	p = q->nextValue;
    for(i=1; i<history; i++)
	{
	  if(p == NULL) break;
	  if(i == history-1)
	  {
	    q->nextValue = NULL;
	    free(p);
		break;
	  }
	  p=p->nextValue;
	  q=q->nextValue;
	}

  }

  newValue=(LVPTvalueList)malloc(sizeof(struct LVPTelement));
  newValue->value=value;
  newValue->nextValue=ad->values;
  ad->values=newValue;

}

sword_t maxValue(LVPTvalueList p)
{
	LVPTvalueList q = p;
	int maximumCount = p->count;
	sword_t maximumValue = p->value;
	while(q != NULL)
	{
		if(q->count > maximumCount)
		{
			maximumCount = q->count;
			maximumValue = q->value;
		}
		q = q->nextValue;
	}
	return maximumValue;
}

sword_t predictValue(LVPTaddrList p, int history)
{
	int i,j;
	LVPTvalueList q = p->values;
	if(!contextual && p->stride[0] == p->stride[1])
		return q->value + p->stride[0];
	if(history == 1 && q != NULL)
		return q->value;
	else if(history>1 && q != NULL)	// contextual prediction
	{
		LVPTvalueList patternValueList = NULL;
		for(i=0; i<history-pattern; i++)
		{
			LVPTvalueList k;
			LVPTvalueList patternValues;
			int isPattern;
			if(q == NULL)
				break;
			k = q->nextValue;
			patternValues = p->values;
			isPattern = 1;
			for(j=0; j<pattern; j++)
			{
				if(k != NULL && patternValues != NULL)
				{
					if(k->value != patternValues->value)
					{
						isPattern = 0;
						break;
					}
				}
				else
				{
					isPattern = 0;
					break;
				}
				k = k->nextValue;
				patternValues = patternValues->nextValue;
			}
			if(isPattern)
			{
				LVPTvalueList newValue;
				if(patternValueList == NULL)	// insert the value
				{
					newValue=(LVPTvalueList)malloc(sizeof(struct LVPTelement));
					newValue->value = q->value; // the value which follows the pattern
					newValue->count = 1;
					newValue->nextValue = patternValueList;
					patternValueList = newValue;
				}
				else
				{
					int found = 0;
					LVPTvalueList temp = patternValueList;
					while(temp != NULL)
					{
						if(temp->value == q->value)
						{
							temp->count++;
							found = 1;
							break;
						}
						temp = temp->nextValue;
					}
					if(!found)	// insert the value
					{
						newValue=(LVPTvalueList)malloc(sizeof(struct LVPTelement));
						newValue->value = q->value; // the value which follows the pattern
						newValue->count = 1;
						newValue->nextValue = patternValueList;
						patternValueList = newValue;
					}
				}
			}
			q = q->nextValue;
		}
		if(patternValueList != NULL)
			return maxValue(patternValueList);	// contextual prediction
		else return p->values->value;	// the last value
	}
	return 0;
}

int foundAssociativeLVPTAddress(md_addr_t addr, sword_t value, int history)
{

  LVPTaddrList p = lvpt;
  LVPTaddrList q;
  int i;
  int found = 0;
  if(lvpt == NULL)
	  return found;
  if(p->addr == addr)
  {
		found = 1;
		if(p->values != NULL)
		{
			if(value == predictValue(p, history))
			{
				if(p->automat == 2 || p->automat == 3) /* predictable */
				{
					valuePrediction++;
					classifiedPred++;
					predictable++;
				}
				if(p->automat == 0 || p->automat == 1) /* unpredictable */
					classifiedUnpred++;
				if(p->automat < 3)
					p->automat++;
			}
			else
			{
				if(p->automat == 2 || p->automat == 3) /* predictable */
					classifiedPred++;
				if(p->automat == 0 || p->automat == 1) /* unpredictable */
				{
					classifiedUnpred++;
					unpredictable++;
				}
				if(p->automat > 0)
					p->automat--;
			}
		}
		else
		{
			if(p->automat > 0)
				p->automat--;
		}
		insertLVPTValue(p, value, history);
		return found;
  }

  else
  {
	q = p->nextAddress;
    for(i=1; i<LVPTdim; i++)
	{
		if(q == NULL) break;
		if(q->addr == addr)
		{
			found = 1;
			if(q->values != NULL)
			{
				if(value == predictValue(q, history))
				{
					if(q->automat == 2 || q->automat == 3) /* predictable */
					{
						valuePrediction++;
						classifiedPred++;
						predictable++;
					}
					if(q->automat == 0 || q->automat == 1) /* unpredictable */
						classifiedUnpred++;
					if(q->automat < 3)
						q->automat++;
				}
				else
				{
					if(q->automat == 2 || q->automat == 3) /* predictable */
						classifiedPred++;
					if(q->automat == 0 || q->automat == 1) /* unpredictable */
					{
						classifiedUnpred++;
						unpredictable++;
					}
					if(q->automat > 0)
						q->automat--;
				}
			}
			else
			{
				if(q->automat > 0)
					q->automat--;
			}
			insertLVPTValue(q, value, history);
			/* the address is moved to the first position in the list */
			p->nextAddress = q->nextAddress;
			q->nextAddress = lvpt;
			lvpt = q;
			break;
		}
	  if(i == LVPTdim-1)
	  {
	    p->nextAddress = NULL;
	    free(q);
		break;
	  }
	    p=p->nextAddress;
		q = q->nextAddress;
	 }
	 }

	 return found;
}

/* LVPT initialization */
LVPTaddrList LVPTinit(LVPTaddrList l)
{
	int i;
	for(i=0; i<LVPTdim; i++)
		l = pushLVPTAddress(l, 0);
	return l;
}

void insertIntoDirrectMappedLVPT(md_addr_t addr, sword_t value, int history)
{

	LVPTaddrList p = lvpt;
	int index;
	int i;
	index = addr % LVPTdim;
	for(i=0; i<index; i++)
		p = p->nextAddress;
	if(p != NULL)
	{
		if(p->addr != addr)
		{
			p->addr = addr;
			p->values = NULL;
			p->automat = 0;
		}
		if(p->values != NULL)
		{
			if(value == predictValue(p, history))
			{
				if(p->automat == 2 || p->automat == 3) /* predictable */
				{
					valuePrediction++;
					classifiedPred++;
					predictable++;
				}
				if(p->automat == 0 || p->automat == 1) /* unpredictable */
					classifiedUnpred++;
				if(p->automat < 3)
					p->automat++;
			}
			else
			{
				if(p->automat == 2 || p->automat == 3) /* predictable */
					classifiedPred++;
				if(p->automat == 0 || p->automat == 1) /* unpredictable */
				{
					classifiedUnpred++;
					unpredictable++;
				}
				if(p->automat > 0)
					p->automat--;
			}
		}
		else
		{

			if(p->automat > 0)
				p->automat--;
		}
		insertLVPTValue(p, value, history);
	}

}

/* register simulator-specific options */
void sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb,
"sim-bpred: This simulator implements a branch predictor analyzer.\n"
		 );

  /* branch predictor options */
  opt_reg_note(odb,
"  Branch predictor configuration examples for 2-level predictor:\n"
"    Configurations:   N, M, W, X\n"
"      N   # entries in first level (# of shift register(s))\n"
"      W   width of shift register(s)\n"
"      M   # entries in 2nd level (# of counters, or other FSM)\n"
"      X   (yes-1/no-0) xor history and address for 2nd level index\n"
"    Sample predictors:\n"
"      GAg     : 1, W, 2^W, 0\n"
"      GAp     : 1, W, M (M > 2^W), 0\n"
"      PAg     : N, W, 2^W, 0\n"
"      PAp     : N, W, M (M == 2^(N+W)), 0\n"
"      gshare  : 1, W, 2^W, 1\n"
"  Predictor `comb' combines a bimodal and a 2-level predictor.\n"
               );

  /* instruction limit */
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
	       &max_insts, /* default */0,
	       /* print */TRUE, /* format */NULL);

//////////////////////////////////////////////////////////////////////////////////////////////
  opt_reg_uint(odb, "-memaddr", "address type {0-instruction address| 1-memory address}",
	       &memAddress, /* default */0,
                 /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-pred", "what to do {0-load value locality| 1-prediction}",
	       &predict, /* default */0,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-assoc", "table type {0-dirrect mapped| 1-associative}",
	       &isAssoc, /* default */1,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-history", "maximum number of values memorized for a certain address",
	       &history, /* default */1,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-lvpt", "dimension of LVPT",
	       &LVPTdim, /* default */4,
              /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-contextual", "type of predictor",
	       &contextual, /* default */1,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-pattern", "size of pattern",
	       &pattern, /* default */3,
	       /* print */TRUE, /* format */NULL);


  opt_reg_string(odb, "-bpred",
		 "branch predictor type {nottaken|taken|bimod|2lev|comb}",
                 &pred_type, /* default */"bimod",
                 /* print */TRUE, /* format */NULL);

/////////////////////////////////////////////////////////////////////////////

}

/* register simulator-specific statistics */
void sim_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions executed",
		   &sim_num_insn, sim_num_insn, NULL);

//////////////////////////////////////////////////////////////////////

  stat_reg_counter(sdb, "valuePrediction",
		   "total number of correctly predicted values",
		   &valuePrediction, valuePrediction, NULL);
  stat_reg_counter(sdb, "classifiedPred",
		   "number of loads classified as predictable",
		   &classifiedPred, classifiedPred, NULL);
  stat_reg_counter(sdb, "classifiedUnpred",
		   "number of loads classified as unpredictable",
		   &classifiedUnpred, classifiedUnpred, NULL);
  stat_reg_counter(sdb, "predictable",
		   "correctly classified predictable loads",
		   &predictable, predictable, NULL);
  stat_reg_counter(sdb, "unpredictable",
		   "correctly classified unpredictable loads",
		   &unpredictable, unpredictable, NULL);
  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
  stat_reg_counter(sdb, "sim_num_loads",
		   "total number of loads executed",
		   &sim_num_loads, 0, NULL);
  stat_reg_counter(sdb, "T1",
                   "T1",&T1, /* initial value */0, /* format */NULL);
  stat_reg_counter(sdb, "T2",
                   "T2",&T2, /* initial value */0, /* format */NULL);

///////////////////////////////////////////////////////////////////////////
  stat_reg_formula(sdb, "sim_IPB",
                   "instruction per branch",
                   "sim_num_insn / sim_num_branches", /* format */NULL);

  stat_reg_formula(sdb,"sim_speed","S","(T1/T2-1)*100",NULL);

}

/* initialize the simulator */
void sim_init(void)
{
  sim_num_refs = 0;
/////////////////////
  sim_num_loads = 0;
/////////////////////
/*  loadValueLocality = 0; */

  /* allocate and initialize register file */
  regs_init(&regs);

  /* allocate and initialize memory space */
  mem = mem_create("mem");
  mem_init(mem);
}

/* local machine state accessor */
/* err str, NULL for no err */
static char* bpred_mstate_obj(
        FILE *stream,			/* output stream */
		char *cmd,			/* optional command string */
		struct regs_t *regs,		/* register to access */
		struct mem_t *mem)		/* memory to access */
{
  /* just dump intermediate stats */
  sim_print_stats(stream);

  /* no error */
  return NULL;
}

/* load program into simulated state */
void sim_load_prog(
    char *fname,		/* program to load */
    int argc, char **argv,	/* program arguments */
    char **envp)		/* program environment */
{
  /* load program text and data, set up environment, memory, and regs */
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

  /* initialize the DLite debugger */
  dlite_init(md_reg_obj, dlite_mem_obj, bpred_mstate_obj);
}

/* print simulator-specific configuration information */
void sim_aux_config(FILE *stream)		/* output stream */
{
  /* nothing currently */
}

/* dump simulator-specific auxiliary simulator statistics */
void sim_aux_stats(FILE *stream)		/* output stream */
{
  /* nada */
}

/* un-initialize simulator-specific state */
void sim_uninit(void)
{
  /* nada */
}


/*
 * configure the execution engine
 */

/*
 * precise architected register accessors
 */

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{

}

/* start simulation, program loaded, processor precise state initialized */
void sim_main(void)
{
  /////////////////////////////
  int out1, out2;
  int in1, in2, in3;
//////////////////////////
  md_inst_t inst;
  register md_addr_t addr, target_PC;
  enum md_opcode op;
  bool is_write = FALSE;
  enum md_fault_type fault;


////////////////////////////////////////////
  /* List initialization */
  l=news();
  // lvpt=news();
  if(!isAssoc)
	  lvpt=LVPTinit(lvpt);
  if(!contextual)	/* if it's an incremental prediction */
	  history = 1;
/////////////////////////////////////////////////////

  while (TRUE)
    {
      /* maintain $r0 semantics */
      regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
      regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

      /* get the next instruction to execute */
      MD_FETCH_INST(inst, mem, regs.regs_PC);

      /* keep an instruction count */
      sim_num_insn++;

      /* set default reference address and access mode */
      addr = 0; is_write = FALSE;

      /* set default fault - none */
      fault = md_fault_none;

      /* decode the instruction */
      MD_SET_OPCODE(op, inst);

      /* execute the instruction */
      switch (op)
	{

#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)	\
	case OP:						\
          out1=O1;						\
          out2=O2;						\
          in1=I1;						\
          in2=I2;						\
          in3=I3;						\
	  SYMCAT(OP,_IMPL);					\
          break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
        case OP:							\
          panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)						\
	  { fault = (FAULT); break; }
#include "machine.def"
	default:
	  panic("attempted to execute a bogus opcode");
      }

      if (fault != md_fault_none)
	fatal("fault (%d) detected @ 0x%08p", fault, regs.regs_PC);

      if (MD_OP_FLAGS(op) & F_MEM)
	{
	  sim_num_refs++;
	  if (MD_OP_FLAGS(op) & F_STORE)
	    is_write = TRUE;
	}
//////////////////////////////////////////////////////////////
	/* keep a load instruction count */
	if(MD_OP_FLAGS(op) & F_LOAD)
	{
		sim_num_loads++;
		if(predict)	/* load value prediction */
		{
			if(isAssoc)
			{
				if(memAddress)
				{
					if(!foundAssociativeLVPTAddress(addr, regs.regs_R[out1], history))
					{
						lvpt = pushLVPTAddress(lvpt, addr);
						insertLVPTValue(lvpt, regs.regs_R[out1], history);
					}
				}
				else
				{
					if(!foundAssociativeLVPTAddress(regs.regs_PC, regs.regs_R[out1], history))
					{
						lvpt = pushLVPTAddress(lvpt, regs.regs_PC);
						insertLVPTValue(lvpt, regs.regs_R[out1], history);
					}
				}
			}
			else
			{
				if(memAddress)
					insertIntoDirrectMappedLVPT(addr, regs.regs_R[out1], history);
				else insertIntoDirrectMappedLVPT(regs.regs_PC, regs.regs_R[out1], history);
			}
		}
		else	/* load value locality verification */
		{
			if(memAddress)
			{
				if(l == NULL)
					l = pushAddress(l, addr, regs.regs_R[out1]);
				else if(!foundAddress(l, addr, regs.regs_R[out1], history))
					l = pushAddress(l, addr, regs.regs_R[out1]);
			}
			else
			{
				if(l == NULL)
					l = pushAddress(l, regs.regs_PC, regs.regs_R[out1]);
				else if(!foundAddress(l, regs.regs_PC, regs.regs_R[out1], history))
					l = pushAddress(l, regs.regs_PC, regs.regs_R[out1]);
			}
		}
	}

////////////////////////////////////////////////////////////

      if (MD_OP_FLAGS(op) & F_CTRL)
	{
	  md_addr_t pred_PC;
	  // struct bpred_update_t update_rec;

      /* finish early? */
      if (max_insts && sim_num_insn >= max_insts)
   {
	  if(isAssoc)
        		Factor_Complexitate_Arhitectura = 2;
  		else
       			Factor_Complexitate_Arhitectura = 1;

//////////////////////////////////////////////////////////
         //if(!vspeedup)
 		 //{

        printf("Factor Complexitate Arhitectura %d \n",Factor_Complexitate_Arhitectura);

		     npenload = sim_num_loads*Tacces_DRAM;
        printf("npenload %d \n", npenload);
     		 T1=(sim_num_insn - sim_num_loads)*1 + npenload;
  		printf("T1 %d \n", T1);
                  //}
  		//else
 		 //{

              if(contextual==0)
                 Tacces_predictor = Factor_Complexitate_Arhitectura*2;
              else
              if(contextual==1)
                 Tacces_predictor = Factor_Complexitate_Arhitectura*5;
              else
                 Tacces_predictor = Factor_Complexitate_Arhitectura*7;

         printf("Contextual %d \n", contextual);
         printf("Tacces_predictor %d \n",  Tacces_predictor);


          npenload=0;
          notfoundA = sim_num_loads - (classifiedPred + classifiedUnpred);

            printf("notfoundA %d \n",  notfoundA);

          foundA = valuePrediction;
             printf("foundA %d \n",  foundA);

	      foundA_miss = classifiedPred - valuePrediction + classifiedUnpred;
             printf("classifiedPred %d \n",   classifiedPred);
             printf("valuePrediction %d \n",  valuePrediction);
             printf("classifiedUnpred %d \n", classifiedUnpred);
             printf("foundAMiss %d \n",  foundA_miss);


          if (notfoundA!=0)
                npenload = npenload + notfoundA* Tacces_DRAM;

          if(foundA!=0)
                npenload=npenload + foundA * Tacces_predictor;

          if(foundA_miss!=0)
                npenload=npenload+foundA_miss * Tacces_DRAM;

           printf("npenload %d \n",  npenload);



           T2=(sim_num_insn - sim_num_loads)*1 + npenload;

 			printf("sim_num_insn  %d \n",  sim_num_insn );
 			printf("sim_num_loads %d \n",  sim_num_loads);
            printf("T2 %d \n",  T2);

             //}

           S=1.00*(T1-T2)/T2*100;
            printf("T2 %f \n",  S);
	return;
    }
//////////////////////////////////////////////////////
    }
}

}
