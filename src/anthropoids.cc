/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#include "config.h"

#include <sys/stat.h>
#include <gsl/gsl_rng.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "Model.h"
#ifndef NOGUI
#include "Observer.h"
#endif

#include "abmtools.h"

#ifndef NOGUI
#include "control.h"
Observer *theObserver;
Glib::RefPtr<Gtk::Application> m;
#endif

Model *theModel;


extern gsl_rng * rng;
extern double MaxRN;
unsigned long int RandomSeed;
bool GUIMode = false;

#ifndef MINGW32
void handle_signal(int sig){
    if(sig == SIGUSR1){
        fprintf(stderr, "\n");
        fprintf(stderr, _("Received signal USR1.\n"));
        theModel->setStatus(0);
    } else{
        if(sig == SIGUSR2){
            fprintf(stderr, "\n");
            fprintf(stderr, _("Signal USR2 received!\n"));
            theModel->setStatus(3);
        } else{
            fprintf(stderr, "\n");
            fprintf(stderr, _("I don't know what to do with this signal (%d)!\n"), sig);
        }
    }
}
#endif

void printHelp()
{
    fprintf(stderr, "\n\n");
    fprintf(stderr, _("Valid options are:"));
    fprintf(stderr, "\n\n  ");
    fprintf(stderr, _("-b          : Run in batch mode."));
    fprintf(stderr, "\n  ");
    fprintf(stderr, _("-d          : Save results in $HOME/anthropoids instead of using currenty directory."));
    fprintf(stderr, "\n  ");
    fprintf(stderr, _("-s          : Select random number seed from current time."));
    fprintf(stderr, "\n  ");
    fprintf(stderr, _("-S <number> : Specify a seed for the random numbers generator."));
    fprintf(stderr, "\n  ");
    fprintf(stderr, _("-h, --help  : Gives this help list and exits."));
    fprintf(stderr, "\n\n");
    exit(0);
}

void initialize(int argc, char **argv)
{
    printf("anthropoids %s\n", VERSION);
    setlocale (LC_ALL, "");
    setlocale (LC_NUMERIC, "C");
    int l = strlen(PACKAGE_DATA_DIR) + strlen("/locale") + 1;
    char *localePath = (char*)malloc(l * sizeof(char));
#ifndef MINGW32
    sprintf(localePath, "%s/locale", PACKAGE_DATA_DIR);
#else
    sprintf(localePath, "locale");
#endif
    bindtextdomain ("anthropoids", localePath);
    textdomain ("anthropoids");
    free(localePath);

    GUIMode = true;
    RandomSeed = 1;
    int i = 1;
    bool homeDir = false;
    while(i < argc){
        if(strcmp(argv[i], "-d") == 0)
            homeDir = true;
        else
            if(strcmp(argv[i], "-b") == 0)
                GUIMode = false;
            else
                if(strcmp(argv[i], "-s") == 0)
                    RandomSeed = time(NULL);
                else
                    if(strcmp(argv[i], "-S") == 0){
                        if(argc > (i + 1)){
                            i++;
                            RandomSeed = strtoul(argv[i], NULL, 0);
                        } else {
                            fprintf(stderr, _("Incorrect use of option \"-S\"."));
                            printHelp();
                        }
                    } else
                        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
                            printHelp();
                        else {
                            fprintf(stderr, _("Unknown option \"%s\"."), argv[i]);
                            printHelp();
                        }
        i++;
    }

#ifdef NOGUI
    GUIMode = false;
#else
    if(GUIMode)
        m = Gtk::Application::create(argc, argv);
#endif

    // The default gsl_rng algorithm is the Mersenne twister (MT19937)
    rng = gsl_rng_alloc (gsl_rng_default);
    gsl_rng_set(rng, RandomSeed);
    MaxRN = (double)(gsl_rng_max(rng));
    if(gsl_rng_min(rng) != 0)
        FATALERR("gsl_rng_min(rng) != 0");

    if(homeDir){ // Save results in $HOME/anthropoids
        char buf[512];
        char *homedir = getenv("HOME");
        if(homedir == NULL)
            FATALERR("HOME environment variable was not found");
        sprintf(buf, "%s/anthropoids", homedir);
        int err = chdir(buf);
        if(err == -1){
#ifndef MINGW32
            err = mkdir(buf, 0777);
#else
            err = mkdir(buf);
#endif
            if(err == -1)
                FATALERR("Could not create directory for virtual anthropoids results!");
            err = chdir(buf);
            if(err == -1)
                FATALERR("Could not enter virtual anthropoids results directory!");
        }
    }

#ifndef MINGW32
    if(!Silent && !GUIMode){
        signal(SIGUSR1, handle_signal);
        signal(SIGUSR2, handle_signal);
        fprintf(stderr, _("The best way of stopping the program at an arbitrary "
                    "time is with the command:"));
        fprintf(stderr, "\n   killall -s USR1 anthropoids\n");
    }
#endif
}

void finish()
{
    gsl_rng_free(rng);
#ifndef NOGUI
    if(GUIMode){
        delete theObserver;
    }
    else
#endif
        delete(theModel);
}


int main (int argc, char **argv)
{
    initialize(argc, argv);

    if(GUIMode){
#ifndef NOGUI
        theObserver = new Observer(m);
        theObserver->go();
#endif
    } else{
        theModel = new Model();
        theModel->go();
    }

    finish();
    return 0;
}
