/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/


#include "Model.h"
#include <values.h>
#include "Cell.h"
#include "Patch.h"
#include "Prey.h"

#define MAXMEATAGE 4

#define UNKNOWN_AGENT (-DBL_MAX)

#ifdef VERBOSEDEBUG
double MinGlobalRecallMem = -2.0;
double MaxGlobalRecallMem = 2.0;
double MinGlobalStoreMem = -2.0;
double MaxGlobalStoreMem = 2.0;
#endif

extern Model *theModel;

void addAgName(char nm[8], Agent *ag);
void removeAgName(char nm[8]);
int comparefriends(const void *a, const void *b);
double recall(char *aName, Remembrance *memory, double tf,
        double vengefulness, double gratitude, int gratStrategy, int vengStrategy, int vengStr2);
double getDoubleValue(double value, double less, double more, double min,
        double max);
inline int nameComp(char a[8], char b[8]);
inline Agent* getAgPointer(char nm[8]);
void printMemory(Remembrance *m);
void copyMigPrefs(char c[4], char p[4]);

// This macro will avoid calling nameComp in 98% of comparisons:
#define STRCMP(x, y) ((x[0] == y[0]) ? nameComp(x, y) : 1)

AgNames *Sentinel;
int Nm;
double Rv;
int Nf, Ne, Nfi, Nei;
double RMin, RMax, RT;
FILE *F; // followed agents.

inline int nameComp(char a[8], char b[8]){
    int i = 0;
    while(a[i] == b[i] && i < 7)
        i++;
    if(i == 7)
        return 0;
    if(a[i] > b[i])
        return 1;
    else
        return -1;
}

inline Agent* getAgPointer(char nm[8]){
    AgNames *tmp = Sentinel;
    int c;
    do{
        c = nameComp(nm, tmp->name);
        if(c == 0)
            return tmp->ag;
        if(c > 0)
            tmp = tmp->right;
        else
            tmp = tmp->left;
    } while(tmp != NULL);
    return NULL;
}

// Sentinel is the root of a linked list of all live agents and their respective
// addresses in the computer's memory. This function adds new agents to the list.
void addAgName(char nm[8], Agent *ag){
    AgNames *tmp = Sentinel;
    int b;
    while(1){
        b = nameComp(nm, tmp->name);
        if(b == 0)
            FATALERR("IMPOSSIBLE!");
        if(b > 0){
            if(tmp->right == NULL){
                tmp->right = (AgNames*)calloc(1, sizeof(AgNames));
                tmp = tmp->right;
                break;
            } else{
                tmp = tmp->right;
            }
        } else{
            if(tmp->left == NULL){
                tmp->left = (AgNames*)calloc(1, sizeof(AgNames));
                tmp = tmp->left;
                break;
            } else{
                tmp = tmp->left;
            }
        }
    }
    tmp->ag = ag;
    NAMECOPY(tmp->name, nm);
    AgNamesCount++;
}

void removeAgName(char nm[8]){
    AgNames *node = Sentinel;
    AgNames *parent = NULL, *temp, *child;
    int b;
    while(1){
        if(node == NULL)
            FATALERR("IMPOSSIBLE!");
        b = nameComp(nm, node->name);
        if(b > 0){
            parent = node;
            node = node->right;
        } else
            if(b < 0){
                parent = node;
                node = node->left;
            } else{
                if(node->right == NULL){
                    if(parent->left == node)
                        parent->left = node->left;
                    else
                        parent->right = node->left;
                } else
                    if (node->left == NULL){
                        if(parent->left == node)
                            parent->left = node->right;
                        else
                            parent->right = node->right;
                    } else {
                        temp = node;
                        child = node->left;
                        while(child->right != NULL){
                            temp = child;
                            child = child->right;
                        }
                        if(child != node->left){
                            temp->right = child->left;
                            child->left = node->left;
                        }
                        child->right = node->right;
                        if(parent->left == node)
                            parent->left = child;
                        else
                            parent->right = child;
                    }

                free(node);
                AgNamesCount--;
                return;
            }
    }
}

typedef struct Frnd{
    Agent *a;
    double value;
} Friend;

int comparefriends(const void *a, const void *b){
    const Friend *c = (const Friend*)a;
    const Friend *d = (const Friend*)b;
    if(c->value == d->value)
        return 0;
    if(c->value > d->value)
        return -1;
    else
        return 1;
}

void copyMigPrefs(char c[4], char p[4]){
    c[0] = p[0];
    c[1] = p[1];
    c[2] = p[2];
    c[3] = p[3];
}

// Recall past actions of other agent and evaluate them as either positive or
// negative remembrances.
// It was easier to put this function outside the Agent class because the
// agents must sometimes think as they were others.
double recall(char *aName, Remembrance *memory, double tf,
        double vengefulness, double gratitude, int gratStrategy, int vengStrategy, int vengStr2){
    if(memory == NULL)
        return UNKNOWN_AGENT;
    Remembrance *tmp;
    tmp = memory;
    int c;
    do{
        c = nameComp(aName, tmp->name);
        if(c > 0)
            tmp = tmp->right;
        else
            if(c < 0)
                tmp = tmp->left;
            else
                break;
    } while(tmp);
    if(tmp == NULL)
        return UNKNOWN_AGENT;

#ifdef DEBUG
    if(STRCMP(tmp->name, aName) != 0)
        FATALERR("IMPOSSIBLE!");
#endif

    double vg, vr;   // each value given and received
    double VG = 0.0; // total value given
    double VR = 0.0; // total value received
    double lg = 0.0; // last value given
    double lr = 0.0; // last value received
    int rt = 0, gt = 0, tm; // last time when received/given
    int i;
    for(i = 0; i < MemSize; i++){ // old actions has less value
        if(tmp->givenTm[i] > 0){
            tm = SimTime - tmp->givenTm[i];
            vg = tmp->given[i] * pow(tf, tm);
            VG += vg;
            if(tmp->givenTm[i] > gt){
                gt = tmp->givenTm[i];
                lg = vg;
            }
        }
        if(tmp->receivedTm[i] > 0){
            tm = SimTime - tmp->receivedTm[i];
            vr = tmp->received[i] * pow(tf, tm);
            VR += vr;
            if(tmp->receivedTm[i] > rt){
                rt = tmp->receivedTm[i];
                lr = vr;
            }
        }
    }

    double result = 0.0;
    if((VR + VG) == 0.0)
        return 0.0;

    if(gt != 0 && rt != 0){
        if(vengStrategy == 2){ // Very vengeful strategies
            switch(vengStr2){
                case 0 :
                    if(VG > VR)
                        result = (-1) * vengefulness * (VG - VR);
                    else
                        if(lg > lr)
                            result = (-1) * vengefulness * (lg - lr);
                    break;
                case 1 :
                    if(lg > lr)
                        result = (-1) * vengefulness * (lg - lr);
                    else
                        if(VG > VR)
                            result = (-1) * vengefulness * (VG - VR);
                    break;
                default :
                    FATALERR("Impossible: %d", vengStr2);
            }
        } else { // Moderately vengeful strategies
            if(vengStrategy == 1 && lg > 0.0 && lr < 0.0){
                switch(vengStr2){
                    case 0 :
                        if(VG > VR)
                            result = (-1) * vengefulness * (VG - VR);
                        else
                            if(lg > lr)
                                result = (-1) * vengefulness * (lg - lr);
                        break;
                    case 1 :
                        if(lg > lr)
                            result = (-1) * vengefulness * (lg - lr);
                        else
                            if(VG > VR)
                                result = (-1) * vengefulness * (VG - VR);
                        break;
                    default :
                        FATALERR("Impossible: %d", vengStr2);
                }
            } else  // Very light vengeful strategies
                if(VR <= 0.0 && rt > gt && VG > 0.0 && lr <= 0.0){
                    switch(vengStr2){
                        case 0 :
                            if(VG > VR)
                                result = (-1) * vengefulness * (VG - VR);
                            else
                                if(lg > lr)
                                    result = (-1) * vengefulness * (lg - lr);
                            break;
                        case 1 :
                            if(lg > lr)
                                result = (-1) * vengefulness * (lg - lr);
                            else
                                if(VG > VR)
                                    result = (-1) * vengefulness * (VG - VR);
                            break;
                        default :
                            FATALERR("Impossible: %d", vengStr2);
                    }
                }
        }
    }

    // The remembrance is the result of the sum of vengefulness and gratitude
    if(VR > 0.0){
        switch(gratStrategy){
            case 0 :
                result += gratitude * VR;
                break;
            case 1 :
                if(VR > VG)
                    result += gratitude *  (VR - VG);
                break;
            default :
                FATALERR("Impossible: %d", gratStrategy);
        }
    }

#ifdef VERBOSEDEBUG
    if(result > MaxGlobalRecallMem){
        fprintf(stderr, "\nMaxGlobalRecallMem: %f (%f %f) %f %f (%f %f)",
                result, gratitude, vengefulness, VR, VG, lg, lr); fflush(stderr);
        MaxGlobalRecallMem = result;
    } else
        if(result < MinGlobalRecallMem){
            fprintf(stderr, "\nMinGlobalRecallMem: %f (%f %f) %f %f (%f %f)",
                    result, gratitude, vengefulness, VR, VG, lg, lr); fflush(stderr);
            MinGlobalRecallMem = result;
        }
#endif
#ifdef DEBUG
    if(result < -400.0 || result > 400.0 || isnan(result) || isinf(result)){
        fprintf(stderr, "recall: aName = %s, tf = %f, vengefulness = %f,\n"
                "gratitude = %f, gratStrategy = %d, vengStrategy = %d\nresult = %f\n",
                aName, tf, vengefulness, gratitude, gratStrategy, vengStrategy, result);
        FATALERR("Strange values");
    }
#endif
    return result;
}

double getDoubleValue(double value, double less, double more, double min,
        double max){
    double result;
    if(ChooseRandomValues)
        result = getRandDouble(min, max);
    else
        result = getRandDouble((value - less), (value + more));
    return result;
}

Remembrance::Remembrance()
{
    received = (double*)calloc(MemSize, sizeof(double));
    given = (double*)calloc(MemSize, sizeof(double));
    receivedTm = (int*)calloc(MemSize, sizeof(int));
    givenTm = (int*)calloc(MemSize, sizeof(int));
    receivedEv = (int*)calloc(MemSize, sizeof(int));
    givenEv = (int*)calloc(MemSize, sizeof(int));
    nInteractions = 0;
    left = NULL;
    right = NULL;
    next = NULL;
}

Remembrance::~Remembrance()
{
    free(received);
    free(given);
    free(receivedTm);
    free(givenTm);
    free(receivedEv);
    free(givenEv);
}


// This function is used to create the first population.
Agent::Agent(int xc, int yc)
{
    x = xc;
    y = yc;
    mother = NULL;
    father = NULL;

    int i;
    for(i = 0; i < 7; i++){
        motherName[i] = getRandInt('A', 'z');
        if(motherName[i] > 'Z' && motherName[i] < 'a')
            motherName[i] = motherName[i] - 42;
        fatherName[i] = getRandInt('A', 'z');
        if(fatherName[i] > 'Z' && fatherName[i] < 'a')
            fatherName[i] = fatherName[i] - 42;
    }
    motherName[7] = 0;
    fatherName[7] = 0;
    mGen = 0;
    fGen = 0;
    metabolism = 1.0;
    huntInterval = getRandInt(20, 40);
    int r = getRandInt(0, huntInterval);
    lastHunt = SimTime - r;
    mutationRate = getRandDouble(0.7, 0.9);
    energy = getRandDouble((3.0 * MaxEnergy / 4.0), (MaxEnergy - 2));
    f_childhood = (int)(MaxEnergy / metabolism); // SimTime enough to get 50% of MaxEnergy
    m_childhood = (int)(MaxEnergy / metabolism);
    m_childhood = (int)(round(m_childhood * getRandDouble(0.9, 1.1)));
    f_childhood = (int)(round(f_childhood * getRandDouble(0.9, 1.1)));
    age = getRandInt((int)((MaxEnergy / metabolism ) + 1), (MaxAge - 1));
    isKid = false;
    m_gratStrategy = getRandInt(0, 1);
    f_gratStrategy = getRandInt(0, 1);
    m_vengStrategy = getRandInt(0, 2);
    f_vengStrategy = getRandInt(0, 2);
    m_vengStr2 = getRandInt(0, 1);
    f_vengStr2 = getRandInt(0, 1);
    m_huntStrategy = 2;
    f_huntStrategy = 2;
    m_terriRemStrategy = getRandInt(0, 1);
    f_terriRemStrategy = getRandInt(0, 1);
    m_hasShame = getRandInt(0, 1);
    f_hasShame = getRandInt(0, 1);
    m_maxHuntPatrolSize = getRandInt(5, 9);
    f_maxHuntPatrolSize = getRandInt(5, 9);
    m_begStrategy = getRandInt(1, 4);
    f_begStrategy = getRandInt(1, 4);

    if(getRandDouble(0.0, 1.0) < initMNorm)
        m_norm = true;
    else
        m_norm = false;

    if(getRandDouble(0.0, 1.0) < initFNorm)
        f_norm = true;
    else
        f_norm = false;

    if(getRandDouble(0.0, 1.0) < initMMetanorm)
        m_metanorm = true;
    else
        m_metanorm = false;

    if(getRandDouble(0.0, 1.0) < initFMetanorm)
        f_metanorm = true;
    else
        f_metanorm = false;

    if(getRandDouble(0.0, 1.0) < initMXenophTM)
        m_xenophTM = true;
    else
        m_xenophTM = false;
    if(getRandDouble(0.0, 1.0) < initFXenophTM)
        f_xenophTM = true;
    else
        f_xenophTM = false;

    if(getRandDouble(0.0, 1.0) < initMXenophTF)
        m_xenophTF = true;
    else
        m_xenophTF = false;
    if(getRandDouble(0.0, 1.0) < initFXenophTF)
        f_xenophTF = true;
    else
        f_xenophTF = false;

    if(getRandDouble(0.0, 1.0) < initMXenophTFwK)
        m_xenophTFwK = true;
    else
        m_xenophTFwK = false;
    if(getRandDouble(0.0, 1.0) < initFXenophTFwK)
        f_xenophTFwK = true;
    else
        f_xenophTFwK = false;

    if(getRandDouble(0.0, 1.0) < 0.5)
        m_zeroPostvNo = true;
    else
        m_zeroPostvNo = false;
    if(getRandDouble(0.0, 1.0) < 0.5)
        f_zeroPostvNo = true;
    else
        f_zeroPostvNo = false;

    m_meatValue = getDoubleValue(initMeatValue, 0.1, 0.1, 0.0, 1.0);
    f_meatValue = getDoubleValue(initMeatValue, 0.1, 0.1, 0.0, 1.0);

    if(getRandDouble(0.0, 1.0) > 0.5)
        askMeatOnly = true;
    else
        askMeatOnly = false;
    oestrFemSel = getDoubleValue(initOestrFemSel, 0.1, 0.1, 0.0, 1.0);
    oestrus = 0;
    femalePromiscuity = getDoubleValue(initFemalePromiscuity, 0.1, 0.1, 0.0, 1.0);
    kidV = getDoubleValue(1.0, 0.1, 0.1, 0.0, 1.0);
    fearOfHPWhenHasKid = getDoubleValue(initKFearOfHP, 0.1, 0.1, 0.0, 1.0);
    maleAgeImportance = getDoubleValue(initMaleAgeImportance, 0.1, 0.1, 0.0, 1.0);
    maleEnergyImportance = getDoubleValue(initMaleEnergyImportance, 0.2, 0.2,
            0.0, 1.0);
    bestMaleAge = MaxAge / 2;

    if(getRandDouble(0.0, 1.0) < initMAcceptInv)
        m_acceptMigInv = true;
    else
        m_acceptMigInv = false;
    if(getRandDouble(0.0, 1.0) < initFAcceptInv)
        f_acceptMigInv = true;
    else
        f_acceptMigInv = false;

    if(getRandDouble(0.0, 1.0) < initMAcceptMoveInv)
        m_acceptMoveInvtFromM = true;
    else
        m_acceptMoveInvtFromM = false;
    if(getRandDouble(0.0, 1.0) < initFAcceptMoveInv)
        f_acceptMoveInvtFromM = true;
    else
        f_acceptMoveInvtFromM = false;

    if(getRandDouble(0.0, 1.0) < initMAcceptMoveInv)
        m_acceptMoveInvtFromF = true;
    else
        m_acceptMoveInvtFromF = false;
    if(getRandDouble(0.0, 1.0) < initFAcceptMoveInv)
        f_acceptMoveInvtFromF = true;
    else
        f_acceptMoveInvtFromF = false;

    if(getRandDouble(0.0, 1.0) < initMFearOfHP)
        m_fearOfHPatches = true;
    else
        m_fearOfHPatches = false;
    if(getRandDouble(0.0, 1.0) < initFFearOfHP)
        f_fearOfHPatches = true;
    else
        f_fearOfHPatches = false;

    f_valueOfNoSex = getDoubleValue(initFVOfNoSex, 0.1, 0.1, -1.0, -0.1);
    f_kidValueForMale = getDoubleValue(initFKidVForMale, (initFKidVForMale / 5.0),
            (initFKidVForMale / 5.0), 0.0, 20);
    f_moV = getDoubleValue(4.0, 0.2, 0.2, 0.0, 4.0);
    f_bnvlcTSibling = getDoubleValue(initFBnvlcTSibling, 0.1, 0.1, 0.0, 1.0);
    f_bnvlcTChild = getDoubleValue(initFBnvlcTChild, 0.1, 0.1, 0.0, 1.0);
    f_bnvlcTMother = getDoubleValue(initFBnvlcTMother, 0.1, 0.1, 0.0, 1.0);
    f_generosity = getDoubleValue(initFGenerosity, 0.1, 0.1, 0.0, 1.0);
    f_meatGenerosity = getDoubleValue(initFMeatGenerosity, 0.1, 0.1, 0.0, 1.0);
    f_pity = getDoubleValue(initFPity, 0.1, 0.1, 0.0, 1.0);
    f_envy = getDoubleValue(initFEnvy, 0.1, 0.1, 0.0, 1.0);
    f_gratitude = getDoubleValue(initFGratitude, 0.1, 0.1, 0.0, 1.0);
    f_vengefulness = getDoubleValue(initFVengefulness, 0.1, 0.1, 0.0, 1.0);
    f_energySel = getDoubleValue(initFEnergySel, 0.1, 0.1, 0.0, 1.0);
    f_childSel = getDoubleValue(initFChildSel, 0.1, 0.1, 0.0, 1.0);
    f_siblingSel = getDoubleValue(initFSiblingSel, 0.1, 0.1, 0.0, 1.0);
    f_motherSel = getDoubleValue(initFMotherSel, 0.1, 0.1, 0.0, 1.0);
    f_friendSel = getDoubleValue(initFFriendSel, 0.1, 0.1, 0.0, 1.0);
    f_otherSexSel = getDoubleValue(initFOtherSexSel, 0.1, 0.1, 0.0, 1.0);
    f_sameSexSel = getDoubleValue(initFSameSexSel, 0.1, 0.1, 0.0, 1.0);
    f_migAgeImportance = getDoubleValue(initFMigAgeImportance, 0.1, 0.1, 0.0, 1.0);
    f_migFriendImportance = getDoubleValue(initFMigFriendImportance, 0.1, 0.1, 0.0, 1.0);
    f_bnvlcTOtherSex = getDoubleValue(initFBnvlcTOtherSex, 0.1, 0.1, 0.0, 1.0);
    f_bnvlcTSameSex = getDoubleValue(initFBnvlcTSameSex, 0.1, 0.1, 0.0, 1.0);
    f_bravery = getDoubleValue(initFBravery, 0.1, 0.1, 0.0, 1.0);
    f_fvalueOfNoH = getDoubleValue(initFFVOfNoH, 0.1, 0.1, -1.0, -0.1);
    f_fvalueOfNoCT = getDoubleValue(initFFVOfNoCT, 0.1, 0.1, -1.0, -0.1);
    f_fvalueOfNoSh = getDoubleValue(initFFVOfNoSh, 0.1, 0.1, -1.0, -0.1);
    f_mvalueOfNoH = getDoubleValue(initFMVOfNoH, 0.1, 0.1, -1.0, -0.1);
    f_mvalueOfNoCT = getDoubleValue(initFMVOfNoCT, 0.1, 0.1, -1.0, -0.1);
    f_mvalueOfNoSh = getDoubleValue(initFMVOfNoSh, 0.1, 0.1, -1.0, -0.1);
    f_vOfFStranger = getDoubleValue(initVOfStrg, 0.1, 0.1, -0.5, 0.5);
    f_vOfMStranger = getDoubleValue(initVOfStrg, 0.1, 0.1, -0.5, 0.5);
    f_enmityThr = getDoubleValue(initEnmityThr, 0.1, 0.1, -0.5, 0.0);
    f_fairness = getDoubleValue(initFairness, 0.1, 0.1, 0.8, 1.2);
    f_expFFairness = getDoubleValue(initFairness, 0.1, 0.1, 0.8, 1.2);
    f_expMFairness = getDoubleValue(initFairness, 0.1, 0.1, 0.8, 1.2);
    f_fvalueOfNotFair = getDoubleValue(initVOfNotFair, 0.1, 0.1, -1.0, -0.1);
    f_mvalueOfNotFair = getDoubleValue(initVOfNotFair, 0.1, 0.1, -1.0, -0.1);
    f_huntValue = getDoubleValue(initFHuntV, 0.1, 0.1, 0.0, 2.0);
    f_audacity = getDoubleValue(initFAudacity, 0.1, 0.1, 0.0, 1.0);
    f_loyalty = getDoubleValue(initFLoyalty, 0.1, 0.1, 0.0, 1.0);
    f_adviceValue = 0.5;
    f_patrolValue = getDoubleValue(initFPatrolV, 0.1, 0.1, 0.0, 1.0);
    f_tmFct = getDoubleValue(initFTmFct, 0.1, 0.1, 0.0, 1.0);
    f_lowDeficit = 0.0;
    f_hiDeficit = 1.5;

    m_valueOfNoSex = getDoubleValue(initMVOfNoSex, 0.1, 0.1, -1.0, -0.1);
    m_kidValueForMale = getDoubleValue(initMKidVForMale, (initMKidVForMale / 5.0),
            (initMKidVForMale / 5.0), 0.0, 20);
    m_moV = getDoubleValue(4.0, 0.2, 0.2, 0.0, 4.0);
    m_bnvlcTSibling = getDoubleValue(initMBnvlcTSibling, 0.1, 0.1, 0.0, 1.0);
    m_bnvlcTChild = getDoubleValue(initMBnvlcTChild, 0.1, 0.1, 0.0, 1.0);
    m_bnvlcTMother = getDoubleValue(initMBnvlcTMother, 0.1, 0.1, 0.0, 1.0);
    m_generosity = getDoubleValue(initMGenerosity, 0.1, 0.1, 0.0, 1.0);
    m_meatGenerosity = getDoubleValue(initMMeatGenerosity, 0.1, 0.1, 0.0, 1.0);
    m_pity = getDoubleValue(initMPity, 0.1, 0.1, 0.0, 1.0);
    m_envy = getDoubleValue(initMEnvy, 0.1, 0.1, 0.0, 1.0);
    m_gratitude = getDoubleValue(initMGratitude, 0.1, 0.1, 0.0, 1.0);
    m_vengefulness = getDoubleValue(initMVengefulness, 0.1, 0.1, 0.0, 1.0);
    m_energySel = getDoubleValue(initMEnergySel, 0.1, 0.1, 0.0, 1.0);
    m_childSel = getDoubleValue(initMChildSel, 0.1, 0.1, 0.0, 1.0);
    m_siblingSel = getDoubleValue(initMSiblingSel, 0.1, 0.1, 0.0, 1.0);
    m_motherSel = getDoubleValue(initMMotherSel, 0.1, 0.1, 0.0, 1.0);
    m_friendSel = getDoubleValue(initMFriendSel, 0.1, 0.1, 0.0, 1.0);
    m_otherSexSel = getDoubleValue(initMOtherSexSel, 0.1, 0.1, 0.0, 1.0);
    m_sameSexSel = getDoubleValue(initMSameSexSel, 0.1, 0.1, 0.0, 1.0);
    m_migAgeImportance = getDoubleValue(initMMigAgeImportance, 0.1, 0.1, 0.0, 1.0);
    m_migFriendImportance = getDoubleValue(initMMigFriendImportance, 0.1, 0.1, 0.0, 1.0);
    m_bnvlcTOtherSex = getDoubleValue(initMBnvlcTOtherSex, 0.1, 0.1, 0.0, 1.0);
    m_bnvlcTSameSex = getDoubleValue(initMBnvlcTSameSex, 0.1, 0.1, 0.0, 1.0);
    m_bravery = getDoubleValue(initMBravery, 0.1, 0.1, 0.0, 1.0);
    m_fvalueOfNoH = getDoubleValue(initMFVOfNoH, 0.1, 0.1, -1.0, -0.1);
    m_fvalueOfNoCT = getDoubleValue(initMFVOfNoCT, 0.1, 0.1, -1.0, -0.1);
    m_fvalueOfNoSh = getDoubleValue(initMFVOfNoSh, 0.1, 0.1, -1.0, -0.1);
    m_mvalueOfNoH = getDoubleValue(initMMVOfNoH, 0.1, 0.1, -1.0, -0.1);
    m_mvalueOfNoCT = getDoubleValue(initMMVOfNoCT, 0.1, 0.1, -1.0, -0.1);
    m_mvalueOfNoSh = getDoubleValue(initMMVOfNoSh, 0.1, 0.1, -1.0, -0.1);
    m_vOfFStranger = getDoubleValue(initVOfStrg, 0.1, 0.1, -0.5, 0.5);
    m_vOfMStranger = getDoubleValue(initVOfStrg, 0.1, 0.1, -0.5, 0.5);
    m_enmityThr = getDoubleValue(initEnmityThr, 0.1, 0.1, -0.5, 0.0);
    m_fairness = getDoubleValue(initFairness, 0.1, 0.1, 0.8, 1.2);
    m_expFFairness = getDoubleValue(initFairness, 0.1, 0.1, 0.8, 1.2);
    m_expMFairness = getDoubleValue(initFairness, 0.1, 0.1, 0.8, 1.2);
    m_fvalueOfNotFair = getDoubleValue(initVOfNotFair, 0.1, 0.1, -1.0, -0.1);
    m_mvalueOfNotFair = getDoubleValue(initVOfNotFair, 0.1, 0.1, -1.0, -0.1);
    m_huntValue = getDoubleValue(initMHuntV, 0.1, 0.1, 0.0, 2.0);
    m_audacity = getDoubleValue(initMAudacity, 0.1, 0.1, 0.0, 1.0);
    m_loyalty = getDoubleValue(initMLoyalty, 0.1, 0.1, 0.0, 1.0);
    m_patrolValue = getDoubleValue(initMPatrolV, 0.1, 0.1, 0.0, 1.0);
    m_adviceValue = 0.5;
    m_tmFct = getDoubleValue(initMTmFct, 0.1, 0.1, 0.0, 1.0);
    m_lowDeficit = 0.0;
    m_hiDeficit = 1.5;

    f_migPrefs[0] = 0;
    f_migPrefs[1] = 1;
    f_migPrefs[2] = 2;
    f_migPrefs[3] = 3;
    m_migPrefs[0] = 0;
    m_migPrefs[1] = 1;
    m_migPrefs[2] = 2;
    m_migPrefs[3] = 3;
    if(getRandInt(0, 1000) > 500){
        m_migPrefs[0] = 1;
        f_migPrefs[0] = 1;
        m_migPrefs[1] = 0;
        f_migPrefs[1] = 0;
    }
    createEnd();
}

