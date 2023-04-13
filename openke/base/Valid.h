#ifndef Valid_H
#define Valid_H
#include "Setting.h"
#include "Reader.h"
#include "Corrupt.h"

/*=====================================================================================
link prediction
======================================================================================*/
namespace Valid{
INT lastHead = 0;
INT lastTail = 0;
INT lastRel = 0;
REAL l1_filter_tot = 0, l1_tot = 0, r1_tot = 0, r1_filter_tot = 0, l_tot = 0, r_tot = 0, l_filter_rank = 0, l_rank = 0, l_filter_reci_rank = 0, l_reci_rank = 0;
REAL l3_filter_tot = 0, l3_tot = 0, r3_tot = 0, r3_filter_tot = 0, l_filter_tot = 0, r_filter_tot = 0, r_filter_rank = 0, r_rank = 0, r_filter_reci_rank = 0, r_reci_rank = 0;
REAL rel3_tot = 0, rel3_filter_tot = 0, rel_filter_tot = 0, rel_filter_rank = 0, rel_rank = 0, rel_filter_reci_rank = 0, rel_reci_rank = 0, rel_tot = 0, rel1_tot = 0, rel1_filter_tot = 0;

REAL l1_filter_tot_constrain = 0, l1_tot_constrain = 0, r1_tot_constrain = 0, r1_filter_tot_constrain = 0, l_tot_constrain = 0, r_tot_constrain = 0, l_filter_rank_constrain = 0, l_rank_constrain = 0, l_filter_reci_rank_constrain = 0, l_reci_rank_constrain = 0;
REAL l3_filter_tot_constrain = 0, l3_tot_constrain = 0, r3_tot_constrain = 0, r3_filter_tot_constrain = 0, l_filter_tot_constrain = 0, r_filter_tot_constrain = 0, r_filter_rank_constrain = 0, r_rank_constrain = 0, r_filter_reci_rank_constrain = 0, r_reci_rank_constrain = 0;
REAL hit1, hit3, hit10, mr, mrr;
REAL hit1TC, hit3TC, hit10TC, mrTC, mrrTC;

extern "C"
void initValid() {
    lastHead = 0;
    lastTail = 0;
    lastRel = 0;
    l1_filter_tot = 0, l1_tot = 0, r1_tot = 0, r1_filter_tot = 0, l_tot = 0, r_tot = 0, l_filter_rank = 0, l_rank = 0, l_filter_reci_rank = 0, l_reci_rank = 0;
    l3_filter_tot = 0, l3_tot = 0, r3_tot = 0, r3_filter_tot = 0, l_filter_tot = 0, r_filter_tot = 0, r_filter_rank = 0, r_rank = 0, r_filter_reci_rank = 0, r_reci_rank = 0;
    REAL rel3_tot = 0, rel3_filter_tot = 0, rel_filter_tot = 0, rel_filter_rank = 0, rel_rank = 0, rel_filter_reci_rank = 0, rel_reci_rank = 0, rel_tot = 0, rel1_tot = 0, rel1_filter_tot = 0;

    l1_filter_tot_constrain = 0, l1_tot_constrain = 0, r1_tot_constrain = 0, r1_filter_tot_constrain = 0, l_tot_constrain = 0, r_tot_constrain = 0, l_filter_rank_constrain = 0, l_rank_constrain = 0, l_filter_reci_rank_constrain = 0, l_reci_rank_constrain = 0;
    l3_filter_tot_constrain = 0, l3_tot_constrain = 0, r3_tot_constrain = 0, r3_filter_tot_constrain = 0, l_filter_tot_constrain = 0, r_filter_tot_constrain = 0, r_filter_rank_constrain = 0, r_rank_constrain = 0, r_filter_reci_rank_constrain = 0, r_reci_rank_constrain = 0;
}

extern "C"
void getHeadBatchValid(INT *ph, INT *pt, INT *pr) {
    for (INT i = 0; i < entityTotal; i++) {
        ph[i] = i;
        pt[i] = validList[lastHead].t;
        pr[i] = validList[lastHead].r;
    }
    lastHead++;
}

extern "C"
void getTailBatchValid(INT *ph, INT *pt, INT *pr) {
    for (INT i = 0; i < entityTotal; i++) {
        ph[i] = validList[lastTail].h;
        pt[i] = i;
        pr[i] = validList[lastTail].r;
    }
    lastTail++;
}

extern "C"
void getRelBatchValid(INT *ph, INT *pt, INT *pr) {
    for (INT i = 0; i < relationTotal; i++) {
        ph[i] = validList[lastRel].h;
        pt[i] = validList[lastRel].t;
        pr[i] = i;
    }
}

extern "C"
void validHead(REAL *con, INT lastHead, bool type_constrain = false) {
    INT h = validList[lastHead].h;
    INT t = validList[lastHead].t;
    INT r = validList[lastHead].r;
    INT lef, rig;
    if (type_constrain) {
        lef = head_lef[r];
        rig = head_rig[r];
    }
    REAL minimal = con[h];
    INT l_s = 0;
    INT l_filter_s = 0;
    INT l_s_constrain = 0;
    INT l_filter_s_constrain = 0;

    for (INT j = 0; j < entityTotal; j++) {
        if (j != h) {
            REAL value = con[j];
            if (value < minimal) {
                l_s += 1;
                if (not _find(j, t, r))
                    l_filter_s += 1;
            }
            if (type_constrain) {
                while (lef < rig && head_type[lef] < j) lef ++;
                if (lef < rig && j == head_type[lef]) {
                    if (value < minimal) {
                        l_s_constrain += 1;
                        if (not _find(j, t, r)) {
                            l_filter_s_constrain += 1;
                        }
                    }  
                }
            }
        }
    }

    if (l_filter_s < 10) l_filter_tot += 1;
    if (l_s < 10) l_tot += 1;
    if (l_filter_s < 3) l3_filter_tot += 1;
    if (l_s < 3) l3_tot += 1;
    if (l_filter_s < 1) l1_filter_tot += 1;
    if (l_s < 1) l1_tot += 1;

    l_filter_rank += (l_filter_s+1);
    l_rank += (1 + l_s);
    l_filter_reci_rank += 1.0/(l_filter_s+1);
    l_reci_rank += 1.0/(l_s+1);

    if (type_constrain) {
        if (l_filter_s_constrain < 10) l_filter_tot_constrain += 1;
        if (l_s_constrain < 10) l_tot_constrain += 1;
        if (l_filter_s_constrain < 3) l3_filter_tot_constrain += 1;
        if (l_s_constrain < 3) l3_tot_constrain += 1;
        if (l_filter_s_constrain < 1) l1_filter_tot_constrain += 1;
        if (l_s_constrain < 1) l1_tot_constrain += 1;

        l_filter_rank_constrain += (l_filter_s_constrain+1);
        l_rank_constrain += (1+l_s_constrain);
        l_filter_reci_rank_constrain += 1.0/(l_filter_s_constrain+1);
        l_reci_rank_constrain += 1.0/(l_s_constrain+1);
    }
}

extern "C"
void validTail(REAL *con, INT lastTail, bool type_constrain = false) {
    INT h = validList[lastTail].h;
    INT t = validList[lastTail].t;
    INT r = validList[lastTail].r;
    INT lef, rig;
    if (type_constrain) {
        lef = tail_lef[r];
        rig = tail_rig[r];
    }
    REAL minimal = con[t];
    INT r_s = 0;
    INT r_filter_s = 0;
    INT r_s_constrain = 0;
    INT r_filter_s_constrain = 0;
    for (INT j = 0; j < entityTotal; j++) {
        if (j != t) {
            REAL value = con[j];
            if (value < minimal) {
                r_s += 1;
                if (not _find(h, j, r))
                    r_filter_s += 1;
            }
            if (type_constrain) {
                while (lef < rig && tail_type[lef] < j) lef ++;
                if (lef < rig && j == tail_type[lef]) {
                        if (value < minimal) {
                            r_s_constrain += 1;
                            if (not _find(h, j ,r)) {
                                r_filter_s_constrain += 1;
                            }
                        }
                }
            }
        }
        
    }

    if (r_filter_s < 10) r_filter_tot += 1;
    if (r_s < 10) r_tot += 1;
    if (r_filter_s < 3) r3_filter_tot += 1;
    if (r_s < 3) r3_tot += 1;
    if (r_filter_s < 1) r1_filter_tot += 1;
    if (r_s < 1) r1_tot += 1;

    r_filter_rank += (1+r_filter_s);
    r_rank += (1+r_s);
    r_filter_reci_rank += 1.0/(1+r_filter_s);
    r_reci_rank += 1.0/(1+r_s);
    
    if (type_constrain) {
        if (r_filter_s_constrain < 10) r_filter_tot_constrain += 1;
        if (r_s_constrain < 10) r_tot_constrain += 1;
        if (r_filter_s_constrain < 3) r3_filter_tot_constrain += 1;
        if (r_s_constrain < 3) r3_tot_constrain += 1;
        if (r_filter_s_constrain < 1) r1_filter_tot_constrain += 1;
        if (r_s_constrain < 1) r1_tot_constrain += 1;

        r_filter_rank_constrain += (1+r_filter_s_constrain);
        r_rank_constrain += (1+r_s_constrain);
        r_filter_reci_rank_constrain += 1.0/(1+r_filter_s_constrain);
        r_reci_rank_constrain += 1.0/(1+r_s_constrain);
    }
}

extern "C"
void validRel(REAL *con) {
    INT h = validList[lastRel].h;
    INT t = validList[lastRel].t;
    INT r = validList[lastRel].r;

    REAL minimal = con[r];
    INT rel_s = 0;
    INT rel_filter_s = 0;

    for (INT j = 0; j < relationTotal; j++) {
        if (j != r) {
            REAL value = con[j];
            if (value < minimal) {
                rel_s += 1;
                if (not _find(h, t, j))
                    rel_filter_s += 1;
            }
        }
    }

    if (rel_filter_s < 10) rel_filter_tot += 1;
    if (rel_s < 10) rel_tot += 1;
    if (rel_filter_s < 3) rel3_filter_tot += 1;
    if (rel_s < 3) rel3_tot += 1;
    if (rel_filter_s < 1) rel1_filter_tot += 1;
    if (rel_s < 1) rel1_tot += 1;

    rel_filter_rank += (rel_filter_s+1);
    rel_rank += (1+rel_s);
    rel_filter_reci_rank += 1.0/(rel_filter_s+1);
    rel_reci_rank += 1.0/(rel_s+1);

    lastRel++;
}


extern "C"
void valid_link_prediction(bool type_constrain = false) {
    l_rank /= validTotal;
    r_rank /= validTotal;
    l_reci_rank /= validTotal;
    r_reci_rank /= validTotal;
 
    l_tot /= validTotal;
    l3_tot /= validTotal;
    l1_tot /= validTotal;
 
    r_tot /= validTotal;
    r3_tot /= validTotal;
    r1_tot /= validTotal;

    // with filter
    l_filter_rank /= validTotal;
    r_filter_rank /= validTotal;
    l_filter_reci_rank /= validTotal;
    r_filter_reci_rank /= validTotal;
 
    l_filter_tot /= validTotal;
    l3_filter_tot /= validTotal;
    l1_filter_tot /= validTotal;
 
    r_filter_tot /= validTotal;
    r3_filter_tot /= validTotal;
    r1_filter_tot /= validTotal;

    printf("no type constraint results:\n");
    
    printf("metric:\t\t\t MRR \t\t MR \t\t hit@10 \t hit@3  \t hit@1 \n");
    printf("l(raw):\t\t\t %f \t %f \t %f \t %f \t %f \n", l_reci_rank, l_rank, l_tot, l3_tot, l1_tot);
    printf("r(raw):\t\t\t %f \t %f \t %f \t %f \t %f \n", r_reci_rank, r_rank, r_tot, r3_tot, r1_tot);
    printf("averaged(raw):\t\t %f \t %f \t %f \t %f \t %f \n",
            (l_reci_rank+r_reci_rank)/2, (l_rank+r_rank)/2, (l_tot+r_tot)/2, (l3_tot+r3_tot)/2, (l1_tot+r1_tot)/2);
    printf("\n");
    printf("l(filter):\t\t %f \t %f \t %f \t %f \t %f \n", l_filter_reci_rank, l_filter_rank, l_filter_tot, l3_filter_tot, l1_filter_tot);
    printf("r(filter):\t\t %f \t %f \t %f \t %f \t %f \n", r_filter_reci_rank, r_filter_rank, r_filter_tot, r3_filter_tot, r1_filter_tot);
    printf("averaged(filter):\t %f \t %f \t %f \t %f \t %f \n",
            (l_filter_reci_rank+r_filter_reci_rank)/2, (l_filter_rank+r_filter_rank)/2, (l_filter_tot+r_filter_tot)/2, (l3_filter_tot+r3_filter_tot)/2, (l1_filter_tot+r1_filter_tot)/2);

    mrr = (l_filter_reci_rank+r_filter_reci_rank) / 2;
    mr = (l_filter_rank+r_filter_rank) / 2;
    hit10 = (l_filter_tot+r_filter_tot) / 2;
    hit3 = (l3_filter_tot+r3_filter_tot) / 2;
    hit1 = (l1_filter_tot+r1_filter_tot) / 2;

    if (type_constrain) {
        //type constrain
        l_rank_constrain /= validTotal;
        r_rank_constrain /= validTotal;
        l_reci_rank_constrain /= validTotal;
        r_reci_rank_constrain /= validTotal;
     
        l_tot_constrain /= validTotal;
        l3_tot_constrain /= validTotal;
        l1_tot_constrain /= validTotal;
     
        r_tot_constrain /= validTotal;
        r3_tot_constrain /= validTotal;
        r1_tot_constrain /= validTotal;

        // with filter
        l_filter_rank_constrain /= validTotal;
        r_filter_rank_constrain /= validTotal;
        l_filter_reci_rank_constrain /= validTotal;
        r_filter_reci_rank_constrain /= validTotal;
     
        l_filter_tot_constrain /= validTotal;
        l3_filter_tot_constrain /= validTotal;
        l1_filter_tot_constrain /= validTotal;
     
        r_filter_tot_constrain /= validTotal;
        r3_filter_tot_constrain /= validTotal;
        r1_filter_tot_constrain /= validTotal;

        printf("type constraint results:\n");
        
        printf("metric:\t\t\t MRR \t\t MR \t\t hit@10 \t hit@3  \t hit@1 \n");
        printf("l(raw):\t\t\t %f \t %f \t %f \t %f \t %f \n", l_reci_rank_constrain, l_rank_constrain, l_tot_constrain, l3_tot_constrain, l1_tot_constrain);
        printf("r(raw):\t\t\t %f \t %f \t %f \t %f \t %f \n", r_reci_rank_constrain, r_rank_constrain, r_tot_constrain, r3_tot_constrain, r1_tot_constrain);
        printf("averaged(raw):\t\t %f \t %f \t %f \t %f \t %f \n",
                (l_reci_rank_constrain+r_reci_rank_constrain)/2, (l_rank_constrain+r_rank_constrain)/2, (l_tot_constrain+r_tot_constrain)/2, (l3_tot_constrain+r3_tot_constrain)/2, (l1_tot_constrain+r1_tot_constrain)/2);
        printf("\n");
        printf("l(filter):\t\t %f \t %f \t %f \t %f \t %f \n", l_filter_reci_rank_constrain, l_filter_rank_constrain, l_filter_tot_constrain, l3_filter_tot_constrain, l1_filter_tot_constrain);
        printf("r(filter):\t\t %f \t %f \t %f \t %f \t %f \n", r_filter_reci_rank_constrain, r_filter_rank_constrain, r_filter_tot_constrain, r3_filter_tot_constrain, r1_filter_tot_constrain);
        printf("averaged(filter):\t %f \t %f \t %f \t %f \t %f \n",
                (l_filter_reci_rank_constrain+r_filter_reci_rank_constrain)/2, (l_filter_rank_constrain+r_filter_rank_constrain)/2, (l_filter_tot_constrain+r_filter_tot_constrain)/2, (l3_filter_tot_constrain+r3_filter_tot_constrain)/2, (l1_filter_tot_constrain+r1_filter_tot_constrain)/2);

        mrrTC = (l_filter_reci_rank_constrain+r_filter_reci_rank_constrain)/2;
        mrTC = (l_filter_rank_constrain+r_filter_rank_constrain) / 2;
        hit10TC = (l_filter_tot_constrain+r_filter_tot_constrain) / 2;
        hit3TC = (l3_filter_tot_constrain+r3_filter_tot_constrain) / 2;
        hit1TC = (l1_filter_tot_constrain+r1_filter_tot_constrain) / 2;
    }
}

extern "C"
void valid_relation_prediction() {
    rel_rank /= validTotal;
    rel_reci_rank /= validTotal;
  
    rel_tot /= validTotal;
    rel3_tot /= validTotal;
    rel1_tot /= validTotal;

    // with filter
    rel_filter_rank /= validTotal;
    rel_filter_reci_rank /= validTotal;
  
    rel_filter_tot /= validTotal;
    rel3_filter_tot /= validTotal;
    rel1_filter_tot /= validTotal;

    printf("no type constraint results:\n");
    
    printf("metric:\t\t\t MRR \t\t MR \t\t hit@10 \t hit@3  \t hit@1 \n");
    printf("averaged(raw):\t\t %f \t %f \t %f \t %f \t %f \n",
            rel_reci_rank, rel_rank, rel_tot, rel3_tot, rel1_tot);
    printf("\n");
    printf("averaged(filter):\t %f \t %f \t %f \t %f \t %f \n",
            rel_filter_reci_rank, rel_filter_rank, rel_filter_tot, rel3_filter_tot, rel1_filter_tot);
}

extern "C"
REAL getValidLinkHit10(bool type_constrain = false) {
    if (type_constrain)
        return hit10TC;
    printf("%f\n", hit10);
    return hit10;
}

extern "C"
REAL  getValidLinkHit3(bool type_constrain = false) {
    if (type_constrain)
        return hit3TC;
    return hit3;
}

extern "C"
REAL  getValidLinkHit1(bool type_constrain = false) {
    if (type_constrain)
        return hit1TC;    
    return hit1;
}

extern "C"
REAL  getValidLinkMR(bool type_constrain = false) {
    if (type_constrain)
        return mrTC;
    return mr;
}

extern "C"
REAL  getValidLinkMRR(bool type_constrain = false) {
    if (type_constrain)
        return mrrTC;    
    return mrr;
}


/*=====================================================================================
triple classification
======================================================================================*/
Triple *negValidList = NULL;

extern "C"
void getNegValid() {
    if (negValidList == NULL)
        negValidList = (Triple *)calloc(validTotal, sizeof(Triple));
    for (INT i = 0; i < validTotal; i++) {
        negValidList[i] = validList[i];
        if (randd(0) % 1000 < 500)
            negValidList[i].t = corrupt_head(0, validList[i].h, validList[i].r);
        else
            negValidList[i].h = corrupt_tail(0, validList[i].t, validList[i].r);
    }
}

extern "C"
void getValidBatch(INT *ph, INT *pt, INT *pr, INT *nh, INT *nt, INT *nr) {
    getNegValid();
    for (INT i = 0; i < validTotal; i++) {
        ph[i] = validList[i].h;
        pt[i] = validList[i].t;
        pr[i] = validList[i].r;
        nh[i] = negValidList[i].h;
        nt[i] = negValidList[i].t;
        nr[i] = negValidList[i].r;
    }
}
}
#endif
