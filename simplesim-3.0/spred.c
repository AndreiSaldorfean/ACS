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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "spred.h"
addrList news()
{
  return NULL;
}

addrList pushAddress(addrList p, md_addr_t addr, sword_t value)
{
  addrList q;
  valueList r;
  q=(addrList)malloc(sizeof(struct location));
  q->addr=addr;
  q->nextAddress=p;
  r=(valueList)malloc(sizeof(struct element));
  r->value=value;
  r->freq = 1;
  r->nextValue = NULL;
  q->values = r;
  return q;
}

void pushValue(addrList p, sword_t value)
{
  valueList q;
  q=(valueList)malloc(sizeof(struct element));
  q->freq = 1;
  q->value=value;
  q->nextValue=p->values;
  p->values=q;
}

int foundValue(addrList l, sword_t value, int history, int distinct)
{
  int found = 0;
  int i;
  valueList q = l->values;	
  valueList p = q->nextValue;
  if(q->value == value)
  {
	  found = 1;
	  loadValueLocality ++;
  }
  else if(history == 1)
  {
	  if(q != NULL)
	  {
		l->values = NULL;
		free(q);
	  }
  }
  for(i=1; i<history; i++)
  {
	  if( (p == NULL) || (found && distinct) )
	  {
		// the value always must be added to the list of not distinct values
		if(!distinct)
			found = 0;
		break;
	  }
	  if(p->value == value)
	  {
	    if(!found)
	        loadValueLocality ++;
		found = 1;
		if(distinct){
			// the value is moved to the first position in the list of distinct values
			q->nextValue = p->nextValue;
			p->nextValue = l->values;
			l->values = p;
			break;
		}
	  }
	  if(i == history-1)
	  {
	    q->nextValue = NULL;
	    free(p);
		// the value always must be added to the list of not distinct values
		if(!distinct)
			found = 0;
	  }
	  p=p->nextValue;
	  q=q->nextValue;
  }
  return found;
}


int foundAddress(addrList l, md_addr_t addr, sword_t value, int history, int distinct)
{

  addrList p = l;
  int found = 0;
  while(p!=NULL)
  {
	if(p->addr == addr)
	{
	  found = 1;
	  if(!foundValue(p, value, history, distinct))
		pushValue(p, value);
	  break;
	}
    p=p->nextAddress;
  }
  return found;
}	

JVPTaddrList pushJVPTAddress(JVPTaddrList p, md_addr_t addr)
{
  JVPTaddrList q;
  q=(JVPTaddrList)malloc(sizeof(struct JVPTlocation));
  q->addr=addr;
  q->nextAddress=p;
  q->values = NULL;
  q->automat = 0;
  return q;
}

