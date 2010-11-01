/*                                                                                       33%

UNET -- A simulation of social psychology and social network structure

by J.J.Vens, 2010  

*/

#include "main.h"
#include "agent.h"
#include "link.h"

#define MAXIMUM_ITERATIONS 100
#define MINIMUM_ITERATIONS 10
#define ITERATION_TRESHOLD 0.001

// global stuff
gsl_rng *r;
int Agent::id_counter = 0;

int main(int argc, char *argv[])
{
  // "global" array of agents (size will be determined by command-line option)
  Agent* population;
  
  // "global" list of links
  list<Link> relation;

  // variables to hold simulation parameters:
  bool verbose = false;
  int population_size;
  int nr_of_links;
  int max_links;
  double assimilation_treshold;
  double assimilation_step;
  double link_treshold;
  
  // filter out the -v option if it's there:
  for (int i=1; i<argc; i++)
  { 
    if (verbose) argv[i-1] = argv[i];
    string option = argv[i];
    if (option == "-v") verbose = true;
  } 
  if (verbose) argc--;

  // show usage message if not correct nr. of arguments
  if (argc != 6)
  {
    cerr << "\nUsage: " << argv[0] << " [-v] pop_size nr_links ass_tres ass_step lnk_tres\n\n"
         << "      -v = Verbose; prints progress messages to STDERR\n"
         << "pop_size = Population size\n"
         << "nr_links = Nr. of links (in the initial network)\n"
         << "ass_tres = Assimilation treshold: The minimum difference between two agents\n"
         << "           that leads to assimilation (between 0 and 1)\n"
         << "ass_step = Assimilation step: The steps with which the agent attributes change\n"
         << "           on each interaction (between 0 and 1)\n"
         << "lnk_tres = Link treshold: The maximum difference between agents' attributes\n"
         << "           (between 0 and 1)\n\n";
                     
    exit(1);
  }
  
  if (verbose) cerr << "Parsing command line arguments...\n";

  population_size = atoi(argv[1]);
  nr_of_links = atoi(argv[2]);
  max_links = (population_size * (population_size-1)) / 2;
  assimilation_treshold = atof(argv[3]);
  assimilation_step = atof(argv[4]);
  link_treshold = atof(argv[5]);
  
  if (verbose) cerr << "Population size: " << population_size << endl
                    << "Nr. of links: " << nr_of_links << endl
                    << "Assimilation treshold: " << assimilation_treshold << endl
                    << "Assimilation step: " << assimilation_step << endl
                    << "Link treshold: " << link_treshold << endl;
   
  // check if the number of links doesn't exceed n(n-1)/2
  if ( nr_of_links > max_links )
  {
    cerr << "fatal error: Nr. of links (" << nr_of_links
         << ") exceeds maximum number of links (" << max_links << ")\n";
    exit(1);
  }

  if (verbose) cerr << "Initializing random number generator...\n";
  
  // initialize the random number generator:
  time_t seed = time(NULL); // the number of seconds since 1970 is the seed
  if (verbose) cerr << "Using seed: " << seed << endl;
  r = gsl_rng_alloc(gsl_rng_mt19937); // aka the 'Mersenne Twister'
  gsl_rng_set(r, seed);
  
  if (verbose) cerr << "Allocating memory for agents...\n";
  
  // reserve heap memory for the agent objects
  population = new Agent[population_size];
  
  if (verbose) cerr << "Creating random social network according to Erdős-Rényi (1959) model";
  if (verbose) cerr << "\n(by picking " << nr_of_links << " random links from n(n-1)/2 = " << max_links << " possible links)";
  if (verbose) cerr << "...\n";
  
  Agent* random_agent1;
  Agent* random_agent2;

  // create some random links between agents:
  for (int i=0; i<nr_of_links; i++)
  {
    do
    { // assign both pointers the address of a random agent:
      random_agent1 = &population[ gsl_rng_uniform_int(r,population_size) ];
      random_agent2 = &population[ gsl_rng_uniform_int(r,population_size) ];
      if (DEBUG) cerr << "Linking agent " << random_agent1->getid() << " to agent " << random_agent2->getid() << "...\n";
    }
    // retry if both agents are the same or if they are already linked:
    while ( !validLink(random_agent1, random_agent2, relation) );
    
    // add the link object to the linklist:
    relation.push_back(Link(random_agent1,random_agent2));
    // NOTE: in the above, the call to the Link constructor returns a value of the class type.
    // When this happens, the copy constructor is invoked, which updates both agents' link
    // lists so that they point to the address of the new (copied) link object.
    // It would be easier if the constructor simply returned a reference...
  }
  
  if (verbose) cerr << "Successfully created random network!\n";
  if (verbose) cerr << "Proceeding with updating the network by social psychological processes...\n";
    
  // now let the fun begin!
  cerr << "#Iteration Removed Links         Density    Cluster.   Assort.\n";

  int i = 0;
  int removed = 0;
  do
  { 
    // print the stats:
    fprintf (stderr, "%-11d", i++);
    fprintf (stderr, "%-22d", removed);
    fprintf (stderr, "%-11.2f", relation.size() / static_cast<double>(max_links));
    fprintf (stderr, "%-11.2f", clustering(population_size, population, relation));
    fprintf (stderr, "%-11.2f", assortativity(relation));
    // fprintf (stderr, "%-11.2f", avgpath(population_size, population));
    cerr << endl;
    
    removed = 0;
    
    // loop over all agents:
    for (int j=0; j<population_size; j++)
    { 
      // loop over all its peers
      list<Agent*> peers = population[j].getAgents();
      list<Agent*>::iterator it;
      for (it=peers.begin(); it!=peers.end(); it++)
      {
        Agent* agent = &population[j];
        Agent* peer = *it;
        
        // the agent makes a social comparison with this peer and adjusts its attribute:
        agent->compare(peer, assimilation_treshold, assimilation_step);
        
        // calculate the attribute difference after this adjustment...
        double difference = abs(agent->getattr() - peer->getattr());
      
        // ... and replace it with a random link if it exceeds the link treshold
        if (difference > link_treshold)
        {
          if (DEBUG) cerr << "Link treshold (" << link_treshold << ") exceeded. Removing the link between agents " << agent->getid() << " and " << peer->getid() << "...\n";
          removeLink(agent, peer, relation);
          removed++;

          // REMOVE THE FOLLOWING TO PREVENT AUTOMATIC NEW LINKS

          do
          { // assign both pointers the address of a random agent:
            random_agent1 = &population[ gsl_rng_uniform_int(r,population_size) ];
            random_agent2 = &population[ gsl_rng_uniform_int(r,population_size) ];
          }
          // retry if both agents are the same or if they are already linked:
          while ( !validLink(random_agent1, random_agent2, relation) );

          // add the link object to the linklist:
          relation.push_back(Link(random_agent1,random_agent2));

        }
        else
        {
          if (DEBUG) cerr << "Link treshold (" << link_treshold << ") not exceeded. Keeping the link between agents " << agent->getid() << " and " << peer->getid() << endl;
        }
      }
    }
    
  } while ( i <= MAXIMUM_ITERATIONS );
  
  if (verbose) cerr << "Sending results to STDOUT...\n";
/*
  // print model parameters and final network statistics
  cout << "pop_size nr_links ass_tres ass_step lnk_tres itrtions rel_size density  clustrng assrtvty avgpath\n";
  printf ("%-9d", population_size);
  printf ("%-9d", nr_of_links);
  printf ("%-9.2f", assimilation_treshold);
  printf ("%-9.2f", assimilation_step);
  printf ("%-9.2f", link_treshold);
  printf ("%-9d",i);
  printf ("%-9d",relation.size());
  printf ("%-9.2f",relation.size() / static_cast<double>(max_links));
  printf ("%-9.2f",clustering(population_size, population, relation));
  printf ("%-9.2f",assortativity(relation));
  printf ("%-9.2f", avgpath(population_size, population));

  cout << endl;
*/
  exit(0);
}



