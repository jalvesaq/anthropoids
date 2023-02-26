/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "abmtools.h"
#include "list.h"
#ifndef NOGUI
#include "zoomraster.h"
#include "control.h"
#endif
#include "Patch.h"
#include "Prey.h"
#include "Tree.h"
#include "THV.h"
#include "Agent.h"
#include "Cell.h"
#include "Alliance.h"

class Observer;

/*
   Name conventions:

   - initVariables (prefix "init"): used to set values for the first
   population. They are inheritable and their values might suffer mutation.

   - GlobalVariables (capitalized): their values don't change for the entire
   simulation.

   - ClassNames (capitalized).

   - classInstances, classMethods and classVariables (not capitalized).

   - localVariables (not capitalized).

   - MACROS (upper case).

*/

extern bool ShowPopDensity, ShowAgents, ShowPreys, ShowGraph, SlowMotion,
       VerySlowMotion;

extern int WorldXSize, WorldYSize, Silent, DumpPopNow,
       SimTime, Hour, Day, Year, LogInterval, ChooseRandomValues,
       ExperimentDuration, DayDuration, YearDuration, Language, Hunt, FoodShare,
       Territoriality, RandomAgentActivation, Norm, Metanorm, MemSize;

// These could be local variables, but it's faster if we avoid billions of
// malloc() + free()
extern int *XL, *YL;


// statistics
extern int nSharedMM, nSharedMF, nSharedFF, nSharedFM, nBegsMM, nBegsFF, nBegsFM, nBegsMF;
extern int NHunts, NHunters, NFoundPreys, NSuccHunts;


#ifndef NOGUI
// The world
extern ZoomRaster *WorldRaster;
#endif

// agents
extern int MaxAge, NearView, MaxVision, MaxDistance,
       AllianceRadius, AverFMAge, AverMMAge, MaxNPreys, NPreys;
extern double MaxEnergy, MinEnergy, ReproEnergy, HuntCost, initMutationRate,
       initMeatValue, PredationRisk, AverFBTOtherSex, AverFBTSameSex,
       AverFBTMother, AverFBTChild, AverFBTSibling, AverFEnvy, AverFPity,
       AverMGratitude, AverMVengefulness, AverFTmFct, AverMTmFct,
       AverMBTOtherSex, AverMBTSameSex, AverMBTMother, AverMBTChild,
       AverMBTSibling, AverMEnvy, AverMPity, AverFGratitude, AverFVengefulness;
extern double initMaleAgeImportance, initMaleEnergyImportance;
extern double initFMigrationThreshold, initFPity, initFEnvy,
       initFGenerosity, initFMeatGenerosity, initFBnvlcTSameSex,
       initFBnvlcTOtherSex, initFBnvlcTMother, initFBnvlcTChild,
       initFBnvlcTSibling, initFAgeImportance, initFEnergySel, initFMotherSel,
       initFChildSel, initFSiblingSel, initFFriendSel, initFGratitude,
       initFVengefulness, initFOtherSexSel, initFSameSexSel, initOestrFemSel,
       initFMigAgeImportance, initFMigFriendImportance, initFBravery,
       initFFemaleBravery, initFFVOfNoH, initFFVOfNoCT,
       initFFVOfNoSh, initFMVOfNoH, initFMVOfNoCT, initFMVOfNoSh,
       initFVOfNoSex, initFHuntV, initFPatrolV, initFTmFct, initFWorryELevel,
       initFAcceptInv, initFAcceptMoveInv, initFFearOfHP, initVOfStrg,
       initEnmityThr, initFKidVForMale, initFAudacity, initFLoyalty,
       initFXenophTM, initFXenophTF, initFXenophTFwK, initKFearOfHP, initFNorm,
       initFMetanorm, initFairness, initVOfNotFair;