// Kinds of inheritance: MUTABLE (crossover, from same sex parent, and
// average from mother and father) and IMMUTABLE (global variable).
Agent::Agent(Agent *m, Agent *f, double e)
{
#ifdef DEBUG
    if(m == f)
        FATALERR("(mother == father)");
    if(m == NULL)
        FATALERR("(mother == NULL)");
    if(f == NULL)
        FATALERR("(father == NULL)");
    if(m->age == -1)
        FATALERR("(mother age == -1)");
    if(f->age == -1)
        FATALERR("(father age == -1)");
#endif

    mother = m;
    father = f;
    x = mother->x;
    y = mother->y;
    mother->addKid(this);
    father->addKid(this);
    mGen = father->mGen + 1;
    NAMECOPY(fatherName, father->name);
    fGen = mother->fGen + 1;
    NAMECOPY(motherName, mother->name);
    energy = e;
    metabolism = 0.5;
    lastHunt = SimTime;
    huntInterval = (int)((father->huntInterval + mother->huntInterval) / 2);
    age = 0;
    isKid = true;
    if(getRandInt(0, 1000) > 500){
        m_childhood = father->m_childhood;
        f_childhood = father->f_childhood;
    } else{
        m_childhood = mother->m_childhood;
        f_childhood = mother->f_childhood;
    }

    if(getRandInt(0, 1000) > 500){
        m_meatValue = father->m_meatValue;
        f_meatValue = father->f_meatValue;
    } else{
        m_meatValue = mother->m_meatValue;
        f_meatValue = mother->f_meatValue;
    }
    if(getRandInt(0, 1000) > 500)
        askMeatOnly = father->askMeatOnly;
    else
        askMeatOnly = mother->askMeatOnly;
    if(getRandInt(0, 1000) > 500)
        bestMaleAge = father->bestMaleAge;
    else
        bestMaleAge = mother->bestMaleAge;
    if(getRandInt(0, 1000) > 500)
        oestrFemSel = father->oestrFemSel;
    else
        oestrFemSel = mother->oestrFemSel;
    if(getRandInt(0, 1000) > 500)
        femalePromiscuity = father->femalePromiscuity;
    else
        femalePromiscuity = mother->femalePromiscuity;
    if(getRandInt(0, 1000) > 500)
        kidV = father->kidV;
    else
        kidV = mother->kidV;
    if(getRandInt(0, 1000) > 500)
        fearOfHPWhenHasKid = father->fearOfHPWhenHasKid;
    else
        fearOfHPWhenHasKid = mother->fearOfHPWhenHasKid;
    if(getRandInt(0, 1000) > 500)
        maleAgeImportance = father->maleAgeImportance;
    else
        maleAgeImportance = mother->maleAgeImportance;
    if(getRandInt(0, 1000) > 500)
        maleEnergyImportance = father->maleEnergyImportance;
    else
        maleEnergyImportance = mother->maleEnergyImportance;
    if(getRandInt(0, 1000) > 500){
        m_valueOfNoSex = father->m_valueOfNoSex;
        f_valueOfNoSex = father->f_valueOfNoSex;
    } else{
        m_valueOfNoSex = mother->m_valueOfNoSex;
        f_valueOfNoSex = mother->f_valueOfNoSex;
    }
    if(getRandInt(0, 1000) > 500){
        m_otherSexSel = father->m_otherSexSel;
        f_otherSexSel = father->f_otherSexSel;
    } else{
        m_otherSexSel = mother->m_otherSexSel;
        f_otherSexSel = mother->f_otherSexSel;
    }
    if(getRandInt(0, 1000) > 500){
        m_sameSexSel = father->m_sameSexSel;
        f_sameSexSel = father->f_sameSexSel;
    } else{
        m_sameSexSel = mother->m_sameSexSel;
        f_sameSexSel = mother->f_sameSexSel;
    }
    if(getRandInt(0, 1000) > 500){
        m_kidValueForMale = father->m_kidValueForMale;
        f_kidValueForMale = father->f_kidValueForMale;
    } else{
        m_kidValueForMale = mother->m_kidValueForMale;
        f_kidValueForMale = mother->f_kidValueForMale;
    }
    if(getRandInt(0, 1000) > 500){
        m_moV = father->m_moV;
        f_moV = father->f_moV;
    } else{
        m_moV = mother->m_moV;
        f_moV = mother->f_moV;
    }
    if(getRandInt(0, 1000) > 500){
        m_bnvlcTOtherSex = father->m_bnvlcTOtherSex;
        f_bnvlcTOtherSex = father->f_bnvlcTOtherSex;
    } else{
        m_bnvlcTOtherSex = mother->m_bnvlcTOtherSex;
        f_bnvlcTOtherSex = mother->f_bnvlcTOtherSex;
    }
    if(getRandInt(0, 1000) > 500){
        m_bnvlcTSameSex = father->m_bnvlcTSameSex;
        f_bnvlcTSameSex = father->f_bnvlcTSameSex;
    } else{
        m_bnvlcTSameSex = mother->m_bnvlcTSameSex;
        f_bnvlcTSameSex = mother->f_bnvlcTSameSex;
    }
    if(getRandInt(0, 1000) > 500){
        m_bnvlcTMother = father->m_bnvlcTMother;
        f_bnvlcTMother = father->f_bnvlcTMother;
    } else{
        m_bnvlcTMother = mother->m_bnvlcTMother;
        f_bnvlcTMother = mother->f_bnvlcTMother;
    }
    if(getRandInt(0, 1000) > 500){
        m_bnvlcTSibling = father->m_bnvlcTSibling;
        f_bnvlcTSibling = father->f_bnvlcTSibling;
    } else{
        m_bnvlcTSibling = mother->m_bnvlcTSibling;
        f_bnvlcTSibling = mother->f_bnvlcTSibling;
    }
    if(getRandInt(0, 1000) > 500){
        m_bnvlcTChild = father->m_bnvlcTChild;
        f_bnvlcTChild = father->f_bnvlcTChild;
    } else{
        m_bnvlcTChild = mother->m_bnvlcTChild;
        f_bnvlcTChild = mother->f_bnvlcTChild;
    }
    if(getRandInt(0, 1000) > 500){
        m_generosity = father->m_generosity;
        f_generosity = father->f_generosity;
    } else{
        m_generosity = mother->m_generosity;
        f_generosity = mother->f_generosity;
    }
    if(getRandInt(0, 1000) > 500){
        m_meatGenerosity = father->m_meatGenerosity;
        f_meatGenerosity = father->f_meatGenerosity;
    } else{
        m_meatGenerosity = mother->m_meatGenerosity;
        f_meatGenerosity = mother->f_meatGenerosity;
    }
    if(getRandInt(0, 1000) > 500){
        m_envy = father->m_envy;
        f_envy = father->f_envy;
    } else{
        m_envy = mother->m_envy;
        f_envy = mother->f_envy;
    }
    if(getRandInt(0, 1000) > 500){
        m_pity = father->m_pity;
        f_pity = father->f_pity;
    } else{
        m_pity = mother->m_pity;
        f_pity = mother->f_pity;
    }
    if(getRandInt(0, 1000) > 500){
        m_gratitude = father->m_gratitude;
        f_gratitude = father->f_gratitude;
    } else{
        m_gratitude = mother->m_gratitude;
        f_gratitude = mother->f_gratitude;
    }
    if(getRandInt(0, 1000) > 500){
        m_vengefulness = father->m_vengefulness;
        f_vengefulness = father->f_vengefulness;
    } else{
        m_vengefulness = mother->m_vengefulness;
        f_vengefulness = mother->f_vengefulness;
    }
    if(getRandInt(0, 1000) > 500){
        m_tmFct = father->m_tmFct;
        f_tmFct = father->f_tmFct;
    } else{
        m_tmFct = mother->m_tmFct;
        f_tmFct = mother->f_tmFct;
    }
    if(getRandInt(0, 1000) > 500){
        m_bravery = father->m_bravery;
        f_bravery = father->f_bravery;
    } else{
        m_bravery = mother->m_bravery;
        f_bravery = mother->f_bravery;
    }
    if(getRandInt(0, 1000) > 500){
        m_energySel = father->m_energySel;
        f_energySel = father->f_energySel;
    } else{
        m_energySel = mother->m_energySel;
        f_energySel = mother->f_energySel;
    }
    if(getRandInt(0, 1000) > 500){
        m_childSel = father->m_childSel;
        f_childSel = father->f_childSel;
    } else{
        m_childSel = mother->m_childSel;
        f_childSel = mother->f_childSel;
    }
    if(getRandInt(0, 1000) > 500){
        m_motherSel = father->m_motherSel;
        f_motherSel = father->f_motherSel;
    } else{
        m_motherSel = mother->m_motherSel;
        f_motherSel = mother->f_motherSel;
    }
    if(getRandInt(0, 1000) > 500){
        m_siblingSel = father->m_siblingSel;
        f_siblingSel = father->f_siblingSel;
    } else{
        m_siblingSel = mother->m_siblingSel;
        f_siblingSel = mother->f_siblingSel;
    }
    if(getRandInt(0, 1000) > 500){
        m_friendSel = father->m_friendSel;
        f_friendSel = father->f_friendSel;
    } else{
        m_friendSel = mother->m_friendSel;
        f_friendSel = mother->f_friendSel;
    }
    if(getRandInt(0, 1000) > 500){
        m_migAgeImportance = father->m_migAgeImportance;
        f_migAgeImportance = father->f_migAgeImportance;
    } else{
        m_migAgeImportance = mother->m_migAgeImportance;
        f_migAgeImportance = mother->f_migAgeImportance;
    }
    if(getRandInt(0, 1000) > 500){
        m_migFriendImportance = father->m_migFriendImportance;
        f_migFriendImportance = father->f_migFriendImportance;
    } else{
        m_migFriendImportance = mother->m_migFriendImportance;
        f_migFriendImportance = mother->f_migFriendImportance;
    }
    if(getRandInt(0, 1000) > 500){
        m_huntValue = father->m_huntValue;
        f_huntValue = father->f_huntValue;
    } else{
        m_huntValue = mother->m_huntValue;
        f_huntValue = mother->f_huntValue;
    }
    if(getRandInt(0, 1000) > 500){
        m_adviceValue = father->m_adviceValue;
        f_adviceValue = father->f_adviceValue;
    } else{
        m_adviceValue = mother->m_adviceValue;
        f_adviceValue = mother->f_adviceValue;
    }
    if(getRandInt(0, 1000) > 500){
        m_loyalty = father->m_loyalty;
        f_loyalty = father->f_loyalty;
    } else{
        m_loyalty = mother->m_loyalty;
        f_loyalty = mother->f_loyalty;
    }
    if(getRandInt(0, 1000) > 500){
        m_audacity = father->m_audacity;
        f_audacity = father->f_audacity;
    } else{
        m_audacity = mother->m_audacity;
        f_audacity = mother->f_audacity;
    }
    if(getRandInt(0, 1000) > 500){
        m_patrolValue = father->m_patrolValue;
        f_patrolValue = father->f_patrolValue;
    } else{
        m_patrolValue = mother->m_patrolValue;
        f_patrolValue = mother->f_patrolValue;
    }
    if(getRandInt(0, 1000) > 500){
        m_fvalueOfNoCT = father->m_fvalueOfNoCT;
        f_fvalueOfNoCT = father->f_fvalueOfNoCT;
    } else{
        m_fvalueOfNoCT = mother->m_fvalueOfNoCT;
        f_fvalueOfNoCT = mother->f_fvalueOfNoCT;
    }
    if(getRandInt(0, 1000) > 500){
        m_fvalueOfNoH = father->m_fvalueOfNoH;
        f_fvalueOfNoH = father->f_fvalueOfNoH;
    } else{
        m_fvalueOfNoH = mother->m_fvalueOfNoH;
        f_fvalueOfNoH = mother->f_fvalueOfNoH;
    }
    if(getRandInt(0, 1000) > 500){
        m_fvalueOfNoSh = father->m_fvalueOfNoSh;
        f_fvalueOfNoSh = father->f_fvalueOfNoSh;
    } else{
        m_fvalueOfNoSh = mother->m_fvalueOfNoSh;
        f_fvalueOfNoSh = mother->f_fvalueOfNoSh;
    }
    if(getRandInt(0, 1000) > 500){
        m_mvalueOfNoCT = father->m_mvalueOfNoCT;
        f_mvalueOfNoCT = father->f_mvalueOfNoCT;
    } else{
        m_mvalueOfNoCT = mother->m_mvalueOfNoCT;
        f_mvalueOfNoCT = mother->f_mvalueOfNoCT;
    }
    if(getRandInt(0, 1000) > 500){
        m_mvalueOfNoH = father->m_mvalueOfNoH;
        f_mvalueOfNoH = father->f_mvalueOfNoH;
    } else{
        m_mvalueOfNoH = mother->m_mvalueOfNoH;
        f_mvalueOfNoH = mother->f_mvalueOfNoH;
    }
    if(getRandInt(0, 1000) > 500){
        m_mvalueOfNoSh = father->m_mvalueOfNoSh;
        f_mvalueOfNoSh = father->f_mvalueOfNoSh;
    } else{
        m_mvalueOfNoSh = mother->m_mvalueOfNoSh;
        f_mvalueOfNoSh = mother->f_mvalueOfNoSh;
    }
    if(getRandInt(0, 1000) > 500){
        m_vOfFStranger = father->m_vOfFStranger;
        f_vOfFStranger = father->f_vOfFStranger;
    } else{
        m_vOfFStranger = mother->m_vOfFStranger;
        f_vOfFStranger = mother->f_vOfFStranger;
    }
    if(getRandInt(0, 1000) > 500){
        m_vOfMStranger = father->m_vOfMStranger;
        f_vOfMStranger = father->f_vOfMStranger;
    } else{
        m_vOfMStranger = mother->m_vOfMStranger;
        f_vOfMStranger = mother->f_vOfMStranger;
    }
    if(getRandInt(0, 1000) > 500){
        m_enmityThr = father->m_enmityThr;
        f_enmityThr = father->f_enmityThr;
    } else{
        m_enmityThr = mother->m_enmityThr;
        f_enmityThr = mother->f_enmityThr;
    }
    if(getRandInt(0, 1000) > 500){
        m_fairness = father->m_fairness;
        f_fairness = father->f_fairness;
    } else{
        m_fairness = mother->m_fairness;
        f_fairness = mother->f_fairness;
    }
    if(getRandInt(0, 1000) > 500){
        m_expFFairness = father->m_expFFairness;
        f_expFFairness = father->f_expFFairness;
    } else{
        m_expFFairness = mother->m_expFFairness;
        f_expFFairness = mother->f_expFFairness;
    }
    if(getRandInt(0, 1000) > 500){
        m_expMFairness = father->m_expMFairness;
        f_expMFairness = father->f_expMFairness;
    } else{
        m_expMFairness = mother->m_expMFairness;
        f_expMFairness = mother->f_expMFairness;
    }
    if(getRandInt(0, 1000) > 500){
        m_fvalueOfNotFair = father->m_fvalueOfNotFair;
        f_fvalueOfNotFair = father->f_fvalueOfNotFair;
    } else{
        m_fvalueOfNotFair = mother->m_fvalueOfNotFair;
        f_fvalueOfNotFair = mother->f_fvalueOfNotFair;
    }
    if(getRandInt(0, 1000) > 500){
        m_mvalueOfNotFair = father->m_mvalueOfNotFair;
        f_mvalueOfNotFair = father->f_mvalueOfNotFair;
    } else{
        m_mvalueOfNotFair = mother->m_mvalueOfNotFair;
        f_mvalueOfNotFair = mother->f_mvalueOfNotFair;
    }
    if(getRandInt(0, 1000) > 500)
        mutationRate = father->mutationRate;
    else
        mutationRate = mother->mutationRate;
    if(getRandInt(0, 1000) > 500){
        m_gratStrategy = father->m_gratStrategy;
        f_gratStrategy = father->f_gratStrategy;
    } else{
        m_gratStrategy = mother->m_gratStrategy;
        f_gratStrategy = mother->f_gratStrategy;
    }
    if(getRandInt(0, 1000) > 500){
        m_vengStrategy = father->m_vengStrategy;
        f_vengStrategy = father->f_vengStrategy;
    } else{
        m_vengStrategy = mother->m_vengStrategy;
        f_vengStrategy = mother->f_vengStrategy;
    }
    if(getRandInt(0, 1000) > 500){
        m_vengStr2 = father->m_vengStr2;
        f_vengStr2 = father->f_vengStr2;
    } else{
        m_vengStr2 = mother->m_vengStr2;
        f_vengStr2 = mother->f_vengStr2;
    }
    if(getRandInt(0, 1000) > 500){
        m_huntStrategy = father->m_huntStrategy;
        f_huntStrategy = father->f_huntStrategy;
    } else{
        m_huntStrategy = mother->m_huntStrategy;
        f_huntStrategy = mother->f_huntStrategy;
    }
    if(getRandInt(0, 1000) > 500){
        m_terriRemStrategy = father->m_terriRemStrategy;
        f_terriRemStrategy = father->f_terriRemStrategy;
    } else{
        m_terriRemStrategy = mother->m_terriRemStrategy;
        f_terriRemStrategy = mother->f_terriRemStrategy;
    }
    if(getRandInt(0, 1000) > 500){
        m_hasShame = father->m_hasShame;
        f_hasShame = father->f_hasShame;
    } else{
        m_hasShame = mother->m_hasShame;
        f_hasShame = mother->f_hasShame;
    }
    if(getRandInt(0, 1000) > 500){
        m_maxHuntPatrolSize = father->m_maxHuntPatrolSize;
        f_maxHuntPatrolSize = father->f_maxHuntPatrolSize;
    } else{
        m_maxHuntPatrolSize = mother->m_maxHuntPatrolSize;
        f_maxHuntPatrolSize = mother->f_maxHuntPatrolSize;
    }
    if(getRandInt(0, 1000) > 500){
        m_fearOfHPatches = father->m_fearOfHPatches;
        f_fearOfHPatches = father->f_fearOfHPatches;
    } else{
        m_fearOfHPatches = mother->m_fearOfHPatches;
        f_fearOfHPatches = mother->f_fearOfHPatches;
    }
    if(getRandInt(0, 1000) > 500){
        m_acceptMigInv = father->m_acceptMigInv;
        f_acceptMigInv = father->f_acceptMigInv;
    } else{
        m_acceptMigInv = mother->m_acceptMigInv;
        f_acceptMigInv = mother->f_acceptMigInv;
    }
    if(getRandInt(0, 1000) > 500){
        m_acceptMoveInvtFromF = father->m_acceptMoveInvtFromF;
        f_acceptMoveInvtFromF = father->f_acceptMoveInvtFromF;
    } else{
        m_acceptMoveInvtFromF = mother->m_acceptMoveInvtFromF;
        f_acceptMoveInvtFromF = mother->f_acceptMoveInvtFromF;
    }
    if(getRandInt(0, 1000) > 500){
        m_acceptMoveInvtFromM = father->m_acceptMoveInvtFromM;
        f_acceptMoveInvtFromM = father->f_acceptMoveInvtFromM;
    } else{
        m_acceptMoveInvtFromM = mother->m_acceptMoveInvtFromM;
        f_acceptMoveInvtFromM = mother->f_acceptMoveInvtFromM;
    }
    if(getRandInt(0, 1000) > 500){
        m_begStrategy = father->m_begStrategy;
        f_begStrategy = father->f_begStrategy;
    } else{
        m_begStrategy = mother->m_begStrategy;
        f_begStrategy = mother->f_begStrategy;
    }
    if(getRandInt(0, 1000) > 500){
        m_norm = father->m_norm;
        f_norm = father->f_norm;
    } else{
        m_norm = mother->m_norm;
        f_norm = mother->f_norm;
    }
    if(getRandInt(0, 1000) > 500){
        m_metanorm = father->m_metanorm;
        f_metanorm = father->f_metanorm;
    } else{
        m_metanorm = mother->m_metanorm;
        f_metanorm = mother->f_metanorm;
    }
    if(getRandInt(0, 1000) > 500){
        m_xenophTF = father->m_xenophTF;
        f_xenophTF = father->f_xenophTF;
    } else{
        m_xenophTF = mother->m_xenophTF;
        f_xenophTF = mother->f_xenophTF;
    }
    if(getRandInt(0, 1000) > 500){
        m_xenophTM = father->m_xenophTM;
        f_xenophTM = father->f_xenophTM;
    } else{
        m_xenophTM = mother->m_xenophTM;
        f_xenophTM = mother->f_xenophTM;
    }
    if(getRandInt(0, 1000) > 500){
        m_xenophTFwK = father->m_xenophTFwK;
        f_xenophTFwK = father->f_xenophTFwK;
    } else{
        m_xenophTFwK = mother->m_xenophTFwK;
        f_xenophTFwK = mother->f_xenophTFwK;
    }
    if(getRandInt(0, 1000) > 500){
        m_zeroPostvNo = father->m_zeroPostvNo;
        f_zeroPostvNo = father->f_zeroPostvNo;
    } else{
        m_zeroPostvNo = mother->m_zeroPostvNo;
        f_zeroPostvNo = mother->f_zeroPostvNo;
    }
    if(getRandInt(0, 1000) > 500){
        m_lowDeficit = father->m_lowDeficit;
        f_lowDeficit = father->f_lowDeficit;
    } else{
        m_lowDeficit = mother->m_lowDeficit;
        f_lowDeficit = mother->f_lowDeficit;
    }
    if(getRandInt(0, 1000) > 500){
        m_hiDeficit = father->m_hiDeficit;
        f_hiDeficit = father->f_hiDeficit;
    } else{
        m_hiDeficit = mother->m_hiDeficit;
        f_hiDeficit = mother->f_hiDeficit;
    }

    if(getRandInt(0, 1000) > 500){
        copyMigPrefs(m_migPrefs, father->m_migPrefs);
        copyMigPrefs(f_migPrefs, father->f_migPrefs);
    } else{
        copyMigPrefs(m_migPrefs, mother->m_migPrefs);
        copyMigPrefs(f_migPrefs, mother->f_migPrefs);
    }

    mutate();
    createEnd();
}

