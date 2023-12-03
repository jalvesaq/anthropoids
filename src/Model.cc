/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#include <unistd.h>
#include "Model.h"
#ifndef NOGUI
#include "Observer.h"
#endif

#ifdef MINGW32
#include <windows.h>
#endif
#include "abmtools.h"
#include "config.h"


extern bool GUIMode;
extern unsigned long int RandomSeed;
inline int compNames(char a[8], char b[8]);

// This macro will avoid calling compNames in 98% of comparisons:
#define MSTRCMP(x, y) ((x[0] == y[0]) ? compNames(x, y) : 1)

Agent *WhiteAgent;


FILE *C, *D;

#ifdef DEBUG
unsigned int n_dead = 0;
#endif

bool ShowPopDensity = true, ShowAgents = true,
     ShowPreys = true, SlowMotion, VerySlowMotion;

int dumpPatchesToScreen, WorldXSize, WorldYSize, Silent, LogInterval,
    ExperimentDuration, DumpPopNow, SimTime, Begin, Hour, Day, Year,
    DayDuration, YearDuration, nSharedMM, nSharedMF, nSharedFF, nSharedFM,
    nBegsMM, nBegsFF, nBegsFM, nBegsMF, ChooseRandomValues,
    RandomAgentActivation, Language, Hunt, FoodShare, Territoriality, Norm,
    Metanorm, MemSize, AgNamesCount, NHunts, NHunters, NFoundPreys, NSuccHunts,
    TN1, TN2, TEnd1, TEnd2, TEnd3, TEnd4, TEnd5, TEnd6;

int MaxAge, NearView, MaxVision, MaxDistance, AllianceRadius,
    AverFMAge, AverMMAge, MaxNPreys, NPreys;
double MaxPreyDensity, Tree1Density, Tree2Density, Tree3Density, MaxEnergy,
       MinEnergy, ReproEnergy, HuntCost, PredationRisk, AverFBTOtherSex,
       AverFBTSameSex, AverFBTMother, AverFBTChild, AverFBTSibling, AverFEnvy,
       AverFPity, AverMGratitude, AverMVengefulness, AverFTmFct, AverMTmFct,
       AverMBTOtherSex, AverMBTSameSex, AverMBTMother, AverMBTChild,
       AverMBTSibling, AverMEnvy, AverMPity, AverFGratitude, AverFVengefulness,
       initVOfStrg, initEnmityThr, initFairness, initVOfNotFair;
double initMutationRate, initMeatValue, initMaleAgeImportance,
       initMaleEnergyImportance;
double initMMigrationThreshold, initMPity, initMEnvy, initMGenerosity,
       initMMeatGenerosity, initMBnvlcTOtherSex, initMBnvlcTSameSex,
       initMBnvlcTMother, initMBnvlcTChild, initMBnvlcTSibling,
       initMEnergyImportance, initMEnergySel, initMMotherSel, initMChildSel,
       initMSiblingSel, initMFriendSel, initMGratitude, initMVengefulness,
       initMOtherSexSel, initMSameSexSel, initOestrFemSel,
       initMMigAgeImportance, initMMigFriendImportance, initMBravery,
       initMFemaleBravery, initMHuntV, initMPatrolV, initMTmFct, initMAcceptInv,
       initMAcceptMoveInv, initMFearOfHP, initFemalePromiscuity,
       initMKidVForMale, initMAudacity, initMLoyalty, initMFVOfNoH,
       initMFVOfNoCT, initMFVOfNoSh, initMMVOfNoH, initMMVOfNoCT, initMMVOfNoSh,
       initFVOfNoSex, initMXenophTM, initMXenophTF, initMXenophTFwK, initMNorm,
       initMMetanorm;
double initFMigrationThreshold, initFPity, initFEnvy, initFGenerosity,
       initFMeatGenerosity, initFBnvlcTSameSex, initFBnvlcTOtherSex,
       initFBnvlcTMother, initFBnvlcTChild, initFBnvlcTSibling, initFEnergySel,
       initFMotherSel, initFChildSel, initFSiblingSel, initFFriendSel,
       initFGratitude, initFVengefulness, initFOtherSexSel, initFSameSexSel,
       initFMigAgeImportance, initFMigFriendImportance, initFMaleBravery,
       initFBravery, initFHuntV, initFPatrolV, initFTmFct, initFAcceptInv,
       initFAcceptMoveInv, initFFearOfHP, initFKidVForMale, initFAudacity,
       initFLoyalty, initFFVOfNoH, initFFVOfNoCT, initFFVOfNoSh, initFMVOfNoH,
       initFMVOfNoCT, initFMVOfNoSh, initMVOfNoSex, initFXenophTM,
       initFXenophTF, initFXenophTFwK, initKFearOfHP, initFNorm, initFMetanorm;

// Tree variables
int TreeN[3], TreeSeasonBegin[3], TreeSeasonEnd[3], TreeNFruitsDay[3],
    TreeMaxFruitAge[3], MinTreePatchSize[3], MaxTreePatchSize[3];
double TreeFruitEnergy[3];
double MaxPlantEnergy, PlantLogisticGrowth;

List *cellList;
List *agList;
List *femaleList;
List *maleList;
List *trList;
List *thvList;
List *patchList;
List *preyList;
List *reaperQueue;
Cell ***cellGrid;

