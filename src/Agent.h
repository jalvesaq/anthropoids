/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef AGENT_H
#define AGENT_H

#include <stdio.h>
#include "abmtools.h"

class Cell;
class Patch;
class Alliance;
class Prey;
class List;

class Remembrance
{
    public:
        Remembrance();
        ~Remembrance();
        char name[8];
        double *given;
        double *received;
        int *givenTm;
        int *receivedTm;
        int *givenEv;
        int *receivedEv;
        int nInteractions;
        int firstInter;
        Remembrance *left;
        Remembrance *right;
        Remembrance *next;
};

typedef struct TreePatchT{
    Patch *patch;
    int lastVisit;
    bool isHostile;
    struct TreePatchT *next;
} TreePatch;

class Agent;
class Alliance;
struct Frnd;


class Agent : public Eukaryote
{
    private:
        Agent(Agent *m, Agent *f, double e);
        Agent *mother, *father;

        char motherName[8], fatherName[8];

        Remembrance *lastMemory, *rootMemory;

        int predationRisk();
        void mutate();

        void femaleStep();
        void maleStep();
        char oestrus;
        int reproAge;
        List *potentialPartners; // suitors and coquettes
        Frnd* sortMales();
        void giveBirth(Agent *fa, double e);

        // the genotype that will be activated if the agent is female
        double f_moV, f_kidValueForMale, f_bnvlcTOtherSex, f_bnvlcTSameSex,
               f_bnvlcTMother, f_bnvlcTSibling, f_bnvlcTChild, f_gratitude,
               f_vengefulness, f_generosity, f_meatGenerosity, f_tmFct, f_energySel,
               f_childSel, f_motherSel, f_siblingSel, f_friendSel, f_otherSexSel,
               f_sameSexSel, f_migAgeImportance, f_migFriendImportance, f_bravery,
               f_pity, f_envy, f_huntValue, f_patrolValue, f_lowDeficit,
               f_hiDeficit, f_fvalueOfNoH, f_fvalueOfNoCT, f_fvalueOfNoSh,
               f_mvalueOfNoH, f_mvalueOfNoCT, f_mvalueOfNoSh, f_audacity, f_loyalty,
               f_adviceValue, f_valueOfNoSex, f_vOfFStranger, f_vOfMStranger,
               f_meatValue, f_fairness, f_expFFairness, f_expMFairness,
               f_fvalueOfNotFair, f_mvalueOfNotFair, f_enmityThr;
        bool f_fearOfHPatches, f_acceptMigInv, f_acceptMoveInvtFromM,
             f_acceptMoveInvtFromF, f_xenophTM, f_xenophTF, f_xenophTFwK,
             f_zeroPostvNo;
        int f_childhood, f_gratStrategy, f_vengStrategy, f_vengStr2, f_begStrategy, f_norm,
            f_metanorm, f_huntStrategy, f_terriRemStrategy, f_hasShame,
            f_maxHuntPatrolSize;
        char f_migPrefs[4];

        // the genotype that will be activated if the agent is male
        double m_moV, m_kidValueForMale, m_bnvlcTOtherSex, m_bnvlcTSameSex,
               m_bnvlcTMother, m_bnvlcTSibling, m_bnvlcTChild, m_gratitude,
               m_vengefulness, m_generosity, m_meatGenerosity, m_tmFct, m_energySel,
               m_childSel, m_motherSel, m_siblingSel, m_friendSel, m_otherSexSel,
               m_sameSexSel, m_migAgeImportance, m_migFriendImportance, m_bravery,
               m_pity, m_envy, m_huntValue, m_patrolValue, m_lowDeficit,
               m_hiDeficit, m_fvalueOfNoH, m_fvalueOfNoCT, m_fvalueOfNoSh,
               m_mvalueOfNoH, m_mvalueOfNoCT, m_mvalueOfNoSh, m_audacity, m_loyalty,
               m_adviceValue, m_valueOfNoSex, m_vOfFStranger, m_vOfMStranger,
               m_meatValue, m_fairness, m_expFFairness, m_expMFairness,
               m_fvalueOfNotFair, m_mvalueOfNotFair, m_enmityThr;
        bool m_fearOfHPatches, m_acceptMigInv, m_acceptMoveInvtFromM,
             m_acceptMoveInvtFromF, m_xenophTM, m_xenophTF, m_xenophTFwK,
             m_zeroPostvNo;
        int m_childhood, m_gratStrategy, m_vengStrategy, m_vengStr2, m_begStrategy, m_norm,
            m_metanorm, m_huntStrategy, m_terriRemStrategy, m_hasShame,
            m_maxHuntPatrolSize;
        char m_migPrefs[4];

        bool isKid;

        bool answerMoveInvitationFrom(Agent *ag);

        void beginMigration();
        void answerGroupMigInvit(Alliance *alli, int m, int mx, int my);
        double agMigValue(Agent *ag);

