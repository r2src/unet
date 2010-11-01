/*                                                                                       30%
link.h: interface of the Link class
*/

#ifndef LINK_H
#define LINK_H

#include "main.h"

class Link
{
public:
  
  // constructor; assigns random weight:
  Link(Agent* agent1, Agent* agent2);
  
  // warning: no default constructor! (to prevent uninitialized links)

  // the following are trivial, so implemented here:
  Agent* getSource() const { return source; }
  Agent* getTarget() const { return target; }
  double getWeight() const { return weight; }
  void setWeight(double value) { weight = value; }
  
  Agent* getOther(Agent* agent);
  // returns the 'other' agent
  
  // the following are less trivial:)
  Link(const Link &linkObject); // copy constructor
  Link& operator=(const Link &linkObject); // overloaded assignment operator
  ~Link(); // destructor
  
private:
  double weight;
  Agent* source;
  Agent* target;
};

#endif
// LINK_H