// copy of nameComp (Agent.cc)
inline int compNames(char a[8], char b[8]){
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

void Model::getOption(OPT *opt, char *p, char *value, FILE *o, bool *haswarnings)
{
    int i = 0;
    bool found = false;
    while(!(opt[i].par == NULL || found == true)){
        if(strcmp(opt[i].par, p) == 0){
            if(opt[i].read == true){
                fprintf(stderr, _("parameter \"%s\" alread initialized!\n"), opt[i].par);
                *haswarnings = true;
            }
            found = true;
            switch(opt[i].type){
                case 'd' :
                    *((double*)opt[i].v) = atof(value);
                    fprintf(o, "%s = %f\n", p, *((double*)opt[i].v));
                    break;
                case 'i' :
                    *((int*)opt[i].v) = atoi(value);
                    fprintf(o, "%s = %d\n", p, *((int*)opt[i].v));
                    break;
                default :
                    FATALERR("IMPOSSIBLE!");
            }
            opt[i].read = true;
        }
        i++;
    }
    if(found == false){
        warnMsg( _("option \"%s\" does not exist!\n"), p);
        *haswarnings = true;
    }
}

void Model::setObserver(Observer *obs)
{
#ifndef NOGUI
    observer = obs;
#endif
}

Model::Model()
{
    readOptionsFile();

    if(LogInterval){
        C = fopen("completeRun.csv", "w");
        if(C == NULL)
            FATALERR("(C == NULL)");
        fprintf(C, "time year day hour nAg nF nM nPr nHunts nHunters nFoundPreys "
                "nSuccHunts nBegsMM nBegsFF nBegsFM nBegsMF nSharedMM nSharedFF "
                "nSharedFM nSharedMF NTConflicts "
                "TN1 TN2 TEnd1 TEnd2 TEnd3 TEnd4 TEnd5 TEnd6\n");

        D = fopen("deadOnes.csv", "w");
        if(D == NULL)
            FATALERR("(D == NULL)");
        fprintf(D, "time sex meatValue askMeatOnly bnvlcTOtherSex bnvlcTSameSex "
                "bnvlcTMother bnvlcTSibling bnvlcTChild maleEnergyImportance "
                "maleAgeImportance gratitude vengefulness sameSexSel energySel "
                "childSel motherSel siblingSel friendSel otherSexSel oestrFemSel "
                "migAgeImportance migFriendImportance energy eFromMom eFromShare "
                "eFromConfl eFromDinner eFromHunt generosity meatGenerosity pity envy "
                "mutationRate age childhood maxAge mGen fGen bestMaleAge "
                "femalePromiscuity kidValueForMale nLiveChld migPref nKnownPatches "
                "nFFriends nFEnemies nFFriendInt nFEnemyInt minFRecall maxFRecall "
                "meanFRecall nMFriends nMEnemies nMFriendInt nMEnemyInt minMRecall "
                "maxMRecall meanMRecall bravery audacity loyalty migrating "
                "acceptMigInv acceptMoveInvtFromF acceptMoveInvtFromM deathCause "
                "huntValue patrolValue gratStrategy vengStrategy vengStr2 begStrategy "
                "huntStrategy huntInterval maxHuntPatrolSize vOfFStranger vOfMStranger "
                "enmityThr lowDeficit hiDeficit tmFct xenophTM xenophTF xenophTFwK "
                "fearOfHPWhenHasKid fearOfHPatches mvalueOfNoSh mvalueOfNoH "
                "mvalueOfNoCT fvalueOfNoSh fvalueOfNoH fvalueOfNoCT valueOfNoSex "
                "fvalueOfNotFair mvalueOfNotFair fmd fmni nFr emd emni nEn nInvt "
                "nAcptInvt nAlliChng nRembr adviceValue norm metanorm terriRemStrategy "
                "hasShame zeroPostvNo fairness expFFairness expMFairness\n");
    }

    if(!Silent && !GUIMode){
        fprintf(stderr, _("We're running without graphics for %d timesteps "
                    "(%d years and %d days)."),
                ExperimentDuration, (ExperimentDuration / YearDuration / DayDuration),
                ((ExperimentDuration % (YearDuration * DayDuration)) / DayDuration));
        fprintf(stderr, "\n\n");
    }

    buildObjects();

    if(!Silent && LogInterval){
        fprintf(stderr, "\n");
        if(LogInterval == 1)
            fprintf(stderr,
                    _("The model is logging data every timestep (hour).\n"));
        else
            fprintf(stderr,
                    _("The model is logging data every %d timesteps (hours).\n"),
                    LogInterval);
    }

    status = 2;
}

Model::~Model()
{
    if(C)
        fclose(C);
    if(D)
        fclose(D);

    if(!Silent)
        fprintf(stderr, "\n");
    fflush(stdout);
    fflush(stderr);
    if(LogInterval){
        dumpEntirePop();
        dumpAgentsMem();

        Prey *pr;
        preyList->begin();
        FILE *PF = fopen("preyPositions.csv", "w");
        while((pr = (Prey*)preyList->next()))
            fprintf(PF, "%d %d\n", pr->x, pr->y);
        preyList->end();
        fclose(PF);

        cellList->begin();
        Cell *c;
        double e;
        int ts;
        FILE *CF = fopen("treeTHV.csv", "w");
        fprintf(CF, "x y type species energy\n");
        while((c = (Cell*)cellList->next())){
            if(c->thv){
                e = c->thv->energy / MaxPlantEnergy;
                fprintf(CF, "%d %d %c %d %f\n", c->x, c->y, 'h', 0, e);
            } else{
                ts = c->patch->trSpecies;
                e = ((double)c->tree->nf) / (double)(TreeMaxFruitAge[ts] * TreeNFruitsDay[ts]);
                fprintf(CF, "%d %d %c %d %f\n", c->x, c->y, 't', ts, e);
            }
        }
        fclose(CF);
    }

    agList->begin();
    Agent *ag;
    while((ag = (Agent*)agList->next()))
        ag->goToReaperQueue(9);
    agList->end();

    // Complete deletion of objects for valgrind --leak-chek=full
    agList->deleteAll();
    trList->deleteAll();
    thvList->deleteAll();
    preyList->deleteAll();
    patchList->deleteAll();
    cellList->deleteAll();
    reaperQueue->deleteAll();

    delete(agList);
    delete(maleList);
    delete(femaleList);
    delete(trList);
    delete(thvList);
    delete(cellList);
    delete(patchList);
    delete(preyList);
    int i;
    for(i = 0; i < WorldXSize; i++)
        free(cellGrid[i]);
    free(cellGrid);
    delete(reaperQueue);
    free(Sentinel);
}

void Model::step()
{
    zeroForAll();
    preyList->stepAll();
    agList->stepAll();
    reapAgents();
    if(Hour == 0){  // only run this step once in a day
        trList->stepAll();
        if(RandomAgentActivation)
            agList->shuffleWholeList();
    }
    thvList->stepAll();
    randomizePeri();
}

void Model::analyse()
{
    if (LogInterval == 0) {
        warnMsg(_("Data is not being logged. Change the value of LogInterval in the \"parameters\" file."));
        return;
    }
    int ll = strlen(PACKAGE_DATA_DIR) + strlen("/doc/anthropoids/analyse.R") + 1;
    char *scriptPath = (char*)malloc(ll * sizeof(char));
    strcpy(scriptPath, "analyse.R");
    FILE *f = fopen("analyse.R", "r");
    if(f){
        fclose(f);
        f = NULL;
    } else{
        sprintf(scriptPath, "%s/doc/anthropoids/analyse.R", PACKAGE_DATA_DIR);
        f = fopen(scriptPath, "r");
        if(f == NULL){
            warnMsg(_("Could not find \"%s\"\n"), scriptPath);
        } else{
            fclose(f);
        }
    }
    if(C)
        fflush(C);
    if(D)
        fflush(D);
    dumpEntirePop();
    dumpAgentsMem();
#ifndef NOGUI
    if(GUIMode)
        observer->saveImg();
#endif
    char Rpath [1024];
    strcpy(Rpath, "");
#ifdef MINGW32
    HKEY keyHandle;
    DWORD size1;
    DWORD Type;

    if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                "Software\\R-core\\R", 0,
                KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS)
    {
        size1=1023;
        RegQueryValueEx( keyHandle, "InstallPath", NULL, &Type,
                (LPBYTE)Rpath, &size1);
        strcat(Rpath, "\\bin\\Rcmd.exe BATCH");
        printf("R path: \"%s\"\n", Rpath);
    }     
    RegCloseKey(keyHandle);
#else
    strcpy(Rpath, "R CMD BATCH");
#endif
    if(strlen(Rpath) > 0){
        char *cmd = (char*)calloc(strlen(Rpath) + strlen(scriptPath) + 2, sizeof(char));
        sprintf(cmd, "%s %s", Rpath, scriptPath);
        free(scriptPath);
        printf("\n%s\n", cmd);
        int i = system(cmd);
        if(i != 0){
            warnMsg(_("Error running \"%s\"."), cmd);
        } else{
            if(!GUIMode)
                return;
            char path [1024];
#ifdef MINGW32
            if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                        "Software\\Classes\\acrobat\\shell\\open\\command", 0,
                        KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS)
            {
                size1=1023;
                RegQueryValueEx( keyHandle, NULL, NULL, &Type,
                        (LPBYTE)path, &size1);
                int j = 0;
                while(j < 512){
                    if(path[j] == '.' && path[j+1] == 'e' && path[j+2] == 'x' && path[j+3] == 'e'){
                        path[j+4] = 0;
                        break;
                    }
                    j++;
                }
                printf("Acrobat Reader path: \"%s\"\n", path);
            }     
            else
                printf("Acrobat Reader is not installed! Is it?\n");
            RegCloseKey(keyHandle);
            free(cmd);
            cmd = (char*)calloc(strlen(path) + 14, sizeof(char));
            sprintf(cmd, "%s graphics.pdf", path);
            i = system(cmd);
            if(i != 0)
                warnMsg(_("Error running \"%s\"."), cmd);
#else
            strcpy(path, "/usr/bin/zathura");
            f = fopen(path, "r");
            if(f == NULL){
                strcpy(path, "/usr/bin/evince");
                f = fopen(path, "r");
            }
            if(f == NULL){
                strcpy(path, "/usr/bin/kpdf");
                f = fopen(path, "r");
            }
            if(f){
                fclose(f);
                free(cmd);
                cmd = (char*)calloc(strlen(path) + 14, sizeof(char));
                sprintf(cmd, "%s graphics.pdf", path);
                FILE *p = popen(cmd, "r");
                if (p == NULL) {
                    warnMsg("Failed to run the command: `%s`", cmd);
                }
            } else{
                warnMsg(_("Could not run a pdf reader."));
            }
#endif
        }
        free(cmd);
    } else{
        warnMsg(_("Is R installed?"));
    }
}

void Model::go()
{
    while(status > 0){
        SimTime++;
        step();
        if(status == 3){ // analyse data
            status = 2; // run at full speed
            analyse();
        }
        if(GUIMode){
#ifndef NOGUI
            observer->update();
            if(status == 1) // step
                break;
#endif
        } else
            if(SimTime > ExperimentDuration)
                status = 0; // finish now
    }
}

void Model::setStatus(int s)
{
    status = s;
}

int Model::getStatus()
{
    return status;
}

void Model::printFrame()
{
    fprintf(stderr,"\033[2J\033[H ");
    int k = 0;
    while(k < WorldXSize && k < 99){
        if((k % 10) == 0)
            fprintf(stderr, "\033[1;%dH\033[36m0\033[m\033[%d;%dH\033[36m0\033[m",
                    k+2, WorldYSize + 2, k+2);
        else
            fprintf(stderr, "\033[1;%dH%d\033[%d;%dH%d", k+2, (k % 10),
                    WorldYSize + 2, k+2, (k % 10));
        k++;
    }

    k = 0;
    while(k < WorldYSize && k < 40){
        if((k % 10) == 0)
            fprintf(stderr, "\033[%d;1H\033[36m0\033[m\033[%d;%dH\033[36m0\033[m",
                    k+2, k+2, WorldXSize + 2);
        else
            fprintf(stderr, "\033[%d;1H%d\033[%d;%dH%d", k+2, (k % 10), k+2,
                    WorldXSize + 2, (k % 10));
        k++;
    }
    fprintf(stderr, "\033[%d;3H \n", (WorldYSize + 3));
}

