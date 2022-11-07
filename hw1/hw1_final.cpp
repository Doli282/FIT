#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>

using Place = size_t;

struct Map {
  size_t places;
  Place start, end;
  std::vector<std::pair<Place, Place>> connections;
  std::vector<std::vector<Place>> items;
};

template < typename F, typename S >
struct std::hash<std::pair<F, S>> {
  std::size_t operator () (const std::pair<F, S> &p) const noexcept {
    // something like boost::combine would be much better
    return std::hash<F>()(p.first) ^ (std::hash<S>()(p.second) << 1);
  }
};

#endif // ----------------------------------------- MY FUNCTIONAL WORK BELOW--------------------------------------------------------

#define DEBUG 0

// creates the list of neighbours for every vertices
std::vector<std::vector<u_int16_t>> findNeighbours (const std::vector<std::pair<Place, Place>> &connections, u_int16_t NoVertices)
{
  // initialize list
  std::vector<std::vector<u_int16_t>> neighbours(NoVertices);
  // fill list with neighbours
  for(const auto & connection : connections)
  {
    neighbours.at(connection.first).push_back(connection.second);
    neighbours.at(connection.second).push_back(connection.first);
  }
  return neighbours;
} 

// prepare inventory - list of rooms with items stored inside
std::vector<u_int16_t> inventoryCheck(const std::vector<std::vector<size_t>> &items, u_int8_t NoItems, u_int16_t NoVertices, bool & lackOfItems)
{
  // initialize inentory list
  std::vector<u_int16_t> inventory(NoVertices, 0);
  // fill inventory list
  for(u_int8_t item = 0; item < NoItems; item++)
  {// for every item in the list ...
    if(items.at(item).empty())
    {
      lackOfItems = true;
      return inventory;
    }
    for(const u_int16_t room : items.at(item))
    { // ... for every room that the item is stored in ...
      // ... write down that the item is stored in the room
      inventory.at(room) |= (1 << item);
    }
  }

  /* if(DEBUG)
  {
    for(u_int16_t vertex = 0; vertex < NoVertices; vertex++)
    {
      std::cout << inventory.at(vertex) << std::endl;
    }
  } */

  return inventory;
}

// structure of a single Node in the graph
struct TNode
{
  TNode(u_int16_t place, u_int16_t iter) : m_place(place), m_iteration(iter) {}
  u_int16_t m_place;
  u_int16_t m_iteration;
  bool m_visited = false;
  TNode* m_previousNode = nullptr;

  friend std::ostream& operator<< (std::ostream & os, const TNode * node);
};
std::ostream& operator << (std::ostream & os, const TNode * node)
{
  return os << node->m_place << "<" << node->m_iteration << ">";
}

// prepare graph with many "floors" for every possible options of having different items collected so far
std::vector<std::vector<TNode>> createGraph(u_int16_t NoVertices, u_int16_t NoIterations)
{
  std::vector<std::vector<TNode>> graph(NoIterations);
  for(u_int16_t i = 0; i < NoIterations; i++)
  {
    for(u_int16_t j = 0; j < NoVertices; j++)
    {
      graph.at(i).push_back(TNode(j, i));
    }
  }
  return graph;
}

// delete allocated nodes
/* void deleteGraph(std::vector<std::vector<TNode*>> & graph)
{
  for(auto & floor : graph)
  {
    for(auto node : floor)
    {
      delete node;
    }
  }
} */