void Agent::createEnd()
{
    deathCause = 0;
    kid = NULL;
    migrating = 0;
    migStep = 0;
    meatAge = 0;
    meat = 0.0;
    followed = false;
    children = new List();
    nonCooperators = new List();
    potentialPartners = new List();
    rootMemory = NULL;
    lastMemory = NULL;
    knownPatches = NULL;
    nRembr = 0;
    nInvt = 0;
    nAcptInvt = 0;
    nAlliChng = 0;
    lastEnergy = energy;
    eFromMom = 0.0;
    eFromShare = 0.0;
    eFromConfl = 0.0;
    eFromDinner = 0.0;
    eFromHunt = 0.0;
    myAlliance = NULL;
    amILeader = false;
    int i;
    do{
        for(i = 0; i < 7; i++){
            name[i] = getRandInt('A', 'z');
            if(name[i] > 'Z' && name[i] < 'a')
                name[i] = name[i] - 42;
        }
    } while(getAgPointer(name) != NULL);
    name[7] = 0;
    int r;
    r = getRandInt(0, 1000);
    if(r > 500){
        sex = 'm';
        acceptMigInv = m_acceptMigInv;
        acceptMoveInvtFromF = m_acceptMoveInvtFromF;
        acceptMoveInvtFromM = m_acceptMoveInvtFromM;
        adviceValue = m_adviceValue;
        audacity = m_audacity;
        fearOfHPatches = m_fearOfHPatches;
        begStrategy = m_begStrategy;
        norm = m_norm;
        metanorm = m_metanorm;
        bnvlcTChild = m_bnvlcTChild;
        bnvlcTMother = m_bnvlcTMother;
        bnvlcTOtherSex = m_bnvlcTOtherSex;
        bnvlcTSameSex = m_bnvlcTSameSex;
        bnvlcTSibling = m_bnvlcTSibling;
        bravery = m_bravery;
        childSel = m_childSel;
        childhood = m_childhood;
        copyMigPrefs(migPrefs, m_migPrefs);
        energySel = m_energySel;
        envy = m_envy;
        vOfFStranger = m_vOfFStranger;
        vOfMStranger = m_vOfMStranger;
        enmityThr = m_enmityThr;
        fairness = m_fairness;
        expFFairness = m_expFFairness;
        expMFairness = m_expMFairness;
        fvalueOfNotFair = m_fvalueOfNotFair;
        mvalueOfNotFair = m_mvalueOfNotFair;
        xenophTM = m_xenophTM;
        xenophTF = m_xenophTF;
        xenophTFwK = m_xenophTFwK;
        zeroPostvNo = m_zeroPostvNo;
        friendSel = m_friendSel;
        fvalueOfNoCT = m_fvalueOfNoCT;
        fvalueOfNoH = m_fvalueOfNoH;
        fvalueOfNoSh = m_fvalueOfNoSh;
        generosity = m_generosity;
        gratStrategy = m_gratStrategy;
        gratitude = m_gratitude;
        huntStrategy = m_huntStrategy;
        huntValue = m_huntValue;
        terriRemStrategy = m_terriRemStrategy;
        hasShame = m_hasShame;
        kidValueForMale = m_kidValueForMale;
        moV = m_moV;
        loyalty = m_loyalty;
        maxHuntPatrolSize = m_maxHuntPatrolSize;
        meatGenerosity = m_meatGenerosity;
        meatValue = m_meatValue;
        migAgeImportance = m_migAgeImportance;
        migFriendImportance = m_migFriendImportance;
        motherSel = m_motherSel;
        mvalueOfNoCT = m_mvalueOfNoCT;
        mvalueOfNoH = m_mvalueOfNoH;
        mvalueOfNoSh = m_mvalueOfNoSh;
        otherSexSel = m_otherSexSel;
        patrolValue = m_patrolValue;
        pity = m_pity;
        sameSexSel = m_sameSexSel;
        siblingSel = m_siblingSel;
        tmFct = m_tmFct;
        valueOfNoSex = m_valueOfNoSex;
        vengStrategy = m_vengStrategy;
        vengStr2 = m_vengStr2;
        vengefulness = m_vengefulness;
        hiDeficit = m_hiDeficit;
        lowDeficit = m_lowDeficit;
    } else{
        sex = 'f';
        acceptMigInv = f_acceptMigInv;
        acceptMoveInvtFromF = f_acceptMoveInvtFromF;
        acceptMoveInvtFromM = f_acceptMoveInvtFromM;
        adviceValue = f_adviceValue;
        audacity = f_audacity;
        fearOfHPatches = f_fearOfHPatches;
        begStrategy = f_begStrategy;
        norm = f_norm;
        metanorm = f_metanorm;
        bnvlcTChild = f_bnvlcTChild;
        bnvlcTMother = f_bnvlcTMother;
        bnvlcTOtherSex = f_bnvlcTOtherSex;
        bnvlcTSameSex = f_bnvlcTSameSex;
        bnvlcTSibling = f_bnvlcTSibling;
        bravery = f_bravery;
        childSel = f_childSel;
        childhood = f_childhood;
        copyMigPrefs(migPrefs, f_migPrefs);
        energySel = f_energySel;
        envy = f_envy;
        vOfFStranger = f_vOfFStranger;
        vOfMStranger = f_vOfMStranger;
        enmityThr = f_enmityThr;
        fairness = f_fairness;
        expFFairness = f_expFFairness;
        expMFairness = f_expMFairness;
        fvalueOfNotFair = f_fvalueOfNotFair;
        mvalueOfNotFair = f_mvalueOfNotFair;
        xenophTM = f_xenophTM;
        xenophTF = f_xenophTF;
        xenophTFwK = f_xenophTFwK;
        zeroPostvNo = f_zeroPostvNo;
        friendSel = f_friendSel;
        fvalueOfNoCT = f_fvalueOfNoCT;
        fvalueOfNoH = f_fvalueOfNoH;
        fvalueOfNoSh = f_fvalueOfNoSh;
        generosity = f_generosity;
        gratStrategy = f_gratStrategy;
        gratitude = f_gratitude;
        huntStrategy = f_huntStrategy;
        huntValue = f_huntValue;
        terriRemStrategy = f_terriRemStrategy;
        hasShame = f_hasShame;
        kidValueForMale = f_kidValueForMale;
        moV = f_moV;
        loyalty = f_loyalty;
        maxHuntPatrolSize = f_maxHuntPatrolSize;
        meatGenerosity = f_meatGenerosity;
        meatValue = f_meatValue;
        migAgeImportance = f_migAgeImportance;
        migFriendImportance = f_migFriendImportance;
        motherSel = f_motherSel;
        mvalueOfNoCT = f_mvalueOfNoCT;
        mvalueOfNoH = f_mvalueOfNoH;
        mvalueOfNoSh = f_mvalueOfNoSh;
        oestrus = 0;
        otherSexSel = f_otherSexSel;
        patrolValue = f_patrolValue;
        pity = f_pity;
        sameSexSel = f_sameSexSel;
        siblingSel = f_siblingSel;
        tmFct = f_tmFct;
        valueOfNoSex = f_valueOfNoSex;
        vengStrategy = f_vengStrategy;
        vengStr2 = f_vengStr2;
        vengefulness = f_vengefulness;
        hiDeficit = f_hiDeficit;
        lowDeficit = f_lowDeficit;
    }

    maxAge = MaxAge + getRandInt((childhood * (-1)), childhood);
    if(sex == 'f')
        reproAge = maxAge - childhood - 5; // 5 for the oestrus period
    myCell = cellGrid[x][y];
    myCell->addAgent(this);
    myPatch = myCell->patch;
    addAgName(name, this);
    if(zeroPostvNo){ // Zero positive values for remembering "false" as answer.
        if(valueOfNoSex > 0.0)
            valueOfNoSex = 0.0;
        if(fvalueOfNoH > 0.0)
            fvalueOfNoH = 0.0;
        if(fvalueOfNoCT > 0.0)
            fvalueOfNoCT = 0.0;
        if(fvalueOfNoSh > 0.0)
            fvalueOfNoSh = 0.0;
        if(mvalueOfNoH > 0.0)
            mvalueOfNoH = 0.0;
        if(mvalueOfNoCT > 0.0)
            mvalueOfNoCT = 0.0;
        if(mvalueOfNoSh > 0.0)
            mvalueOfNoSh = 0.0;
    }
}