/*****************************************************************************/



bool validLink(Agent* agent1, Agent* agent2, list<Link> &relation)
// returns false if the Agents are the same or already linked
// (does not presuppose that the lowest agent is always source)
{
  // return false if they are the same agent:
  if (agent1->getid() == agent2->getid())
  {
    if (DEBUG) cerr << "Cannot link an agent to itself, retrying...\n";
    return false;
  }

  // return false if the link already exists:
  Agent* source;
  Agent* target;
  for (list<Link>::iterator it = relation.begin(); it!=relation.end(); it++)
  {
    source = it->getSource();
    target = it->getTarget();
    if ( (source == agent1 && target == agent2)
      || (source == agent2 && target == agent1) )
    {
      if (DEBUG) cerr << "That link already exists, retrying...\n";
      return false;
    }
  }
  // else, return true:
  return true;
}



/*****************************************************************************/



void removeLink(Agent* agent1, Agent* agent2, list<Link> &relation)
// searches for and removes a link between two agents
{ 
  bool errorflag = 1;
  Agent* source;
  Agent* target;
  
  // loop over all links to find it:
  for (list<Link>::iterator it = relation.begin(); it!=relation.end(); it++)
  {
    source = it->getSource();
    target = it->getTarget();
    if ( (source == agent1 && target == agent2)
      || (source == agent2 && target == agent1) )
    {
      if (!errorflag) cerr << "error: There are duplicate links between agents " << source->getid() << " and " << target->getid() << endl;
      it = relation.erase(it);
      // NOTE: the destructor of the Link class takes care of updating the agents' link lists
      errorflag = 0;
      it--; // ugly hack to be able to continue looping
    }
  }
  if (errorflag) cerr << "error: The link between agents " << source->getid() << " and " << target->getid() << " could not be found\n";
}



