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
#include "spred.h"
#include "sim.h"



/*
 * This file implements a branch predictor analyzer.
 */

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

/* maximum number of inst's to execute */
static unsigned int max_insts;

/* branch predictor type {nottaken|taken|perfect|bimod|2lev} */
static char *pred_type;

///////////////////////////////////// ADDED /////////////////////

/* store accesses */
static counter_t store_accesses = 0;

/* load accesses */
static counter_t load_accesses = 0;

/* load or store? */
static unsigned int stat_ld;

//////////////////////////////////////////////////////////////////

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

/* flag for whether to show stores or loads */
static int opt_flags = 0; 






////////////////////  CONFIGURABLE /////////////////////////


/* associativity of lvp */
static unsigned int lvp_associatvity = 0;

/* number of sets for lvp */
static unsigned int lvp_sets = 0;

/* history length*/
static unsigned int history_length = 1;

/* locality */
static counter_t locality;


/* replacement policy */
static char lvpred_replacement = 'l';

static replacement_policy policy;

//////////////////////////////////////////////


////////////////   INPUT  ///////////////////


static char* lvpred_opt;

////////////////////////////////////////

//////////////// LVPRED ////////////////////////

static lvp_table lvp;

/////////////////////////////////////////////////////////

////////////// SPRED ///////////////////


static int mem_address = 0; /* 1 - memory address, 0 - instruction address*/

static int contextual = 0; /* 1 - contextual, 0 - incremental */

static int pattern = 3; /* used by contextual */

/* what to do? */
static int predict = 0;	/* 1-prediction; 0-determining load value locality */

/* type of LVPT */
static int is_assoc = 0;		/* 1-associative; 0-dirrect mapped */


/* Total number of correctly predicted loads */
static counter_t value_prediction = 0;

/* Total number of loads classified as predictable */
static counter_t classified_pred = 0;

/* Total number of loads classified as unpredictable */
static counter_t classified_unpred = 0;

/* Total number of correctly classified predictable loads */
static counter_t predictable = 0;

/* Total number of correctly classified unpredictable loads */
static counter_t unpredictable = 0;


static unsigned int lvpt_size = 256;

static counter_t T1 = 0;

static counter_t T2 = 0;

static double localityPercentage;


static double lvpt_prediction_acc;
///////////////////////////////////////


static counter_t npenload=1;
static counter_t foundA=0;

static counter_t foundA_miss=0;
static counter_t notfoundA=0;
float S;

int Factor_Complexitate_Arhitectura = 1;
static int Tacces_incremental;
static int Tacces_contextual;
static int Tacces_hibrid;

static int Tacces_predictor;

static int Tacces_DRAM = 50; // suppose 50ns


static double predictablePerc;

static double unpredictablePerc;

void compute_predict_stats();
void computeSpeedup();