void Agent::mutate()
{
    double r = getRandDouble(0.0, 1.0);
    if(r > mutationRate)
        return;

    double mr2 = mutationRate / 2.0;
    double frac = mutationRate / 10.0;

    r = getRandDouble(0.0, 8.0);
    if(r < (mr2 / 2)){
        int i, j;
        char c;
        i = getRandInt(0, 2);
        j = getRandInt(0, 2);
        c = m_migPrefs[i];
        m_migPrefs[i] = m_migPrefs[j];
        m_migPrefs[j] = c;
    }

    r = getRandDouble(0.0, 8.0);
    if(r < (mr2 / 2)){
        int i, j;
        char c;
        i = getRandInt(0, 2);
        j = getRandInt(0, 2);
        c = f_migPrefs[i];
        f_migPrefs[i] = f_migPrefs[j];
        f_migPrefs[j] = c;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_fearOfHPatches = !m_fearOfHPatches;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_fearOfHPatches = !f_fearOfHPatches;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_acceptMigInv = !m_acceptMigInv;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_acceptMigInv = !f_acceptMigInv;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_acceptMoveInvtFromM = !m_acceptMoveInvtFromM;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_acceptMoveInvtFromM = !f_acceptMoveInvtFromM;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_acceptMoveInvtFromF = !m_acceptMoveInvtFromF;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_acceptMoveInvtFromF = !f_acceptMoveInvtFromF;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_huntStrategy = getRandInt(0, 2);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_huntStrategy = getRandInt(0, 2);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_terriRemStrategy = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_terriRemStrategy = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_hasShame = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_hasShame = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_xenophTF = !f_xenophTF;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_xenophTF = !m_xenophTF;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_xenophTM = !f_xenophTM;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_xenophTM = !m_xenophTM;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_xenophTFwK = !f_xenophTFwK;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_xenophTFwK = !m_xenophTFwK;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_zeroPostvNo = !f_zeroPostvNo;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_zeroPostvNo = !m_zeroPostvNo;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_norm = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_norm = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_metanorm = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_metanorm = getRandInt(0, 1);

    if(f_norm == false)
        f_metanorm = false;
    if(m_norm == false)
        m_metanorm = false;

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_begStrategy = getRandInt(1, 4);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_begStrategy = getRandInt(1, 4);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_gratStrategy = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_gratStrategy = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_vengStrategy = getRandInt(0, 2);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_vengStrategy = getRandInt(0, 2);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        m_vengStr2 = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        f_vengStr2 = getRandInt(0, 1);

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_maxHuntPatrolSize++;
        else
            if(m_maxHuntPatrolSize > 1)
                m_maxHuntPatrolSize--;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_maxHuntPatrolSize++;
        else
            if(f_maxHuntPatrolSize > 1)
                f_maxHuntPatrolSize--;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_childhood++;
        else
            m_childhood--;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_childhood++;
        else
            f_childhood--;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_migFriendImportance += frac;
        else
            m_migFriendImportance -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_migFriendImportance += frac;
        else
            f_migFriendImportance -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_migAgeImportance += frac;
        else
            m_migAgeImportance -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_migAgeImportance += frac;
        else
            f_migAgeImportance -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            m_hiDeficit += mr2;
            if(m_hiDeficit > MaxEnergy)
                m_hiDeficit = MaxEnergy;
        } else{
            m_hiDeficit -= mr2;
            if(m_hiDeficit < m_lowDeficit)
                m_hiDeficit = m_lowDeficit;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            m_lowDeficit += mr2;
            if(m_lowDeficit > m_hiDeficit)
                m_lowDeficit = m_hiDeficit;
        } else{
            m_lowDeficit -= mr2;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            f_lowDeficit += mr2;
            if(f_lowDeficit > f_hiDeficit)
                f_lowDeficit = f_hiDeficit;
        } else{
            f_lowDeficit -= mr2;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            f_hiDeficit += mr2;
            if(f_hiDeficit > MaxEnergy)
                f_hiDeficit = MaxEnergy;
        } else{
            f_hiDeficit -= mr2;
            if(f_hiDeficit < f_lowDeficit)
                f_hiDeficit = f_lowDeficit;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_pity += frac;
        else
            m_pity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_pity += frac;
        else
            f_pity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_envy += frac;
        else
            m_envy -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_envy += frac;
        else
            f_envy -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_generosity += frac;
        else
            m_generosity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_generosity += frac;
        else
            f_generosity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_meatGenerosity += frac;
        else
            m_meatGenerosity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_meatGenerosity += frac;
        else
            f_meatGenerosity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_bnvlcTMother += frac;
        else
            m_bnvlcTMother -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_bnvlcTMother += frac;
        else
            f_bnvlcTMother -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_bnvlcTSibling += frac;
        else
            m_bnvlcTSibling -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_bnvlcTSibling += frac;
        else
            f_bnvlcTSibling -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_bnvlcTChild += frac;
        else
            m_bnvlcTChild -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_bnvlcTChild += frac;
        else
            f_bnvlcTChild -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_otherSexSel += frac;
        else
            m_otherSexSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_otherSexSel += frac;
        else
            f_otherSexSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_sameSexSel += frac;
        else
            m_sameSexSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_sameSexSel += frac;
        else
            f_sameSexSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_huntValue += frac;
        else
            m_huntValue -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_huntValue += frac;
        else
            f_huntValue -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_adviceValue += frac;
        else{
            m_adviceValue -= frac;
            if(m_adviceValue < 0.0)
                m_adviceValue = 0.0;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_adviceValue += frac;
        else{
            f_adviceValue -= frac;
            if(f_adviceValue < 0.0)
                f_adviceValue = 0.0;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_audacity += frac;
        else
            m_audacity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_audacity += frac;
        else
            f_audacity -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_loyalty += frac;
        else
            m_loyalty -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_loyalty += frac;
        else
            f_loyalty -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_patrolValue += frac;
        else
            m_patrolValue -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_patrolValue += frac;
        else
            f_patrolValue -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_fvalueOfNoSh += frac;
        else
            m_fvalueOfNoSh -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_fvalueOfNoSh += frac;
        else
            f_fvalueOfNoSh -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_fvalueOfNoH += frac;
        else
            m_fvalueOfNoH -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_fvalueOfNoH += frac;
        else
            f_fvalueOfNoH -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_fvalueOfNoCT += frac;
        else
            m_fvalueOfNoCT -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_fvalueOfNoCT += frac;
        else
            f_fvalueOfNoCT -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_mvalueOfNoSh += frac;
        else
            m_mvalueOfNoSh -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_mvalueOfNoSh += frac;
        else
            f_mvalueOfNoSh -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_vOfMStranger += frac;
        else
            m_vOfMStranger -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_vOfMStranger += frac;
        else
            f_vOfMStranger -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_enmityThr += frac;
        else
            m_enmityThr -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_enmityThr += frac;
        else
            f_enmityThr -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_fairness += frac;
        else
            m_fairness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_fairness += frac;
        else
            f_fairness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_expFFairness += frac;
        else
            m_expFFairness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_expFFairness += frac;
        else
            f_expFFairness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_expMFairness += frac;
        else
            m_expMFairness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_expMFairness += frac;
        else
            f_expMFairness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_mvalueOfNotFair += frac;
        else
            m_mvalueOfNotFair -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_mvalueOfNotFair += frac;
        else
            f_mvalueOfNotFair -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_fvalueOfNotFair += frac;
        else
            m_fvalueOfNotFair -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_fvalueOfNotFair += frac;
        else
            f_fvalueOfNotFair -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_vOfFStranger += frac;
        else
            m_vOfFStranger -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_vOfFStranger += frac;
        else
            f_vOfFStranger -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_mvalueOfNoH += frac;
        else
            m_mvalueOfNoH -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_mvalueOfNoH += frac;
        else
            f_mvalueOfNoH -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_mvalueOfNoCT += frac;
        else
            m_mvalueOfNoCT -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_mvalueOfNoCT += frac;
        else
            f_mvalueOfNoCT -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_bravery += frac;
        else
            m_bravery -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_bravery += frac;
        else
            f_bravery -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_gratitude += frac;
        else
            m_gratitude -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_gratitude += frac;
        else
            f_gratitude -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_vengefulness += frac;
        else
            m_vengefulness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_vengefulness += frac;
        else
            f_vengefulness -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            m_tmFct += frac;
            if(m_tmFct > 1.0)
                m_tmFct = 1.0;
        } else{
            m_tmFct -= frac;
            if(m_tmFct < 0.0)
                m_tmFct = 0.0;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            f_tmFct += frac;
            if(f_tmFct > 1.0)
                f_tmFct = 1.0;
        } else{
            f_tmFct -= frac;
            if(f_tmFct < 0.0)
                f_tmFct = 0.0;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_friendSel += frac;
        else
            m_friendSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_friendSel += frac;
        else
            f_friendSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_energySel += frac;
        else
            m_energySel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_energySel += frac;
        else
            f_energySel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_childSel += frac;
        else
            m_childSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_childSel += frac;
        else
            f_childSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_motherSel += frac;
        else
            m_motherSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_motherSel += frac;
        else
            f_motherSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_siblingSel += frac;
        else
            m_siblingSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_siblingSel += frac;
        else
            f_siblingSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2){
            mutationRate += frac;
            if(mutationRate > 1.0)
                mutationRate = 1.0;
        } else{
            mutationRate -= frac;
            if(mutationRate < 0.3)
                mutationRate = 0.3;
        }
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_bnvlcTOtherSex += frac;
        else
            m_bnvlcTOtherSex -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_bnvlcTOtherSex += frac;
        else
            f_bnvlcTOtherSex -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_bnvlcTSameSex += frac;
        else
            m_bnvlcTSameSex -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_bnvlcTSameSex += frac;
        else
            f_bnvlcTSameSex -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_kidValueForMale += (m_kidValueForMale * frac);
        else
            m_kidValueForMale -= (m_kidValueForMale * frac);
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_kidValueForMale += (f_kidValueForMale * frac);
        else
            f_kidValueForMale -= (f_kidValueForMale * frac);
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_moV += (m_moV * frac);
        else
            m_moV -= (m_moV * frac);
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_moV += (f_moV * frac);
        else
            f_moV -= (f_moV * frac);
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            kidV += (kidV * frac);
        else
            kidV -= (kidV * frac);
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_valueOfNoSex += frac;
        else
            m_valueOfNoSex -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_valueOfNoSex += frac;
        else
            f_valueOfNoSex -= frac;
    }

    // Features relevant only for either one sex or another
    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            bestMaleAge += (int)(bestMaleAge * frac);
        else
            bestMaleAge -= (int)(bestMaleAge * frac);
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            femalePromiscuity += frac;
        else
            femalePromiscuity -= frac;
        if(femalePromiscuity > 1.0)
            femalePromiscuity = 1.0;
        else
            if(femalePromiscuity < 0.0)
                femalePromiscuity = 0.0;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < frac)
        fearOfHPWhenHasKid = !fearOfHPWhenHasKid;

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            maleAgeImportance += frac;
        else
            maleAgeImportance -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            maleEnergyImportance += frac;
        else
            maleEnergyImportance -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            oestrFemSel += frac;
        else
            oestrFemSel -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            f_meatValue += frac;
        else
            f_meatValue -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            m_meatValue += frac;
        else
            m_meatValue -= frac;
    }

    r = getRandDouble(0.0, 1.0);
    if(r < mutationRate){
        if(r < mr2)
            askMeatOnly = true;
        else
            askMeatOnly = false;
    }
}


void Agent::readMomMemory()
{
    double value;
    Remembrance *tmp = mother->rootMemory;
    Agent *ag;
    int tht = mother->huntInterval;
    int n = 1, d1, d2;
    while(tmp){
        ag = getAgPointer(tmp->name);
        if(ag){
            if(x > ag->x)
                d1 = x - ag->x;
            else
                d1 = ag->x - x;
            if(y > ag->y)
                d2 = y - ag->y;
            else
                d2 = ag->y - y;
            if(d1 <= MaxVision && d2 <= MaxVision){ // Remember only agents that aren't too far.
                value = recall(tmp->name, mother->rootMemory, mother->tmFct,
                        mother->vengefulness, mother->gratitude, mother->gratStrategy,
                        mother->vengStrategy, mother->vengStr2);
                if(value != 0.0 && value != UNKNOWN_AGENT){
                    storeInMemory(value, tmp->name, 'r', 3);
                    value = recall(mother->name, ag->rootMemory, ag->tmFct,
                            ag->vengefulness, ag->gratitude, ag->gratStrategy, ag->vengStrategy, ag->vengStr2);
                    if(value != 0.0 && value != UNKNOWN_AGENT)
                        ag->storeInMemory(value, name, 'g', 14);
                }
                tht += ag->huntInterval;
                n++;
            }
        }
        tmp = tmp->next;
    }
    huntInterval = tht / n;
}

void Agent::becomeAdult()
{
#ifdef DEBUG
    if(mother != NULL && mother == father)
        FATALERR("(mother != NULL && mother == father)");
#endif
    metabolism *= 2;
    isKid = false;
    if(mother){
        mother->kid = NULL;
        readMomMemory();
        storeInMemory(moV, motherName, 'r', 4);
        mother->storeInMemory(kidV, name, 'g', 5);
    } else{
        huntInterval = 10;
    }
    if(father)
        father->kid = NULL;
}

void Agent::learnGeography()
{
    Cell *c;
    Patch *p1, *p2 = NULL;
    int d, i, k, n;
    int *xl = (int*)malloc(MaxVision * 16 * sizeof(int));
    int *yl = (int*)malloc(MaxVision * 16 * sizeof(int));

    for(k = 1; k < 2; k++){
        i = 0;
        d = MaxVision * k;
        n = theModel->getPerimeterFor(x, y, d, xl, yl, false);
        while(i < n){
            //c = theModel->cellGrid[xl[i], yl[i]];
            c = cellGrid[xl[i]][yl[i]];
            p1 = c->patch;
            if(p1 && p1 != p2){
                storePatch(p1);
                p2 = p1;
            }
            i++;
        }
    }
    free(xl);
    free(yl);
}

void Agent::makeFirstFriends()
{
    Agent *ag;
    myCell->aglist->begin();
    while((ag = (Agent*)myCell->aglist->next()))
        storeInMemory(0.01, ag->name, 'r', 1);
    myCell->aglist->end();
}

void Agent::registerHost(Patch *p)
{
    TreePatch *tp = knownPatches;
    bool knew = false;
    while(tp && !knew){
        if(tp->patch == p){
            knew = true;
            tp->isHostile = true;
        }
        tp = tp->next;
    }
    if(!knew){
        tp = (TreePatch*)malloc(sizeof(TreePatch));
        tp->patch = p;
        tp->lastVisit = age;
        tp->isHostile = true;
        tp->next = knownPatches;
        knownPatches = tp;
    }
}

void Agent::storePatch(Patch *p)
{
#ifdef DEBUG
    if(p == NULL)
        FATALERR("IMPOSSIBLE!");
#endif
    TreePatch *tp = knownPatches;
    bool knew = false;
    while(tp && !knew){
        if(tp->patch == p){
            knew = true;
            tp->lastVisit = age;
        }
        tp = tp->next;
    }
    if(!knew){
        tp = (TreePatch*)malloc(sizeof(TreePatch));
        tp->patch = p;
        tp->lastVisit = age;
        tp->next = knownPatches;
        tp->isHostile = false;
        knownPatches = tp;
    }
}

bool Agent::answerMoveInvitationFrom(Agent *ag)
{
#ifdef DEBUG
    if(isKid)
        FATALERR("IMPOSSIBLE!");
#endif
    if((ag->sex == 'f' && acceptMoveInvtFromF == false) ||
            (ag->sex == 'm' && acceptMoveInvtFromM == false))
        return false;
    double myV = evaluateCell(myCell);
    double otV = evaluateCell(ag->myCell);
    if(otV > myV){
        moveTowardsX(ag->x, ag->y);
        return true;
    }
    return false;
}

/* Invite friends that before the last movement were in adjacent cells, but that
 * now are more than one cell distant from self. */
void Agent::inviteFriendsToMoveFrom(int oldX, int oldY)
{
    int i, n = 0;
    int xl[5], yl[5]; // List of coordinates of cells that no longer are adjacent
    if(x > oldX){
        if(y == oldY){  // (x > oldX && y == oldY)
            if(oldX > 0){
                xl[n] = oldX - 1;
                yl[n] = oldY;
                n++;
                if(oldY > 0){
                    xl[n] = oldX - 1;
                    yl[n] = oldY - 1;
                    n++;
                }
                if(oldY < (WorldYSize - 1)){
                    xl[n] = oldX - 1;
                    yl[n] = oldY + 1;
                    n++;
                }
            }
        } else
            if(y > oldY){  // (x > oldX && y > oldY)
                if(oldX > 0){
                    if(oldY < (WorldYSize - 1)){
                        xl[n] = oldX - 1;
                        yl[n] = oldY + 1;
                        n++;
                    }
                    xl[n] = oldX - 1;
                    yl[n] = oldY;
                    n++;
                    if(oldY > 0){
                        xl[n] = oldX - 1;
                        yl[n] = oldY - 1;
                        n++;
                    }
                }
                if(oldY > 0){
                    xl[n] = oldX;
                    yl[n] = oldY - 1;
                    n++;
                    if(oldX < (WorldXSize - 1)){
                        xl[n] = oldX + 1;
                        yl[n] = oldY - 1;
                        n++;
                    }
                }
            } else{  // (x > oldX && y < oldY)
                if(oldX > 0){
                    if(oldY > 0){
                        xl[n] = oldX - 1;
                        yl[n] = oldY - 1;
                        n++;
                    }
                    xl[n] = oldX - 1;
                    yl[n] = oldY;
                    n++;
                    if(oldY < (WorldYSize - 1)){
                        xl[n] = oldX - 1;
                        yl[n] = oldY + 1;
                        n++;
                    }
                }
                if(oldY < (WorldYSize - 1)){
                    xl[n] = oldX;
                    yl[n] = oldY + 1;
                    n++;
                    if(oldX < (WorldXSize -1)){
                        xl[n] = oldX + 1;
                        yl[n] = oldY + 1;
                        n++;
                    }
                }
            }
    } else
        if(x < oldX){
            if(y == oldY){  // (x < oldX && y == oldY)
                if(oldX < (WorldXSize - 1)){
                    xl[n] = oldX + 1;
                    yl[n] = oldY;
                    n++;
                    if(oldY > 0){
                        xl[n] = oldX + 1;
                        yl[n] = oldY - 1;
                        n++;
                    }
                    if(oldY < (WorldYSize - 1)){
                        xl[n] = oldX + 1;
                        yl[n] = oldY + 1;
                        n++;
                    }
                }
            } else
                if(y > oldY){  // (x < oldX && y > oldY)
                    if(oldY > 0){
                        if(oldX > 0){
                            xl[n] = oldX - 1;
                            yl[n] = oldY - 1;
                            n++;
                        }
                        xl[n] = oldX;
                        yl[n] = oldY - 1;
                        n++;
                        if(oldX < (WorldXSize - 1)){
                            xl[n] = oldX + 1;
                            yl[n] = oldY - 1;
                            n++;
                        }
                    }
                    if(oldX < (WorldXSize - 1)){
                        xl[n] = oldX + 1;
                        yl[n] = oldY;
                        n++;
                        if(oldY < (WorldYSize - 1)){
                            xl[n] = oldX + 1;
                            yl[n] = oldY + 1;
                            n++;
                        }
                    }
                } else{  // (x < oldX && y < oldY)
                    if(oldY < (WorldYSize - 1)){
                        if(oldX > 0){
                            xl[n] = oldX - 1;
                            yl[n] = oldY + 1;
                            n++;
                        }
                        xl[n] = oldX;
                        yl[n] = oldY + 1;
                        n++;
                        if(oldX < (WorldXSize - 1)){
                            xl[n] = oldX + 1;
                            yl[n] = oldY + 1;
                            n++;
                        }
                    }
                    if(oldX < (WorldXSize - 1)){
                        xl[n] = oldX + 1;
                        yl[n] = oldY;
                        n++;
                        if(oldY > 0){
                            xl[n] = oldX + 1;
                            yl[n] = oldY - 1;
                            n++;
                        }
                    }
                }
        } else{
            if(y > oldY && oldY > 0){  // (x == oldX && y > oldY)
                if(oldX > 0){
                    xl[n] = oldX - 1;
                    yl[n] = oldY - 1;
                    n++;
                }
                xl[n] = oldX;
                yl[n] = oldY - 1;
                n++;
                if(oldX < (WorldXSize - 1)){
                    xl[n] = oldX + 1;
                    yl[n] = oldY - 1;
                    n++;
                }
            } else{  // (x == oldX && y < oldY)
                if(y < oldY && oldY < (WorldYSize - 1)){
                    if(oldX > 0){
                        xl[n] = oldX - 1;
                        yl[n] = oldY + 1;
                        n++;
                    }
                    xl[n] = oldX;
                    yl[n] = oldY + 1;
                    n++;
                    if(oldX < (WorldXSize - 1)){
                        xl[n] = oldX + 1;
                        yl[n] = oldY + 1;
                        n++;
                    }
                }
            }
        }

    Cell *c;
    Agent *ag;
    int nag;
    double v;
    for(i = 0; i < n; i++){
        c = cellGrid[xl[i]][yl[i]];
        nag = c->aglist->getCount();
        if(nag){
            Agent **aa = (Agent**)malloc(nag * sizeof(void*));
            c->aglist->begin();
            int na = 0;
            while((ag = (Agent*)c->aglist->next())){
                aa[na] = ag;
                na++;
            }
            c->aglist->end();
#ifdef DEBUG
            if(na != nag)
                FATALERR("IMPOSSIBLE!");
#endif
            for(int k = 0; k < na; k++){
                v = recall(aa[k]->name, rootMemory, tmFct, vengefulness,
                        gratitude, gratStrategy, vengStrategy, vengStr2);
                if(v > 0.0)
                    aa[k]->answerMoveInvitationFrom(this);
            }
            free(aa);
        }
    }

#ifdef DEBUG
    if(n != 3 && oldX > 0 && oldY > 0 && oldX < (WorldXSize - 1) &&
            oldY < (WorldYSize - 1)
            && (oldY == y || oldX == x))
        FATALERR("IMPOSSIBLE!");
    if(n < 5 && oldX > 0 && oldY > 0 && oldX < (WorldXSize - 1) &&
            oldY < (WorldYSize - 1)
            && oldY != y && oldX != x)
        FATALERR("IMPOSSIBLE!");
    if(n > 5)
        FATALERR("IMPOSSIBLE!");
    if(n == 0)
        if(!((oldX == 0 && oldY == 0) ||
                    ((oldX == 0 || oldX == (WorldXSize - 1)) && (oldY - y) == 0) ||
                    ((oldY == 0 || oldY == (WorldYSize - 1)) && (oldX - x) == 0) ||
                    (oldX == (WorldXSize - 1) && oldY == 0) ||
                    (oldY == (WorldYSize - 1) && oldX == 0) ||
                    (oldX == (WorldXSize - 1) && oldY == (WorldYSize - 1))))
            FATALERR("IMPOSSIBLE!");
    if((oldX == 0 && oldY == 0) || (oldX == (WorldXSize - 1) &&
                oldY == (WorldYSize - 1)))
        if(n != 0)
            FATALERR("IMPOSSIBLE!");
    if(oldX == x && oldY == y)
        FATALERR("IMPOSSIBLE!");
#endif
}