std::list<Place> find_path(const Map &map) {
  // TODO

  // inicialization of data structures  
  bool lackOfItems = false; // check if every item is in at least one room
  u_int16_t NoVertices = map.places; // number of vertices in the graph
  u_int8_t NoItems = map.items.size(); // number of items searched for
  u_int16_t NoIterations = (1 << NoItems); // number of possible outcomes in correspondence to having picked up certain items
  std::list<Place> shortestRoute = {}; // shortest path in the graph
  std::vector<std::vector<u_int16_t>> neighbours = findNeighbours(map.connections, NoVertices); // map of vertices and their neighbouring vertices
  std::vector<u_int16_t> inventory = inventoryCheck(map.items, NoItems, NoVertices, lackOfItems); // list of rooms and items they store inside
  if(lackOfItems)
  { // there are items that are not stored anywhere - return empty path
    return shortestRoute;
  }

  // BFS initialization
  std::vector<std::vector<TNode>> graph = createGraph(NoVertices, NoIterations); // multi-dimensional graph
  std::queue<TNode*> queue;
  // BFS from start
  //if(DEBUG) std::cout << "--- initialization completed ---" << std:: endl;

  // check for items in the starting point
  // if there are items in the room close the inspected node and move to higher floor according to the found items
  u_int16_t startingPlace = map.start;
  u_int16_t startingIteration = inventory.at(startingPlace);

  TNode * start = &graph.at(startingIteration).at(map.start);
  queue.push(start);
  start->m_visited = true;
  //if(DEBUG) std::cout << "start:" << start << std::endl;

  while(!queue.empty())
  { // while there are nodes to inspect
    TNode* currentNode = queue.front();
    queue.pop();
    //if(DEBUG) std::cout << currentNode << std::endl;

    // check for end - the last iteration and the same place as the end
    if((currentNode->m_iteration == (NoIterations - 1)) && (currentNode->m_place == map.end))
    { // if at the end with all items, backtrack the path
      while(currentNode->m_previousNode)
      {
        shortestRoute.push_front(currentNode->m_place);
        currentNode = currentNode->m_previousNode;
      }
      shortestRoute.push_front(currentNode->m_place); // do not forget to add the starting point
      break;
    }

    for(auto neighbour : neighbours.at(currentNode->m_place))
    { // for every possible neighbour of the current node ...
      TNode * inspectedNode = &graph.at(currentNode->m_iteration).at(neighbour);
      if(!inspectedNode->m_visited)
      { // ... if the neighbours have not been visited yet ... -> open them, save how we got them, push them to the queue
        //if(DEBUG) std::cout << "inspecting " << inspectedNode << std::endl;
        inspectedNode->m_visited = true;
        // check for items in the inspected room
        if(u_int16_t elevation = inventory.at(inspectedNode->m_place)) // != 0
        { // if there are items in the room close the inspected node and move to higher floor according to the found items
          //if(DEBUG) std::cout << "original iteration: " << inspectedNode->m_iteration << std::endl;
          u_int16_t currentIteration = inspectedNode->m_iteration;
          elevation |= currentIteration;
          //if(DEBUG) std::cout << "new iteration: " << elevation << std::endl;
          // inspect the node at the higher floor
          inspectedNode = &graph.at(elevation).at(inspectedNode->m_place);
          if(inspectedNode->m_visited)
          { 
            if(elevation == currentIteration)
            { // I am the one visiting it right now
              inspectedNode->m_previousNode = currentNode;
              queue.push(inspectedNode);
              continue;
            }
            else 
            {
              // if the higher node was already inspected call it off
              continue;
            }
          }
          inspectedNode->m_visited = true;
        }

        inspectedNode->m_previousNode = currentNode;
        queue.push(inspectedNode);
      }
    }
  }

  // BFS from every point of interest
  // finish BFS in 'end' while having all items
  // backtrack the fastest route

  // return the shortest route
  return shortestRoute;
}

#ifndef __PROGTEST__ // ------------------------------------------------------------------------------------------------------------

using TestCase = std::pair<size_t, Map>;



// Class template argument deduction exists since C++17 :-)
const std::array examples = {
  TestCase{ 1, Map{ 2, 0, 0,
    { { 0, 1 } },
    { { 0 } }
  }},
  TestCase{ 3, Map{ 2, 0, 0,
    { { 0, 1 } },
    { { 1 } }
  }},
  TestCase{ 3, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    {}
  }},
  TestCase{ 4, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 } }
  }},
  // item not in any room
  TestCase{ 0, Map{ 4, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 }, {} }
  }},
  // start is not connected to the end
  TestCase{ 0, Map{ 5, 0, 4,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 } }
  }},
  // item unreachable
  TestCase{ 0, Map{ 5, 0, 1,
    { { 0, 2 }, { 2, 3 }, { 0, 3 }, { 3, 1 } },
    { { 2 }, {4} }
  }},
  TestCase{ 7, Map{ 7, 0, 5,
    { { 0, 1 }, {1, 2}, { 1, 3 }, { 4, 3 }, { 3, 6 }, {5, 4} },
    { { 2, 4 }, {6} }
  }},
  // more items in one room
  TestCase{ 9, Map{ 7, 0, 5,
    { { 0, 1 }, {1, 2}, { 1, 3 }, { 4, 3 }, { 3, 6 }, {5, 4} },
    { { 2, 4 }, {6}, {2}, {2}, {3, 1} }
  }},
  // isolated start
  TestCase{ 0, Map{ 5, 0, 1,
    { { 1, 2 }, { 2, 3 }, { 4, 3 }, { 3, 1 } },
    { { 2 }, {} }
  }},
  // more items of the same kind
  TestCase{ 7, Map{ 7, 0, 5,
    { { 0, 1 }, {1, 2}, { 1, 3 }, { 4, 3 }, { 3, 6 }, {5, 4} },
    { { 2, 2, 4, 4 }, {6} }
  }}
  };