void Model::writeDefaultParameters()
{
    FILE *f = fopen("parameters", "w");
    if(f == NULL)
        FATALERR(_("Error creating file \"parameters\" with default values."));

    fprintf(f, 
            "#################################################################\n"
            "# Random values for all init variables?\n"
            "ChooseRandomValues = 1\n"
            "#################################################################\n"
            "\n"
            "#################################################################\n"
            "# Do agents have these features:\n"
            "Language = 1\n"
            "Hunt = 1\n"
            "FoodShare = 1\n"
            "Territoriality = 1\n"
            "Norm = 1\n"
            "Metanorm = 1\n"
            "MemSize = 4\n"
            "#################################################################\n"
            "\n"
            "# Global variables\n"
            "\n"
            "WorldXSize = 20\n"
            "WorldYSize = 18\n"
            "DayDuration = 4			# Number of hours a single day has\n"
            "YearDuration = 50		# Number of days a single year has\n"
            "ExperimentDuration = 492000000\n"
            "Silent = 0\n"
            "LogInterval = 0			# 0 for not logging data\n"
            "dumpPatchesToScreen = 0\n"
            "RandomAgentActivation = 1	# Shuffle agent list every time step\n"
            "\n"
            "# Variables used to build the terrestrial herbaceous vegetation\n"
            "\n"
            "MaxPlantEnergy = 1.1\n"
            "PlantLogisticGrowth = 0.01\n"
            "\n"
            "# Variable used to build the trees\n"
            "\n"
            "Tree1Density = 0.025		# Relative number of trees of specie 1\n"
            "Tree1SeasonBegin = 1		# Begin of fruit season\n"
            "Tree1SeasonEnd = 49		# End of fruit season\n"
            "Tree1NFruitsDay = 30		# Number of fruits produced each day\n"
            "Tree1MaxFruitAge = 14		# Age after which a fruit becomes roted\n"
            "Tree1FruitEnergy = 0.25		# Amount of energy of each fruit\n"
            "MinTree1PatchSize = 3\n"
            "MaxTree1PatchSize = 5\n"
            "\n"
            "Tree2Density = 0.015\n"
            "Tree2SeasonBegin = 20\n"
            "Tree2SeasonEnd = 35\n"
            "Tree2NFruitsDay = 60\n"
            "Tree2MaxFruitAge = 5\n"
            "Tree2FruitEnergy = 0.23\n"
            "MinTree2PatchSize = 2\n"
            "MaxTree2PatchSize = 4\n"
            "\n"
            "Tree3Density = 0.02\n"
            "Tree3SeasonBegin = 30\n"
            "Tree3SeasonEnd = 49\n"
            "Tree3NFruitsDay = 90\n"
            "Tree3MaxFruitAge = 27\n"
            "Tree3FruitEnergy = 0.39\n"
            "MinTree3PatchSize = 4\n"
            "MaxTree3PatchSize = 5\n"
            "\n"
            "# Variables used during the entire simulation\n"
            "\n"
            "MaxEnergy = 180.0\n"
            "HuntCost = 0.5\n"
            "PredationRisk = 0.0001\n"
            "MaxPreyDensity = 0.03\n"
            "\n"
            "# Variables with the 'init' prefix are used only to build\n"
            "# the first population of agents\n"
            "\n"
            "initMutationRate = 0.8\n"
            "MaxVision = 6\n"
            "NearView = 3\n"
            "\n"
            "# How far must an agent look for allies when building an alliance:\n"
            "AllianceRadius = 2\n"
            "\n"
            "# Variables used by a female to choose the father of its kid:\n"
            "initMaleAgeImportance = 0.8\n"
            "initMaleEnergyImportance = 0.5\n"
            "initFemalePromiscuity = 0.3\n"
            "\n"
            "initMKidVForMale = 16.0\n"
            "initFKidVForMale = 3.0\n"
            "\n"
            "# Variables used in migration:\n"
            "initMMigAgeImportance = 0.2\n"
            "initMMigFriendImportance = 0.2\n"
            "initFMigAgeImportance = 1.0\n"
            "initFMigFriendImportance = 0.2\n"
            "initFAcceptInv = 0.8\n"
            "initFAcceptMoveInv = 0.6\n"
            "initMAcceptInv = 0.8\n"
            "initMAcceptMoveInv = 0.6\n"
            "\n"
            "# Variables that determine the probability of sharing food.\n"
            "initMeatValue = 0.5\n"
            "initMGenerosity = 0.4\n"
            "initMMeatGenerosity = 0.4\n"
            "initMPity = 0.1\n"
            "initMEnvy = 0.1\n"
            "initFGenerosity = -0.3\n"
            "initFMeatGenerosity = 0.5\n"
            "initFPity = 0.5\n"
            "initFEnvy = 0.6\n"
            "initMBnvlcTOtherSex = 1.2\n"
            "initMBnvlcTSameSex = -0.5\n"
            "initMBnvlcTMother = 1.0\n"
            "initMBnvlcTChild = 0.6\n"
            "initMBnvlcTSibling = 0.5\n"
            "initFBnvlcTMother = 0.6\n"
            "initFBnvlcTChild = 0.5\n"
            "initFBnvlcTSibling = 0.2\n"
            "initFBnvlcTOtherSex = 1.0\n"
            "initFBnvlcTSameSex = -0.2\n"
            "initMGratitude = 0.6\n"
            "initFGratitude = 0.6\n"
            "initMVengefulness = 0.1\n"
            "initFVengefulness = 0.1\n"
            "\n"
            "# Variables that determine the value of a cell (where to go).\n"
            "# Their values might real number:\n"
            "initMChildSel = 0.4\n"
            "initMMotherSel = 0.5\n"
            "initMSiblingSel = 0.0\n"
            "initMFriendSel = 0.5\n"
            "initMOtherSexSel = 1.0\n"
            "initMSameSexSel = 0.5\n"
            "initOestrFemSel = 2.0\n"
            "initMEnergySel = 0.8\n"
            "initFMotherSel = 0.1\n"
            "initFChildSel = 0.1\n"
            "initFSiblingSel = 1.0\n"
            "initFFriendSel = 0.5\n"
            "initFOtherSexSel = 0.3\n"
            "initFSameSexSel = 0.1\n"
            "initFEnergySel = 0.4\n"
            "\n"
            "# Variables related with conflict and Hunt.\n"
            "# Their values must be between 0 and 1:\n"
            "initMBravery = 1.0\n"
            "initMAudacity = 0.8\n"
            "initMLoyalty = 0.2\n"
            "initFVOfNoSex = -1.0\n"
            "initMVOfNoSex = -1.0\n"
            "initFFVOfNoH = 0.2\n"
            "initMFVOfNoH = 0.0\n"
            "initFMVOfNoH = 0.0\n"
            "initMMVOfNoH = 0.0\n"
            "initMFVOfNoSh = 0.0\n"
            "initFFVOfNoSh = -1.2\n"
            "initMMVOfNoSh = -1.9\n"
            "initFMVOfNoSh = -1.2\n"
            "initMMVOfNoCT = -1.0\n"
            "initMFVOfNoCT = -1.0\n"
            "initFFVOfNoCT = -0.5\n"
            "initFMVOfNoCT = -0.5\n"
            "initMHuntV = 0.5\n"
            "initMPatrolV = 0.4\n"
            "initMTmFct = 0.5\n"
            "initFBravery = 1.1\n"
            "initFAudacity = 0.8\n"
            "initFLoyalty = 0.2\n"
            "initFHuntV = 0.5\n"
            "initFPatrolV = 0.4\n"
            "initFTmFct = 0.5\n"
            "initFXenophTM = 0.5\n"
            "initFXenophTF = 0.5\n"
            "initFXenophTFwK = 0.5\n"
            "initKFearOfHP = 0.5\n"
            "initFFearOfHP = 0.7\n"
            "initMXenophTM = 0.5\n"
            "initMXenophTF = 0.5\n"
            "initMXenophTFwK = 0.5\n"
            "initMFearOfHP = 0.5\n"
            "initMNorm = 0.5\n"
            "initFNorm = 0.5\n"
            "initMMetanorm = 0.5\n"
            "initFMetanorm = 0.5\n"
            "\n"
            "initVOfStrg = 0.2\n"
            "initEnmityThr = -1.5\n"
            "initFairness = 1.0\n"
            "initVOfNotFair = 1.0\n"
            );
    fclose(f);
    if(!Silent)
        fprintf(stderr, "\n");
    warnMsg(_("The file \"parameters\" was written with default values.\n"
                "         Edit it to change \"anthropoids\" behavior."));
    if(!Silent)
        fprintf(stderr, "\n");
}