/*****************************************************************************/



void printXML(int nr_of_agents, list<Link> &relation)
// prints output in GraphML format
{
  // header:
  cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  cout << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" ";
  cout << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
  cout << "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns ";
  cout << "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";
  cout << "  <graph id=\"G\" edgedefault=\"undirected\">\n";

  // list of nodes:
  for (int i=0; i<nr_of_agents; i++)
  {
    cout << "    <node id=\"" << i << "\"/>\n";
  }
  
  int i=0;
  // list of edges:
  for (list<Link>::iterator it=relation.begin(); it!=relation.end(); it++)
  {
    cout << "    <edge id=\"e" << i << "\" source=\""
         << (it->getSource())->getid() << "\" target=\""
         << (it->getTarget())->getid() << "\"/>\n";
    i++;
  }

  // footer:
  cout << "  </graph>\n</graphml>\n";
}



/*****************************************************************************/



void printDOT(list<Link> &relation)
// prints output in GraphViz DOT format
{
  cout << "graph {" << endl;
  for (list<Link>::iterator it=relation.begin(); it!=relation.end(); it++)
  {
    cout << (it->getSource())->getid() << " -- "
         << (it->getTarget())->getid() << endl;
  }
  cout << "}\n";
}



/*****************************************************************************/



double assortativity(list<Link> &relation)
// returns the assortativity coefficient of the network
{

  double n = static_cast<double>(relation.size());
  double x = 0;
  double y = 0;
  double sum_x = 0;
  double sum_y = 0;
  double sum_x_y = 0;
  double sum_x_2 = 0;
  double sum_y_2 = 0;
  double result = 0;
  
  // calculate the needed values:
  for (list<Link>::iterator it=relation.begin(); it!=relation.end(); it++)
  {
    x = static_cast<double>((it->getSource())->getnr());
    y = static_cast<double>((it->getTarget())->getnr());
    sum_x += x;
    sum_y += y;
    sum_x_y += x * y;
    sum_x_2 += pow(x,2);
    sum_y_2 += pow(y,2);
  }
  
  // If the n * sum of squares equals the square of the sum,
  // a division by zero will happen when calculating the correlation
  if ( n*sum_x_2 == pow(sum_x,2) || n*sum_y_2 == pow(sum_y,2) )
  {
    if (DEBUG) cerr << "warning: No variance in degree of links\n";
    // in actual simulations, this _very_ unlikely to happen
    return 0;
  }
  else
  {
    // calculate Pearson r correlation between x and y:
    
    result = (n * sum_x_y - sum_x * sum_y) /               // divided by
  /*--------------------------------------------------------------------*/
    sqrt( (n * sum_x_2 - pow(sum_x,2)) * (n * sum_y_2 - pow(sum_y,2)) );

    return result;
  }
}