/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-lvpred: This simulator implements a last value predictor analyzer.\n"
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
               
   //////////////////    ADDED //////////////////////////////////

   /* history length */
   opt_reg_uint(odb, "-history", "history length for last value predictor. defaults to 1.",
   		&history_length, 
   		/* default */ 1,
   		/* print */ TRUE,
   		/* format */ NULL);

  opt_reg_note(odb, 
  "  Last Value Predictor uses internally a cache like structure. In order to edit it, you have to use the following format: \n\n"
  "   cache:<sets>:<assoc>:<repl> \n\n"
  "   <sets>  - number of sets for the cache (1 set yields a fully associative cache)\n"
  "   <assoc> - associativity of the cache (1 associativity yields a direct mapped cache) \n"
  "   <repl>  - replacement policy, `l`-LRU, `f`-FIFO, `r`-Random\n\n"
  "   Examples:   -lvpred:cache cache:2:4:r\n"
  "               -lvpred:cache cache:4:8:l\n"
  "");

  // SPRED //

  opt_reg_uint(odb, "-memaddr", "address type {0-instruction address| 1-memory address}",
	       &mem_address, /* default */0,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-pred", "what to do {0-load value locality| 1-prediction}",
	       &predict, /* default */0,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-assoc", "table type {0-dirrect mapped| 1-associative}",
	       &is_assoc, /* default */1,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-lvpt", "dimension of LVPT",
	       &lvpt_size, /* default */4,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-contextual", "type of predictor",
	       &contextual, /* default */1,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-pattern", "size of pattern",
	       &pattern, /* default */3,
	       /* print */TRUE, /* format */NULL);


  // SPRED //

  opt_reg_string(odb, "-lvpred:cache", "config for lvpred cache, cache:<sets>:<assoc>:<repl>",
    &lvpred_opt, "cache:1:8:f", TRUE, NULL);

  
  
  

  ////////////////////////////////////////////////////////////////////

  /* instruction limit */
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
	       &max_insts, /* default */0,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-contor:LD", "registers stats for load or store (0 for store, 1 for load)",
  		&stat_ld, 0, TRUE, NULL);

  opt_reg_string(odb, "-bpred",
		 "branch predictor type {nottaken|taken|bimod|2lev|comb}",
                 &pred_type, /* default */"bimod",
                 /* print */TRUE, /* format */NULL);

  opt_reg_int_list(odb, "-bpred:bimod",
		   "bimodal predictor config (<table size>)",
		   bimod_config, bimod_nelt, &bimod_nelt,
		   /* default */bimod_config,
		   /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

  opt_reg_int_list(odb, "-bpred:2lev",
                   "2-level predictor config "
		   "(<l1size> <l2size> <hist_size> <xor>)",
                   twolev_config, twolev_nelt, &twolev_nelt,
		   /* default */twolev_config,
                   /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

  opt_reg_int_list(odb, "-bpred:comb",
		   "combining predictor config (<meta_table_size>)",
		   comb_config, comb_nelt, &comb_nelt,
		   /* default */comb_config,
		   /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

  opt_reg_int(odb, "-bpred:ras",
              "return address stack size (0 for no return stack)",
              &ras_size, /* default */ras_size,
              /* print */TRUE, /* format */NULL);

  opt_reg_int_list(odb, "-bpred:btb",
		   "BTB config (<num_sets> <associativity>)",
		   btb_config, btb_nelt, &btb_nelt,
		   /* default */btb_config,
		   /* print */TRUE, /* format */NULL, /* !accrue */FALSE);
}

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{
  if (!mystricmp(pred_type, "taken"))
    {
      /* static predictor, not taken */
      pred = bpred_create(BPredTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
  else if (!mystricmp(pred_type, "nottaken"))
    {
      /* static predictor, taken */
      pred = bpred_create(BPredNotTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
  else if (!mystricmp(pred_type, "bimod"))
    {
      if (bimod_nelt != 1)
	fatal("bad bimod predictor config (<table_size>)");
      if (btb_nelt != 2)
	fatal("bad btb config (<num_sets> <associativity>)");

      /* bimodal predictor, bpred_create() checks BTB_SIZE */
      pred = bpred_create(BPred2bit,
			  /* bimod table size */bimod_config[0],
			  /* 2lev l1 size */0,
			  /* 2lev l2 size */0,
			  /* meta table size */0,
			  /* history reg size */0,
			  /* history xor address */0,
			  /* btb sets */btb_config[0],
			  /* btb assoc */btb_config[1],
			  /* ret-addr stack size */ras_size);
    }
  else if (!mystricmp(pred_type, "2lev"))
    {
      /* 2-level adaptive predictor, bpred_create() checks args */
      if (twolev_nelt != 4)
	fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
      if (btb_nelt != 2)
	fatal("bad btb config (<num_sets> <associativity>)");

      pred = bpred_create(BPred2Level,
			  /* bimod table size */0,
			  /* 2lev l1 size */twolev_config[0],
			  /* 2lev l2 size */twolev_config[1],
			  /* meta table size */0,
			  /* history reg size */twolev_config[2],
			  /* history xor address */twolev_config[3],
			  /* btb sets */btb_config[0],
			  /* btb assoc */btb_config[1],
			  /* ret-addr stack size */ras_size);
    }
  else if (!mystricmp(pred_type, "comb"))
    {
      /* combining predictor, bpred_create() checks args */
      if (twolev_nelt != 4)
	fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
      if (bimod_nelt != 1)
	fatal("bad bimod predictor config (<table_size>)");
      if (comb_nelt != 1)
	fatal("bad combining predictor config (<meta_table_size>)");
      if (btb_nelt != 2)
	fatal("bad btb config (<num_sets> <associativity>)");

      pred = bpred_create(BPredComb,
			  /* bimod table size */bimod_config[0],
			  /* l1 size */twolev_config[0],
			  /* l2 size */twolev_config[1],
			  /* meta table size */comb_config[0],
			  /* history reg size */twolev_config[2],
			  /* history xor address */twolev_config[3],
			  /* btb sets */btb_config[0],
			  /* btb assoc */btb_config[1],
			  /* ret-addr stack size */ras_size);
    }
  else
    fatal("cannot parse predictor type `%s'", pred_type);

  if(sscanf(lvpred_opt, "cache:%d:%d:%c",
    &lvp_sets, &lvp_associatvity, &lvpred_replacement) != 3)
      fatal("bad lvpred cache params: cache:<sets>:<assoc>:<repl>");
  
  // policy
  if(lvpred_replacement == 'r')
      policy = RANDOM;
  
  else if(lvpred_replacement == 'l')
      policy = LRU;
  
  else if(lvpred_replacement == 'f')
      policy = FIFO;

  else
      fatal("Unknown replacement policy");

  
  
}


/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions executed",
		   &sim_num_insn, sim_num_insn, NULL);
		   

  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
		   
  if(stat_ld == 0) {
  	stat_reg_counter(sdb, "sim_store_refs",
  		   "total number of stores executed",
  		   &store_accesses, 0, NULL);
 
  }
  else {
  	stat_reg_counter(sdb, "sim_load_refs",
  		   "total number of loads executed",
  		   &load_accesses, 0, NULL);
  	
  }	   
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       &sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);

  stat_reg_counter(sdb, "sim_num_branches",
                   "total number of branches executed",
                   &sim_num_branches, /* initial value */0, /* format */NULL);
  stat_reg_formula(sdb, "sim_IPB",
                   "instruction per branch",
                   "sim_num_insn / sim_num_branches", /* format */NULL);

  stat_reg_counter(sdb, "loadValueLocality",
                  "load value locality for all LD instructions executed",
                  &locality, 0, NULL);

  stat_reg_double(sdb, "loadValueLocalityPerc",
                    "load value locality / number of loads",
                 &localityPercentage, 0, NULL);

  stat_reg_float(sdb, "speedUp",
                    "(T2 / T1 - 1) * 100 [%]",
                  &S, 1, NULL);

  stat_reg_double(sdb, "predictableLoad", "Percentage of predictable load instructions [%]",
                  &predictablePerc, 0, NULL);

  stat_reg_double(sdb, "unpredictableLoad", "Percentage of unpredictable load instructions [%]",
                  &unpredictablePerc, 0, NULL);

  // ADDED //////////////

  if(predict){

   stat_reg_counter(sdb, "valuePrediction",
		   "total number of correctly predicted values",
		   &value_prediction, value_prediction, NULL);
  stat_reg_counter(sdb, "classifiedPred",
		   "number of loads classified as predictable",
		   &classified_pred, classified_pred, NULL);
  stat_reg_counter(sdb, "classifiedUnpred",
		   "number of loads classified as unpredictable",
		   &classified_unpred, classified_unpred, NULL);
  stat_reg_counter(sdb, "predictable",
		   "correctly classified predictable loads",
		   &predictable, predictable, NULL);
  stat_reg_counter(sdb, "unpredictable",
		   "correctly classified unpredictable loads",
		   &unpredictable, unpredictable, NULL);

  stat_reg_double(sdb, "lvptPredictionAccuracy",
      "Prediction accuracy for LVPT",
      &lvpt_prediction_acc, 0, NULL);

  }


  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
  stat_reg_counter(sdb, "sim_num_loads",
		   "total number of loads executed",
		   &load_accesses, 0, NULL);
  stat_reg_counter(sdb, "T1",
                   "T1",&T1, /* initial value */0, /* format */NULL);
  stat_reg_counter(sdb, "T2",
                   "T2",&T2, /* initial value */0, /* format */NULL);  


  ////////////////////

  /* register predictor stats */
  if (pred)
    bpred_reg_stats(pred, sdb);
}

/* initialize the simulator */
void
sim_init(void)
{
  sim_num_refs = 0;

  value_predictor_type type = contextual == 0 ? STRIDE: CONTEXTUAL;

  // use flags!!!

  if(is_assoc){ // fully associative, ways = lvpt size, number of sets = 1

    lvp_associatvity = lvpt_size;
    lvp_sets = 1;

  }
  else{ // direct mapped, ways = 1, numbers of sets = lvpt size

    lvp_associatvity = 1;
    lvp_sets = lvpt_size;

  }

  // initialize lvpred
  init_lvp(&lvp, lvp_sets, lvp_associatvity, history_length, LRU, type); // using lru for now...
  
  store_accesses = 0;
  load_accesses = 0;

  /* allocate and initialize register file */
  regs_init(&regs);

  /* allocate and initialize memory space */
  mem = mem_create("mem");
  mem_init(mem);
}

/* local machine state accessor */
static char *					/* err str, NULL for no err */
bpred_mstate_obj(FILE *stream,			/* output stream */
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
void
sim_load_prog(char *fname,		/* program to load */
	      int argc, char **argv,	/* program arguments */
	      char **envp)		/* program environment */
{
  /* load program text and data, set up environment, memory, and regs */
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

  /* initialize the DLite debugger */
  dlite_init(md_reg_obj, dlite_mem_obj, bpred_mstate_obj);
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream)		/* output stream */
{
  /* nothing currently */
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)		/* output stream */
{
  /* nada */
}

/* un-initialize simulator-specific state */
void
sim_uninit(void)
{
  /* nada */
}


/*
 * configure the execution engine
 */

/*
 * precise architected register accessors
 */

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


//// ADDED ////////////

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

////////// END  //////////////////////

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

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{

  int out1;

  md_inst_t inst;
  register md_addr_t addr, target_PC;
  enum md_opcode op;
  register int is_write;
  int stack_idx;
  enum md_fault_type fault;

  fprintf(stderr, "sim: ** starting functional simulation w/ predictors **\n");

  /* set up initial default next PC */
  regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

  /* check for DLite debugger entry condition */
  if (dlite_check_break(regs.regs_PC, /* no access */0, /* addr */0, 0, 0))
    dlite_main(regs.regs_PC - sizeof(md_inst_t), regs.regs_PC,
	       sim_num_insn, &regs, mem);

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
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
	case OP:		\
          out1=O1;        \
          SYMCAT(OP,_IMPL);						\
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
	  
	  
	  if (MD_OP_FLAGS(op) & F_STORE) {
    		is_write = TRUE;
	    	store_accesses = store_accesses + 1;
	    }
	  if(MD_OP_FLAGS(op) & F_LOAD) {

	       load_accesses = load_accesses + 1;

         word_t value = MEM_READ_WORD(mem, addr);

         md_addr_t indexer = mem_address == 0 ? regs.regs_PC: addr;

         if(predict == 0) { // load value locality

            update_lvp(&lvp, indexer, value);

         }

         else { // prediction



            // predict first
            word_t predicted = predict_value(&lvp, indexer, pattern);

            update_prediction_confidence(&lvp, indexer, value, predicted);

            update_lvp(&lvp, indexer, value); // still need to update

         }

    }
	       
	   
	}

      if (MD_OP_FLAGS(op) & F_CTRL)
	{
	  md_addr_t pred_PC;
	  struct bpred_update_t update_rec;

	  sim_num_branches++;

	  if (pred)
	    {
	      /* get the next predicted fetch address */
	      pred_PC = bpred_lookup(pred,
				     /* branch addr */regs.regs_PC,
				     /* target */target_PC,
				     /* inst opcode */op,
				     /* call? */MD_IS_CALL(op),
				     /* return? */MD_IS_RETURN(op),
				     /* stash an update ptr */&update_rec,
				     /* stash return stack ptr */&stack_idx);

	      /* valid address returned from branch predictor? */
	      if (!pred_PC)
		{
		  /* no predicted taken target, attempt not taken target */
		  pred_PC = regs.regs_PC + sizeof(md_inst_t);
		}

	      bpred_update(pred,
			   /* branch addr */regs.regs_PC,
			   /* resolved branch target */regs.regs_NPC,
			   /* taken? */regs.regs_NPC != (regs.regs_PC +
							 sizeof(md_inst_t)),
			   /* pred taken? */pred_PC != (regs.regs_PC +
							sizeof(md_inst_t)),
			   /* correct pred? */pred_PC == regs.regs_NPC,
			   /* opcode */op,
			   /* predictor update pointer */&update_rec);
	    }
	}

      /* check for DLite debugger entry condition */
      if (dlite_check_break(regs.regs_NPC,
			    is_write ? ACCESS_WRITE : ACCESS_READ,
			    addr, sim_num_insn, sim_num_insn))
	dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);

      /* go to the next instruction */
      regs.regs_PC = regs.regs_NPC;
      regs.regs_NPC += sizeof(md_inst_t);

      /* finish early? */
      if (max_insts && sim_num_insn >= max_insts) {

          locality = lvp.locality_hits;
          localityPercentage = load_value_locality(&lvp);

          if(predict){

            compute_predict_stats();

            computeSpeedup();

          }


          
	        return;
	    }

    }

    locality = lvp.locality_hits;
    localityPercentage = load_value_locality(&lvp);

    if(predict){

      compute_predict_stats();

       computeSpeedup();

    }
          

}

void compute_predict_stats(){


      value_prediction = lvp.correct_predictions;
      classified_pred = lvp.loads_predictable;
      predictable = lvp.correct_predictable;

      classified_unpred = lvp.loads_unpredictable;
      unpredictable = lvp.correct_unpredictable;


      lvpt_prediction_acc = value_prediction * 1.0 / load_accesses;

      predictablePerc = (value_prediction * 100.0) / classified_pred;
      unpredictablePerc = (unpredictable * 100.0) / classified_unpred;


}

void computeSpeedup(){


  printf("Factor Complexitate Arhitectura %d \n",Factor_Complexitate_Arhitectura);

  npenload = load_accesses * Tacces_DRAM; 
  printf("npenload %d \n", npenload);
  T1=(sim_num_insn - load_accesses)*1 + npenload;
  printf("T1 %d \n", T1);
        
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
  notfoundA = load_accesses - (classified_pred + classified_unpred);
    
  printf("notfoundA %d \n",  notfoundA);  

  foundA = value_prediction;
      printf("foundA %d \n",  foundA);  
    
foundA_miss = classified_pred - value_prediction + classified_unpred;
      printf("classifiedPred %d \n",   classified_pred);
      printf("valuePrediction %d \n",  value_prediction);
      printf("classifiedUnpred %d \n", classified_unpred);
      printf("foundAMiss %d \n",  foundA_miss);


  if (notfoundA!=0) 
        npenload = npenload + notfoundA* Tacces_DRAM;
  
  if(foundA!=0) 
        npenload=npenload + foundA * Tacces_predictor;

  if(foundA_miss!=0) 
        npenload=npenload+foundA_miss * Tacces_DRAM;

    printf("npenload %d \n",  npenload);



  T2=(sim_num_insn - load_accesses)*1 + npenload;

  printf("sim_num_insn  %d \n",  sim_num_insn );
  printf("sim_num_loads %d \n",  load_accesses);
  printf("T2 %d \n",  T2);

  S=1.00*(T1-T2)/T2*100;
  printf("S %f \n",  S);

}