void Model::readOptionsFile()
{
    setlocale (LC_NUMERIC, "C");
    char parameter[64], value[64], b[128], *s;
    FILE *f = fopen("parameters", "r");
    if(f == NULL){
        writeDefaultParameters();
        f = fopen("parameters", "r");
        if(f == NULL)
            FATALERR(_("Error opening file \"parameters\""));
    }
    FILE *o = fopen("lastParameters", "w");
    if(o == NULL)
        FATALERR(_("Error writing file \"lastParameters\""));
    fprintf (o, "Random seed = %lu\n", RandomSeed);
    int i;
    OPT opt[] = {
        {"Language", &Language, 'i', false},
        {"Hunt", &Hunt, 'i', false},
        {"FoodShare", &FoodShare, 'i', false},
        {"Territoriality", &Territoriality, 'i', false},
        {"Norm", &Norm, 'i', false},
        {"Metanorm", &Metanorm, 'i', false},
        {"MemSize", &MemSize, 'i', false},
        {"WorldXSize", &WorldXSize, 'i', false},
        {"WorldYSize", &WorldYSize, 'i', false},
        {"LogInterval", &LogInterval, 'i', false},
        {"Silent", &Silent, 'i', false},
        {"DayDuration", &DayDuration, 'i', false},
        {"YearDuration", &YearDuration, 'i', false},
        {"ExperimentDuration", &ExperimentDuration, 'i', false},
        {"MaxPlantEnergy", &MaxPlantEnergy, 'd', false},
        {"PlantLogisticGrowth", &PlantLogisticGrowth, 'd', false},
        {"Tree1Density", &Tree1Density, 'd', false},
        {"Tree1SeasonBegin", &(TreeSeasonBegin[0]), 'i', false},
        {"Tree1SeasonEnd", &(TreeSeasonEnd[0]), 'i', false},
        {"Tree1NFruitsDay", &(TreeNFruitsDay[0]), 'i', false},
        {"Tree1MaxFruitAge", &(TreeMaxFruitAge[0]), 'i', false},
        {"Tree1FruitEnergy", &(TreeFruitEnergy[0]), 'd', false},
        {"MinTree1PatchSize", &(MinTreePatchSize[0]), 'i', false},
        {"MaxTree1PatchSize", &(MaxTreePatchSize[0]), 'i', false},
        {"Tree2Density", &Tree2Density, 'd', false},
        {"Tree2SeasonBegin", &(TreeSeasonBegin[1]), 'i', false},
        {"Tree2SeasonEnd", &(TreeSeasonEnd[1]), 'i', false},
        {"Tree2NFruitsDay", &(TreeNFruitsDay[1]), 'i', false},
        {"Tree2MaxFruitAge", &(TreeMaxFruitAge[1]), 'i', false},
        {"Tree2FruitEnergy", &(TreeFruitEnergy[1]), 'd', false},
        {"MinTree2PatchSize", &(MinTreePatchSize[1]), 'i', false},
        {"MaxTree2PatchSize", &(MaxTreePatchSize[1]), 'i', false},
        {"Tree3Density", &Tree3Density, 'd', false},
        {"Tree3SeasonBegin", &(TreeSeasonBegin[2]), 'i', false},
        {"Tree3SeasonEnd", &(TreeSeasonEnd[2]), 'i', false},
        {"Tree3NFruitsDay", &(TreeNFruitsDay[2]), 'i', false},
        {"Tree3MaxFruitAge", &(TreeMaxFruitAge[2]), 'i', false},
        {"Tree3FruitEnergy", &(TreeFruitEnergy[2]), 'd', false},
        {"MinTree3PatchSize", &(MinTreePatchSize[2]), 'i', false},
        {"MaxTree3PatchSize", &(MaxTreePatchSize[2]), 'i', false},
        {"MaxEnergy", &MaxEnergy, 'd', false},
        {"HuntCost", &HuntCost, 'd', false},
        {"initMutationRate", &initMutationRate, 'd', false},
        {"initMeatValue", &initMeatValue, 'd', false},
        {"MaxVision", &MaxVision, 'i', false},
        {"NearView", &NearView, 'i', false},
        {"AllianceRadius", &AllianceRadius, 'i', false},
        {"PredationRisk", &PredationRisk, 'd', false},
        {"ChooseRandomValues", &ChooseRandomValues, 'i', false},
        {"RandomAgentActivation", &RandomAgentActivation, 'i', false},
        {"MaxPreyDensity", &MaxPreyDensity, 'd', false},
        {"initMEnergySel", &initMEnergySel, 'd', false},
        {"initFEnergySel", &initFEnergySel, 'd', false},
        {"initMMotherSel", &initMMotherSel, 'd', false},
        {"initMChildSel", &initMChildSel, 'd', false},
        {"initMSiblingSel", &initMSiblingSel, 'd', false},
        {"initFMotherSel", &initFMotherSel, 'd', false},
        {"initFChildSel", &initFChildSel, 'd', false},
        {"initFSiblingSel", &initFSiblingSel, 'd', false},
        {"initMFriendSel", &initMFriendSel, 'd', false},
        {"initFFriendSel", &initFFriendSel, 'd', false},
        {"initMOtherSexSel", &initMOtherSexSel, 'd', false},
        {"initFOtherSexSel", &initFOtherSexSel, 'd', false},
        {"initMSameSexSel", &initMSameSexSel, 'd', false},
        {"initFSameSexSel", &initFSameSexSel, 'd', false},
        {"initOestrFemSel", &initOestrFemSel, 'd', false},
        {"initMMigAgeImportance", &initMMigAgeImportance, 'd', false},
        {"initFMigAgeImportance", &initFMigAgeImportance, 'd', false},
        {"initMMigFriendImportance", &initMMigFriendImportance, 'd', false},
        {"initFMigFriendImportance", &initFMigFriendImportance, 'd', false},
        {"initMGratitude", &initMGratitude, 'd', false},
        {"initFGratitude", &initFGratitude, 'd', false},
        {"initMVengefulness", &initMVengefulness, 'd', false},
        {"initFVengefulness", &initFVengefulness, 'd', false},
        {"initMPity", &initMPity, 'd', false},
        {"initFPity", &initFPity, 'd', false},
        {"initMEnvy", &initMEnvy, 'd', false},
        {"initFEnvy", &initFEnvy, 'd', false},
        {"initMGenerosity", &initMGenerosity, 'd', false},
        {"initFGenerosity", &initFGenerosity, 'd', false},
        {"initMMeatGenerosity", &initMMeatGenerosity, 'd', false},
        {"initFMeatGenerosity", &initFMeatGenerosity, 'd', false},
        {"initMBravery", &initMBravery, 'd', false},
        {"initFBravery", &initFBravery, 'd', false},
        {"initFLoyalty", &initFLoyalty, 'd', false},
        {"initMLoyalty", &initMLoyalty, 'd', false},
        {"initFAudacity", &initFAudacity, 'd', false},
        {"initMAudacity", &initMAudacity, 'd', false},
        {"initMBnvlcTOtherSex", &initMBnvlcTOtherSex, 'd', false},
        {"initFBnvlcTOtherSex", &initFBnvlcTOtherSex, 'd', false},
        {"initMBnvlcTSameSex", &initMBnvlcTSameSex, 'd', false},
        {"initFBnvlcTSameSex", &initFBnvlcTSameSex, 'd', false},
        {"initMBnvlcTMother", &initMBnvlcTMother, 'd', false},
        {"initMBnvlcTChild", &initMBnvlcTChild, 'd', false},
        {"initMBnvlcTSibling", &initMBnvlcTSibling, 'd', false},
        {"initFBnvlcTMother", &initFBnvlcTMother, 'd', false},
        {"initFBnvlcTChild", &initFBnvlcTChild, 'd', false},
        {"initFBnvlcTSibling", &initFBnvlcTSibling, 'd', false},
        {"initFemalePromiscuity", &initFemalePromiscuity, 'd', false},
        {"initMaleAgeImportance", &initMaleAgeImportance, 'd', false},
        {"initMaleEnergyImportance", &initMaleEnergyImportance, 'd', false},
        {"initMKidVForMale", &initMKidVForMale, 'd', false},
        {"initFKidVForMale", &initFKidVForMale, 'd', false},
        {"dumpPatchesToScreen", &dumpPatchesToScreen, 'i', false},
        {"initFFVOfNoSh", &initFFVOfNoSh, 'd', false},
        {"initFFVOfNoCT", &initFFVOfNoCT, 'd', false},
        {"initFFVOfNoH", &initFFVOfNoH, 'd', false},
        {"initMFVOfNoSh", &initMFVOfNoSh, 'd', false},
        {"initMFVOfNoCT", &initMFVOfNoCT, 'd', false},
        {"initMFVOfNoH", &initMFVOfNoH, 'd', false},
        {"initFMVOfNoSh", &initFMVOfNoSh, 'd', false},
        {"initFMVOfNoCT", &initFMVOfNoCT, 'd', false},
        {"initFMVOfNoH", &initFMVOfNoH, 'd', false},
        {"initMMVOfNoSh", &initMMVOfNoSh, 'd', false},
        {"initMVOfNoSex", &initMVOfNoSex, 'd', false},
        {"initFVOfNoSex", &initFVOfNoSex, 'd', false},
        {"initMMVOfNoCT", &initMMVOfNoCT, 'd', false},
        {"initMMVOfNoH", &initMMVOfNoH, 'd', false},
        {"initMHuntV", &initMHuntV, 'd', false},
        {"initMPatrolV", &initMPatrolV, 'd', false},
        {"initMTmFct", &initMTmFct, 'd', false},
        {"initFHuntV", &initFHuntV, 'd', false},
        {"initFPatrolV", &initFPatrolV, 'd', false},
        {"initFTmFct", &initFTmFct, 'd', false},
        {"initFAcceptInv", &initFAcceptInv, 'd', false},
        {"initMAcceptInv", &initMAcceptInv, 'd', false},
        {"initFAcceptMoveInv", &initFAcceptMoveInv, 'd', false},
        {"initMAcceptMoveInv", &initMAcceptMoveInv, 'd', false},
        {"initFFearOfHP", &initFFearOfHP, 'd', false},
        {"initMFearOfHP", &initMFearOfHP, 'd', false},
        {"initVOfStrg", &initVOfStrg, 'd', false},
        {"initEnmityThr", &initEnmityThr, 'd', false},
        {"initFairness", &initFairness, 'd', false},
        {"initVOfNotFair", &initVOfNotFair, 'd', false},
        {"initMXenophTM", &initMXenophTM, 'd', false},
        {"initMXenophTF", &initMXenophTF, 'd', false},
        {"initMXenophTFwK", &initMXenophTFwK, 'd', false},
        {"initFXenophTM", &initFXenophTM, 'd', false},
        {"initFXenophTF", &initFXenophTF, 'd', false},
        {"initFXenophTFwK", &initFXenophTFwK, 'd', false},
        {"initKFearOfHP", &initKFearOfHP, 'd', false},
        {"initMNorm", &initMNorm, 'd', false},
        {"initFNorm", &initFNorm, 'd', false},
        {"initMMetanorm", &initMMetanorm, 'd', false},
        {"initFMetanorm", &initFMetanorm, 'd', false},
        {NULL, NULL, 0, false}
    };

    bool haswarnings = false;
    while(fgets(b, 127, f)){
        s = b;
        while(s[0] == ' ' || s[0] == '\t')
            s++;
        if(strlen(s) < 4)
            continue;
        if(s[0] == '#')
            continue;
        for(i = 0; i < 64; i++){
            parameter[i] = 0;
            value[i] = 0;
        }
        i = 0;
        while(i < 64 && ((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' &&
                        s[0] <= 'Z')
                    || (s[0] >= '0' && s[0] <= '9'))){
            parameter[i] = s[0];
            i++;
            s++;
        }
        while(s[0] == ' ' || s[0] == '\t' || s[0] == '=' || s[0] == '\"')
            s++;
        i = 0;
        while(i < 64 && s[i] != '\n'){
            value[i] = s[i];
            i++;
        }
        i--;
        while(value[i] == ' ' || value[i] == '\t' || value[i] == '\"'){
            value[i] = 0;
            i--;
        }
        getOption(opt, parameter, value, o, &haswarnings);
    }
    fclose(f);

    if(!Silent)
        fprintf(stderr, "\n");
    i = 0;
    while(opt[i].par != NULL){
        if(opt[i].read == false){
            fprintf(stderr, _("parameter \"%s\" is unitialized!\n"), opt[i].par);
            haswarnings = true;
        }
        i++;
    }
    if(!Silent && haswarnings)
        fprintf(stderr, "\n");
    if(haswarnings){
        dumpPatchesToScreen = 0;
#ifndef MINGW32
        sleep(2);
#endif
    }

    MaxNPreys = (int)(round(MaxPreyDensity * (double)WorldXSize * (double)WorldYSize));
    fprintf(o, "# MaxNPreys = %d\n", MaxNPreys);
    TreeN[0] = (int)(round(Tree1Density * (double)WorldXSize * (double)WorldYSize));
    TreeN[1] = (int)(round(Tree2Density * (double)WorldXSize * (double)WorldYSize));
    TreeN[2] = (int)(round(Tree3Density * (double)WorldXSize * (double)WorldYSize));
    fprintf(o, "# Number of trees: %d, %d and %d\n", TreeN[0], TreeN[1], TreeN[2]);
    if((TreeN[0] + TreeN[1] + TreeN[2]) == 0){
        warnMsg("The world must have at list one tree.");
        exit(1);
    }
    double e1 = (double)(TreeN[0] * TreeNFruitsDay[0] * (TreeSeasonEnd[0] - TreeSeasonBegin[0])) * TreeFruitEnergy[0];
    double e2 = (double)(TreeN[1] * TreeNFruitsDay[1] * (TreeSeasonEnd[1] - TreeSeasonBegin[1])) * TreeFruitEnergy[1];
    double e3 = (double)(TreeN[2] * TreeNFruitsDay[2] * (TreeSeasonEnd[2] - TreeSeasonBegin[2])) * TreeFruitEnergy[2];
    double nAg1 = 0.8 * (e1 + e2 + e3);
    unsigned int nTr = TreeN[0] + TreeN[1] + TreeN[2];
    double nAg2 = 0.3 * (double)((WorldXSize * WorldYSize) - nTr) * MaxPlantEnergy;
    nAg = (int)((nAg1 + nAg2) / double(YearDuration * DayDuration));
    fprintf(o, "# Number of agents: %d\n", nAg);

    if((Tree1Density + Tree2Density + Tree3Density) >= 1.0){
        warnMsg(_("The world dimensions (%d x %d) aren't enough to carry %d "
                    "trees! Please, choose a lower density for trees.\n"),
                WorldXSize, WorldYSize, (TreeN[0]  + TreeN[1] + TreeN[2]));
        exit(1);
    }

    for(i = 0; i < 3; i++){
        if(TreeSeasonBegin[i] >= TreeSeasonEnd[i]){
            warnMsg(_("Tree%dSeasonEnd must be bigger than "
                        "Tree%dSeasonBegin!\n"), i+1, i+1);
            exit(1);
        }
        if(TreeSeasonEnd[i] >= YearDuration){
            warnMsg(_("Tree%dSeasonEnd must be lower than YearDuration!\n"), i);
            exit(1);
        }
    }
    if(WorldXSize < (MaxVision * 2) || WorldYSize < (MaxVision * 2)){
        fprintf(stderr, _("Both WorldXSize and WorldYSize must be at least "
                    "twice as big as MaxVision!\n"));
    }
    if(MaxVision < NearView){
        fprintf(stderr, _("MaxVision can't be shorter than NearView!\n"));
    }
    if(MaxVision > AllianceRadius)
        MaxDistance = MaxVision;
    else
        MaxDistance = AllianceRadius;
    MinEnergy = MaxEnergy * 0.3;
    ReproEnergy = MaxEnergy - 4;
    if(MaxEnergy < 5.0){
        fprintf(stderr, _("MaxEnergy must be at least 5.0!\n"));
    }
    Begin = DayDuration * YearDuration;
    MaxAge = (int)(16.0 * MaxEnergy); // 16 times first pop. childh. duration.

    fprintf(o, "# MaxAge = %d\n", MaxAge);
    fclose(o);
}