void Agent::moveToX(int newX, int newY)
{
#ifdef DEBUG
    if(x == newX && y == newY)
        FATALERR("IMPOSSIBLE!");
    if(x < 0){
        dumpInfoAboutMe();
        FATALERR("Impossible to move a dead agent: %c, %d",
                sex, age);
    }
#endif
    Patch *p1 = myPatch;
    myCell->removeAgent(this);
    x = newX;
    y = newY;
    myCell = cellGrid[x][y];
    myCell->addAgent(this);
    myPatch = myCell->patch;
    if(p1 && p1 != myPatch) // Store patch in memory when leaving it.
        storePatch(p1);
}

void Agent::flyFromX(int bx, int by, Patch *p)
{
    if(followed)
        printf(_("I'm flying from (%d %d)\n"), bx, by);
    int goodX, goodY;
    if(bx > x)
        goodX = x - NearView;
    else
        goodX = x + NearView;
    if(by > y)
        goodY = y - NearView;
    else
        goodY = y + NearView;
    NORMALIZE(goodX, goodY);
    registerHost(p);
    if(goodX == x && goodY == y){ // The agent is in the corner of the world.
        migrating = 4;
        migStep = 0;
        migX = getRandInt(0, (WorldXSize - 1));
        migY = getRandInt(0, (WorldYSize - 1));
    } else{
        moveToX(goodX, goodY);
        migrating = 0;
        migStep = 0;
    }
}

// This function doesn't check whether "theX" and "theY" are valid values.
// This verification must already have been done by the caller function.
void Agent::moveTowardsX(int theX, int theY)
{
    int goodX, goodY;
    if((theX - x) > 0)
        goodX = x + 1;
    else
        if((x - theX) > 0)
            goodX = x - 1;
        else
            goodX = x;
    if((theY - y) > 0)
        goodY = y + 1;
    else
        if((y - theY) > 0)
            goodY = y - 1;
        else
            goodY = y;
    moveToX(goodX, goodY);
}

// Children simply follow their mothers.
void Agent::childStep()
{
    double minE = MinEnergy * (double)age  / (double)childhood;
    if(energy <= minE){
        goToReaperQueue(4);
        return;
    }
    if(age == childhood){
        becomeAdult();
        return;
    }
    if(!mother)
        return;
    if(mother->x != x || mother->y != y)
        moveTowardsX(mother->x, mother->y);
#ifdef DEBUG
    if(migrating)
        FATALERR("IMPOSSIBLE!");
#endif
}

double Agent::evaluatePatch(Patch *p)
{
    // Calculate when the agent will be in the patch
    int dx, dy, day, treeSp;
    p->getCenterX(&dx, &dy);
    dx = abs(x - dx);
    dy = abs(y - dy);
    if(dx > dy)
        day = Day + (dx / DayDuration);
    else
        day = Day + (dy / DayDuration);
    if(day >= YearDuration)
        day -= YearDuration;

    //Check the perspective of fruit availability
    treeSp = p->trSpecies;
    if(day > TreeSeasonBegin[treeSp] && day < TreeSeasonEnd[treeSp]){
        int tempo = TreeSeasonEnd[treeSp] - day;
        int nfruits = tempo * TreeNFruitsDay[treeSp];
        nfruits *= p->nTrees;
        double value = (double)nfruits * TreeFruitEnergy[treeSp];
        return value;
    }
    return 0.0;
}

// Get list of friends of same cell and adjacent ones
Friend* Agent::getFriendsAtDistance(int  d, int *nf)
{
    int i, n = 0;
    for(i = 1; i <= d; i++)
        n += i;
    List **ll = (List**)malloc((1 + (n * 8)) * sizeof(void*));
    ll[0] = myCell->aglist;

    int distance = 1, j = 1, k, max = 20;
    Cell **cc;
    while(distance <= d){
        cc = myCell->perimeter[distance];
        n = myCell->periLen[distance];
        k = 0;
        do{
            ll[j] = cc[k]->aglist;
            j++;
            k++;
        } while(k < n);
        distance++;
    }

    Friend *friends = (Friend*)malloc(max * sizeof(Friend));
    Agent *ag;
    n = 0;
    double value;
    for(i = 0; i < j; i++){
        ll[i]->begin();
        while((ag = (Agent*)ll[i]->next())){
            if(ag == this || ag->isKid)
                continue;
            value = recall(ag->name, rootMemory, tmFct, vengefulness,
                    gratitude, gratStrategy, vengStrategy, vengStr2);
            if(value == UNKNOWN_AGENT){
                if(ag->sex == 'f')
                    value = vOfFStranger;
                else
                    value = vOfMStranger;
            }
            if(value >= 0.0){
                if(n == (max - 1)){
                    max *= 2;
                    friends = (Friend*)realloc(friends, sizeof(Friend) * max);
                }
                friends[n].a = ag;
                friends[n].value = value;
                n++;
            }
        }
        ll[i]->end();
    }
    free(ll);
    if(n > 1)
        qsort(friends, n, sizeof(Friend), comparefriends);

    *nf = n;
    return friends;
}

int Agent::migToBestNearCell()
{
    int distance, n, k;
    int mx = -100;
    int my = -100;
    Cell **cc;
    distance = 2;
    double value, bestV = 0.0;
    do{
        cc = myCell->perimeter[distance];
        n = myCell->periLen[distance];
        k = 0;
        do{
            value = evaluateCell(cc[k]);
            if(value > bestV){
                mx = cc[k]->x;
                my = cc[k]->y;
                bestV = value;
            }
            k++;
        } while(k < n);
        distance++;
    } while(distance < MaxVision);

    if(mx >= 0){
        int dx = abs(mx - x);
        int dy = abs(my - y);
        if(dx > dy)
            value = bestV / dx;
        else
            value = bestV / dy;
        if(value > metabolism){
            migX = mx;
            migY = my;
            createGroupToMig(1);
            return 1;
        }
    }
    return 0;
}

double Agent::agMigValue(Agent *ag)
{
    double v = recall(ag->name, rootMemory, tmFct, vengefulness,
            gratitude, gratStrategy, vengStrategy, vengStr2) / 2.0;
    if(v == UNKNOWN_AGENT){
        if(ag->sex == 'f')
            v = vOfFStranger;
        else
            v = vOfMStranger;
    }
    v *= migFriendImportance;
    v += migAgeImportance * ((double)ag->age / (double)age);
    return v;
}

void Agent::beginMigration()
{
#ifdef DEBUG
    if(migPrefs[3] != 3)
        FATALERR("IMPOSSIBLE!");
    int t;
    t = migPrefs[0];
    t += migPrefs[1];
    t += migPrefs[2];
    if(t != 3)
        FATALERR("IMPOSSIBLE!");
#endif
    int i, n, k, r, distance;
    double value, bestV;
    migX = x;
    migY = y;
    bool fearOfHP;

    // Do four attempts of choosing a place to migrate. At most three attempts
    // will fail and, in this case, the agent will migrate to a random place.
    // migPrefs[3] isn't subject to mutation.
    for(i = 0; i < 4; i++){
        switch(migPrefs[i]){
            case 0 :
                // Check whether there is a known patch that is producing fruits.
                if(knownPatches){
                    bestV = 0.0;
                    Patch *p, *bestPatch = NULL;
                    TreePatch *tmp = knownPatches;
                    if(sex == 'f' && kid)
                        fearOfHP = fearOfHPWhenHasKid;
                    else
                        fearOfHP = fearOfHPatches;
                    while(tmp){
                        p = tmp->patch;
                        if(p != myPatch && !(tmp->isHostile && fearOfHP)){
                            value = evaluatePatch(p);
                            if(value > bestV){ // Do not migrate to any patch if none of
                                bestV = value;   // them will have fruits.
                                bestPatch = p;
                            }
                        }
                        tmp = tmp->next;
                    }
                    if(bestPatch){
                        bestPatch->getCenterX(&migX, &migY);
                        createGroupToMig(2);
                        return;
                    }
                }
                break;

            case 1 :
                // Look the migration direction of other agents and follow one of them.
                // (a): look for someone in the current cell
                {
                    bestV = 0.0;
                    Agent *neighbor, *bestN = NULL;
                    List *list = myCell->aglist;
                    list->begin();
                    while((neighbor = (Agent*)list->next())){
                        if(neighbor != this && neighbor->migrating){
                            value = agMigValue(neighbor);
                            if(value > bestV){
                                bestV = value;
                                bestN = neighbor;
                            }
                        }
                    }
                    list->end();
                    // (b): look for someone nearby
                    distance = 1;
                    Cell **cc;
                    do{
                        cc = myCell->perimeter[distance];
                        n = myCell->periLen[distance];
                        k = 0;
                        do{
                            if(cc[k]->getNAgents()){
                                list = cc[k]->aglist;
                                list->begin();
                                while((neighbor = (Agent*)list->next())){
                                    if(neighbor->migrating){
                                        value = agMigValue(neighbor);
                                        if(value > bestV){
                                            bestV = value;
                                            bestN = neighbor;
                                        }
                                    }
                                }
                                list->end();
                            }
                            k++;
                        } while(k < n);
                        distance++;
                    } while(distance < MaxVision);
                    if(bestN){
                        bestN->getMigX(&migX, &migY);
                        createGroupToMig(3);
                        return;
                    }
                }
                break;

            case 2:
                // Look for a good cell at most MaxVision distance.
                if(migToBestNearCell())
                    return;
                break;

            case 3:
                // Choose a random place more distant than MaxVision.
                r = getRandInt(MaxVision + 1, MaxVision * 2);
                if(getRandInt(0, 1000) > 500)
                    migX = x + r;
                else
                    migX = x - r;
                r = getRandInt(MaxVision + 1, MaxVision * 2);
                if(getRandInt(0, 1000) > 500)
                    migY = y + r;
                else
                    migY = y - r;
                NORMALIZE(migX, migY);
                createGroupToMig(4);
                return;
                break;
        }
    }
#ifdef DEBUG
    for(i = 0; i < 4; i++)
        printf("%d ", migPrefs[i]);
    printf("\n%s %p %d %c %f\n", name, (void*)this, age, sex, energy);
    fflush(stdout);
    FATALERR("IMPOSSIBLE!");
#endif
}

void Agent::createGroupToMig(int m)
{
#ifdef DEBUG
    if(isKid)
        FATALERR("IMPOSSIBLE!");
    if(amILeader)
        FATALERR("IMPOSSIBLE!");
    if(myAlliance) // Only create an alliance if not already in one.
        FATALERR("IMPOSSIBLE!");
#endif

    myAlliance = new Alliance();
    myAlliance->setLeader(this, name, energy);
    migrating = m;

    if(followed){
        printf(_("I'm migrating to (%d %d) "), migX, migY);
        switch(migrating){
            case 1 :
                printf(_("[Best cell]\n"));
                break;
            case 2 :
                printf(_("[Best patch]\n"));
                break;
            case 3 :
                printf(_("[Following other]\n"));
                break;
            case 4 :
                printf(_("[Random place]\n"));
                break;
            case 5 :
                printf(_("[Accepted invitation]\n"));
                break;
            default:
                printf("[IMPOSSIBLE]\n");
        }
    }

    // Get list of friends of same cell and adjacent ones
    int nf, i;
    Friend *friends = getFriendsAtDistance(AllianceRadius, &nf);

    // invite friends to form alliance
    for(i = 0; i < nf; i++)
        if(friends[i].a->myAlliance != myAlliance && friends[i].a->myPatch == myPatch)
            friends[i].a->answerGroupMigInvit(myAlliance, m, migX, migY);
    free(friends);
    delete(myAlliance);
}

void Agent::answerGroupMigInvit(Alliance *alli, int m, int mx, int my)
{
#ifdef DEBUG
    if(myAlliance) // myAlliance will alway be NULL unless the code is changed.
        FATALERR("IMPOSSIBLE!");
    if(amILeader && STRCMP(name, alli->ldName) != 0)
        FATALERR("IMPOSSIBLE!");
#endif
    if(acceptMigInv == 0)
        return;
    if((lastEnergy - energy) >= lowDeficit)
        return;
    //  if(Language && m == 4) // Don't go to a random place
    //    return;
    if(amILeader)
        return;

    // Memorize patch if do not know it yet
    if(Language){
        Patch *p = cellGrid[mx][my]->patch;
        if(p){
            TreePatch *tmp = knownPatches;
            bool knownPatch = false;
            while(tmp){
                if(p == tmp->patch){
                    knownPatch = true;
                    break;
                }
                tmp = tmp->next;
            }
            if(!knownPatch)
                storePatch(p);
        }
    }

    if(fearOfHPatches || (sex == 'f' && fearOfHPWhenHasKid && kid != NULL)){
        Patch *p = cellGrid[mx][my]->patch;
        TreePatch *tmp = knownPatches;
        while(tmp){
            if(p == tmp->patch && tmp->isHostile)
                return;
            tmp = tmp->next;
        }
    }
    Agent *ag;
    double value = 0.0;
    alli->allies->begin();
    while((ag = (Agent*)alli->allies->next())){
        value += agMigValue(ag);
    }
    alli->allies->end();
    if(value > 0.0){
        if(myAlliance != NULL && STRCMP(name, myAlliance->ldName) != 0){
            double v2 = 0.0;
            myAlliance->allies->begin();
            while((ag = (Agent*)myAlliance->allies->next())){
                v2 += agMigValue(ag);
            }
            myAlliance->allies->end();
            if(value > v2)
                myAlliance->removeAlly(this, energy);
            else
                return;
        }
        myAlliance = alli;
        alli->addAlly(this, energy);
        migrating = 5;
        migX = mx;
        migY = my;

        if(followed)
            printf(_("I'm migrating to (%d %d), [Accepted invitation]\n"), migX, migY);

        int nf, i;
        Friend *friends = getFriendsAtDistance(AllianceRadius, &nf);
        for(i = 0; i < nf; i++)
            if(friends[i].a->myAlliance != myAlliance && friends[i].a->myPatch == myPatch)
                friends[i].a->answerGroupMigInvit(myAlliance, m, migX, migY);
        free(friends);
    }
}

void Agent::migrationStep()
{
    if(migX == x && migY == y){
        migrating = 0;
        migStep = 0;
        return;
    }

    int goodX, goodY;
    if(migX > x)
        goodX = x + 1;
    else
        if(migX < x)
            goodX = x - 1;
        else
            goodX = x;
    if(migY > y)
        goodY = y + 1;
    else
        if(migY < y)
            goodY = y - 1;
        else
            goodY = y;

    moveToX(goodX, goodY);
    migStep++;
    if(migStep == MaxVision){
        if(migrating == 4){
            migStep = 0;
            migToBestNearCell();
        } else{
            Cell **cc;
            int n, k, distance = 1;
            do{
                cc = myCell->perimeter[distance];
                n = myCell->periLen[distance];
                k = 0;
                do{
                    if(cc[k]->patch)
                        storePatch(cc[k]->patch);
                    k++;
                } while(k < n);
                distance++;
            } while(distance < MaxVision);
        }
    }
}

Frnd* Agent::sortMales()
{
    Agent *ag;
    double value, v;
    int i, j, nf, p = 0, di;
    int nPotPart = potentialPartners->getCount();
    Friend *partnersList = (Friend*)malloc(nPotPart * sizeof(Friend));
    Friend *fl = NULL;
    potentialPartners->begin();
    while((ag = (Agent*)potentialPartners->next())){
        partnersList[p].a = ag;

        value = recall(ag->name, rootMemory, tmFct, vengefulness, gratitude, gratStrategy, vengStrategy, vengStr2);
        if(value == UNKNOWN_AGENT)
            value = vOfMStranger;

        // Get the opinion of best female friend about this agent
        if(value == 0.0 && Language == 1){
            if(fl == NULL)
                fl = getFriendsAtDistance(1, &nf);
            for(i = 0; i < nf; i++){
                if(fl[i].a->sex == 'f'){
                    v = recall(ag->name, fl[i].a->rootMemory, 
                            fl[i].a->tmFct, fl[i].a->vengefulness, fl[i].a->gratitude,
                            fl[i].a->gratStrategy, fl[i].a->vengStrategy, fl[i].a->vengStr2);
                    if(v != 0.0 && v != UNKNOWN_AGENT){
                        value = v * adviceValue;
                        break;
                    }
                }
            }
        }

        if(value != 0.0){
            // How much energy does the agent have?
            if(value > 0.0)
                value *= (1.0 + (ag->energy * maleEnergyImportance / MaxEnergy));
            else
                value *= (1.0 - (ag->energy * maleEnergyImportance / MaxEnergy));

            // Is the agent too old or too young?
            di = abs(ag->age - bestMaleAge);
            if(value > 0.0)
                value *= (1.0 - ( maleAgeImportance * di / ag->maxAge));
            else
                value *= (1.0 + ( maleAgeImportance * di / ag->maxAge));

            // The values of distant agents are less important
            i = abs(x - ag->x) + 1;
            j = abs(y - ag->y) + 1;
            if(value > 0.0){
                if(i > j)
                    value /= (double)i;
                else
                    value /= (double)j;
            } else{
                if(i > j)
                    value *= (double)i;
                else
                    value *= (double)j;
            }
        }
#ifdef DEBUG
        if(ag->potentialPartners->contains(this) == false)
            FATALERR("IMPOSSIBLE!");
#endif
        partnersList[p].value = value;
        p++;
    }
    potentialPartners->end();
    if(fl)
        free(fl);
    if(nPotPart > 1)
        qsort(partnersList, nPotPart, sizeof(Friend), comparefriends);
    return partnersList;
}

void Agent::giveBirth(Agent *fa, double e)
{
    Agent *child = new Agent(this, fa, e);
    agList->addLast(child);
    if(child->sex == 'f')
        femaleList->addLast(child);
    else
        maleList->addLast(child);
    energy -= (2 * metabolism);
}

void Agent::femaleStep()
{
    if(kid){
        kid->receiveFromMom(metabolism);
        energy -= metabolism;
        return;
    }

    if(oestrus == 0){
        if(energy >= ReproEnergy && age < reproAge)
            oestrus = 1;
        return;
    }

    oestrus++;
    if(oestrus == 5){
        oestrus = 0;
        if(potentialPartners->getCount()){
            Friend *pPartners = sortMales();
            int nPotPart = potentialPartners->getCount();
            int nMates = (int)(round(pow(nPotPart, femalePromiscuity)));
            int i;

            // Before calculating the probabilities of being the father, we have to
            // convert negative values into positive ones.
            double minV = 0.0;
            for(i = 0; i < nMates; i++){
                if(pPartners[i].value < minV)
                    minV = pPartners[i].value;
            }
            if(minV < 0.0){
                minV = fabs(minV);
                for(i = 0; i < nMates; i++)
                    pPartners[i].value += minV;
            }

            double tValue = 0.0;
            for(i = 0; i < nMates; i++)
                tValue += pPartners[i].value;
            if(tValue == 0.0){
                tValue = nMates;
                for(i = 0; i < nMates; i++)
                    pPartners[i].value = 1.0;
            }
            double r = getRandDouble(0.0, tValue);
            double vv = pPartners[0].value;
            int chosen = 0;
            while(vv < r){
                chosen++;
#ifdef DEBUG
                if(chosen >= nMates){
                    fprintf(stderr, "\nchosen = %d, nMates = %d, femalePromiscuity = %f\n, nPotPart = %d\n",
                            chosen, nMates, femalePromiscuity, nPotPart);
                    for(i = 0; i < nMates; i++)
                        fprintf(stderr, "%f ", pPartners[i].value);
                    FATALERR("IMPOSSIBLE!");
                }
#endif
                vv += pPartners[chosen].value;
            } while(vv < r);
            // TODO: remember the child as a probable son/daughter.
            for(i = 0; i < nMates; i++){ // Be grateful to the probability of being the father.
                pPartners[i].a->storeInMemory((pPartners[i].a->kidValueForMale * pPartners[i].value / tValue),
                        name, 'r', 6);
                storeInMemory((kidValueForMale * pPartners[i].value / tValue),
                        pPartners[i].a->name, 'g', 7);
            }

            for(i = nMates; i < nPotPart; i++){
                pPartners[i].a->storeInMemory(pPartners[i].a->valueOfNoSex, name, 'r', 13);
                storeInMemory(valueOfNoSex, pPartners[i].a->name, 'g', 16);
            }

            giveBirth(pPartners[chosen].a, metabolism * 2);
            // FIXME 1: The probability of being the father should not be known
            // exactly.
            // FIXME 2: In the real life, female anthropoids do not choose with
            // whom to mate. Thus the males in potentialPartners list should
            // compete and form alliances to get access to the females.
            Agent *ag;
            potentialPartners->begin();
            while((ag = (Agent*)potentialPartners->next()))
            {
#ifdef DEBUG
                if(ag->potentialPartners->remove(this) == false)
                    FATALERR("IMPOSSIBLE!");
#else
                ag->potentialPartners->remove(this);
#endif
            }
            potentialPartners->end();
            potentialPartners->removeAll();
            free(pPartners);
        }
    }
}

