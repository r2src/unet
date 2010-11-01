/*
agent.h: interface of the Agent class
*/

#ifndef AGENT_H
#define AGENT_H

#include "main.h"

class Agent
{
public:
  
  // default constructor for agents, assigns an id and random attribute value
  Agent();
  
  // the following are trivial, so implemented here:
  int getid() { return id; }
  int getnr() { return links.size(); }
  list<Link*>& getLinks() { return links; } // returns (a reference to) the link list
  double getattr() { return attribute; }
  void setattr(double value) { attribute = value; }
  
  Agent* getRandom();
  // returns a random agent from the link list

  list<Agent*> getAgents();
  // returns a list of linked agents
  
  void compare(Agent* peer, double assimilation_treshold, double assimilation_step);
  // adjusts attribute based on the outcome of a social comparison
  
  friend class Link;
  // the Link class manages the (private) list of links

private:
  static int id_counter;
  int id;
  double attribute;
  list<Link*> links; // list of pointers to Link objects
};

#endif
// AGENT_H