int Model::getPerimeterFor(int x, int y, int d, int *xl, int *yl, bool rndmz)
{
    static int zigzag = 0;
    int xa, xb, ya, yb, r, i, j, n = 0;
    xa = x - d;
    ya = y - d;
    xb = x + d;
    yb = y + d;
    for(i = xa; i <= xb; i++){
        if(i >= 0 && i < WorldXSize){
            if(ya >= 0){
                xl[n] = i;
                yl[n] = ya;
                n++;
            }
            if(yb < WorldYSize){
                xl[n] = i;
                yl[n] = yb;
                n++;
            }
        }
    }
    for(j = (ya + 1); j < yb; j++){
        if(j >= 0 && j < WorldYSize){
            if(xa >= 0){
                xl[n] = xa;
                yl[n] = j;
                n++;
            }
            if(xb < WorldXSize){
                xl[n] = xb;
                yl[n] = j;
                n++;
            }
        }
    }

    if(rndmz){
        int s;
        for(i = 0; i < 100; i++){
            r = getRandInt(0, (n - 1));
            s = getRandInt(0, (n - 1));
            x = xl[s];
            y = yl[s];
            xl[s] = xl[r];
            yl[s] = yl[r];
            xl[r] = x;
            yl[r] = y;
        }
        if(zigzag){
            zigzag = 0;
            i = 0;
            while(i < n){
                x = xl[i];
                y = yl[i];
                r = getRandInt(0, (n - 1));
                xl[i] = xl[r];
                yl[i] = yl[r];
                xl[r] = x;
                yl[r] = y;
                i++;
            }
        } else{
            zigzag = 1;
            i = n;
            while(i > 0){
                i--;
                x = xl[i];
                y = yl[i];
                r = getRandInt(0, (n - 1));
                xl[i] = xl[r];
                yl[i] = yl[r];
                xl[r] = x;
                yl[r] = y;
            }
        }
    }
#ifdef DEBUG
    for(i = 0; i < n; i++)
        if(xl[i] >= WorldXSize || yl[i] >= WorldYSize)
            FATALERR("xl[i] = %d, yl[i] = %d\n", xl[i], yl[i]);
#endif
    return n;
}

void Model::calcAverAges()
{
    double Gratitude = 0.0, Vengefulness = 0.0, TmFct = 0.0, BnvlcTOtherSex = 0.0,
           BnvlcTSameSex = 0.0, BnvlcTMother = 0.0, BnvlcTChild = 0.0,
           BnvlcTSibling = 0.0, Envy = 0.0, Pity = 0.0, Age = 0.0, n;
    Agent *ag;
    maleList->begin();
    while((ag = (Agent*)maleList->next())){
        Age += ((double)(ag->age));
        Gratitude += ag->gratitude;
        Vengefulness += ag->vengefulness;
        TmFct += ag->tmFct;
        BnvlcTOtherSex += ag->bnvlcTOtherSex;
        BnvlcTSameSex += ag->bnvlcTSameSex;
        BnvlcTMother += ag->bnvlcTMother;
        BnvlcTChild += ag->bnvlcTChild;
        BnvlcTSibling += ag->bnvlcTSibling;
        Envy += ag->envy;
        Pity += ag->pity;
    }
    maleList->end();
    n = (double)(maleList->getCount());
    AverMMAge = (int)(Age / n);
    AverMGratitude = Gratitude / n;
    AverMVengefulness = Vengefulness / n;
    AverMBTOtherSex =  BnvlcTOtherSex / n;
    AverMBTSameSex =  BnvlcTSameSex / n;
    AverMBTMother = BnvlcTMother / n;
    AverMBTChild = BnvlcTChild / n;
    AverMBTSibling = BnvlcTSibling / n;
    AverMEnvy = Envy / n;
    AverMPity = Pity / n;
    AverMTmFct = TmFct / n;

    Age = 0.0;
    Gratitude = 0.0;
    Vengefulness = 0.0;
    TmFct = 0.0;
    BnvlcTOtherSex = 0.0;
    BnvlcTSameSex = 0.0;
    Pity = 0.0;
    BnvlcTMother = 0.0;
    BnvlcTChild = 0.0;
    BnvlcTSibling = 0.0;
    Envy = 0.0;
    femaleList->begin();
    while((ag = (Agent*)femaleList->next())){
        Age += ag->age;
        Gratitude += ag->gratitude;
        Vengefulness += ag->vengefulness;
        TmFct += ag->tmFct;
        BnvlcTOtherSex += ag->bnvlcTOtherSex;
        BnvlcTSameSex += ag->bnvlcTSameSex;
        BnvlcTMother += ag->bnvlcTMother;
        BnvlcTChild += ag->bnvlcTChild;
        BnvlcTSibling += ag->bnvlcTSibling;
        Envy += ag->envy;
        Pity += ag->pity;
    }
    femaleList->end();
    n = femaleList->getCount();
    AverFMAge = (int)(Age / n);
    AverFGratitude = Gratitude / n;
    AverFVengefulness = Vengefulness / n;
    AverFBTOtherSex =  BnvlcTOtherSex / n;
    AverFBTSameSex =  BnvlcTSameSex / n;
    AverFBTMother = BnvlcTMother / n;
    AverFBTChild = BnvlcTChild / n;
    AverFBTSibling = BnvlcTSibling / n;
    AverFEnvy = Envy / n;
    AverFPity = Pity / n;
    AverFTmFct = TmFct / n;
}