void insertJVPTValue(JVPTaddrList ad, sword_t value, int history, int contextual)
{
  int i;
  JVPTvalueList q = ad->values;	
  JVPTvalueList p;
  JVPTvalueList newValue;
  if(!contextual || contextual == 2)	// stride or hybrid predictor
  {
	  if(ad->values != NULL)
	  {
		ad->stride[1] = ad->stride[0];
		ad->stride[0] = value - q->value;
	  }
  }
  if(history == 1)
  {
	  if(ad->values != NULL)
	  {
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
  newValue=(JVPTvalueList)malloc(sizeof(struct JVPTelement));
  newValue->value=value;
  newValue->nextValue=ad->values;
  ad->values=newValue;  	
}

sword_t maxValue(JVPTvalueList p)
{
	JVPTvalueList q = p;
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

sword_t predictValue(JVPTaddrList p, int history, int contextual, int pattern)
{
	int i,j;
	JVPTvalueList q = p->values;
	if(!contextual && p->stride[0] == p->stride[1])		// stride prediction
		return q->value + p->stride[0];
	if(history == 1 && q != NULL)	// last value prediction
		return q->value;
	if(contextual && history>1 && q != NULL)	// contextual or hybrid prediction
	{
		int currentPattern = pattern;
		while(currentPattern > 0)
		{
			JVPTvalueList patternValueList = NULL;
			for(i=0; i<history-currentPattern; i++)
			{
				JVPTvalueList k;
				JVPTvalueList patternValues;
				int isPattern;
				if(q == NULL)
					break;
				k = q->nextValue;
				patternValues = p->values;
				isPattern = 1;
				for(j=0; j<currentPattern; j++)
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
					JVPTvalueList newValue;
					if(patternValueList == NULL)	// insert the value
					{
						newValue=(JVPTvalueList)malloc(sizeof(struct JVPTelement));
						newValue->value = q->value; // the value which follows the pattern
						newValue->count = 1;
						newValue->nextValue = patternValueList;
						patternValueList = newValue;
					}
					else
					{
						int found = 0;
						JVPTvalueList temp = patternValueList;
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
							newValue=(JVPTvalueList)malloc(sizeof(struct JVPTelement));
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
			currentPattern--;
		}
		if(contextual == 2 && p->stride[0] == p->stride[1])	// stride prediction of hybrid predictor
			return p->values->value + p->stride[0];
		else return p->values->value;	// the last value
	}
	return 0;
}

void freeValueList(JVPTvalueList *values)
{
	JVPTvalueList p = *values;
	JVPTvalueList q;
	while(p != NULL)
	{
		q = p->nextValue;
		free(p);
		p = q;
	}

}

int foundAssociativeJVPTAddress(md_addr_t addr, sword_t value, int history, JVPTaddrList *jvpt, int JVPTdim, int contextual, int pattern)
{

  JVPTaddrList p = *jvpt;
  JVPTaddrList q; 
  int i;
  int found = 0;
  if(*jvpt == NULL)
	  return found;
  if(p->addr == addr)
  {
		found = 1;
		if(p->values != NULL)
		{
			if(value == predictValue(p, history, contextual, pattern))
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
					wpredicted++;
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
		insertJVPTValue(p, value, history, contextual);
		return found;
  }  
  else
  {
	q = p->nextAddress;
    for(i=1;; i++)
	{
		if( (q == NULL) || (i == JVPTdim) ) break;
		if(q->addr == addr)
		{
			found = 1;
			if(q->values != NULL)
			{
				if(value == predictValue(q, history, contextual, pattern))
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
						wpredicted++;
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
			insertJVPTValue(q, value, history, contextual);
			/* the address is moved to the first position in the list */
			p->nextAddress = q->nextAddress;
			q->nextAddress = *jvpt;
			*jvpt = q;
			break;
		}
		if(i == JVPTdim-1)
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

/* JVPT initialization */
JVPTaddrList JVPTinit(JVPTaddrList l, int JVPTdim)
{
	int i;
	for(i=0; i<JVPTdim; i++)
		l = pushJVPTAddress(l, 0);
	return l;
}

void insertIntoDirrectMappedJVPT(md_addr_t addr, sword_t value, int history, JVPTaddrList *jvpt, int contextual, int JVPTdim, int pattern)
{

	JVPTaddrList p = *jvpt;
	JVPTvalueList q;
	int index;
	int i;
	index = addr % JVPTdim;
	for(i=0; i<index; i++)
		p = p->nextAddress;
	if(p != NULL)
	{
		if(p->addr != addr)
		{
			p->addr = addr;
			q = p->values;
			p->values = NULL;
			freeValueList(&q);
			p->automat = 0;
		}
		if(p->values != NULL)
		{
			if(value == predictValue(p, history, contextual, pattern))
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
					wpredicted++;
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
		insertJVPTValue(p, value, history, contextual);
	}

}

/*** stabilirea aritatii ***/

int foundAddress_INDIR(addrList l, md_addr_t addr, sword_t value, int history)
{
  addrList p = l;
  int found = 0;
  while(p!=NULL)
  {
	if(p->addr == addr)
	{
	    found = 1;
		if(!foundValue_INDIR(p, value, history))
			pushValue(p, value);
		break;
	}
    p=p->nextAddress;
  }
  return found;
}

int foundValue_INDIR(addrList l, sword_t value, int history)
{
  int found = 0;
  int i;
  valueList q = l->values;	
  valueList p = q->nextValue;
  if(q->value == value)
  {
	  found = 1;
	  INDIRValueLocality ++;
	  q->freq++;
  }
  else /*if(history == 1)
  {
	  if(q != NULL)
	  {
		l->values = NULL;
		free(q);
	  }
  }*/
  for(i=1; /*i<history*/; i++)
  {
	  if(p == NULL) break; // nu mai exista elemente in lista de valori
	  if(p->value == value)
	  {
	    if(!found)
	    {	
	     found = 1;
	     INDIRValueLocality ++;
		 p->freq++;
		 break;
	    } 
	  }
	  /*if(i == history-1)
	  {
	    q->nextValue = NULL;
	    free(p);
	  }*/
	  p=p->nextValue;
	  q=q->nextValue;
  }
  return found;
}

void aritate_Address(addrList l)
{
  addrList p = l;
  valueList q;
  long monomorfe_s = 0,duomorfe_s = 0,polimorfe_s = 0,i,suma_verif = 0;
  long monomorfe_d = 0,duomorfe_d = 0,polimorfe_d = 0,temp_m, temp_d=0, temp_p=0;

  while(p!=NULL)	
  {
	q = p->values;
	i = 0;
	temp_m=0;
    fprintf(stderr,"\nAlt__PC 0x%x \n",p->addr);
	while(q!=NULL)
	{

		suma_verif+=q->freq;
		i++;
		fprintf(stderr,"i=%ld Target 0x%x  freq = %d\n",i,q->value,q->freq);
		temp_m+=q->freq;
		q = q->nextValue;
	}
 	if(i==0)
		printf("Prostie");
	else
		if(i==1)
		{
			monomorfe_s++;
			monomorfe_d+=temp_m;
		}
		else
			if(i==2)
			{
				duomorfe_d+=temp_m;
				duomorfe_s++;
			}
			else
			{
				polimorfe_s++;
				polimorfe_d+=temp_m;
			}
	p = p->nextAddress;
  }
  fprintf(stderr,"monomorfe_s %d duomorfe_s %d polimorfe_s %d \n suma verificare %d",monomorfe_s,duomorfe_s,polimorfe_s,suma_verif);
  fprintf(stderr,"monomorfe_d %d duomorfe_d %d polimorfe_d %d ",monomorfe_d,duomorfe_d,polimorfe_d);
}

/*****/
/* PPM value predictor stats */
void
vpred_ppm_stats(struct stat_sdb_t *sdb)	/* stats database */
{

/*/  stat_reg_counter(sdb, "loadValueLocality",
		   "total number of before seen load values",
		   &loadValueLocality, 0, NULL);*/
  stat_reg_counter(sdb, "JIndirValueLocality",
		   "jindir value locality",
		   &INDIRValueLocality, 0, NULL);
  stat_reg_counter(sdb, "valuePrediction",
		   "total number of correctly predicted values",
		   &valuePrediction, 0, NULL);
  stat_reg_counter(sdb, "classifiedPred",
		   "number of loads classified as predictable",
		   &classifiedPred, 0, NULL);
  stat_reg_counter(sdb, "classifiedUnpred",
		   "number of loads classified as unpredictable",
		   &classifiedUnpred, 0, NULL);
  stat_reg_counter(sdb, "predictable",
		   "correctly classified predictable loads",
		   &predictable, 0, NULL);
  stat_reg_counter(sdb, "wpredicted",
		   "wrong predicted JIndir's",
		   &wpredicted, 0, NULL);
}

/************************
	Target Cache
************************/
IVPTCache pushIVPTCacheLocation(IVPTCache p)
{
  IVPTCache q;
  q=(IVPTCache)malloc(sizeof(struct IVPTlocation));
  q->nextPos=p;
  q->valueList = NULL;
  return q;
}

void insertIVPTValue(IVPTCache ad, md_addr_t addr,md_addr_t tag, int nposcache)
{
  int i;
  IVPTValueList q = ad->valueList;	
  IVPTValueList p;
  IVPTValueList newValue;
  if(nposcache == 1)
  {
	  if(ad->valueList != NULL)
	  {		  
		  q->addr = addr;
		  q->tag = tag;
		  return;	
	  }
  }
  
  else if(q != NULL)
  {
	p = q->nextPos;
    for(i=1; i<nposcache; i++)
	{
	  if(p == NULL) break;
	  if(q->tag == tag)
	  {
	    q->addr = addr;
	    return;
	  }

	  if(i == nposcache-1)
	  {
	    q->nextPos= NULL;
	    free(p);
		break;
	  }
	  p=p->nextPos;
	  q=q->nextPos;
	}

  }
	
  newValue=(IVPTValueList)malloc(sizeof(struct IVPTelement));
  newValue->addr = addr;
  newValue->tag = tag;
  newValue->nextPos=ad->valueList;
  ad->valueList = newValue;  	

}

sword_t TCPredictValue(IVPTCache p,md_addr_t tag ,int nposcache)
{
	int i;
	IVPTValueList q = p->valueList;

	if(nposcache == 1 && q != NULL)
		if(tag == q->tag)
			return q->addr;
		else
			return -1;
	else if(nposcache > 1 && q != NULL)	
	{
		for(i=0; i<nposcache; i++)
		{
			if(q == NULL)
				break;

			if(tag == q->tag)
				return q->addr;
				
			q = q->nextPos;
		}
		return -1;
	}
	return -1;
}

IVPTCache IVPTinit(IVPTCache l, int dimcache)
{
	int i;
	for(i=0; i<dimcache; i++)
		l = pushIVPTCacheLocation(l);
	return l;
}

void insertIntoDirrectMappedTargetCache(md_addr_t addr, md_addr_t tag, int index , int dimcache , int nposcache)
{

	IVPTCache p = ivpt;
	int i;
	for(i=0; i<index; i++)
		p = p->nextPos;
	if(p != NULL)
	{
		if(p->valueList != NULL)
		{
			if(addr == TCPredictValue(p, tag ,nposcache))
			{
				valuePrediction++;
			}
			else
			{
				insertIVPTValue(p, addr, tag, nposcache);
			
			}
		}
		else
			insertIVPTValue(p, addr, tag, nposcache);
		
	}

}

/* target cache value predictor stats */
void
vpred_tc_stats(struct stat_sdb_t *sdb)	/* stats database */
{
	stat_reg_counter(sdb, "valuePrediction",
		   "total number of correctly predicted values",
		   &valuePrediction, 0, NULL);
}
/* turn this on to enable the SimpleScalar 2.0 RAS bug */
/* #define RAS_BUG_COMPATIBLE */

/* create a branch predictor */
struct bpred_t *			/* branch predictory instance */
bpred_create(enum bpred_class class,	/* type of predictor to create */
	     unsigned int bimod_size,	/* bimod table size */
	     unsigned int l1size,	/* 2lev l1 table size */
	     unsigned int l2size,	/* 2lev l2 table size */
	     unsigned int meta_size,	/* meta table size */
	     unsigned int shift_width,	/* history register width */
	     unsigned int xor,  	/* history xor address flag */
	     unsigned int btb_sets,	/* number of sets in BTB */ 
	     unsigned int btb_assoc,	/* BTB associativity */
	     unsigned int retstack_size) /* num entries in ret-addr stack */
{
  struct bpred_t *pred;

  if (!(pred = calloc(1, sizeof(struct bpred_t))))
    fatal("out of virtual memory");

  pred->class = class;

  switch (class) {
  case BPredComb:
    /* bimodal component */
    pred->dirpred.bimod = 
      bpred_dir_create(BPred2bit, bimod_size, 0, 0, 0);

    /* 2-level component */
    pred->dirpred.twolev = 
      bpred_dir_create(BPred2Level, l1size, l2size, shift_width, xor);

    /* metapredictor component */
    pred->dirpred.meta = 
      bpred_dir_create(BPred2bit, meta_size, 0, 0, 0);

    break;

  case BPred2Level:
    pred->dirpred.twolev = 
      bpred_dir_create(class, l1size, l2size, shift_width, xor);

    break;

  case BPred2bit:
    pred->dirpred.bimod = 
      bpred_dir_create(class, bimod_size, 0, 0, 0);

  case BPredTaken:
  case BPredNotTaken:
    /* no other state */
    break;

  default:
    panic("bogus predictor class");
  }

  /* allocate ret-addr stack */
  switch (class) {
  case BPredComb:
  case BPred2Level:
  case BPred2bit:
    {
      int i;

      /* allocate BTB */
      if (!btb_sets || (btb_sets & (btb_sets-1)) != 0)
	fatal("number of BTB sets must be non-zero and a power of two");
      if (!btb_assoc || (btb_assoc & (btb_assoc-1)) != 0)
	fatal("BTB associativity must be non-zero and a power of two");

      if (!(pred->btb.btb_data = calloc(btb_sets * btb_assoc,
					sizeof(struct bpred_btb_ent_t))))
	fatal("cannot allocate BTB");

      pred->btb.sets = btb_sets;
      pred->btb.assoc = btb_assoc;

      if (pred->btb.assoc > 1)
	for (i=0; i < (pred->btb.assoc*pred->btb.sets); i++)
	  {
	    if (i % pred->btb.assoc != pred->btb.assoc - 1)
	      pred->btb.btb_data[i].next = &pred->btb.btb_data[i+1];
	    else
	      pred->btb.btb_data[i].next = NULL;
	    
	    if (i % pred->btb.assoc != pred->btb.assoc - 1)
	      pred->btb.btb_data[i+1].prev = &pred->btb.btb_data[i];
	  }

      /* allocate retstack */
      if ((retstack_size & (retstack_size-1)) != 0)
	fatal("Return-address-stack size must be zero or a power of two");
      
      pred->retstack.size = retstack_size;
      if (retstack_size)
	if (!(pred->retstack.stack = calloc(retstack_size, 
					    sizeof(struct bpred_btb_ent_t))))
	  fatal("cannot allocate return-address-stack");
      pred->retstack.tos = retstack_size - 1;
      
      break;
    }

  case BPredTaken:
  case BPredNotTaken:
    /* no other state */
    break;

  default:
    panic("bogus predictor class");
  }

  return pred;
}

/* create a branch direction predictor */
struct bpred_dir_t *		/* branch direction predictor instance */
bpred_dir_create (
  enum bpred_class class,	/* type of predictor to create */
  unsigned int l1size,	 	/* level-1 table size */
  unsigned int l2size,	 	/* level-2 table size (if relevant) */
  unsigned int shift_width,	/* history register width */
  unsigned int xor)	    	/* history xor address flag */
{
  struct bpred_dir_t *pred_dir;
  unsigned int cnt;
  int flipflop;

  if (!(pred_dir = calloc(1, sizeof(struct bpred_dir_t))))
    fatal("out of virtual memory");

  pred_dir->class = class;

  cnt = -1;
  switch (class) {
  case BPred2Level:
    {
      if (!l1size || (l1size & (l1size-1)) != 0)
	fatal("level-1 size, `%d', must be non-zero and a power of two", 
	      l1size);
      pred_dir->config.two.l1size = l1size;
      
      if (!l2size || (l2size & (l2size-1)) != 0)
	fatal("level-2 size, `%d', must be non-zero and a power of two", 
	      l2size);
      pred_dir->config.two.l2size = l2size;
      
      if (!shift_width || shift_width > 30)
	fatal("shift register width, `%d', must be non-zero and positive",
	      shift_width);
      pred_dir->config.two.shift_width = shift_width;
      
      pred_dir->config.two.xor = xor;
      pred_dir->config.two.shiftregs = calloc(l1size, sizeof(int));
      if (!pred_dir->config.two.shiftregs)
	fatal("cannot allocate shift register table");
      
      pred_dir->config.two.l2table = calloc(l2size, sizeof(unsigned char));
      if (!pred_dir->config.two.l2table)
	fatal("cannot allocate second level table");

      /* initialize counters to weakly this-or-that */
      flipflop = 1;
      for (cnt = 0; cnt < l2size; cnt++)
	{
	  pred_dir->config.two.l2table[cnt] = flipflop;
	  flipflop = 3 - flipflop;
	}

      break;
    }

  case BPred2bit:
    if (!l1size || (l1size & (l1size-1)) != 0)
      fatal("2bit table size, `%d', must be non-zero and a power of two", 
	    l1size);
    pred_dir->config.bimod.size = l1size;
    if (!(pred_dir->config.bimod.table =
	  calloc(l1size, sizeof(unsigned char))))
      fatal("cannot allocate 2bit storage");
    /* initialize counters to weakly this-or-that */
    flipflop = 1;
    for (cnt = 0; cnt < l1size; cnt++)
      {
	pred_dir->config.bimod.table[cnt] = flipflop;
	flipflop = 3 - flipflop;
      }

    break;

  case BPredTaken:
  case BPredNotTaken:
    /* no other state */
    break;

  default:
    panic("bogus branch direction predictor class");
  }

  return pred_dir;
}

/* print branch direction predictor configuration */
void
bpred_dir_config(
  struct bpred_dir_t *pred_dir,	/* branch direction predictor instance */
  char name[],			/* predictor name */
  FILE *stream)			/* output stream */
{
  switch (pred_dir->class) {
  case BPred2Level:
    fprintf(stream,
      "pred_dir: %s: 2-lvl: %d l1-sz, %d bits/ent, %s xor, %d l2-sz, direct-mapped\n",
      name, pred_dir->config.two.l1size, pred_dir->config.two.shift_width,
      pred_dir->config.two.xor ? "" : "no", pred_dir->config.two.l2size);
    break;

  case BPred2bit:
    fprintf(stream, "pred_dir: %s: 2-bit: %d entries, direct-mapped\n",
      name, pred_dir->config.bimod.size);
    break;

  case BPredTaken:
    fprintf(stream, "pred_dir: %s: predict taken\n", name);
    break;

  case BPredNotTaken:
    fprintf(stream, "pred_dir: %s: predict not taken\n", name);
    break;

  default:
    panic("bogus branch direction predictor class");
  }
}

/* print branch predictor configuration */
void
bpred_config(struct bpred_t *pred,	/* branch predictor instance */
	     FILE *stream)		/* output stream */
{
  switch (pred->class) {
  case BPredComb:
    bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
    bpred_dir_config (pred->dirpred.twolev, "2lev", stream);
    bpred_dir_config (pred->dirpred.meta, "meta", stream);
    fprintf(stream, "btb: %d sets x %d associativity", 
	    pred->btb.sets, pred->btb.assoc);
    fprintf(stream, "ret_stack: %d entries", pred->retstack.size);
    break;

  case BPred2Level:
    bpred_dir_config (pred->dirpred.twolev, "2lev", stream);
    fprintf(stream, "btb: %d sets x %d associativity", 
	    pred->btb.sets, pred->btb.assoc);
    fprintf(stream, "ret_stack: %d entries", pred->retstack.size);
    break;

  case BPred2bit:
    bpred_dir_config (pred->dirpred.bimod, "bimod", stream);
    fprintf(stream, "btb: %d sets x %d associativity", 
	    pred->btb.sets, pred->btb.assoc);
    fprintf(stream, "ret_stack: %d entries", pred->retstack.size);
    break;

  case BPredTaken:
    bpred_dir_config (pred->dirpred.bimod, "taken", stream);
    break;
  case BPredNotTaken:
    bpred_dir_config (pred->dirpred.bimod, "nottaken", stream);
    break;

  default:
    panic("bogus branch predictor class");
  }
}

/* print predictor stats */
void
bpred_stats(struct bpred_t *pred,	/* branch predictor instance */
	    FILE *stream)		/* output stream */
{
  fprintf(stream, "pred: addr-prediction rate = %f\n",
	  (double)pred->addr_hits/(double)(pred->addr_hits+pred->misses));
  fprintf(stream, "pred: dir-prediction rate = %f\n",
	  (double)pred->dir_hits/(double)(pred->dir_hits+pred->misses));
}

/* register branch predictor stats */
void
bpred_reg_stats(struct bpred_t *pred,	/* branch predictor instance */
		struct stat_sdb_t *sdb)	/* stats database */
{
  char buf[512], buf1[512], *name;

  /* get a name for this predictor */
  switch (pred->class)
    {
    case BPredComb:
      name = "bpred_comb";
      break;
    case BPred2Level:
      name = "bpred_2lev";
      break;
    case BPred2bit:
      name = "bpred_bimod";
      break;
    case BPredTaken:
      name = "bpred_taken";
      break;
    case BPredNotTaken:
      name = "bpred_nottaken";
      break;
    default:
      panic("bogus branch predictor class");
    }

  sprintf(buf, "%s.lookups", name);
  stat_reg_counter(sdb, buf, "total number of bpred lookups",
		   &pred->lookups, 0, NULL);
  sprintf(buf, "%s.updates", name);
  sprintf(buf1, "%s.dir_hits + %s.misses", name, name);
  stat_reg_formula(sdb, buf, "total number of updates", buf1, "%12.0f");
  sprintf(buf, "%s.addr_hits", name);
  stat_reg_counter(sdb, buf, "total number of address-predicted hits", 
		   &pred->addr_hits, 0, NULL);
  sprintf(buf, "%s.dir_hits", name);
  stat_reg_counter(sdb, buf, 
		   "total number of direction-predicted hits "
		   "(includes addr-hits)", 
		   &pred->dir_hits, 0, NULL);
  if (pred->class == BPredComb)
    {
      sprintf(buf, "%s.used_bimod", name);
      stat_reg_counter(sdb, buf, 
		       "total number of bimodal predictions used", 
		       &pred->used_bimod, 0, NULL);
      sprintf(buf, "%s.used_2lev", name);
      stat_reg_counter(sdb, buf, 
		       "total number of 2-level predictions used", 
		       &pred->used_2lev, 0, NULL);
    }
  sprintf(buf, "%s.misses", name);
  stat_reg_counter(sdb, buf, "total number of misses", &pred->misses, 0, NULL);
  sprintf(buf, "%s.jr_hits", name);
  stat_reg_counter(sdb, buf,
		   "total number of address-predicted hits for JR's",
		   &pred->jr_hits, 0, NULL);
  sprintf(buf, "%s.jr_seen", name);
  stat_reg_counter(sdb, buf,
		   "total number of JR's seen",
		   &pred->jr_seen, 0, NULL);
  sprintf(buf, "%s.jr_non_ras_hits.PP", name);
  stat_reg_counter(sdb, buf,
		   "total number of address-predicted hits for non-RAS JR's",
		   &pred->jr_non_ras_hits, 0, NULL);
  sprintf(buf, "%s.jr_non_ras_seen.PP", name);
  stat_reg_counter(sdb, buf,
		   "total number of non-RAS JR's seen",
		   &pred->jr_non_ras_seen, 0, NULL);
  sprintf(buf, "%s.bpred_addr_rate", name);
  sprintf(buf1, "%s.addr_hits / %s.updates", name, name);
  stat_reg_formula(sdb, buf,
		   "branch address-prediction rate (i.e., addr-hits/updates)",
		   buf1, "%9.4f");
  sprintf(buf, "%s.bpred_dir_rate", name);
  sprintf(buf1, "%s.dir_hits / %s.updates", name, name);
  stat_reg_formula(sdb, buf,
		  "branch direction-prediction rate (i.e., all-hits/updates)",
		  buf1, "%9.4f");
  sprintf(buf, "%s.bpred_jr_rate", name);
  sprintf(buf1, "%s.jr_hits / %s.jr_seen", name, name);
  stat_reg_formula(sdb, buf,
		  "JR address-prediction rate (i.e., JR addr-hits/JRs seen)",
		  buf1, "%9.4f");
  sprintf(buf, "%s.bpred_jr_non_ras_rate.PP", name);
  sprintf(buf1, "%s.jr_non_ras_hits.PP / %s.jr_non_ras_seen.PP", name, name);
  stat_reg_formula(sdb, buf,
		   "non-RAS JR addr-pred rate (ie, non-RAS JR hits/JRs seen)",
		   buf1, "%9.4f");
  sprintf(buf, "%s.retstack_pushes", name);
  stat_reg_counter(sdb, buf,
		   "total number of address pushed onto ret-addr stack",
		   &pred->retstack_pushes, 0, NULL);
  sprintf(buf, "%s.retstack_pops", name);
  stat_reg_counter(sdb, buf,
		   "total number of address popped off of ret-addr stack",
		   &pred->retstack_pops, 0, NULL);
  sprintf(buf, "%s.used_ras.PP", name);
  stat_reg_counter(sdb, buf,
		   "total number of RAS predictions used",
		   &pred->used_ras, 0, NULL);
  sprintf(buf, "%s.ras_hits.PP", name);
  stat_reg_counter(sdb, buf,
		   "total number of RAS hits",
		   &pred->ras_hits, 0, NULL);
  sprintf(buf, "%s.ras_rate.PP", name);
  sprintf(buf1, "%s.ras_hits.PP / %s.used_ras.PP", name, name);
  stat_reg_formula(sdb, buf,
		   "RAS prediction rate (i.e., RAS hits/used RAS)",
		   buf1, "%9.4f");
}

void
bpred_after_priming(struct bpred_t *bpred)
{
  if (bpred == NULL)
    return;

  bpred->lookups = 0;
  bpred->addr_hits = 0;
  bpred->dir_hits = 0;
  bpred->used_ras = 0;
  bpred->used_bimod = 0;
  bpred->used_2lev = 0;
  bpred->jr_hits = 0;
  bpred->jr_seen = 0;
  bpred->misses = 0;
  bpred->retstack_pops = 0;
  bpred->retstack_pushes = 0;
  bpred->ras_hits = 0;
}

#define BIMOD_HASH(PRED, ADDR)						\
  ((((ADDR) >> 19) ^ ((ADDR) >> MD_BR_SHIFT)) & ((PRED)->config.bimod.size-1))
    /* was: ((baddr >> 16) ^ baddr) & (pred->dirpred.bimod.size-1) */

/* predicts a branch direction */
char *						/* pointer to counter */
bpred_dir_lookup(struct bpred_dir_t *pred_dir,	/* branch dir predictor inst */
		 md_addr_t baddr)		/* branch address */
{
  unsigned char *p = NULL;

  /* Except for jumps, get a pointer to direction-prediction bits */
  switch (pred_dir->class) {
    case BPred2Level:
      {
	int l1index, l2index;

        /* traverse 2-level tables */
        l1index = (baddr >> MD_BR_SHIFT) & (pred_dir->config.two.l1size - 1);
        l2index = pred_dir->config.two.shiftregs[l1index];
        if (pred_dir->config.two.xor)
	  {
#if 1
	    /* this L2 index computation is more "compatible" to McFarling's
	       verison of it, i.e., if the PC xor address component is only
	       part of the index, take the lower order address bits for the
	       other part of the index, rather than the higher order ones */
	    l2index = (((l2index ^ (baddr >> MD_BR_SHIFT))
			& ((1 << pred_dir->config.two.shift_width) - 1))
		       | ((baddr >> MD_BR_SHIFT)
			  << pred_dir->config.two.shift_width));
#else
	    l2index = l2index ^ (baddr >> MD_BR_SHIFT);
#endif
	  }
	else
	  {
	    l2index =
	      l2index
		| ((baddr >> MD_BR_SHIFT) << pred_dir->config.two.shift_width);
	  }
        l2index = l2index & (pred_dir->config.two.l2size - 1);

        /* get a pointer to prediction state information */
        p = &pred_dir->config.two.l2table[l2index];
      }
      break;
    case BPred2bit:
      p = &pred_dir->config.bimod.table[BIMOD_HASH(pred_dir, baddr)];
      break;
    case BPredTaken:
    case BPredNotTaken:
      break;
    default:
      panic("bogus branch direction predictor class");
    }

  return (char *)p;
}

/* probe a predictor for a next fetch address, the predictor is probed
   with branch address BADDR, the branch target is BTARGET (used for
   static predictors), and OP is the instruction opcode (used to simulate
   predecode bits; a pointer to the predictor state entry (or null for jumps)
   is returned in *DIR_UPDATE_PTR (used for updating predictor state),
   and the non-speculative top-of-stack is returned in stack_recover_idx 
   (used for recovering ret-addr stack after mis-predict).  */
md_addr_t				/* predicted branch target addr */
bpred_lookup(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* branch target if taken */
	     enum md_opcode op,		/* opcode of instruction */
	     int is_call,		/* non-zero if inst is fn call */
	     int is_return,		/* non-zero if inst is fn return */
	     struct bpred_update_t *dir_update_ptr, /* pred state pointer */
	     int *stack_recover_idx)	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
{
  struct bpred_btb_ent_t *pbtb = NULL;
  int index, i;

  if (!dir_update_ptr)
    panic("no bpred update record");

  /* if this is not a branch, return not-taken */
  if (!(MD_OP_FLAGS(op) & F_CTRL))
    return 0;

  pred->lookups++;

  dir_update_ptr->dir.ras = FALSE;
  dir_update_ptr->pdir1 = NULL;
  dir_update_ptr->pdir2 = NULL;
  dir_update_ptr->pmeta = NULL;
  /* Except for jumps, get a pointer to direction-prediction bits */
  switch (pred->class) {
    case BPredComb:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  char *bimod, *twolev, *meta;
	  bimod = bpred_dir_lookup (pred->dirpred.bimod, baddr);
	  twolev = bpred_dir_lookup (pred->dirpred.twolev, baddr);
	  meta = bpred_dir_lookup (pred->dirpred.meta, baddr);
	  dir_update_ptr->pmeta = meta;
	  dir_update_ptr->dir.meta  = (*meta >= 2);
	  dir_update_ptr->dir.bimod = (*bimod >= 2);
	  dir_update_ptr->dir.twolev  = (*twolev >= 2);
	  if (*meta >= 2)
	    {
	      dir_update_ptr->pdir1 = twolev;
	      dir_update_ptr->pdir2 = bimod;
	    }
	  else
	    {
	      dir_update_ptr->pdir1 = bimod;
	      dir_update_ptr->pdir2 = twolev;
	    }
	}
      break;
    case BPred2Level:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  dir_update_ptr->pdir1 =
	    bpred_dir_lookup (pred->dirpred.twolev, baddr);
	}
      break;
    case BPred2bit:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  dir_update_ptr->pdir1 =
	    bpred_dir_lookup (pred->dirpred.bimod, baddr);
	}
      break;
    case BPredTaken:
      return btarget;
    case BPredNotTaken:
      if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND))
	{
	  return baddr + sizeof(md_inst_t);
	}
      else
	{
	  return btarget;
	}
    default:
      panic("bogus predictor class");
  }

  /*
   * We have a stateful predictor, and have gotten a pointer into the
   * direction predictor (except for jumps, for which the ptr is null)
   */

  /* record pre-pop TOS; if this branch is executed speculatively
   * and is squashed, we'll restore the TOS and hope the data
   * wasn't corrupted in the meantime. */
  if (pred->retstack.size)
    *stack_recover_idx = pred->retstack.tos;
  else
    *stack_recover_idx = 0;

  /* if this is a return, pop return-address stack */
  if (is_return && pred->retstack.size)
    {
      md_addr_t target = pred->retstack.stack[pred->retstack.tos].target;
      pred->retstack.tos = (pred->retstack.tos + pred->retstack.size - 1)
	                   % pred->retstack.size;
      pred->retstack_pops++;
      dir_update_ptr->dir.ras = TRUE; /* using RAS here */
      return target;
    }

#ifndef RAS_BUG_COMPATIBLE
  /* if function call, push return-address onto return-address stack */
  if (is_call && pred->retstack.size)
    {
      pred->retstack.tos = (pred->retstack.tos + 1)% pred->retstack.size;
      pred->retstack.stack[pred->retstack.tos].target = 
	baddr + sizeof(md_inst_t);
      pred->retstack_pushes++;
    }
#endif /* !RAS_BUG_COMPATIBLE */
  
  /* not a return. Get a pointer into the BTB */
  index = (baddr >> MD_BR_SHIFT) & (pred->btb.sets - 1);

  if (pred->btb.assoc > 1)
    {
      index *= pred->btb.assoc;

      /* Now we know the set; look for a PC match */
      for (i = index; i < (index+pred->btb.assoc) ; i++)
	if (pred->btb.btb_data[i].addr == baddr)
	  {
	    /* match */
	    pbtb = &pred->btb.btb_data[i];
	    break;
	  }
    }	
  else
    {
      pbtb = &pred->btb.btb_data[index];
      if (pbtb->addr != baddr)
	pbtb = NULL;
    }

  /*
   * We now also have a pointer into the BTB for a hit, or NULL otherwise
   */

  /* if this is a jump, ignore predicted direction; we know it's taken. */
  if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) == (F_CTRL|F_UNCOND))
    {
      return (pbtb ? pbtb->target : 1);
    }

  /* otherwise we have a conditional branch */
  if (pbtb == NULL)
    {
      /* BTB miss -- just return a predicted direction */
      return ((*(dir_update_ptr->pdir1) >= 2)
	      ? /* taken */ 1
	      : /* not taken */ 0);
    }
  else
    {
      /* BTB hit, so return target if it's a predicted-taken branch */
      return ((*(dir_update_ptr->pdir1) >= 2)
	      ? /* taken */ pbtb->target
	      : /* not taken */ 0);
    }
}

/* Speculative execution can corrupt the ret-addr stack.  So for each
 * lookup we return the top-of-stack (TOS) at that point; a mispredicted
 * branch, as part of its recovery, restores the TOS using this value --
 * hopefully this uncorrupts the stack. */
void
bpred_recover(struct bpred_t *pred,	/* branch predictor instance */
	      md_addr_t baddr,		/* branch address */
	      int stack_recover_idx)	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */
{
  if (pred == NULL)
    return;

  pred->retstack.tos = stack_recover_idx;
}

/* update the branch predictor, only useful for stateful predictors; updates
   entry for instruction type OP at address BADDR.  BTB only gets updated
   for branches which are taken.  Inst was determined to jump to
   address BTARGET and was taken if TAKEN is non-zero.  Predictor 
   statistics are updated with result of prediction, indicated by CORRECT and 
   PRED_TAKEN, predictor state to be updated is indicated by *DIR_UPDATE_PTR 
   (may be NULL for jumps, which shouldn't modify state bits).  Note if
   bpred_update is done speculatively, branch-prediction may get polluted. */
void
bpred_update(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* resolved branch target */
	     int taken,			/* non-zero if branch was taken */
	     int pred_taken,		/* non-zero if branch was pred taken */
	     int correct,		/* was earlier addr prediction ok? */
	     enum md_opcode op,		/* opcode of instruction */
	     struct bpred_update_t *dir_update_ptr)/* pred state pointer */
{
  struct bpred_btb_ent_t *pbtb = NULL;
  struct bpred_btb_ent_t *lruhead = NULL, *lruitem = NULL;
  int index, i;

  /* don't change bpred state for non-branch instructions or if this
   * is a stateless predictor*/
  if (!(MD_OP_FLAGS(op) & F_CTRL))
    return;

  /* Have a branch here */

  if (correct)
    pred->addr_hits++;

  if (!!pred_taken == !!taken)
    pred->dir_hits++;
  else
    pred->misses++;

  if (dir_update_ptr->dir.ras)
    {
      pred->used_ras++;
      if (correct)
	pred->ras_hits++;
    }
  else if ((MD_OP_FLAGS(op) & (F_CTRL|F_COND)) == (F_CTRL|F_COND))
    {
      if (dir_update_ptr->dir.meta)
	pred->used_2lev++;
      else
	pred->used_bimod++;
    }

  /* keep stats about JR's; also, but don't change any bpred state for JR's
   * which are returns unless there's no retstack */
  if (MD_IS_INDIR(op))
    {
      pred->jr_seen++;
      if (correct)
	pred->jr_hits++;
      
      if (!dir_update_ptr->dir.ras)
	{
	  pred->jr_non_ras_seen++;
	  if (correct)
	    pred->jr_non_ras_hits++;
	}
      else
	{
	  /* return that used the ret-addr stack; no further work to do */
	  return;
	}
    }

  /* Can exit now if this is a stateless predictor */
  if (pred->class == BPredNotTaken || pred->class == BPredTaken)
    return;

  /* 
   * Now we know the branch didn't use the ret-addr stack, and that this
   * is a stateful predictor 
   */

#ifdef RAS_BUG_COMPATIBLE
  /* if function call, push return-address onto return-address stack */
  if (MD_IS_CALL(op) && pred->retstack.size)
    {
      pred->retstack.tos = (pred->retstack.tos + 1)% pred->retstack.size;
      pred->retstack.stack[pred->retstack.tos].target = 
	baddr + sizeof(md_inst_t);
      pred->retstack_pushes++;
    }
#endif /* RAS_BUG_COMPATIBLE */

  /* update L1 table if appropriate */
  /* L1 table is updated unconditionally for combining predictor too */
  if ((MD_OP_FLAGS(op) & (F_CTRL|F_UNCOND)) != (F_CTRL|F_UNCOND) &&
      (pred->class == BPred2Level || pred->class == BPredComb))
    {
      int l1index, shift_reg;
      
      /* also update appropriate L1 history register */
      l1index =
	(baddr >> MD_BR_SHIFT) & (pred->dirpred.twolev->config.two.l1size - 1);
      shift_reg =
	(pred->dirpred.twolev->config.two.shiftregs[l1index] << 1) | (!!taken);
      pred->dirpred.twolev->config.two.shiftregs[l1index] =
	shift_reg & ((1 << pred->dirpred.twolev->config.two.shift_width) - 1);
    }

  /* find BTB entry if it's a taken branch (don't allocate for non-taken) */
  if (taken)
    {
      index = (baddr >> MD_BR_SHIFT) & (pred->btb.sets - 1);
      
      if (pred->btb.assoc > 1)
	{
	  index *= pred->btb.assoc;
	  
	  /* Now we know the set; look for a PC match; also identify
	   * MRU and LRU items */
	  for (i = index; i < (index+pred->btb.assoc) ; i++)
	    {
	      if (pred->btb.btb_data[i].addr == baddr)
		{
		  /* match */
		  assert(!pbtb);
		  pbtb = &pred->btb.btb_data[i];
		}
	      
	      dassert(pred->btb.btb_data[i].prev 
		      != pred->btb.btb_data[i].next);
	      if (pred->btb.btb_data[i].prev == NULL)
		{
		  /* this is the head of the lru list, ie current MRU item */
		  dassert(lruhead == NULL);
		  lruhead = &pred->btb.btb_data[i];
		}
	      if (pred->btb.btb_data[i].next == NULL)
		{
		  /* this is the tail of the lru list, ie the LRU item */
		  dassert(lruitem == NULL);
		  lruitem = &pred->btb.btb_data[i];
		}
	    }
	  dassert(lruhead && lruitem);
	  
	  if (!pbtb)
	    /* missed in BTB; choose the LRU item in this set as the victim */
	    pbtb = lruitem;	
	  /* else hit, and pbtb points to matching BTB entry */
	  
	  /* Update LRU state: selected item, whether selected because it
	   * matched or because it was LRU and selected as a victim, becomes 
	   * MRU */
	  if (pbtb != lruhead)
	    {
	      /* this splices out the matched entry... */
	      if (pbtb->prev)
		pbtb->prev->next = pbtb->next;
	      if (pbtb->next)
		pbtb->next->prev = pbtb->prev;
	      /* ...and this puts the matched entry at the head of the list */
	      pbtb->next = lruhead;
	      pbtb->prev = NULL;
	      lruhead->prev = pbtb;
	      dassert(pbtb->prev || pbtb->next);
	      dassert(pbtb->prev != pbtb->next);
	    }
	  /* else pbtb is already MRU item; do nothing */
	}
      else
	pbtb = &pred->btb.btb_data[index];
    }
      
  /* 
   * Now 'p' is a possibly null pointer into the direction prediction table, 
   * and 'pbtb' is a possibly null pointer into the BTB (either to a 
   * matched-on entry or a victim which was LRU in its set)
   */

  /* update state (but not for jumps) */
  if (dir_update_ptr->pdir1)
    {
      if (taken)
	{
	  if (*dir_update_ptr->pdir1 < 3)
	    ++*dir_update_ptr->pdir1;
	}
      else
	{ /* not taken */
	  if (*dir_update_ptr->pdir1 > 0)
	    --*dir_update_ptr->pdir1;
	}
    }

  /* combining predictor also updates second predictor and meta predictor */
  /* second direction predictor */
  if (dir_update_ptr->pdir2)
    {
      if (taken)
	{
	  if (*dir_update_ptr->pdir2 < 3)
	    ++*dir_update_ptr->pdir2;
	}
      else
	{ /* not taken */
	  if (*dir_update_ptr->pdir2 > 0)
	    --*dir_update_ptr->pdir2;
	}
    }

  /* meta predictor */
  if (dir_update_ptr->pmeta)
    {
      if (dir_update_ptr->dir.bimod != dir_update_ptr->dir.twolev)
	{
	  /* we only update meta predictor if directions were different */
	  if (dir_update_ptr->dir.twolev == (unsigned int)taken)
	    {
	      /* 2-level predictor was correct */
	      if (*dir_update_ptr->pmeta < 3)
		++*dir_update_ptr->pmeta;
	    }
	  else
	    {
	      /* bimodal predictor was correct */
	      if (*dir_update_ptr->pmeta > 0)
		--*dir_update_ptr->pmeta;
	    }
	}
    }

  /* update BTB (but only for taken branches) */
  if (pbtb)
    {
      /* update current information */
      dassert(taken);

      if (pbtb->addr == baddr)
	{
	  if (!correct)
	    pbtb->target = btarget;
	}
      else
	{
	  /* enter a new branch in the table */
	  pbtb->addr = baddr;
	  pbtb->op = op;
	  pbtb->target = btarget;
	}
    }
}
