/*
link.cpp: the implementation file of the Link class
*/

#include "main.h"
#include "agent.h"
#include "link.h"

using namespace std;

Link::Link(Agent* agent1, Agent* agent2)
// constructor for links; updates the link lists of both linked agents
{
  // assign a random weight:
  weight = gsl_rng_uniform(r);

  // set source and target agents
  if (agent1->getid() < agent2->getid())
  { // agent1 has lowest id
    source = agent1;
    target = agent2;
  }
  else
  { // agent2 has lowest id
    source = agent2;
    target = agent1;
  }
  
  // update the link list of both agents to include this new link object
  source->links.push_back(this);
  target->links.push_back(this);
}



Agent* Link::getOther(Agent* agent)
// returns the 'other' agent
{
  if (target == agent) { return source; }
  else if (source == agent) { return target; }
  else
  {
    cerr << "fatal error: Agent " << agent->getid() << "'s link list is corrupted\n";
    exit(1);
  }
}



Link::Link(const Link &linkObject)
// copy constructor; invoked when the construtor returns a link object by value
: weight(linkObject.getWeight()),
  source(linkObject.getSource()),
  target(linkObject.getTarget())
{
  // add this new link object to both agents' link lists
  source->links.push_back(this);
  target->links.push_back(this);
}



Link& Link::operator= (const Link &linkObject)
// overloaded assignment operator; appears not to be used, yet...
{
  // stop processing if both objects are actually the same object
  if (this == &linkObject) return *this;
  
  // initialize link variables
  weight = linkObject.getWeight();
  source = linkObject.getSource();
  target = linkObject.getTarget();
  
  // add this new link object to both agents' link lists
  source->links.push_back(this);
  target->links.push_back(this);
  
  // return a reference to this new object like the textbook told me :-)
  return *this;
}



Link::~Link ()
// destructor; invoked when a link object is deleted
{
  // remove the pointers to this object from the agent's link list
  source->links.remove(this);
  target->links.remove(this);
}