void Agent::maleStep()
{
    int k, n, nag, distance = 1;
    Agent *neighbor = NULL;
    Cell **cc;
    List *list;

    nag = myCell->getNAgents();

    // Is there any female in oestrus in my cell?
    if(nag > 1){
        list = myCell->aglist;
        list->begin();
        while((neighbor = (Agent*)list->next())){
            if(neighbor != this && neighbor->sex == 'f' && neighbor->oestrus > 0
                    && potentialPartners->contains(neighbor) == false
                    && STRCMP(neighbor->name, motherName) != 0 ){
#ifdef DEBUG
                if(neighbor->potentialPartners->contains(this))
                    FATALERR("([neighbor containsPartner: this])");
#endif
                neighbor->potentialPartners->addFirst(this);
                potentialPartners->addFirst(neighbor);
            }
        }
        list->end();
    }

    // Is there any female in oestrus nearby?
    do{
        cc = myCell->perimeter[distance];
        n = myCell->periLen[distance];
        k = 0;
        do{
            nag = cc[k]->getNAgents();
            if(nag){
                list = cc[k]->aglist;
                list->begin();
                while((neighbor = (Agent*)list->next())){
                    if(neighbor->sex == 'f' && neighbor->oestrus > 0
                            && potentialPartners->contains(neighbor) == false){
#ifdef DEBUG
                        if(neighbor->potentialPartners->contains(this))
                            FATALERR("([neighbor containsPartner: this])");
#endif
                        neighbor->potentialPartners->addFirst(this);
                        potentialPartners->addFirst(neighbor);
                    }
                }
                list->end();
            }
            k++;
        } while(k < n);
        distance++;
    } while(distance < MaxVision);
}

void Agent::step()
{
#ifdef DEBUG
    if(age < 0)
        FATALERR("step: age < 0");
    if(deathCause)
        FATALERR("deathCause = %d", deathCause);
    if(amILeader)
        FATALERR("IMPOSSIBLE!");
#endif
    age++;
    energy -= metabolism;
    if(meatAge > 1){
        meatAge--;
        if(meat > 0.0){
            if(meat > (2 * metabolism)){
                meat -= 2 * metabolism;
                energy += 2 * metabolism;
            } else{
                energy += meat;
                meatAge = 0;
                meat = 0.0;
            }
        }
        if(energy > MaxEnergy)
            energy = MaxEnergy;
    }
    if(predationRisk()){
        goToReaperQueue(3);
        return;
    }
    if(age <= childhood){
        childStep();
        return;
    }
    if(age == maxAge){
        goToReaperQueue(1);
        return;
    }
    if(energy < MinEnergy){
        goToReaperQueue(2);
        return;
    }

    if(Territoriality && bravery > 0.0)
        patrolTerritory();

    if(sex == 'm')
        maleStep();
    else
        femaleStep();

    if(Hunt)
        hunt();

    if(FoodShare && (lastEnergy - energy) > lowDeficit)
        begForFood();

    if(!migrating && (lastEnergy - energy) > hiDeficit)
        beginMigration();

    if(migrating)
        migrationStep();
    else
        moveToBestCell();
    lastEnergy = energy;
    if(energy < MaxEnergy)
        eatPlant();	// It's important that eatPlant be the last action because
                        // it will be effective only after the activation of cells.
}

double Agent::evaluateCell(Cell *cell)
{
#ifdef DEBUG
    if(cell == NULL)
        FATALERR("IMPOSSIBLE!");
#endif
    int n_othersex = 0, n_samesex = 0, n_oestrus = 0,
        hasMother = 0, n_siblings = 0, n_children = 0, na;
    double friends = 0.0, v;
    Agent *ag;
    na = cell->getNAgents();

    if(cell != myCell) // The agent must count itself in the future
        na++;

    cell->aglist->begin();
    while((ag = (Agent*)cell->aglist->next())){
        if(ag == this)
            continue;

        if(STRCMP(ag->name, motherName) == 0)
            hasMother = 1;
        else
            if(STRCMP(ag->motherName, motherName) == 0)
                n_siblings++;
            else
                if(STRCMP(ag->motherName, name) == 0)
                    n_children++;

        v = recall(ag->name, rootMemory, tmFct, vengefulness,
                gratitude, gratStrategy, vengStrategy, vengStr2);
        if(v == UNKNOWN_AGENT){
            if(ag->sex == 'f')
                friends += vOfFStranger;
            else
                friends += vOfMStranger;
        } else{
            friends += v;
        }

        if(sex == ag->sex){
            n_samesex++;
        } else{
            n_othersex++;
            if(sex == 'm' && ag->sex == 'f' && ag->oestrus > 0)
                n_oestrus++;
        }
    }
    cell->aglist->end();
    double ce = cell->getEnergy();
    v = ce / (double)na;
    v *= 1 + ((MaxEnergy - energy) * energySel)/ MaxEnergy;
    if(na > 1){
        v *= 1 + (friends * friendSel);
        v *= 1 + (n_children * childSel);
        v *= 1 + (hasMother * motherSel);
        v *= 1 + (n_siblings * siblingSel);
        v *= 1 + (n_othersex * otherSexSel);
        v *= 1 + (n_samesex * sameSexSel);
        v *= 1 + (n_oestrus * oestrFemSel);
    }
    return v;
}

// Find the best adjacent cell. Stay put if the current cell is better than
// the adjacent ones.
void Agent::moveToBestCell()
{
    double v, bestV = 0.0;
    Cell *bestCell, **cc;

    bestCell = myCell;
    bestV = evaluateCell(bestCell);
    int bestX = x;
    int bestY = y;

    // Check whether an adjacent cell is good enough
    int i, n;
    cc = myCell->perimeter[1];
    n = myCell->periLen[1];

    for(i = 0; i < n; i++){
        v = evaluateCell(cc[i]);
        if(v > bestV){
            bestX = cc[i]->x;
            bestY = cc[i]->y;
            bestV = v;
            bestCell = cc[i];
        }
    }

    if(bestCell != myCell){
        int oldX = x;
        int oldY = y;
        moveToX(bestX, bestY);
        inviteFriendsToMoveFrom(oldX, oldY);
    } else{
        if(bestV == 0.0){
            bestX = x + getRandInt(-1, 1);
            bestY = y + getRandInt(-1, 1);
            NORMALIZE(bestX, bestY);
            if(!(bestX == x && bestY == y)){
                int oldX = x;
                int oldY = y;
                moveToX(bestX, bestY);
                inviteFriendsToMoveFrom(oldX, oldY);
            }
        }
    }
}

void Agent::eatPlant()
{
    double e;
    e = 2.0;	       // Limit to storage: there is no refrigerator to
    if((energy + e) > MaxEnergy) // conserve food, and the agent must grow slowly.
        e = MaxEnergy - energy;
#ifdef DEBUG
    if((energy + e) > MaxEnergy)
        FATALERR("%f  %f %f", e, energy, (MaxEnergy - energy));
#endif
    e = myCell->getEatenBy(e);
    energy += e;
    eFromDinner += e;
}

void Agent::receiveFood(double food)
{
#ifdef DEBUG
    if(food < 0.0)
        FATALERR("SimTime = %d, food = %f", SimTime, food);
#endif
    energy += food;
    if(energy > MaxEnergy){	// The agent might have received a conflict
        food = energy - MaxEnergy;	// reward after requesting food.
        energy = MaxEnergy;
    }
    eFromDinner += food;
}

int Agent::createAllianceToHunt()
{
#ifdef DEBUG
    if(isKid)
        FATALERR("IMPOSSIBLE!");
    if(amILeader)
        FATALERR("IMPOSSIBLE!");
    if(myAlliance) // Only create an alliance if not already in one.
        FATALERR("IMPOSSIBLE!");
#endif

    myAlliance = new Alliance();
    myAlliance->setLeader(this, name, energy);

    // Get list of friends of same cell and adjacent ones
    int nf;
    Friend *friends = getFriendsAtDistance(AllianceRadius, &nf);

    // invite friends to form alliance
    int i = 0, na = 1;
    double t = huntInterval;
    while(i < nf && na < maxHuntPatrolSize){
        if(friends[i].a->answerAllianceInvitationFrom(this, myAlliance, 'h')){
            t += friends[i].a->huntInterval;
            na++;
        } else{
            if(friends[i].a->sex == 'f'){
                storeInMemory(fvalueOfNoH, friends[i].a->name, 'r', 11);
                friends[i].a->storeInMemory(friends[i].a->fvalueOfNoH, name, 'g', 11);
            } else{
                storeInMemory(mvalueOfNoH, friends[i].a->name, 'r', 11);
                friends[i].a->storeInMemory(friends[i].a->mvalueOfNoH, name, 'g', 11);
            }
        }
        i++;
    }
    free(friends);
    if(getRandDouble(0.0, 1.0) > 0.5)
        return (int)(round(t / na));
    else
        return (int)(round((t - 0.0001) / na));
}

Alliance* Agent::createAllianceAgainst(Agent *enemy)
{
#ifdef DEBUG
    Agent *ag;
    if(enemy == this)
        FATALERR("IMPOSSIBLE!");
    if(isKid)
        FATALERR("IMPOSSIBLE!");
    if(myAlliance) // Only create an alliance if not already in one.
        FATALERR("IMPOSSIBLE!");
    if(STRCMP(name, enemy->name) == 0)
        FATALERR("IMPOSSIBLE!");
#endif
    double r = getRandDouble(0.0, 1.0);
    if(r > bravery)
        return NULL;

    myAlliance = new Alliance();
    myAlliance->setLeader(this, name, energy);

    // Get list of friends of same cell and adjacent ones
    int nf;
    Friend *friends = getFriendsAtDistance(AllianceRadius, &nf);

    int i = 0;
    while(i < nf){
#ifdef DEBUG
        char *fname = friends[i].a->name;
        if(STRCMP(fname, name) == 0 && this == friends[i].a)
            FATALERR("name == name : %s %p %p\n",
                    name, this, friends[i].a);
        Alliance *otherA;
        otherA = friends[i].a->myAlliance;
        if(otherA == myAlliance){
            int w,z;
            ag = (Agent*)myAlliance->leader;
            fprintf(stderr, "\nPerimeter cells of Leader of Alliance (%s %d %d)\n", ag->name, ag->x, ag->y);
            for(w = 1; w <= AllianceRadius; w++){
                fprintf(stderr, "d = %d (n = %d) : ", w, ag->myCell->periLen[w]);
                for(z = 0; z < ag->myCell->periLen[w]; z++)
                    fprintf(stderr, "(%d %d) ", ag->myCell->perimeter[w][z]->x, ag->myCell->perimeter[w][z]->y);
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "%s already is in this alliance (%p)\n", friends[i].a->name, (void*)myAlliance);
            otherA->allies->begin();
            while((ag = (Agent*)otherA->allies->next()))
                fprintf(stderr, "%s ", ag->name);
            otherA->allies->end(); printf("\n");
            myAlliance->allies->begin();
            while((ag = (Agent*)myAlliance->allies->next()))
                fprintf(stderr, "%s ", ag->name);
            myAlliance->allies->end(); printf("\n");
            ag = friends[i].a;
            FATALERR("%s |%p %p | %s %s | %d %d | %f %f",
                    myAlliance->ldName, this, ag, name, ag->name, age, ag->age, energy,
                    ag->energy);
        }
#endif

        if(enemy != friends[i].a){
            if(friends[i].a->myAlliance != myAlliance){
                if(friends[i].a->answerAllianceInvitationFrom(this, myAlliance, 't') == false){
                    if(friends[i].a->sex == 'f'){
                        storeInMemory(fvalueOfNoCT, friends[i].a->name, 'r', 8);
                        friends[i].a->storeInMemory(friends[i].a->fvalueOfNoCT, name, 'g', 8);
                        if(Norm)
                            nonCooperators->addFirst(friends[i].a);
                    } else{
                        storeInMemory(mvalueOfNoCT, friends[i].a->name, 'r', 8);
                        friends[i].a->storeInMemory(friends[i].a->mvalueOfNoCT, name, 'g', 8);
                    }
                }
            }
        }
        i++;
    }
    free(friends);
    return myAlliance;
}

bool Agent::answerAllianceInvitationFrom(Agent *ld, Alliance *a, char t)
{
#ifdef DEBUG
    if(myAlliance == a)
        FATALERR("IMPOSSIBLE!");
    if(isKid)
        FATALERR("IMPOSSIBLE!");
    if(STRCMP(ld->name, name) == 0)
        FATALERR("Repeated names \"%s\" Date:->%d:%02d:%02d()\n", name, Year, Day, Hour);
    if(amILeader)
        FATALERR("IMPOSSIBLE!");
#endif


    nInvt++;
    double p = recall(ld->name, rootMemory, tmFct, vengefulness, gratitude,
            gratStrategy, vengStrategy, vengStr2);
    if(p < 0.0 && p != UNKNOWN_AGENT)
        return false;
    if(p == 0.0 || p == UNKNOWN_AGENT){
        if(Language == 1){ // Get information about this agent before accepting it.
            int i, nf = 0;
            Friend *fl = getFriendsAtDistance(1, &nf);
            for(i = 0; i < nf; i++){
                p = recall(ld->name, fl[i].a->rootMemory,
                        fl[i].a->tmFct, fl[i].a->vengefulness, fl[i].a->gratitude,
                        fl[i].a->gratStrategy, fl[i].a->vengStrategy, fl[i].a->vengStr2);
                if(p != 0.0 && p != UNKNOWN_AGENT){
                    p *= adviceValue;
                    break;
                }
            }
            free(fl);
            if(p == UNKNOWN_AGENT){
                if(ld->sex == 'f')
                    p = vOfFStranger;
                else
                    p = vOfMStranger;
            }
            if(p < 0.0)
                return false;
        }
    }

    if(myAlliance){
        double v = recall(myAlliance->ldName, rootMemory, tmFct, vengefulness,
                gratitude, gratStrategy, vengStrategy, vengStr2);
        if(v == UNKNOWN_AGENT){
            if(((Agent*)(myAlliance->leader))->sex == 'f')
                v = vOfFStranger;
            else
                v = vOfMStranger;
        }
        if(v >= p){
            return false;
        } else{
#ifdef DEBUG
            Alliance *oldali = myAlliance;
#endif
            myAlliance->removeAlly(this, energy);
#ifdef DEBUG
            if(oldali->nallies == 0)
                FATALERR("IMPOSSIBLE!");
#endif
            myAlliance = a;
            a->addAlly(this, energy);
            nAlliChng++;
            return true;
        }
    }

    if(t == 'h' && ((SimTime - lastHunt) < (huntInterval / 2) || huntStrategy == 0)){
        return false;
    } else{
        double r = getRandDouble(0.0, 1.0);
        if(r > loyalty)
            return false;
    }

    myAlliance = a;
    a->addAlly(this, energy);
    nAcptInvt++;
    return true;
}

bool Agent::isHostile(Patch *p)
{
    TreePatch *tp = knownPatches;
    while(tp){
        if(tp->patch == p)
            return(tp->isHostile);
        tp = tp->next;
    }
    return false;
}

void Agent::askAboutPatches(Agent *ag)
{
    TreePatch *tp = knownPatches;
    while(tp){
        if(tp->isHostile == false)
            tp->isHostile = ag->isHostile(tp->patch);
        tp = tp->next;
    }
}

void Agent::rememberPatrolAllies(List *a)
{
#ifdef DEBUG
    Alliance *ma = myAlliance;
    if(a != ma->allies)
        FATALERR("IMPOSSIBLE!");
#endif
    Agent *ag;
    a->begin();
    while((ag = (Agent*)a->next())){
        if(ag != this)
            storeInMemory(patrolValue, ag->name, 'r', 9);
    }
    a->end();
}

void Agent::rememberPatrolRivals(List *r)
{
    Agent *ag;
    r->begin();
    while((ag = (Agent*)r->next())){
        storeInMemory((-1 * patrolValue), ag->name, 'r', 10);
    }
    r->end();
}


void Agent::rememberHuntPartners(List *mypartners)
{
    Agent *ag;
    mypartners->begin();
    while((ag = (Agent*)mypartners->next())){
        if(ag != this){
            storeInMemory(huntValue, ag->name, 'r', 12);
            ag->storeInMemory(huntValue, name, 'g', 12);
        }
    }
    mypartners->end();
}

Prey* Agent::seekPrey()
{
    Prey *prey;
    if(myCell->preylist->getCount() > 0){
        prey = (Prey*)(myCell->preylist->getFirst());
        return prey;
    }
    int i, d, np;
    for(d = 1; d <= NearView; d++){
        Cell **cc = myCell->perimeter[d];
        for(i = 0; i < myCell->periLen[d]; i++){
            np = cc[i]->preylist->getCount();
            if(np > 0){
                prey = (Prey*)(cc[i]->preylist->getFirst());
                return prey;
            }
        }
    }
    return NULL;
}

void Agent::distributeMeat(double preyMeat, int mhi)
{
    double meatPiece = fairness * preyMeat / (double)myAlliance->nallies;
    if(meatPiece < 0.0)
        meatPiece = 0.0;
    myAlliance->allies->begin();
    Agent *a;
    while((a = (Agent*)myAlliance->allies->next()))
        a->receiveMeat(meatPiece, fairness, mhi);
    myAlliance->allies->end();

    if(myAlliance->nallies > 1){
        Agent **aa = (Agent**)malloc(myAlliance->nallies * sizeof(void*));
        int n = 0;
        myAlliance->allies->begin();
        while((a = (Agent*)myAlliance->allies->next())){
            aa[n] = a;
            n++;
        }
        myAlliance->allies->end();
        for(int i = 0; i < n; i++)
            aa[i]->rememberHuntPartners(myAlliance->allies);
        free(aa);
    }
}

/* Hunt strategies:
 *  0 Never hunt
 *  1 Hunt only during migrations
 *  2 Hunt when invited and when huntInterval was reached
 * */
void Agent::hunt()
{
    if(huntStrategy == 0)
        return;

    if(huntStrategy == 1 && !(migrating))
        return;

    if((SimTime - lastHunt) < huntInterval)
        return;

    NHunts++;
    int meanHuntInterval = createAllianceToHunt();
    NHunters += myAlliance->nallies;

    Prey *prey = seekPrey();
    if(prey == NULL){
        Agent *ag;
        myAlliance->allies->begin();
        while((ag = (Agent*)myAlliance->allies->next())){
            if(ag->x != x && ag->y != y){
                prey = ag->seekPrey();
                if(prey){
                    break;
                }
            }
        }
        myAlliance->allies->end();
    }
    double preyMeat = 0.0;
    if(prey){
#ifdef DEBUG
        if(prey->x > WorldXSize)
            FATALERR("IMPOSSIBLE!");
#endif
        NFoundPreys++;
        preyMeat = prey->getHuntedBy(double(myAlliance->nallies));
        if(preyMeat > 0.0){
            delete(prey);
            NSuccHunts++;
        }
    }
    distributeMeat(preyMeat, meanHuntInterval);
    delete(myAlliance);
}

// Hunt process
//   - Form alliance.
//   - Calculate probability of catching the prey.
//   - If the hunt was successful:
//       - Kill the prey, distribute the food, distribute scores.
//       - Carry rapidly degrading unconsumed food which will be
//         available for share along with "energy".

