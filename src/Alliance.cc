/* This file is part of "anthropoids"
 **
 ** It is distributed under the GNU General Public License.
 ** See the file COPYING for details.
 **
 ** (C) 2008 Jakson Aquino: jalvesaq@gmail.com
 **
 ***************************************************************/


#include "Model.h"



Alliance::Alliance()
{
    allies = new List();
}

void Alliance::setLeader(Agent *ld, char* nm, double e)
{
    leader = ld;
    leader->amILeader = true;
    NAMECOPY(ldName, nm);
    energy = e;
    nallies = 1;
    allies->addLast(ld);
}

void Alliance::addAlly(Agent *ag, double e)
{
    energy += e;
    nallies++;
    allies->addLast(ag);
}

void Alliance::removeAlly(Agent *ag, double e)
{
#ifdef DEBUG
    if(ag->myAlliance != this)
        FATALERR("[ag getAlliance] != this (%p %p)", ag, this);
    if(nallies == 1)
        FATALERR("error");
    if(allies->remove(ag) == false)
        FATALERR("remove ag == NULL");
#else
    allies->remove(ag);
#endif
    nallies--;
    energy -= e;
    ag->myAlliance = NULL;
}

bool Alliance::decideWhetherToFightWith(Alliance *oAl, double e2)
{
    Agent *a;
    double fight = 0.0;
    double p = e2 / (energy + e2); // probability of being defeated
    int n = 0;
    Agent **aa = (Agent**)malloc(nallies * sizeof(Agent*));
    allies->begin();
    while((a = (Agent*)allies->next())){
        aa[n] = a;
        n++;
        a->rememberPatrolRivals(oAl->allies);
        if(a->audacity > p)
            fight += 1.0;
        if(a != leader)
            leader->askAboutPatches(a); // The leader asks for everyone
    }
    allies->end();

    for(int i = 0; i < n; i++)
        if(aa[i]->terriRemStrategy == 0) // Remember allies even with there is no fight
            aa[i]->rememberPatrolAllies(allies);

    allies->begin();
    while((a = (Agent*)allies->next()))
        if(a != leader)
            a->askAboutPatches(leader); // Everyone asks for the leader
    allies->end();
    if((fight / (double)nallies) > 0.5){
        for(int i = 0; i < n; i++)
            if(aa[i]->terriRemStrategy == 1) // Only remember allies if there was fight
                aa[i]->rememberPatrolAllies(allies);
        free(aa);
        return true;
    } else{
        free(aa);
        return false;
    }
}

void Alliance::flyFrom(Agent *ag, Patch *p)
{
    Agent *a;
    allies->begin();
    while((a = (Agent*)allies->next())){
#ifdef DEBUG
        if(a->myAlliance != this)
            FATALERR("a getAlliance != this (%p %p)", a, this);
#endif
        a->flyFromX(ag->x, ag->y, p);
    }
    allies->end();
}

Alliance::~Alliance()
{
    Agent *a;
    Agent *l = leader;

    double n2 = l->nonCooperators->getCount();
    if(n2 > 0.0){
        List *nonPunishers  = new List();
        List *punishers = new List();

        allies->begin();
        while((a = (Agent*)allies->next())){
            if(a->norm)
                punishers->addFirst(a);
            else
                nonPunishers->addFirst(a);
        }
        allies->end();

        // FIXME: Is the cost of punishment too high? According to strong
        // reciprocity theory the invention of weapons decreased the cost of
        // punishment of non-cooperators.
        double n1 = punishers->getCount();
        if(n1 > 0.0){
            double c1 = n2 / (2.0 * n1);
            double c2 = n1 / (2.0 * n2);

            Agent *ag;
            punishers->begin();
            while((a = (Agent*)punishers->next())){
                a->energy -= c1;
                a->eFromConfl -= c1;
                l->nonCooperators->begin();
                while((ag = (Agent*)l->nonCooperators->next())){
                    a->storeInMemory(((-1) * c1), ag->name, 'r', 17);
                    if(ag->hasShame)
                        ag->storeInMemory(((-1) * c1), a->name, 'g', 18);
                    else
                        ag->storeInMemory(((-1) * c2), a->name, 'r', 18);
                }
                l->nonCooperators->end();
            }
            punishers->end();

            l->nonCooperators->begin();
            while((a = (Agent*)l->nonCooperators->next())){
                a->energy -= c2;
                a->eFromConfl -= c2;
            }
            l->nonCooperators->end();


            n2 = nonPunishers->getCount();
            if(Metanorm && n2 > 0){

                List *metaPunishers = new List();
                punishers->begin();
                while((a = (Agent*)punishers->next())){
                    if(a->metanorm)
                        metaPunishers->addFirst(a);
                }
                punishers->end();

                n1 = metaPunishers->getCount();
                if(n1 > 0.0){
                    c1 = n2 / (2.0 * n1);
                    c2 = n1 / (2.0 * n2);
                    metaPunishers->begin();
                    while((a = (Agent*)metaPunishers->next())){
                        nonPunishers->begin();
                        while((ag = (Agent*)nonPunishers->next())){
                            a->storeInMemory(((-1) * c1), ag->name, 'r', 17);
                            if(ag->hasShame)
                                ag->storeInMemory(((-1) * c1), a->name, 'g', 18);
                            else
                                ag->storeInMemory(((-1) * c2), a->name, 'r', 18);
                        }
                        nonPunishers->end();
                        a->energy -= c1;
                        a->eFromConfl -= c1;
                    }
                    metaPunishers->end();
                    nonPunishers->begin();
                    while((a = (Agent*)nonPunishers->next())){
                        a->energy -= c2;
                        a->eFromConfl -= c2;
                    }
                    nonPunishers->end();
                }
                metaPunishers->removeAll();
                delete(metaPunishers);
            }
        }
        punishers->removeAll();
        delete(punishers);
        nonPunishers->removeAll();
        delete(nonPunishers);
        l->nonCooperators->removeAll();
    }

    allies->begin();
    while((a = (Agent*)allies->next())){
#ifdef DEBUG
        if(a->myAlliance != this)
            FATALERR("a getAlliance != this (%p %p)", a, this);
#endif
        a->myAlliance = NULL;
    }
    allies->end();

    allies->removeAll();
    leader->amILeader = false;
    delete allies;
}

char* Alliance::getLeaderName()
{
    return ldName;
}

List* Alliance::getAlliesList(){
    return allies;
}

void Alliance::dropPatrolAlliesEnergyBy(double e)
{
    allies->begin();
    Agent *ag;
    while((ag = (Agent*)allies->next())){
        ag->energy -= e;
    }
    allies->end();
}

double Alliance::getTotalEnergy()
{
    return energy;
}

int Alliance::getNAllies()
{
    return nallies;
}