        List *children;
        void addKid(Agent *newborn);
        void childStep();
        void receiveFromMom(double e);
        void becomeAdult();
        void readMomMemory();
        void childIsDead(Agent *child);
#ifdef DEBUG
        void parentIsDead(Agent *parent, char s);
#else
        void parentIsDead(char s);
#endif

        void eatPlant();

        double evaluatePatch(Patch *p);
        double evaluateCell(Cell *cell);
        void moveToBestCell();
        void moveTowardsX(int theX, int theY);
        void moveToX(int newX, int newY);
        void inviteFriendsToMoveFrom(int oldX, int oldY);
        void createGroupToMig(int m);
        int migX, migY, migStep;
        int migToBestNearCell();
        void migrationStep();
        char migPrefs[4];
        TreePatch *knownPatches;
        void storePatch(Patch *p);
        void registerHost(Patch *p);
        bool isHostile(Patch *p);


        void patrolTerritory();
        bool answerAllianceInvitationFrom(Agent *ld, Alliance *a, char t);

        double meat, metabolism, lastEnergy;
        int meatAge;
        void begForFood();
        double shareFoodWith(Agent *other, char *otherName);
        void receiveFood(double food);


        void doNotFollowMe();
        void drawNetwork(Remembrance *m);
        void dumpInfoAboutMe();

        void hunt();
        int createAllianceToHunt();
        Prey* seekPrey();
        void receiveMeat(double m, double ldFairness, int mhi);
        void distributeMeat(double preyMeat, int mhi);

    public:
        Agent(int xc, int yc);
        ~Agent();
        void goToReaperQueue(int cause);
        int x, y;
        Cell *myCell;
        Patch *myPatch;
        char sex, migrating;
        bool followed, amILeader;
        char name[8];
        double energy;
        double eFromMom, eFromShare, eFromConfl, eFromHunt, eFromDinner;
        int age, maxAge, mGen, fGen, huntInterval;
        int deathCause, nInvt, nAcptInvt, nAlliChng;
        Agent *kid; // Mothers have at most one small child.
        List *nonCooperators;
        int nRembr, lastHunt;
        Alliance *myAlliance;

        // genetically inheritable features (bnvlc = benevolence; Sel = selectivity)
        double moV, kidValueForMale, bnvlcTOtherSex, bnvlcTSameSex, bnvlcTMother,
               bnvlcTSibling, bnvlcTChild, gratitude, vengefulness, generosity,
               meatGenerosity, tmFct, energySel, childSel, motherSel, siblingSel,
               friendSel, otherSexSel, sameSexSel, migAgeImportance,
               migFriendImportance, bravery, pity, envy, huntValue,
               patrolValue, lowDeficit, hiDeficit,
               fvalueOfNoH, fvalueOfNoCT, fvalueOfNoSh, mvalueOfNoH,
               mvalueOfNoCT, mvalueOfNoSh, audacity,
               loyalty, adviceValue, valueOfNoSex, vOfFStranger,
               vOfMStranger, enmityThr, meatValue, fairness, expFFairness,
               expMFairness, fvalueOfNotFair, mvalueOfNotFair;
        bool fearOfHPatches, acceptMigInv, acceptMoveInvtFromM,
             acceptMoveInvtFromF,
             xenophTM, xenophTF, xenophTFwK, zeroPostvNo;
        int childhood, gratStrategy, vengStrategy, vengStr2, begStrategy, huntStrategy,
            terriRemStrategy, hasShame, maxHuntPatrolSize, norm, metanorm;

        // genes that only one sex has or that is the same for both sexes (no need
        // of m_ and f_ copies of them)
        bool askMeatOnly, fearOfHPWhenHasKid;
        double oestrFemSel, maleEnergyImportance, maleAgeImportance,
               mutationRate, femalePromiscuity, kidV;
        int bestMaleAge;

        void createEnd();
        void learnGeography();
        void makeFirstFriends();

        Frnd* getFriendsAtDistance(int  d, int *nf);

        void flyFromX(int bx, int by, Patch *p);

        void step();

        Alliance* createAllianceAgainst(Agent *enemy);
        void rememberHuntPartners(List *mypartners);
        void rememberPatrolAllies(List *a);
        void rememberPatrolRivals(List *r);
        void askAboutPatches(Agent *ag);

        void storeInMemory(double value, char *oName, char type, char e);
        void followMe();
        void drawSelfOn();

        void getNFandE(int *nf, int *ne);
        void getNFFriends(int *f, int *e, int *fi, int *ei, double *min, double *max, double *mean);
        void getNMFriends(int *f, int *e, int *fi, int *ei, double *min, double *max, double *mean);

        void getMigX(int *i, int *j);
        int getMigPref();
        int getNKnownPatches();
        int getNChildren();

        void getInteractionStatsForSex(char s);
        int getFrMeanDist(double *fmd, double *fmni, int *nFr, double *emd, double *emni, int *nEn);
        Alliance* getAlliance();

        void dumpMemTo(FILE *MF, FILE *PF, Agent **list, Agent **slist);
};

#endif