void Agent::receiveMeat(double m, double ldFairness, int mhi)
{
    if(myAlliance->leader != this){
        if(myAlliance->leader->sex == 'f' && ldFairness < expFFairness)
            storeInMemory((fvalueOfNotFair * (expFFairness - ldFairness)),
                    myAlliance->leader->name, 'r', 19);
        else
            if(ldFairness < expMFairness)
                storeInMemory((mvalueOfNotFair * (expMFairness - ldFairness)),
                        myAlliance->leader->name, 'r', 19);
    }

    double huntCost;
    if(sex == 'f' && kid)
        huntCost = 2 * HuntCost;
    else
        huntCost = HuntCost;
    eFromHunt += m - huntCost;
    // meat is edible for MAXMEATAGE time steps (counted regressively).
    if(m > 0.0){
        if(meat > 0.0) // Calculate the remaining value of current meat
            meat = meat * meatAge / MAXMEATAGE;
        meat += m;
        meatAge = MAXMEATAGE;
    }
    if(huntInterval > mhi) // Cultural transmission of preferred interval between hunts.
        huntInterval--;
    else
        if(huntInterval < mhi)
            huntInterval++;
    if(m > huntCost){  // Individual reaction to (un)successful hunt.
        if(huntInterval > 1)
            huntInterval--;
    } else
        huntInterval++;
    energy -= huntCost;
    lastHunt = SimTime;
}

void Agent::patrolTerritory()
{
    Cell **cc;
    if(myPatch == NULL)
        return;
    if(xenophTM == false && xenophTF == false && xenophTFwK == false)
        return;
    if(Day < TreeSeasonBegin[myPatch->trSpecies] || Day >= TreeSeasonEnd[myPatch->trSpecies])
        return;
    int nc, na, d = 1, i, r, nn = 0, max = 50;
    Agent *neighbor, **nlist;
    nlist = (Agent**)malloc(max * sizeof(void*));
    double value;
    while(d <= NearView){
        cc = myCell->perimeter[d];
        nc = myCell->periLen[d];
        i = 0;
        while(i < nc){
            na = cc[i]->aglist->getCount();
            if(na){
                cc[i]->aglist->begin();
                while((neighbor = (Agent*)cc[i]->aglist->next())){
                    if(neighbor->isKid)
                        continue;
                    if(neighbor->sex == 'm' && xenophTM == false)
                        continue; // Don't attack males
                    if(neighbor->sex == 'f' && neighbor->kid == NULL && xenophTF == false)
                        continue; // Don't attack females
                    if(neighbor->sex == 'f' && neighbor->kid != NULL && xenophTFwK == false)
                        continue; // Don't attack females with kids
                    nlist[nn] = neighbor;
                    nn++;
                    if(nn == max){
                        max += 50;
                        nlist = (Agent**)realloc(nlist, max * sizeof(void*));
                    }
                }
                cc[i]->aglist->end();
            }
            i++;
        }
        d++;
    }
    for(i = 0; i < nn; i++){
        value = recall(nlist[i]->name, rootMemory, tmFct,
                vengefulness, gratitude, gratStrategy, vengStrategy, vengStr2);
        if(value == UNKNOWN_AGENT){
            if(nlist[i]->sex == 'f')
                value = vOfFStranger;
            else
                value = vOfMStranger;
        }
        if(value < enmityThr){
            r = theModel->runConflictBetween(this, nlist[i]);
            if(r == 2){    // My alliance lost the fight (and we flied)
                break;
            }
        }
    }
    free(nlist);
}


// Replace the oldest remembrance of the other's past action with the new one.
void Agent::storeInMemory(double value, char *oName, char type, char ev)
{
#ifdef VERBOSEDEBUG
    if(value > MaxGlobalStoreMem){
        fprintf(stderr, "\nMaxGlobalStoreMem: (%d) %f", ev, value);
        MaxGlobalStoreMem = value; fflush(stderr);
    } else
        if(value < MinGlobalStoreMem){
            fprintf(stderr, "\nMinGlobalStoreMem: (%d) %f", ev, value);
            MinGlobalStoreMem = value; fflush(stderr);
        }
#endif
#ifdef DEBUG
    if(value < -200.0 || value > 200.0 || isnan(value) || isinf(value))
        FATALERR("value = %f, type = %c, event = %d", value, type, ev);
#endif

    if(followed){
        char str[160];
        printf("%s %d ", name, SimTime);
        switch(ev){
            case 1 :
                sprintf(str, _("I received food:            "));
                break;
            case 2 :
                sprintf(str, _("I gave food:                "));
                break;
            case 3 :
                sprintf(str, _("Reading my mother's memory: "));
                break;
            case 4 :
                sprintf(str, _("My gratitude to my mother:  "));
                break;
            case 5 :
                sprintf(str, _("My child is adult now:      "));
                break;
            case 6 :
                sprintf(str, _("I received a child:         "));
                break;
            case 7 :
                sprintf(str, _("I gave a child:             "));
                break;
            case 8 :
                sprintf(str, _("No in territory confl. ivt: "));
                break;
            case 9 :
                sprintf(str, _("Patrol ally:                "));
                break;
            case 10 :
                sprintf(str, _("Patrol enemy:               "));
                break;
            case 11 :
                sprintf(str, _("No in hunt invitation:      "));
                break;
            case 12 :
                sprintf(str, _("Hunt partner:               "));
                break;
            case 13 :
                sprintf(str, _("No in sex proposal:         "));
                break;
            case 14 :
                sprintf(str, _("I was in a kid's mother mem:"));
                break;
            case 15 :
                sprintf(str, _("No in food share:           "));
                break;
            case 16 :
                sprintf(str, _("I said no in sex proposal:  "));
                break;
            case 17 :
                sprintf(str, _("I punished someone:         "));
                break;
            case 18 :
                sprintf(str, _("I was punished:             "));
                break;
            case 19 :
                sprintf(str, _("Not fair in meat distrib.:  "));
                break;
            case 20 :
                sprintf(str, _("Bold request for food:      "));
                break;
            default :
                FATALERR("IMPOSSIBLE: %d", ev);
                break;
        }
        Agent *ag = getAgPointer(oName);
        if(value < 0.0){
            fprintf(F, "%s%f  %c  %s  \n", str, value, ag->sex, oName);
            printf("%s%f  %c  %s  \n", str, value, ag->sex, oName);
        } else{
            fprintf(F, "%s %f  %c  %s  \n", str, value, ag->sex, oName);
            printf("%s %f  %c  %s  \n", str, value, ag->sex, oName);
        }
    }

    Remembrance *tmp, *parent = NULL;
    int i;
    if(rootMemory == NULL){ // Create the root memory
        rootMemory = new Remembrance();
        lastMemory = rootMemory;
        NAMECOPY(rootMemory->name, oName);
        rootMemory->firstInter = SimTime;
        tmp = rootMemory;
        i = 0;
        nRembr = 1;
    } else{ // Try to remember the agent
        int c = 0;
        tmp = rootMemory;
        do{
            c = nameComp(oName, tmp->name);
            if(c > 0){
                parent = tmp;
                tmp = tmp->right;
            } else
                if(c < 0){
                    parent = tmp;
                    tmp = tmp->left;
                } else{ // The agent was recalled. Now, find the less important remembrance.
                    int *a;
                    double *v;
                    if(type == 'g'){
                        a = tmp->givenTm;
                        v = tmp->given;
                    } else{
                        a = tmp->receivedTm;
                        v = tmp->received;
                    }
                    i = 0;
                    int k = 0;
                    double V = fabs(v[0] * pow(tmFct, (age - a[0])));
                    double bestV = V;
                    do{
                        if(V < bestV){ // get the index of the less important remembrance
                            i = k;
                            bestV = V;
                        }
                        k++;
                        if(k == MemSize)
                            break;
                        V = fabs(v[k] * pow(tmFct, (age - a[k])));
                    } while(1);
                    break;
                }
        } while(tmp);

        if(tmp == NULL){  // ...create a place for it.
#ifdef DEBUG
            if(c == 0)
                FATALERR("IMPOSSIBLE!");
            if(nameComp(rootMemory->name, oName) == 0)
                FATALERR("IMPOSSIBLE!");
#endif
            if(c > 0){
                parent->right = new Remembrance();
                tmp = parent->right;
            } else{
                parent->left = new Remembrance();
                tmp = parent->left;
            }
            nRembr++;
            lastMemory->next = tmp;	   // lastMemory is necessary for reading the memory
            lastMemory = tmp;		   // sequentially, starting from rootMemory, as in
            NAMECOPY(tmp->name, oName);  // readMomMemory, die, drawSelfOn, and getFrMeanDist
            tmp->firstInter = SimTime;
            i = 0;
        }
    }

    // Store the value
    switch(type){
        case 'g' :
            tmp->given[i] = value;
            tmp->givenTm[i] = SimTime;
            tmp->givenEv[i] = ev;
            break;
        case 'r' :
            tmp->received[i] = value;
            tmp->receivedTm[i] = SimTime;
            tmp->receivedEv[i] = ev;
            break;
    }
    tmp->nInteractions++;
}

// Beg for food to agents that are in the same cell
void Agent::begForFood()
{
    double value = 0.0, received, bestValue = 0.0;
    Agent *neighbor, *donor = NULL;
    myCell->aglist->begin();
    while((neighbor = (Agent*)myCell->aglist->next())){
        if(neighbor != this && !(neighbor->isKid)){
            // Did I give any favor to this neighbor before? (An empathetic calculus).
            // There are two strategies: use average values and use own values.
            switch(begStrategy){
                case 1:
                case 2:
                    if(askMeatOnly && neighbor->meat <= 0.0){
                        value = 0.0;
                    } else{
                        if(neighbor->sex == 'f'){
                            value = recall(name, neighbor->rootMemory,
                                    AverFTmFct, AverFVengefulness, AverFGratitude,
                                    gratStrategy, vengStrategy, vengStr2);
                            if(value == UNKNOWN_AGENT){
                                value = vOfFStranger;
                            }

                            // benevolence towards relatives
                            if(STRCMP(neighbor->motherName, name) == 0)
                                value += AverFBTMother;
                            else
                                if(STRCMP(neighbor->motherName, motherName) == 0)
                                    value += AverFBTSibling;
                                else
                                    if(STRCMP(neighbor->name, motherName) == 0)
                                        value += AverFBTChild;

                            // neighbor's benevolence toward my sex
                            if(sex == neighbor->sex)
                                value += AverFBTSameSex;
                            else
                                value += AverFBTOtherSex;

                            if(energy > neighbor->energy)
                                value -= AverFEnvy;
                            else
                                value += AverFPity;
                        } else{
                            value = recall(name, neighbor->rootMemory,
                                    AverMTmFct, AverMVengefulness, AverMGratitude,
                                    gratStrategy, vengStrategy, vengStr2);
                            if(value == UNKNOWN_AGENT){
                                value = vOfMStranger;
                            }

                            // benevolence towards relatives
                            if(STRCMP(neighbor->motherName, name) == 0)
                                value += AverMBTMother;
                            else
                                if(STRCMP(neighbor->motherName, motherName) == 0)
                                    value += AverMBTSibling;
                                else
                                    if(STRCMP(neighbor->name, motherName) == 0)
                                        value += AverMBTChild;

                            // neighbor's benevolence toward my sex
                            if(sex == neighbor->sex)
                                value += AverMBTSameSex;
                            else
                                value += AverMBTOtherSex;

                            if(energy > neighbor->energy)
                                value -= AverMEnvy;
                            else
                                value += AverMPity;
                        }
                    }
                    break;
                case 3:
                case 4:
                    if(askMeatOnly && neighbor->meat <= 0.0){
                        value = 0.0;
                    } else{
                        value = recall(name, neighbor->rootMemory,
                                tmFct, vengefulness, gratitude, gratStrategy,
                                vengStrategy, vengStr2);
                        if(value == UNKNOWN_AGENT){
                            if(neighbor->sex == 'f')
                                value = vOfFStranger;
                            else
                                value = vOfMStranger;
                        }

                        // benevolence towards relatives
                        if(STRCMP(neighbor->motherName, name) == 0)
                            value += bnvlcTMother;
                        else
                            if(STRCMP(neighbor->motherName, motherName) == 0)
                                value += bnvlcTSibling;
                            else
                                if(STRCMP(neighbor->name, motherName) == 0)
                                    value += bnvlcTChild;

                        // neighbor's benevolence toward my sex
                        if(sex == neighbor->sex)
                            value += bnvlcTSameSex;
                        else
                            value += bnvlcTOtherSex;

                        if(energy > neighbor->energy)
                            value -= envy;
                        else
                            value += pity;
                        break;
                        default :
                        FATALERR("IMPOSSIBLE!");
                        break;
                    }
            }
            if(neighbor->meat > 0.0 && value > 0.0)
                value *= (1.0 + meatValue);
            if(value > bestValue){ // Don't ask food if value == 0
                bestValue = value;
                donor = neighbor;
            } else{
                if(value == bestValue && (begStrategy == 1 || begStrategy == 3)){
                    bestValue = value;
                    donor = neighbor;
                }
            }
        }
    }
    myCell->aglist->end();

    if(donor){
        received = donor->shareFoodWith(this, name);
#ifdef DEBUG
        if(received < 0.0)
            FATALERR("%f", received);
#endif

        if(received == 0.0){
            if(donor->sex == 'f'){
                if(fvalueOfNoSh < 0.0)
                    storeInMemory(fvalueOfNoSh, donor->name, 'r', 15);
            } else{
                if(mvalueOfNoSh < 0.0)
                    storeInMemory(mvalueOfNoSh, donor->name, 'r', 15);
            }
            if(sex == 'f'){
                if(donor->fvalueOfNoSh < 0.0)
                    donor->storeInMemory(donor->fvalueOfNoSh, name, 'g', 15);
            } else{
                if(donor->mvalueOfNoSh < 0.0)
                    donor->storeInMemory(donor->mvalueOfNoSh, name, 'g', 15);
            }
        } else{
            storeInMemory(received, donor->name, 'r', 1);
            donor->storeInMemory(received, name, 'g', 2);
        }
        energy += received;
        if(energy > MaxEnergy){ // The lowWorryingELevel might be > MaxEnergy
            received = energy - MaxEnergy;
            energy = MaxEnergy;
        }
#ifdef DEBUG
        if(received < 0.0)
            FATALERR("%f", received);
#endif
        eFromShare += received;
    }
}

double Agent::shareFoodWith(Agent *other, char *otherName)
{
#ifdef DEBUG
    if(isKid)
        FATALERR("IMPOSSIBLE!");
#endif

    if(sex == 'm'){
        if(other->sex == 'f')
            nBegsMF++;
        else
            nBegsMM++;
    } else{
        if(other->sex == 'f')
            nBegsFF++;
        else
            nBegsFM++;
    }

#ifdef DEBUG
    if(meat < 0.0)
        FATALERR("IMPOSSIBLE!");
#endif

    if(meat == 0.0 && generosity <= 0.0)
        return 0.0;
    if(meatGenerosity <= 0.0 && generosity <= 0.0)
        return 0.0;

    double p, r;
    // probability of sharing food depends on gratitude, vengefulness, and...
    p = recall(otherName, rootMemory, tmFct, vengefulness, gratitude,
            gratStrategy, vengStrategy, vengStr2);

    // Get the opinion of best friend about this agent
    if(p == UNKNOWN_AGENT && Language == 1){
        int i, nf;
        Friend *fl = getFriendsAtDistance(1, &nf);
        for(i = 0; i < nf; i++){
            if(!(STRCMP(fl[i].a->name, otherName) == 0)){
                p = recall(otherName, fl[i].a->rootMemory,
                        fl[i].a->tmFct, fl[i].a->vengefulness, fl[i].a->gratitude,
                        fl[i].a->gratStrategy, fl[i].a->vengStrategy, fl[i].a->vengStr2);
#ifdef DEBUG
                if(p != UNKNOWN_AGENT && (p < -200.0 || p > 200)){
                    printf("\n>>>>>>>> %s remembers %s as %f <<<<<<<<<<\n\n", fl[i].a->name, otherName, p);
                    fl[i].a->dumpInfoAboutMe();
                    other->dumpInfoAboutMe();
                    FATALERR(" ");
                }
#endif
                if(p != 0.0 && p != UNKNOWN_AGENT){
                    p *= adviceValue;
                    break;
                }
            }
        }
        free(fl);
    }

    if(p == UNKNOWN_AGENT){
        if(other->sex == 'f')
            p = vOfFStranger;
        else
            p = vOfMStranger;
    }

    // benevolence towards relatives
    if(STRCMP(other->name, motherName) == 0)
        p += bnvlcTMother;
    else
        if(STRCMP(other->motherName, motherName) == 0)
            p += bnvlcTSibling;
        else
            if(STRCMP(other->motherName, name) == 0)
                p += bnvlcTChild;

    // benevolence toward other sex
    if(sex == other->sex)
        p += bnvlcTSameSex;
    else
        p += bnvlcTOtherSex;

    if(other->energy > energy){
        p -= envy;
        if(other->sex == 'f')
            storeInMemory((envy * fvalueOfNoSh), other->name, 'r', 20);
        else
            storeInMemory((envy * fvalueOfNoSh), other->name, 'r', 20);
    } else{
        p += pity;
    }

#ifdef DEBUG
    if(sex == 'm' && STRCMP(other->motherName, name) == 0)
        FATALERR("Male mother!");
#endif

    r = getRandDouble(0.0, 1.0);
    if(r < p){
        double donation = 0.0;
        if(meat > 0.0 && meatGenerosity > 0.0){
            donation = meat * meatGenerosity;
            if(donation > (metabolism * 1.5))
                donation = metabolism * 1.5;
            if(donation > meat)
                donation = meat;
            meat -= donation;
        }
        if(donation < metabolism && generosity > 0.0){
            double d2 = metabolism * generosity;
            if((donation + d2) > metabolism){
                d2 = metabolism - donation;
            }
            donation += d2;
            energy -= d2;
        }
        if(donation > 0.0){
            if(sex == 'm'){
                if(other->sex == 'f')
                    nSharedMF++;
                else
                    nSharedMM++;
            } else{
                if(other->sex == 'f')
                    nSharedFF++;
                else
                    nSharedFM++;
            }
        }
        return (donation);
    }
    return 0.0;
}


int Agent::predationRisk()
{
    double p;
    if(myCell->thv)
        p = PredationRisk * 6.0 / myCell->nAgents;
    else
        p = PredationRisk / myCell->nAgents;
    double r = getRandDouble(0.0, 1.0);
    if(r < p)
        return 1;
    else
        return 0;
}

void Agent::goToReaperQueue(int cause)
{
    Agent *ag;
    reaperQueue->addLast(this);
#ifdef DEBUG
    if(myAlliance)
        FATALERR("IMPOSSIBLE!");
#endif
    if(nonCooperators->getCount())
        FATALERR("IMPOSSIBLE!");
    if(potentialPartners->getCount()){
        potentialPartners->begin();
        while((ag = (Agent*)potentialPartners->next())){
#ifdef DEBUG
            if(ag->potentialPartners->remove(this) == false)
                FATALERR("IMPOSSIBLE!");
#else
            ag->potentialPartners->remove(this);
#endif
        }
        potentialPartners->end();
    }
    delete(potentialPartners);
    potentialPartners = NULL;

    deathCause = cause;
    if(mother)
        mother->childIsDead(this);
    if(father)
        father->childIsDead(this);
    myCell->removeAgent(this);
    if(children->getCount()){
        children->begin();
        while((ag = (Agent*)children->next())){
#ifdef DEBUG
            ag->parentIsDead(this, sex);
#else
            ag->parentIsDead(sex);
#endif
        }
        children->end();
    }

#ifdef DEBUG
    if(getAgPointer(name) != this)
        FATALERR("IMPOSSIBLE!");
    removeAgName(name);
    if(getAgPointer(name) != NULL)
        FATALERR("IMPOSSIBLE!");
#else
    removeAgName(name);
#endif
    if(followed){
        WhiteAgent = NULL;
        followed = false;
        if(F){
            fclose(F);
            F = NULL;
        }
    }
    agList->remove(this);
#ifdef DEBUG
    n_dead++;
    if(sex == 'f'){
        if(femaleList->remove(this) == false)
            FATALERR("IMPOSSIBLE!");
    } else{
        if(maleList->remove(this) == false)
            FATALERR("IMPOSSIBLE!");
    }
#else
    if(sex == 'f')
        femaleList->remove(this);
    else
        maleList->remove(this);
#endif
}

Agent::~Agent()
{
    delete(children);
    children = NULL;
    delete(nonCooperators);
    nonCooperators = NULL;
    void *tmp;
    while(rootMemory){
        tmp = rootMemory->next;
        delete(rootMemory);
        rootMemory = (Remembrance*)tmp;
    }
    rootMemory = NULL;
    while(knownPatches){
        tmp = knownPatches->next;
        free(knownPatches);
        knownPatches = (TreePatch*)tmp;
    }
    knownPatches = NULL;
}