double clustering(int nr_of_agents, Agent population[], list<Link> &relation)
// returns the global clustering coefficient
// TODO: simplify by using Agent::getAgents instead of Agent::getLinks
{
  Agent* agent;
  Agent* other;

  list<Link*> agent_links;
  list<Link*> peer_links;
  list<Link*>::iterator linkit;

  list<Agent*> personal_network;
  list<Agent*>::iterator agentit_outer;
  list<Agent*>::iterator agentit_inner;

  double nr_possible_links;
  double nr_actual_links;
  double sum = 0;
  
  // for all agents
  for (int i=0; i<nr_of_agents; i++)
  { 
    agent = &population[i];
    agent_links = agent->getLinks(); // TODO: use getAgents() instead
    
    // determine its peers...
    for (linkit=agent_links.begin(); linkit!=agent_links.end(); linkit++)
    {
      // ... by storing their pointer in the personal_network list
      personal_network.push_back((*linkit)->getOther(agent));
      
    }
  
    // for all agents in the personal network
    for (agentit_outer=personal_network.begin(); agentit_outer!=personal_network.end(); agentit_outer++)
    {
      peer_links = (*agentit_outer)->getLinks(); // TODO: use getAgents() instead
      
      // loop over all their links and determine the other agent
      for (linkit=peer_links.begin(); linkit !=peer_links.end(); linkit++)
      {
        other = (*linkit)->getOther(*agentit_outer);
        
        // loop over all agents... again
        for (agentit_inner=personal_network.begin(); agentit_inner!=personal_network.end(); agentit_inner++)
        {
          // test if the other agent is also within the personal network
          if (other == *agentit_inner)
          { 
            nr_actual_links++;
            break; // premature optimization
          }
        }
      }
    }
    // NOTE: the above algorithm counts each links in the personal network _twice_
    
    // now do the math
    double size = static_cast<double>(personal_network.size());
    nr_possible_links =  size * (size - 1);
    sum += nr_possible_links ? (nr_actual_links / nr_possible_links) : 0; // prevent zero-division
    
    nr_actual_links = 0;
    personal_network.clear();
  }
  return sum / nr_of_agents;
}

bool exists(Agent* target, list<Agent*> &agents)
// returns true if the target is in the list of agents
{
  list<Agent*>::iterator it;
  for (it=agents.begin(); it!=agents.end(); it++)
  {
    if (*it == target) return true;
  }
  return false;
}

double avgpath(int nr_of_agents, Agent population[])
// returns the average path length
{
  bool DBG = false;
  list<Agent*> current_queue;
  list<Agent*> next_queue;
  list<Agent*> visited;
  list<Agent*> peers;
  list<Agent*>::iterator it1;
  list<Agent*>::iterator it2;
  int steps;
  int sum = 0;
  
  // for each agent
  for (int i=0; i<nr_of_agents; i++)
  {
    
    if (DBG) cerr << "\n\nCalculation of total pathlength for agent " << i << endl
                  << "-------------------------------------------\n";
    
    // clear any previous state
    visited.clear();
    current_queue.clear();
    next_queue.clear();
    current_queue.push_back(&population[i]);
    steps = 1;
    
    // sum up the pathlengths to every other agent
    // in a dynamic breadth-first loop
    while (!current_queue.empty())
    {
      for (it1=current_queue.begin(); it1!=current_queue.end(); it1++)
      {
        if (DBG) cerr << "Visiting agent " << (*it1)->getid() << ". Its peers are:\n";
                
        // mark current agent as visited
        visited.push_back(*it1);
        
        // for all its direct peers
        peers = (*it1)->getAgents();
        for (it2=peers.begin(); it2!=peers.end(); it2++)
        {
          if (DBG) cerr << "  - agent " << (*it2)->getid();
          // skip if they're already visited
          if (exists(*it2, visited)) {
            if (DBG) cerr << " (skipping)\n"; continue; }
          // else, mark them as visited
          else { visited.push_back(*it2); }
          
          if (DBG) cerr << " (" << steps << " step" << (steps>1 ? "s away)\n" : " away)\n");
          
          // record the path length
          sum += steps;
          
          // add peer to queue
          next_queue.push_back(*it2);
        }
      }
      
      // all agents in the current_queue have now been visited,
      // time to move on to the next level
      steps++;
      current_queue.clear();
      current_queue.splice(current_queue.begin(), next_queue);
    }
    
    if (DBG) cerr << "\nThe total pathlength so far is " << sum;
  }
  
  double result = (static_cast<double>(sum) / (nr_of_agents * (nr_of_agents - 1)));

  if (DBG) cerr << "\n\nDone! The result is an average pathlength of " << sum
       << " / n(n-1) = " << sum << " / " << nr_of_agents * (nr_of_agents - 1)
       << " = " << result << endl << endl;
  if (DBG) exit(0);
       
  return result;
}