void printConnections(const std::vector<std::pair<Place, Place>> &connections)
{
  for(const auto & edge : connections)
  {
    std::cout << "{" << edge.first << "," << edge.second << "}, ";
  }
  std::cout << std::endl;
}
void printItems(const std::vector<std::vector<Place>> & items)
{
  for(const auto &item : items)
  {
    std::cout << "{";
    for(const auto & room : item)
    {
      std::cout << room << ",";
    }
    std::cout << "},";
  }
  std::cout << std::endl;
}
void printMap(const Map & map)
{
  std::cout << "Generated graph: " << std::endl;
  std::cout << "nodes: " << map.places << ", start: " << map.start << ", end: " << map.end << std::endl;
  std::cout << "endges: "; printConnections(map.connections);
  std::cout << "items: "; printItems(map.items);
}

// create random graph
Map createMap(u_int16_t maxNodes, u_int16_t maxItems, u_int16_t maxEdges, u_int16_t maxRoomsPerItem, bool debug = false)
{
  Place start =(rand() % maxNodes);
  Place end =(rand() % maxNodes);
  
  // create edges
  std::vector<std::pair<Place, Place>> connections;
  
  
  for(u_int16_t i = 0; i < maxEdges; i += rand() % 10)
  {
    connections.emplace_back(rand() % maxNodes, rand() % maxNodes);
  }
 /*
  u_int16_t node1 = 0;
  u_int16_t node2;
  while(node1 < maxNodes)
  {
    for(u_int16_t degree = rand() % maxDegree; degree > 0; degree--)
    {
      node2 = rand() % maxNodes;
      connections.emplace_back(node1, node2);
    }
    node1 += (rand() % 3); //% maxNodes) - (maxNodes/3);
  } */

  // create storage
  std::vector<std::vector<Place>> items;
  for(u_int16_t NOitems = 0; NOitems < maxItems; NOitems++)
  {
    std::vector<Place> storedAt;
    for(u_int16_t NoRoomStoredAt = 0; NoRoomStoredAt < maxRoomsPerItem; NoRoomStoredAt += (rand()%4)+1)
    {
      storedAt.push_back(rand() % maxNodes);
    }
    items.push_back(storedAt);
  }

  Map map{maxNodes, start, end, connections, items};
  if(debug)
  {
     printMap(map);
  }
  return map;
}

void testGraph(u_int16_t NOTests, u_int16_t maxNodes, u_int16_t maxItems, u_int16_t maxEdges, u_int16_t maxRoomsPerItem, bool debug = false)
{
  for(u_int16_t test = 0; test < NOTests; test++)
  {
    auto solution = find_path(createMap(maxNodes, maxItems, maxEdges, maxRoomsPerItem, debug));
    if(debug)
    {
      std::cout << "pathLength: " << solution.size() << std::endl;
    }
  }
  std::cout << "random graphs completed" << std::endl;
  return;
}

void printPath(const std::list<Place> & path)
{
  std::cout << "[" << path.size() << "]: " << std::flush;
  for(auto node : path)
  {
    std::cout << node << " - ";
  }
  std::cout << std::endl;
  std::cout << "---------------" << std::endl;
}

int main() {
  int fail = 0;
  for (size_t i = 0; i < examples.size(); i++) {
    auto sol = find_path(examples[i].second);
    if (sol.size() != examples[i].first) {
      std::cout << "Wrong anwer for map " << i << std::endl;
      fail++;
    }
    else
    {
      std::cout << "Correct answer for map " << i << std::endl;
    }
    printPath(sol);
  }

  if (fail) std::cout << "Failed " << fail << " tests" << std::endl;
  else std::cout << "All tests completed" << std::endl;

  srand(time(0));
  testGraph(1000, 3000, 6, 10000, 10, false);

  return 0;
}

#endif