void Model::reapAgents()
{
    Hour++;
    if(Hour == DayDuration){
        Hour = 0;
        Day++;
        if(Day == YearDuration){
            Day = 0;
            Year++;
            if((Year % 10) == 0)
                calcAverAges();
            if(!Silent){
                if((Year % 50) == 0)
                    fprintf(stderr, ": %i (%i f, %i m, %i p)\n", Year,
                            femaleList->getCount(), maleList->getCount(),
                            preyList->getCount());
                else
                    if((Year % 10) == 0)
                        fprintf(stderr, ":");
                    else
                        fprintf(stderr, ".");
                fflush(stderr);
            }
        }
    }

#ifdef DEBUG
    if(n_dead != (unsigned int)reaperQueue->getCount())
        FATALERR("(n_dead != reaperQueue->getCount()) (%d != %d)",
                n_dead, reaperQueue->getCount());
    n_dead = 0;
#endif
    int s;
    int ff, fe, fei, ffi;
    int mf, me, mei, mfi;
    double fminr, fmaxr, fmeanr;
    double mminr, mmaxr, mmeanr;
    double fmd, fmni, emd, emni;
    int nFr, nEn;
    Agent *a;
    reaperQueue->begin();
    while((a = (Agent*)reaperQueue->next())){
        if(a->sex == 'f')
            s = 1;
        else
            s = 0;
        if(LogInterval && (SimTime % LogInterval) == 0){
            a->getNFFriends(&ff, &fe, &ffi, &fei, &fminr, &fmaxr, &fmeanr);
            a->getNMFriends(&mf, &me, &mfi, &mei, &mminr, &mmaxr, &mmeanr);
            a->getFrMeanDist(&fmd, &fmni, &nFr, &emd, &emni, &nEn);
            fprintf(D, "%d %d %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f "
                    "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d "
                    "%d %d %d %f %f %d %d %d %d %d %d %d %f %f %f %d %d %d %d %f "
                    "%f %f %f %f %f %d %d %d %d %d %f %f %d %d %d %d %d "
                    "%d %d %f %f %f %f %f %f %d %d %d %d %d %f %f %f %f %f "
                    "%f %f %f %f %f %f %d %f %f %d %d %d %d %d %f %d %d %d %d %d %f %f %f\n",
                    SimTime, s, a->meatValue, a->askMeatOnly, a->bnvlcTOtherSex,
                    a->bnvlcTSameSex, a->bnvlcTMother, a->bnvlcTSibling, a->bnvlcTChild,
                    a->maleEnergyImportance, a->maleAgeImportance, a->gratitude,
                    a->vengefulness, a->sameSexSel, a->energySel, a->childSel,
                    a->motherSel, a->siblingSel, a->friendSel, a->otherSexSel,
                    a->oestrFemSel, a->migAgeImportance, a->migFriendImportance,
                    a->energy, a->eFromMom, a->eFromShare, a->eFromConfl, a->eFromDinner,
                    a->eFromHunt, a->generosity, a->meatGenerosity, a->pity, a->envy,
                    a->mutationRate, a->age, a->childhood, a->maxAge, a->mGen, a->fGen,
                    a->bestMaleAge, a->femalePromiscuity, a->kidValueForMale,
                    a->getNChildren(), a->getMigPref(), a->getNKnownPatches(), ff, fe,
                    ffi, fei, fminr, fmaxr, fmeanr, mf, me, mfi, mei, mminr, mmaxr,
                    mmeanr, a->bravery, a->audacity, a->loyalty, a->migrating,
                    a->acceptMigInv, a->acceptMoveInvtFromF, a->acceptMoveInvtFromM,
                    a->deathCause, a->huntValue, a->patrolValue, a->gratStrategy,
                    a->vengStrategy, a->vengStr2, a->begStrategy, a->huntStrategy,
                    a->huntInterval, a->maxHuntPatrolSize, a->vOfFStranger,
                    a->vOfMStranger, a->enmityThr, a->lowDeficit, a->hiDeficit, a->tmFct,
                    a->xenophTM, a->xenophTF, a->xenophTFwK, a->fearOfHPWhenHasKid,
                    a->fearOfHPatches, a->mvalueOfNoSh, a->mvalueOfNoH, a->mvalueOfNoCT,
                    a->fvalueOfNoSh, a->fvalueOfNoH, a->fvalueOfNoCT, a->valueOfNoSex,
                    a->fvalueOfNotFair, a->mvalueOfNotFair, fmd, fmni, nFr, emd, emni,
                    nEn, a->nInvt, a->nAcptInvt, a->nAlliChng, a->nRembr, a->adviceValue,
                    a->norm, a->metanorm, a->terriRemStrategy, a->hasShame,
                    a->zeroPostvNo, a->fairness, a->expFFairness, a->expMFairness);
        }

        delete(a);
    }
    reaperQueue->end();
    reaperQueue->removeAll();

    if(SimTime == Begin){
        createAgents();
        createPreys();
    }

    if(LogInterval && (SimTime % LogInterval) == 0)
        outputToFile();

    if(NPreys == 0 && agList->getCount())
        createPrey(false);

#ifdef DEBUG
    if(NPreys != (int)preyList->getCount())
        FATALERR("(NPreys != preyList->getCount()) (%d != %d)",
                NPreys, preyList->getCount());
#endif
}

void Model::createPrey(bool randAge)
{
    Prey *p;
    p = new Prey();
    p->x = getRandInt(0, (WorldXSize - 1));
    p->y = getRandInt(0, (WorldYSize - 1));
    p->myCell = cellGrid[p->x][p->y];
    p->myCell->preylist->addFirst(p);
    preyList->addFirst(p);
    if(randAge)
        p->age = getRandInt(1, 60);
    else
        p->age = 0;
    NPreys++;
}

void Model::createPreys()
{
    if(Hunt == 0)
        return;
    int nPreys = MaxNPreys / 2;
    for(int i = 0; i < nPreys; i++)
        createPrey(true);
}

double Model::getShareMM()
{
    double r;
    if(nBegsMM == 0)
        r = -1.0;
    else
        r = (double)nSharedMM / (double)nBegsMM;
    return r;
}

double Model::getShareFM()
{
    double r;
    if(nSharedFM == 0)
        r = -1.0;
    else
        r = (double)nSharedFM / (double)nBegsFM;
    return r;
}

double Model::getShareMF()
{
    double r;
    if(nBegsMF == 0)
        r = -1.0;
    else
        r = (double)nSharedMF / (double)nBegsMF;
    return r;
}

double Model::getShareFF()
{
    double r;
    if(nBegsFF == 0)
        r = -1.0;
    else
        r = (double)nSharedFF / (double)nBegsFF;
    return r;
}

void Model::zeroForAll()
{
    nBegsMM = 0;
    nBegsFF = 0;
    nBegsFM = 0;
    nBegsMF = 0;
    nSharedMM = 0;
    nSharedMF = 0;
    nSharedFF = 0;
    nSharedFM = 0;
    NHunts = 0;
    NHunters = 0;
    NFoundPreys = 0;
    NSuccHunts = 0;
    NTConflicts = 0;
    TN1 = 0;
    TN2 = 0;
    TEnd1 = 0;
    TEnd2 = 0;
    TEnd3 = 0;
    TEnd4 = 0;
    TEnd5 = 0;
    TEnd6 = 0;

    if(agList->getCount() == 0 && SimTime > Begin){
        warnMsg("Number of agents = 0.");
        exit(1);
    }

    if(DumpPopNow){
        DumpPopNow = 0;
        fprintf(stderr, _("Dumping agents' memory to \"agRemembrances.csv\" and entire population to \"entirePop.csv\".\n"));
        dumpEntirePop();
        dumpAgentsMem();
#ifndef NOGUI
        observer->saveImg();
#endif
    }
}

