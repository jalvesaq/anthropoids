/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/

#ifndef ALLIANCE_H
#define ALLIANCE_H

#include "list.h"

class Agent;
class Patch;

// alliances of agents that go into conflict

class Alliance
{
    private:
        List* getAlliesList();
        char* getLeaderName();
        double getTotalEnergy();

    public:
        Agent *leader;
        char ldName[8];
        List *allies;
        int nallies;
        double energy;

        Alliance();
        ~Alliance();
        void setLeader(Agent *ld, char *nm, double e);

        bool decideWhetherToFightWith(Alliance *oAl, double e2);

        void addAlly(Agent *ag, double e);
        void removeAlly(Agent *ag, double e);
        void flyFrom(Agent *ag, Patch *p);

        int getNAllies();
        void dropPatrolAlliesEnergyBy(double e);
};

#endif