extern double initMMigrationThreshold, initMPity, initMEnvy,
       initMGenerosity, initMMeatGenerosity, initMBnvlcTOtherSex,
       initMBnvlcTSameSex, initMBnvlcTMother, initMBnvlcTChild,
       initMBnvlcTSibling, initMEnergySel, initMMotherSel, initMChildSel,
       initMSiblingSel, initMFriendSel, initMGratitude, initMVengefulness,
       initMOtherSexSel, initMSameSexSel, initMMigAgeImportance,
       initMMigFriendImportance, initMMaleBravery, initMBravery, initMFVOfNoH,
       initMFVOfNoCT, initMFVOfNoSh, initMMVOfNoH,
       initMMVOfNoCT, initMMVOfNoSh, initMVOfNoSex, initMHuntV, initMPatrolV,
       initMTmFct, initMWorryELevel, initMAcceptInv, initMAcceptMoveInv,
       initMFearOfHP, initVOfMStrg, initFemalePromiscuity, initMKidVForMale,
       initMAudacity, initMLoyalty, initMXenophTM, initMXenophTF,
       initMXenophTFwK, initMNorm, initMMetanorm;

extern Agent *WhiteAgent;

extern List *cellList;
extern List *agList;
extern List *femaleList;
extern List *maleList;
extern List *patchList;
extern List *reaperQueue;
extern Cell ***cellGrid;

// binary tree list of agents
typedef struct AgNamesT{
    Agent *ag;
    char name[8];
    struct AgNamesT *left;
    struct AgNamesT *right;
} AgNames;
extern int AgNamesCount;

extern AgNames *Sentinel;

// terrestrial herbaceous vegetation
extern double MaxPlantEnergy, PlantLogisticGrowth;
extern List *thvList;

// trees
extern List *trList;
extern int TreeSeasonBegin[3], TreeSeasonEnd[3], TreeNFruitsDay[3],
       TreeMaxFruitAge[3], MinTreePatchSize[3], MaxTreePatchSize[3];
extern double  TreeFruitEnergy[3];

// preys
extern List *preyList;

#ifdef DEBUG
extern unsigned int n_dead;
extern FILE *R;
#endif

#define NORMALIZE(x, y)		\
    if(x < 0)			\
x = 0;			\
else				\
if(x >= WorldXSize)	\
x = WorldXSize - 1;	\
if(y < 0)			\
y = 0;			\
else				\
if(y >= WorldYSize)		\
y = WorldYSize - 1;	

#define NAMECOPY(dest, orig)	\
    dest[0] = orig[0];		\
dest[1] = orig[1];		\
dest[2] = orig[2];		\
dest[3] = orig[3];		\
dest[4] = orig[4];		\
dest[5] = orig[5];		\
dest[6] = orig[6];		\
dest[7] = 0;


typedef struct{
    const char *par;
    void *v;
    char type;
    bool read;
} OPT;


class Model
{
    private:
#ifndef NOGUI
        Control *control;
        Observer *observer;
#endif
        int status;
        void getOption(OPT *opt, char *p, char *value, FILE *o, bool *haswarnings);
        void step();
        void reapAgents();
        void zeroForAll();
        void printFrame();
#ifdef DEBUG
        void doubleCountAgents();
#endif

        void finish();

        void createAgents();
        void createPrey(bool randAge);
        void createPreys();
        void calcAverAges();
        void analyse();

        void writeDefaultParameters();
        void readOptionsFile();
        void buildObjects();
        void dumpEntirePop();
        void dumpAgentsMem();
        void outputToFile();
        void randomizePeri();
        int nAg;
        int NTConflicts;

    public:
        Model();
        ~Model();
        void setObserver(Observer *obs);
        void go();
        void setStatus(int s);
        int getStatus();
        int getPerimeterFor(int x, int y, int d, int *xl, int *yl, bool rndmz);
        int runConflictBetween(Agent *ag1, Agent *ag2);

        double getShareMM();
        double getShareFF();
        double getShareMF();
        double getShareFM();
};

#endif
