/*
main.h: contains all forward declarations and necessary includes
*/

#ifndef MAIN_H
#define MAIN_H

#define DEBUG false

#include <cstdlib>
#include <iostream>
#include <string>   
#include <cmath>
#include <list>
#include <ctime>
#include <unistd.h>
#include <gsl/gsl_rng.h>

using namespace std;

extern gsl_rng *r; // global random number generator:

class Agent; // a thinking human agent
class Link; // a link between two agents

bool validLink(Agent* agent1, Agent* agent2, list<Link> &relation);
// returns false if the Agents are the same or already linked

void removeLink(Agent* agent1, Agent* agent2, list<Link> &relation);
// searches for and removes the link between two agents

void printXML(int nr_of_agents, list<Link> &relation);
// prints XML output

void printDOT(list<Link> &relation);
// prints GraphViz DOT output

double connectivity();
// TODO: returns some statistic indicating the fat-tailness of the connectivity distribution

double assortativity(list<Link> &relation);
// returns the assortativity coefficient of the network

double clustering(int nr_of_agents, Agent population[], list<Link> &relation);
// returns the global clustering coefficient

bool exists(Agent* target, list<Agent*> &agents);
// returns true if the target is in the list of agents

double avgpath(int nr_of_agents, Agent population[]);
// returns the average path length

#endif
// MAIN_H
