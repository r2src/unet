/*                                                                                       30%
agent.cpp: the implementation of the Agent class
*/

#include "main.h"
#include "agent.h"
#include "link.h"

Agent::Agent()
// default constructor for agents, assigns an incremental id and random attribute
{
  id = id_counter;
  id_counter += 1;
  attribute = gsl_rng_uniform(r);
}



Agent* Agent::getRandom()
// returns a pointer to a random friend
{
  // return this object if it has no friends...
  if (links.empty()) { return this; }
  
  // generate random link number
  int random_link = gsl_rng_uniform_int(r,links.size());

  // advance iterator to it
  list<Link*>::iterator it = links.begin();
  advance(it,random_link); // for (int i=0; i<random_link; i++) { it++; }
  
  // return the 'other' agent:
  return (*it)->getOther(this);  
}



list<Agent*> Agent::getAgents()
// returns a list of linked agents
{
  list<Link*>::iterator it;
  list<Agent*> agents;
  
  for (it=links.begin(); it!=links.end(); it++)
  {
    agents.push_back((*it)->getOther(this));
  }
  
  return agents;
}



void Agent::compare(Agent* peer, double assimilation_treshold, double assimilation_step)
// a social comparison resulting in contrast or assimilation
{
  double difference = attribute - peer->getattr();
    
  if (DEBUG) cerr << "Agent " << id << " (" << attribute << ") compares itself to agent " <<
  peer->getid() << " (" << peer->getattr() << "): " << "difference = " << abs(difference) << endl;

  if (abs(difference) > assimilation_treshold) // assimilate!
  {
    if (DEBUG) cerr << "Assimilation treshold (" << assimilation_treshold << ") exceeded, assimilating...\n";
    if (difference < 0) // peer is better
    {
      if (DEBUG) cerr << "Agent " << id << "'s attribute is lower, so increasing it with " << assimilation_step << ": ";
      attribute = attribute + assimilation_step;
      if (attribute > 1) attribute = 1;
    }
    else if (difference > 0) // agent is better
    {
      if (DEBUG) cerr << "Agent " << id << "'s attribute is higher, so decreasing it with " << assimilation_step << ": ";
      attribute = attribute - assimilation_step;
      if (attribute < 0) attribute = 0;
    }
    else
    {
      // honestly, this cannot happen
      cerr << "error: Impossible condition in Agent::compare: difference=" << difference
           << ", attr1=" << attribute << ", attr2=" << peer->getattr() << endl;
    }
  }  
  else if (abs(difference) < assimilation_treshold) // contrast!
  {
    if (DEBUG) cerr << "Assimilation treshold (" << assimilation_treshold << ") not exceeded, contrasting...\n";
    if (difference < 0) // peer is better
    {
      if (DEBUG) cerr << "Agent " << id << "'s attribute is lower, so decreasing it with " << assimilation_step << ": ";
      attribute = attribute - assimilation_step;
      if (attribute < 0) attribute = 0;
    }
    else if (difference > 0) // agent is better
    {
      if (DEBUG) cerr << "Agent " << id << "'s attribute is higher, so increasing it with " << assimilation_step << ": ";
      attribute = attribute + assimilation_step;
      if (attribute > 1) attribute = 1;
    }
    else
    {
      // this can happen; and is problematic, but let's ignore that for now...
    }
  }
  else
  {
    // well, this happens if both attributes are either 1.00 or 0.00
    if (DEBUG) cerr << "Both attributes are equal, so nothing happens\n";
  }

  if (DEBUG) cerr << "attribute is now " << attribute << endl;
  
}    