void Agent::drawNetwork(Remembrance *m)
{
#ifndef NOGUI
    if(m == NULL)
        return;
    Agent *ptr = getAgPointer(m->name);
    int c;
    if(ptr){
        double r = recall(m->name, rootMemory, tmFct, vengefulness,
                gratitude, gratStrategy, vengStrategy, vengStr2);
#ifdef DEBUG
        if(r == UNKNOWN_AGENT)
            FATALERR("IMPOSSIBLE!");
#endif
        if(ptr->x == x && ptr->y == y){
            Nm += 1;
            Rv += r;
        }
        if(r == 0.0)
            c = 255;
        else
            if(r > 0.0)
                c = 29;
            else
                c = 20;
        WorldRaster->drawLine((x*5+2), (y*5+2), (ptr->x * 5+2), (ptr->y * 5+2), c);
    }
    drawNetwork(m->left);
    drawNetwork(m->right);
#endif
}

void Agent::drawSelfOn()
{
#ifndef NOGUI
    if(followed){
        if(rootMemory != NULL){
            Nm = 0; // Number of remembered agents
            Rv = 0.0; // Sum of remembrances
            myCell->drawPerimeter(AllianceRadius);
            myCell->drawPerimeter(MaxVision);
            myCell->drawPerimeter(NearView);
            int c;
            double r;
            Remembrance *m = rootMemory;
            Agent *ptr;
            while(m){
                ptr = getAgPointer(m->name);
                if(ptr){
                    r = recall(m->name, rootMemory, tmFct, vengefulness,
                            gratitude, gratStrategy, vengStrategy, vengStr2);
#ifdef DEBUG
                    if(r == UNKNOWN_AGENT)
                        FATALERR("IMPOSSIBLE!");
#endif
                    if(ptr->x == x && ptr->y == y){
                        Nm += 1;
                        Rv += r;
                    }
                    if(r == 0.0)
                        c = 255;
                    else
                        if(r > 0.0)
                            c = 29;
                        else
                            c = 20;
                    WorldRaster->drawLine((x*5+2), (y*5+2), (ptr->x * 5+2), (ptr->y * 5+2), c);
                }
                m = m->next;
            }

            if(Nm == 0 || Rv == 0.0)
                c = 255;
            else
                if(Rv < 0.0)
                    c = 20;
                else
                    c = 29;

            WorldRaster->fillRectangle((x*5+1), (y*5+1), 3, 3, c);
        } else{
            WorldRaster->fillRectangle((x*5+1), (y*5+1), 3, 3, 255);
        }
    }else{
        myCell->drawSelfOn();
    }
#endif
}

void Agent::receiveFromMom(double e)
{
    energy += e;
#ifdef DEBUG
    if(energy > MaxEnergy)
        FATALERR("energy = %f", energy);
#endif
    if(energy > MaxEnergy){
        e = energy - MaxEnergy;
        energy = MaxEnergy;
    }
    eFromMom += e;
}

#ifdef DEBUG
void Agent::parentIsDead(Agent *parent, char s)
{
    if(parent != mother && parent != father)
        FATALERR("%p : %p : %p", parent, mother, father);
#else
    void Agent::parentIsDead(char s)
    {
#endif
        if(s == 'f')
            mother = NULL;
        else
            father = NULL;
    }

    void Agent::childIsDead(Agent *child)
    {
#ifdef DEBUG
        if(children == NULL)
            FATALERR("(children == NULL)\n");
        if(children->getCount() == 0)
            FATALERR("(children->getCount() == 0)\n");
        if(children->remove(child) == false)
            FATALERR("%p %c, born=%d age=%e\n", child, child->sex, SimTime - child->age, child->age);
#else
        children->remove(child);
#endif
        if(kid == child)
            kid = NULL;
    }

    void Agent::addKid(Agent *newborn)
    {
#ifdef DEBUG
        if(deathCause)
            FATALERR("IMPOSSIBLE!");
#endif
        children->addLast(newborn);
        kid = newborn;
    }

    int Agent::getNChildren()
    {
        return children->getCount();
    }

    void Agent::getMigX(int *i, int *j)
    {
        *i = migX;
        *j = migY;
    }

    void Agent::followMe()
    {
        if(followed){
            doNotFollowMe();
        } else{
            if(WhiteAgent)
                WhiteAgent->doNotFollowMe();
            followed = true;
            F = fopen(_("followedAgents"), "a");
            if(F == NULL)
                FATALERR(_("Could not open file \"followedAgents\""));
            WhiteAgent = this;
            fprintf(F, "\n\n%s >> (sex %c) (gratitude %f) (vengefulness %f) \n"
                    "  (tmFct %f) (gratStrategy %d) (vengStrategy %d) (vengStr2 %d)\n"
                    "  (fvalueOfNoCT %f) (fvalueOfNoSh %f)\n"
                    "  (mvalueOfNoCT %f) (mvalueOfNoSh %f)\n",
                    name, sex, gratitude, vengefulness, tmFct, gratStrategy, vengStrategy,
                    vengStr2, fvalueOfNoCT, fvalueOfNoSh, mvalueOfNoCT, mvalueOfNoSh);
            printf("\n\n%s >> (sex %c) (gratitude %f) (vengefulness %f) \n"
                    "  (tmFct %f) (gratStrategy %d) (vengStrategy %d) (vengStr2 %d)\n"
                    "  (fvalueOfNoCT %f) (fvalueOfNoSh %f)\n"
                    "  (mvalueOfNoCT %f) (mvalueOfNoSh %f)\n",
                    name, sex, gratitude, vengefulness, tmFct, gratStrategy, vengStrategy,
                    vengStr2, fvalueOfNoCT, fvalueOfNoSh, mvalueOfNoCT, mvalueOfNoSh);
        }

        Cell *c;
        cellList->begin();
        while((c = (Cell*)cellList->next()))
            c->drawSelfOn();
        cellList->end();
    }

    void Agent::doNotFollowMe()
    {
        WhiteAgent = NULL;
        followed = false;
        if(F){
            fclose(F);
            F = NULL;
        }
        drawSelfOn();
    }

    int Agent::getMigPref()
    {
        if(migPrefs[0] == 0){
            if(migPrefs[1] == 1)
                return 1;
            else
                return 3;
        }
        if(migPrefs[0] == 1){
            if(migPrefs[1] == 0)
                return 2;
            else
                return 4;
        }
        if(migPrefs[0] == 2){
            if(migPrefs[1] == 0)
                return 5;
            else
                return 6;
        }
#ifdef DEBUG
        FATALERR("IMPOSSIBLE!");
#endif
        return 0;
    }

    void Agent::getNFandE(int *nf, int *ne)
    {
        Remembrance *tmp = rootMemory;
        Agent *ag;
        double r;
        while(tmp){
            ag = getAgPointer(tmp->name);
            if(ag){
                r = recall(tmp->name, rootMemory, tmFct, vengefulness,
                        gratitude, gratStrategy, vengStrategy, vengStr2);
                if(r > 0.0)
                    *nf += 1;
                else
                    if(r < 0.0)
                        *ne += 1;
            }
            tmp = tmp->next;
        }
    }

    void Agent::getInteractionStatsForSex(char s)
    {
        Remembrance *tmp = rootMemory;
        Agent *ag;
        double r;
        Nf = 0; Ne = 0; Nfi = 0; Nei = 0;
        RMin = 0.0; RMax = 0.0; RT = 0.0; Nm = 0;

        while(tmp){
            ag = getAgPointer(tmp->name);
            if(ag && ag->sex == s){
                r = recall(tmp->name, rootMemory, tmFct, vengefulness,
                        gratitude, gratStrategy, vengStrategy, vengStr2);
#ifdef DEBUG
                if(r == UNKNOWN_AGENT)
                    FATALERR("IMPOSSIBLE!");
#endif
                RT += r;
                Nm += 1;
                if(r < 0.0){
                    Ne++;
                    Nei += tmp->nInteractions;
                    if(r < RMin)
                        RMin = r;
                } else
                    if(r > 0.0){
                        Nf++;
                        Nfi += tmp->nInteractions;
                        if(r > RMax)
                            RMax = r;
                    }
            }
            tmp = tmp->next;
        }
    }

    void Agent::getNFFriends(int *f, int *e, int *fi, int *ei, double *min, double *max, double *mean)
    {
        getInteractionStatsForSex('f');

        *f = Nf;
        *e = Ne;
        *fi = Nfi;
        *ei = Nei;
        *min = RMin;
        *max = RMax;
        if(Nm > 0)
            *mean = RT / Nm;
        else
            *mean = 0.0;
    }

    void Agent::getNMFriends(int *f, int *e, int *fi, int *ei, double *min, double *max, double *mean)
    {
        getInteractionStatsForSex('m');

        *f = Nf;
        *e = Ne;
        *fi = Nfi;
        *ei = Nei;
        *min = RMin;
        *max = RMax;
        if(Nm > 0)
            *mean = RT / Nm;
        else
            *mean = 0.0;
    }

    int Agent::getFrMeanDist(double *fmd, double *fmni, int *nFr, double *emd, double *emni, int *nEn)
    {
        int d1, d2;
        Agent *ptr;
        int n = 0, nf = 0, ne = 0, fd = 0, ed = 0, nfi = 0, nei = 0, distance;
        double value;
        Remembrance *m = rootMemory;
        while(m && n < nRembr){
            ptr = getAgPointer(m->name);
            if(ptr){
                if(x > ptr->x)
                    d1 = x - ptr->x;
                else
                    d1 = ptr->x - x;
                if(y > ptr->y)
                    d2 = y - ptr->y;
                else
                    d2 = ptr->y - y;
                distance = ((d1 > d2) ? d1 : d2);
                value = recall(m->name, rootMemory, tmFct, vengefulness,
                        gratitude, gratStrategy, vengStrategy, vengStr2);
#ifdef DEBUG
                if(value == UNKNOWN_AGENT)
                    FATALERR("IMPOSSIBLE!");
#endif
                if(value > 0.0){
                    fd += distance;
                    nfi += m->nInteractions;
                    nf++;
                } else
                    if(value < 0.0){
                        ed += distance;
                        nei += m->nInteractions;
                        ne++;
                    }
                n++;
            }
            m = m->next;
        }
        *nFr = nf;
        if(nf > 0){
            *fmd = (double)fd / (double)nf;
            *fmni = (double)nfi / (double)nf;
        } else{
            *fmd = -1.0;
            *fmni = -1.0;
        }
        *nEn = ne;
        if(ne > 0){
            *emd = (double)ed / (double)ne;
            *emni = (double)nei / (double)ne;
        } else{
            *emd = -1.0;
            *emni = -1.0;
        }
        return n;
    }

    int Agent::getNKnownPatches()
    {
        if(knownPatches){
            int n = 0;
            TreePatch *tmp;
            tmp = knownPatches;
            while(tmp){
                n++;
                tmp = tmp->next;
            }
            return n;
        }
        return 0;
    }

    void printMemory(Remembrance *m)
    {
        if(m == NULL)
            return;
        printMemory(m->left);
        int i;
        printf("%s [ ", m->name);
        for(i = 0; i < MemSize; i++)
            printf("(%2d %d %2.2f) ", m->receivedEv[i], m->receivedTm[i], m->received[i]);
        printf("] [");
        for(i = 0; i < MemSize; i++)
            printf("(%2d %d %2.2f) ", m->givenEv[i], m->givenTm[i], m->given[i]);
        printf("]\n");
        printMemory(m->right);
    }

    void Agent::dumpMemTo(FILE *mf, FILE *pf, Agent **list, Agent **slist)
    {
        // Agents whose mother died before they become adults
        if(rootMemory == NULL){
            return;
        }
        int n = 0, ns = 0, d1, d2, i, distance, kinship, j, myID = 0, otherID, mySID = 0, otherSID = -1;
        int nSF, nSE, nSK; // Number shared friends, enemies and known agents
        double value, v1, v2;
        bool found = false;
        while(list[myID])
            if(nameComp(name, list[myID]->name) == 0){
                found = true;
                break;
            } else {
                myID++;
            }
        myID++;
        if(!found)
            fprintf(stderr, "Where I am? (%d %d %d)\n", age, childhood, isKid);
        found = false;
        while(slist[mySID])
            if(nameComp(name, slist[mySID]->name) == 0){
                found = true;
                break;
            } else{
                mySID++;
            }
        mySID++;
        if(!found)
            fprintf(stderr, "Where I am? (II) (%c)\n", sex);

        Agent *ptr, *ptr2;
        Remembrance *m = rootMemory;
        Remembrance *m2, *rootM2;
        while(m){
            ptr = getAgPointer(m->name);
            if(ptr){
                otherID = 0;
                while(list[otherID])
                    if(nameComp(ptr->name, list[otherID]->name) == 0)
                        break;
                    else
                        otherID++;
                otherID++;
                if(ptr->sex == sex){
                    otherSID = 0;
                    while(slist[otherSID])
                        if(nameComp(ptr->name, slist[otherSID]->name) == 0)
                            break;
                        else
                            otherSID++;
                    otherSID++;
                    ns++;
                } else {
                    otherSID = -1;
                }

                n++;
                if(x > ptr->x)
                    d1 = x - ptr->x;
                else
                    d1 = ptr->x - x;
                if(y > ptr->y)
                    d2 = y - ptr->y;
                else
                    d2 = ptr->y - y;
                distance = ((d1 > d2) ? d1 : d2);

                value = recall(m->name, rootMemory, tmFct, vengefulness,
                        gratitude, gratStrategy, vengStrategy, vengStr2);
#ifdef DEBUG
                if(value == UNKNOWN_AGENT)
                    FATALERR("IMPOSSIBLE!");
#endif

                kinship = 1;
                if(STRCMP(motherName, ptr->name) == 0)
                    kinship = 2;
                else
                    if(STRCMP(motherName, ptr->motherName) == 0)
                        kinship = 3;
                    else
                        if(STRCMP(name, ptr->motherName) == 0)
                            kinship = 4;

                nSF = 0;
                nSE = 0;
                nSK = 0;
                m2 = ptr->rootMemory;
                rootM2 = m2;
                while(m2){
                    ptr2 = getAgPointer(m2->name);
                    if(ptr2){
                        v1 = recall(m2->name, rootMemory, tmFct, vengefulness,
                                gratitude, gratStrategy, vengStrategy, vengStr2);
                        v2 = recall(m2->name, rootM2, ptr->tmFct, ptr->vengefulness,
                                ptr->gratitude, ptr->gratStrategy, ptr->vengStrategy, ptr->vengStr2);
                        if(v1 != UNKNOWN_AGENT){
                            if(v1 > 0.0 && v2 > 0.0)
                                nSF++;
                            else
                                if(v1 < 0.0 && v2 < 0.0)
                                    nSE++;
                                else
                                    nSK++;
                        }
                    }
                    m2 = m2->next;
                }

                j = 0;
                for(i = 0; i < MemSize; i++){
                    if(m->receivedTm[i] > 0){
                        j++;
                        fprintf(mf, "%s %d %d %c %d %d %d %d %d %s %d %d %d %d %c %d %d %d %d %f %f r %d %d %f %d %d %d\n",
                                name, myID, mySID, sex, x, y, n, ns, j, ptr->name, otherID, otherSID,
                                (SimTime - m->firstInter), m->nInteractions, ptr->sex, ptr->x,
                                ptr->y, kinship, distance, value, enmityThr, m->receivedEv[i],
                                (SimTime - m->receivedTm[i]), m->received[i], nSF, nSE, nSK);
                    }
                    if(m->givenTm[i] > 0){
                        j++;
                        fprintf(mf, "%s %d %d %c %d %d %d %d %d %s %d %d %d %d %c %d %d %d %d %f %f g %d %d %f %d %d %d\n",
                                name, myID, mySID, sex, x, y, n, ns, j, ptr->name, otherID,
                                otherSID, (SimTime - m->firstInter), m->nInteractions, ptr->sex,
                                ptr->x, ptr->y, kinship, distance, value, enmityThr,
                                m->givenEv[i], (SimTime - m->givenTm[i]), m->given[i], nSF, nSE,
                                nSK);
                    }
                }
            }
            m = m->next;
        }

        fprintf(pf, "%s", name);
        TreePatch *tmp = knownPatches;
        while(tmp){
            fprintf(pf, " [%p %d]", (void*)tmp->patch, tmp->lastVisit);
            tmp = tmp->next;
        }
        fprintf(pf, "\n");
    }

    void Agent::dumpInfoAboutMe()
    {
        printf("\n\n\nINFORMATION ABOUT %s:\n", name);
        if(children->getCount()){
            Agent *ch;
            printf("child x,->y() sex age energy\n");
            children->begin();
            while((ch = (Agent*)children->next())){
                printf("%s %d,->%d() %c %d %f\n", ch->name, ch->x, ch->y, ch->sex, ch->age, ch->energy);
            }
            children->end();
        }
        printf("\n\n");

        printf(_("NAME [ received ] [ given ]\n"));
        printMemory(rootMemory);
        printf("\n\n");

        if(knownPatches){
            TreePatch *tmp;
            tmp = knownPatches;
            printf(_("Visited Patches: "));
            while(tmp){
                printf("[%p, %d] ", (void*)tmp->patch, tmp->lastVisit);
                tmp = tmp->next;
            }
            printf("\n");
        } else{
            printf("I didn't visit any patch!\n");
        }

        printf("kidValueForMale = %f\n" "bnvlcTOtherSex = %f\n" "bnvlcTSameSex = %f\n"
                "bnvlcTMother = %f\n" "bnvlcTSibling = %f\n" "bnvlcTChild = %f\n"
                "gratitude = %f\n" "vengefulness = %f\n" "generosity = %f\n"
                "meatGenerosity = %f\n" "tmFct = %f\n" "energySel = %f\n"
                "childSel = %f\n" "motherSel = %f\n" "siblingSel = %f\n"
                "friendSel = %f\n" "otherSexSel = %f\n" "sameSexSel = %f\n"
                "migAgeImportance = %f\n" "migFriendImportance = %f\n" "bravery = %f\n"
                "pity = %f\n" "envy = %f\n" "huntValue = %f\n"
                "patrolValue = %f\n" "fvalueOfNoH = %f\n"
                "fvalueOfNoCT = %f\n" "fvalueOfNoSh = %f\n"
                "mvalueOfNoH = %f\n" "mvalueOfNoCT = %f\n"
                "mvalueOfNoSh = %f\n" "audacity = %f\n"
                "loyalty = %f\n" "adviceValue = %f\n"
                "valueOfNoSex = %f\n" "oestrFemSel = %f\n" "maleEnergyImportance = %f\n"
                "maleAgeImportance = %f\n" "mutationRate = %f\n" "femalePromiscuity = %f\n"
                "meatValue = %f\n", kidValueForMale, bnvlcTOtherSex, bnvlcTSameSex,
                bnvlcTMother, bnvlcTSibling, bnvlcTChild, gratitude, vengefulness,
                generosity, meatGenerosity, tmFct, energySel, childSel, motherSel,
                siblingSel, friendSel, otherSexSel, sameSexSel, migAgeImportance,
                migFriendImportance, bravery, pity, envy, huntValue,
                patrolValue, fvalueOfNoH,
                fvalueOfNoCT, fvalueOfNoSh, mvalueOfNoH, mvalueOfNoCT,
                mvalueOfNoSh, audacity, loyalty, adviceValue,
                valueOfNoSex, oestrFemSel, maleEnergyImportance, maleAgeImportance,
                mutationRate, femalePromiscuity, meatValue);

        printf("fearOfHPatches = %d\n" "acceptMigInv = %d\n" "acceptMoveInvtFromM = %d\n"
                "acceptMoveInvtFromF = %d\n" 
                "askMeatOnly = %d\n", fearOfHPatches, acceptMigInv, acceptMoveInvtFromM,
                acceptMoveInvtFromF, askMeatOnly);

        printf("bestMaleAge = %d\n" "childhood = %d\n" "gratStrategy = %d\n" "vengStrategy = %d\n"
                "begStrategy = %d\n" "huntStrategy = %d\n" "maxHuntPatrolSize = %d\n",
                bestMaleAge, childhood, gratStrategy, vengStrategy, begStrategy,
                huntStrategy, maxHuntPatrolSize);

        printf("migPrefs = %d, %d, %d, %d]\n", migPrefs[0], migPrefs[1], migPrefs[2], migPrefs[3]);
    }