void Model::buildObjects()
{
    int i, j, k, n;

    remove("followedAgents");
    Sentinel = (AgNames*)calloc(1, sizeof(AgNames));
    Sentinel->ag = NULL;
    strcpy(Sentinel->name, "Zzzzzzz");	// "Z" is almost in the middle
                                        // between "A" and "z"

    cellList = new List();
    agList = new List();
    femaleList = new List();
    maleList = new List();
    thvList = new List();
    trList = new List();
    patchList = new List();
    preyList = new List();
    reaperQueue = new List();

    if(!Silent)
        fprintf(stderr, _("Creating cells.\n"));
    cellGrid = (Cell***)malloc(WorldXSize * sizeof(Cell**));
    Cell *cell;
    for(i = 0; i < WorldXSize; i++){
        cellGrid[i] = (Cell**)malloc(WorldYSize * sizeof(void*));
        for(j = 0; j < WorldYSize; j++){
            cell = new Cell();
            cell->setX(i, j);
            cellGrid[i][j] = cell;
            cellList->addLast(cell);
        }
    }

    // Build pointers to cells in the perimeter for each xy coordinate.  The goal
    // is to avoid during the simulation billions of calculus of who are the
    // neighbors.
    int d;
    int x, y;
    int *xl = (int*)malloc((MaxDistance + 2) * 8 * sizeof(int));
    int *yl = (int*)malloc((MaxDistance + 2) * 8 * sizeof(int));
    for(x = 0; x < WorldXSize; x++){
        for(y = 0; y < WorldYSize; y++){
            cell = cellGrid[x][y];
            cell->perimeter = (Cell***)malloc((MaxDistance + 1) * sizeof(Cell**));
            cell->periLen = (int*)malloc((MaxDistance + 1) * sizeof(int));
            for(d = 1; d <= MaxDistance; d++){
                n = getPerimeterFor(x, y, d, xl, yl, true);
                cell->periLen[d] = n;
                cell->perimeter[d] = (Cell**)malloc(n * sizeof(void*));
                for(j = 0; j < n; j++)
                    cell->perimeter[d][j] = cellGrid[xl[j]][yl[j]];
            }
        }
    }
    free(xl);
    free(yl);
    int **forbiddenCell;
    int gridx, gridy;
    FILE *f = fopen("gridTemplate.pbm", "r");
    if(f != NULL){

        // The code for reading a pbm file was adapted from swarm/src/space/Discrete2d.m.
        i = fgetc(f);
        if(i != 'P'){
            fclose(f);
            FATALERR(_("gridTemplate.pbm isn't a valid pbm file in ascii format."));
        }
        i = fgetc(f);
        if(i != '1'){
            fclose(f);
            FATALERR(_("gridTemplate.pbm isn't a valid pbm file in ascii format."));
        }

        k = fscanf(f, "%d", &gridx);
        if(k == 1)
            k = fscanf(f, "%d", &gridy);
        else
            FATALERR(_("Error reading gridTemplate.pbm. Could not find the pbm width."));
        if(k != 1)
            FATALERR(_("Error reading gridTemplate.pbm. Could not find the pbm height."));
        if(WorldXSize > gridx || WorldYSize > gridy)
            FATALERR(_("The gridTemplate.pbm dimensions must be either the same or bigger than the world ones."));

        forbiddenCell = (int**)malloc(gridx * sizeof(int*));
        for(x = 0; x < gridx; x++)
            forbiddenCell[x] = (int*)calloc(gridy, sizeof(int));

        int v;
        for(x = 0; x < gridx; x++){
            for (y = 0; y < gridy; y++){
                if((k = fscanf(f, "%d", &v)) != 1)
                    FATALERR(_("Ran out of data reading gridTemplate.pbm."));
                if(v == 1)
                    forbiddenCell[x][y] = 1;
            }
        }

        fclose(f);
        f = fopen("lastGrid.pbm", "w");
        fprintf(f, "P1\n%d %d\n", gridx, gridy);
        for(x = 0; x < gridx; x++){
            for(y = 0; y < gridy; y++){
                fprintf(f, "%d ", forbiddenCell[x][y]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    } else{
        gridx = WorldXSize;
        forbiddenCell = (int**)malloc(WorldXSize * sizeof(int*));
        for(x = 0; x < WorldXSize; x++)
            forbiddenCell[x] = (int*)calloc(WorldYSize, sizeof(int));
    }


    if(!Silent)
        fprintf(stderr, _("Creating trees.\n"));
    unsigned int nPatches = 0;
    int patch_size, newX, newY, theX, theY, r, distance, *xl2, *yl2, trType = 0;
    Tree *tr;
    Patch *pa, *pa2;
    xl2 = (int*)malloc(8 * sizeof(int));
    yl2 = (int*)malloc(8 * sizeof(int));
    while(trType < 3){
        i = 0;
        while(i < TreeN[trType]){
            do{
                newX = getRandInt(0, (WorldXSize-1));
                newY = getRandInt(0, (WorldYSize-1));
            } while(cellGrid[newX][newY]->tree || forbiddenCell[newX][newY]);
            tr = new Tree(); // create the first tree
            i++;
            tr->set(newX, newY, trType, TreeSeasonBegin[trType], TreeSeasonEnd[trType],
                    TreeMaxFruitAge[trType], TreeNFruitsDay[trType], TreeFruitEnergy[trType]);
            trList->addLast(tr);
            pa = new Patch();
            pa->setTreeSpecies(trType);
            patchList->addLast(pa);
            pa->addX(newX, newY);
            cell = cellGrid[newX][newY];
            cell->setTree(tr);
            cell->setPatch(pa);
            if(MinTreePatchSize[trType] == MaxTreePatchSize[trType])
                patch_size = MinTreePatchSize[trType];
            else
                patch_size = getRandInt(MinTreePatchSize[trType], MaxTreePatchSize[trType]);
            j = 1;
            distance = 1;	// create the remaining trees around the first one
            while(j < patch_size && i < TreeN[trType]){
                xl = (int*)malloc(8 * distance * sizeof(int));
                yl = (int*)malloc(8 * distance * sizeof(int));
                n = getPerimeterFor(newX, newY, distance, xl, yl, true);
                r = getRandInt(0, (n - 1));
                k = 0;
                do{
                    if(r >= n)
                        r = 0;
                    theX = xl[r];
                    theY = yl[r];
                    tr = cellGrid[theX][theY]->tree;
                    if(tr == NULL && forbiddenCell[theX][theY] == 0){
                        tr = new Tree();
                        i++;
                        tr->set(theX, theY, trType, TreeSeasonBegin[trType],
                                TreeSeasonEnd[trType], TreeMaxFruitAge[trType],
                                TreeNFruitsDay[trType], TreeFruitEnergy[trType]);
                        trList->addLast(tr);
                        pa->addX(theX, theY);
                        cell = cellGrid[theX][theY];
                        cell->setPatch(pa);
                        cell->setTree(tr);
                    }
                    k++;
                    r++;
                    j++;
                    if(j == patch_size || i == TreeN[trType])
                        k = n;
                } while(k < n);
                        distance++;
                        free(xl);
                        free(yl);
            }
            nPatches++;
        }
        trType++;
    }
    free(xl2);
    free(yl2);

    // Fusion of adjacent patches of the same tree species. Patches of the same
    // tree species that happened to be created next to each other will be
    // considered the same patch.
    if(!Silent){
        fprintf(stderr, _("Merging adjacent patches of trees.\n"));
        fprintf(stderr, "   ");
        fprintf(stderr, _("Initial number of patches: %d\n"), nPatches);
    }

    xl = (int*)malloc(8 * sizeof(int));
    yl = (int*)malloc(8 * sizeof(int));
    for(i = 0; i < WorldXSize; i++)
        for(j = 0; j < WorldYSize; j++){
            cell = cellGrid[i][j];
            pa = cell->getPatch();
            if(pa){
                n = getPerimeterFor(i, j, 1, xl, yl, false);
                for(k = 0; k < n; k++){
                    pa2 = cellGrid[xl[k]][yl[k]]->patch;
                    if(pa2 && pa != pa2 && pa->getTreeSpecies() == pa2->getTreeSpecies()){
                        pa->receivePatch(pa2);
                        patchList->remove(pa2);
                        delete(pa2);
                        nPatches--;
                    }
                }
            }
        }
    free(xl);
    free(yl);

    if(!Silent){
        fprintf(stderr, "   ");
        fprintf(stderr, _("Final number of patches: %d\n"), nPatches);
    }

    patchList->begin();
    i = 0;
    while((pa = (Patch*)patchList->next())){
        pa->optimizeMemory();
    }
    patchList->end();

#ifdef DEBUG
    if((unsigned int)patchList->getCount() != nPatches)
        FATALERR("[patchList getCount] != nPatches (%d, %d)",
                (unsigned int)patchList->getCount(), nPatches);
    patchList->begin();
    n = 0;
    while((pa = (Patch*)patchList->next()))
        n += pa ->getNTrees();
    patchList->end();
    unsigned int nTr = TreeN[0] + TreeN[1] + TreeN[2];
    if(n != (int)nTr || (int)(trList->getCount()) != n)
        FATALERR("(n != nTr || trList getCount] != n) [%d, %d,->%d()",
                n, nTr, trList->getCount());
#endif

    if(!Silent)
        fprintf(stderr, _("Creating terrestrial herbaceous vegetation.\n"));
    THV *thv;
    for(i = 0; i < WorldXSize; i++)
        for(j = 0; j < WorldYSize; j++){
            cell = cellGrid[i][j];
            if(cell->tree == NULL && forbiddenCell[i][j] == 0){
                thv = new THV();
                thv->setX(i, j, MaxPlantEnergy);
                thvList->addLast(thv);
                cell->setTHV(thv);
            }
        }

    for(i = 0; i < gridx; i++)
        free(forbiddenCell[i]);
    free(forbiddenCell);

    if(dumpPatchesToScreen && !GUIMode && WorldXSize < 97 && WorldYSize < 37 &&
            getenv("TERM")){
        printFrame();
        patchList->begin();
        char letter = 'A';
        while((pa = (Patch*)patchList->next()) && letter <= 'z'){
            pa ->printSelfOn(letter);
            letter++;
        }
        patchList->end();
        fprintf(stderr, "\033[%i;1H", (WorldYSize + 3));
        fflush(stdout);
    }

    fflush(stderr);
}

void Model::createAgents()
{
    Tree *tr;
    Agent *ag;
    int i;
    trList->begin();
    for (i = 0; i < nAg; i++){
        tr = (Tree*)trList->next();  // Put agents inside patches
        if(tr == NULL){
            trList->end();
            trList->begin();
            tr = (Tree*)trList->next();
        }
        ag = new Agent(tr->x, tr->y);
        agList->addLast(ag);
        if(ag->sex == 'f')
            femaleList->addLast(ag);
        else
            maleList->addLast(ag);
        ag->learnGeography();
    }
    trList->end();
    agList->begin();
    while((ag = (Agent*)agList->next()))
        ag->makeFirstFriends();
    agList->end();
    calcAverAges();
}

#ifdef DEBUG
void Model::doubleCountAgents()
{
    int na = 0, i, j, a, m, f;
    for(i = 0; i < WorldXSize; i++)
        for(j = 0; j < WorldYSize; j++)
            na += cellGrid[i][j]->getNAgents();
    a = agList->getCount();
    f = femaleList->getCount();
    m = maleList->getCount();
    if((m + f) != a)
        FATALERR("(%i + %i) != %i)", m, f, a);
    if(na != a)
        FATALERR("(%i != %i)", na, a);
    if(a != AgNamesCount)
        FATALERR("(%i != %i)", AgNamesCount, a);
}
#endif

void Model::randomizePeri()
{
    int i, j, k;
    int frac = WorldXSize * WorldYSize / 21;
    for(k = 0; k < frac; k++){
        i = getRandInt(0, (WorldXSize - 1));
        j = getRandInt(0, (WorldYSize - 1));
        cellGrid[i][j]->randomizePerimeter1();
    }
    frac = WorldXSize * WorldYSize / 233;
    for(k = 0; k < frac; k++){
        i = getRandInt(0, (WorldXSize - 1));
        j = getRandInt(0, (WorldYSize - 1));
        cellGrid[i][j]->randomizePerimeter2();
    }
}

int Model::runConflictBetween(Agent* ag1, Agent* ag2)
{
#ifdef DEBUG
    if(ag1->myPatch == NULL)
        FATALERR("IMPOSSIBLE!");
#endif

    NTConflicts++;
    int result = 0;
    double e1 = 0.0, e2 = 0.0;
    Alliance *al1 = ag1->createAllianceAgainst(ag2);

    if(al1){
        TN1 += al1->nallies;
        e1 = al1->energy;
        Alliance *al2 = ag2->createAllianceAgainst(ag1);
        if(al2 == NULL){
            ag2->flyFromX(ag1->x, ag1->y, ag1->myPatch); // Fly without fighting
            result = 1;
            TEnd1++;
        } else{
#ifdef DEBUG
            if(MSTRCMP(ag1->name, al2->ldName) == 0)
                FATALERR("Error 1");
            if(MSTRCMP(ag2->name, al1->ldName) == 0)
                FATALERR("Error 2");
#endif
            e2 = al2->energy;
            TN2 += al2->nallies;
            bool f1 = al1->decideWhetherToFightWith(al2, e2);
            bool f2 = al2->decideWhetherToFightWith(al1, e1);
            if(f1 && f2){			// The two alliances decided to fight:
                double p1 = e1 / (e1 + e2);	// there will be loss of energy!
                double r = getRandDouble(0.0, 1.0);
                al1->dropPatrolAlliesEnergyBy((e2 / (2.0 * e1)));
                al2->dropPatrolAlliesEnergyBy((e1 / (2.0 * e2)));
                if(r <= p1){
                    al2->flyFrom(ag1, ag1->myPatch);
                    result = 1;
                    TEnd2++;
                } else{
                    al1->flyFrom(ag2, ag1->myPatch);
                    result = 2;
                    TEnd3++;
                }
            } else{
                if(f1 && !f2){
                    al2->flyFrom(ag1, ag1->myPatch);
                    result = 1;
                    TEnd4++;
                }
                if(f2 && !f1){
                    al1->flyFrom(ag2, ag1->myPatch);
                    result = 2;
                    TEnd5++;
                }
                if(!f1 && !f2){
                    TEnd6++;
                }
            }
            delete(al2);
        }
        delete(al1);
    }
    return result;
}

void Model::dumpEntirePop()
{
    FILE *L = fopen("entirePop.csv", "w");
    if(L == NULL)
        FATALERR("(L == NULL)");
    fprintf(L, "name sex meatValue askMeatOnly bnvlcTOtherSex bnvlcTSameSex "
            "bnvlcTMother bnvlcTSibling bnvlcTChild maleEnergyImportance "
            "maleAgeImportance gratitude vengefulness sameSexSel energySel childSel "
            "motherSel siblingSel friendSel otherSexSel oestrFemSel migAgeImportance "
            "migFriendImportance energy generosity meatGenerosity pity envy "
            "mutationRate age childhood maxAge mGen fGen bestMaleAge "
            "femalePromiscuity kidValueForMale nLiveChld migPref bravery audacity "
            "loyalty migrating acceptMigInv acceptMoveInvtFromF acceptMoveInvtFromM "
            "huntValue patrolValue gratStrategy vengStrategy vengStr2 begStrategy "
            "huntStrategy huntInterval maxHuntPatrolSize vOfFStranger vOfMStranger "
            "enmityThr lowDeficit hiDeficit tmFct xenophTM xenophTF xenophTFwK "
            "fearOfHPWhenHasKid fearOfHPatches fvalueOfNoSh fvalueOfNoH "
            "fvalueOfNoCT mvalueOfNoSh mvalueOfNoH mvalueOfNoCT adviceValue "
            "valueOfNoSex fvalueOfNotFair mvalueOfNotFair norm metanorm "
            "terriRemStrategy hasShame zeroPostvNo fairness expFFairness expMFairness\n");

    Agent *a;
    agList->begin();
    while((a = (Agent*)agList->next())){
        fprintf(L, "%s ", a->name);
        if(a->sex == 'f')
            fprintf(L, "1 ");
        else
            fprintf(L, "0 ");
        fprintf(L, "%f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f "
                "%f %f %f %f %f %f %f %d %d %d %d %d %d %f %f %d %d %f %f %f %d %d %d "
                "%d %f %f %d %d %d %d %d %d %d %f %f %f %f %f %f %d %d %d %d %d %f %f "
                "%f %f %f %f %f %f %f %f %d %d %d %d %d %f %f %f\n",
                a->meatValue, a->askMeatOnly, a->bnvlcTOtherSex, a->bnvlcTSameSex,
                a->bnvlcTMother, a->bnvlcTSibling, a->bnvlcTChild,
                a->maleEnergyImportance, a->maleAgeImportance, a->gratitude,
                a->vengefulness, a->sameSexSel, a->energySel, a->childSel, a->motherSel,
                a->siblingSel, a->friendSel, a->otherSexSel, a->oestrFemSel,
                a->migAgeImportance, a->migFriendImportance, a->energy, a->generosity,
                a->meatGenerosity, a->pity, a->envy, a->mutationRate, a->age,
                a->childhood, a->maxAge, a->mGen, a->fGen, a->bestMaleAge,
                a->femalePromiscuity, a->kidValueForMale, a->getNChildren(),
                a->getMigPref(), a->bravery, a->audacity, a->loyalty, a->migrating,
                a->acceptMigInv, a->acceptMoveInvtFromF, a->acceptMoveInvtFromM,
                a->huntValue, a->patrolValue, a->gratStrategy, a->vengStrategy,
                a->vengStr2, a->begStrategy, a->huntStrategy, a->huntInterval,
                a->maxHuntPatrolSize, a->vOfFStranger, a->vOfMStranger, a->enmityThr,
                a->lowDeficit, a->hiDeficit, a->tmFct, a->xenophTM, a->xenophTF,
                a->xenophTFwK, a->fearOfHPWhenHasKid, a->fearOfHPatches,
                a->fvalueOfNoSh, a->fvalueOfNoH, a->fvalueOfNoCT, a->mvalueOfNoSh,
                a->mvalueOfNoH, a->mvalueOfNoCT, a->adviceValue, a->valueOfNoSex,
                a->fvalueOfNotFair, a->mvalueOfNotFair, a->norm, a->metanorm,
                a->terriRemStrategy, a->hasShame, a->zeroPostvNo, a->fairness,
                a->expFFairness, a->expMFairness);
    }
    agList->end();
    fclose(L);
    L = NULL;
}

void Model::outputToFile()
{
    if(SimTime < Begin)
        return;

    int nf, nm, na, np;

    nf = femaleList->getCount();
    nm = maleList->getCount();
    na = nf + nm;
    np = preyList->getCount();
    if(nf == 0.0){
        fclose(C);
        fclose(D);
        fprintf(stderr, "\n");
        warnMsg(_("THE END: Year %d, day %d (n_males = %d, "
                    "n_females = %d)\n"), Year, Day, nm, nf);
        exit(1);
    }

    fprintf(C, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
            "%d %d %d %d %d %d %d %d %d\n", SimTime, Year, Day, Hour, na, nf,
            nm, np, NHunts, NHunters, NFoundPreys, NSuccHunts, nBegsMM, nBegsFF,
            nBegsFM, nBegsMF, nSharedMM, nSharedFF, nSharedFM, nSharedMF,
            NTConflicts, TN1, TN2, TEnd1, TEnd2, TEnd3, TEnd4, TEnd5, TEnd6);
}

void Model::dumpAgentsMem()
{
    if(agList->getCount() == 0)
        return;
    FILE *MF = fopen("agRemembrances.csv", "w");
    if(MF == NULL){
        fprintf(stderr, _("Error trying to create the file \"agRemembrances.csv\".\n"));
        return;
    }
    FILE *PF = fopen("knownPatches", "w");
    if(PF == NULL){
        fprintf(stderr, _("Error trying to create the file \"knownPatches\".\n"));
        return;
    }

    fprintf(MF, "# nAg = %d, nFemales = %d, nMales = %d, Time = %d\n",
            agList->getCount(), femaleList->getCount(), maleList->getCount(), SimTime);
    fprintf(MF, "agent myID mySID sex1 x1 y1 n ns ord other otherID otherSID first "
            "nInter sex2 x2 y2 kinship dist recall enmityThr type event time value nSF nSE nSK\n");

    // Build a list of agents IDs
    Agent *ag;
    int nA = agList->getCount();
    Agent **list = (Agent**)calloc(nA + 1, sizeof(void*));
    int i = 0;
    agList->begin();
    while((ag = (Agent*)agList->next())){
        if(ag->age >= ag->childhood){
            list[i] = ag;
            i++;
        }
    }
    agList->end();

    // Build a list of male IDs
    nA = maleList->getCount();
    Agent **mlist = (Agent**)calloc(nA + 1, sizeof(void*));
    i = 0;
    maleList->begin();
    while((ag = (Agent*)maleList->next())){
        if(ag->age >= ag->childhood){
            mlist[i] = ag;
            i++;
        }
#ifdef DEBUG
        if(agList->contains(ag) == false)
            fprintf(stderr, "Model::dumpAgentsMem : missing male\n");
#endif
    }
    maleList->end();

    // Build a list of female IDs
    nA = femaleList->getCount();
    Agent **flist = (Agent**)calloc(nA + 1, sizeof(void*));
    i = 0;
    femaleList->begin();
    while((ag = (Agent*)femaleList->next())){
        if(ag->age >= ag->childhood){
            flist[i] = ag;
            i++;
        }
#ifdef DEBUG
        if(agList->contains(ag) == false)
            fprintf(stderr, "Model::dumpAgentsMem : missing female\n");
#endif
    }
    femaleList->end();


    fprintf(stderr, _("\nDumping the memory of agents into \"agRemembrances.csv\".\n"));
    agList->begin();
    i = 0;
    while((ag = (Agent*)agList->next())){
        if(ag->sex == 'f')
            ag->dumpMemTo(MF, PF, list, flist);
        else
            ag->dumpMemTo(MF, PF, list, mlist);
        i++;
        if((i % 100) == 0){
            fprintf(stderr, "%5d ", i);
            fflush(stderr);
            if((i % 1000) == 0)
                fprintf(stderr, "\n");
        }
    }
    if(i > 100)
        fprintf(stderr, "\n");
    agList->end();
    fclose(MF);
    fclose(PF);
    free(list);
    free(mlist);
    free(flist);
}

