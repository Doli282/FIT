#ifndef __PROGTEST__
#include <cassert>
#include <cstdint>
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
#include <random>

using ChristmasTree = size_t;

struct TreeProblem
{
  int max_group_size;
  std::vector<uint64_t> gifts;
  std::vector<std::pair<ChristmasTree, ChristmasTree>> connections;
};

#endif

struct Vertex
{
  Vertex(uint64_t gifts, ChristmasTree number) : giftsVertexIncluded(gifts), giftsVertexIncluded2(gifts), ID(number) {}
  uint64_t giftsVertexIncluded; // vertex in group of 1
  u_int64_t giftsVertexIncluded2; // vertex in group of 2
  uint64_t giftsVertexExcluded = 0; // vertex is excluded
  ChristmasTree ID; // id of the vertex
  ChristmasTree father = 0; // id of the father of this vertex
  std::vector<ChristmasTree> relatives; // ids of all relatives

  // is it better to be excluded or in group of 1
  uint64_t maxGifts()
  {
    return (giftsVertexExcluded < giftsVertexIncluded) ? giftsVertexIncluded : giftsVertexExcluded;
  }

  // is it better to be excluded / in group of 1 / in group of 2
  uint64_t maxGifts2()
  {
    return std::max({giftsVertexIncluded, giftsVertexIncluded2, giftsVertexExcluded});
  }

  // returns the difference in gifts if vertex is included/excluded
  // if excluded > included => return 0
  uint64_t diff()
  {
    if(giftsVertexIncluded > giftsVertexExcluded)
    {
      return giftsVertexIncluded - giftsVertexExcluded;
    }
    else
    {
      return 0;
    }
  }
};

uint64_t solve(const TreeProblem &town)
{
  // structures, variables
  ChristmasTree NOVertices = 0; // total number of vertices
  uint64_t giftsOfSonIncluded; // difference if the son is included as well (opt 2)
  Vertex *vertex;
  Vertex *vertexSon;
  std::deque<Vertex *> stack; // own stack in topological order -> stocks are on the top
  std::queue<Vertex *> queue;
  std::vector<Vertex> vertices;

  //  counting vertices
  for (uint64_t gift : town.gifts)
  {
    vertices.emplace_back(gift, NOVertices++);
  }

  //  symetrization of connections
  for (const auto &connection : town.connections)
  {
    vertices.at(connection.first).relatives.push_back(connection.second);
    vertices.at(connection.second).relatives.push_back(connection.first);
  }

  //  BFS to create a spanning-tree
  queue.push(&vertices.at(0));
  stack.push_back(&vertices.at(0));
  while (!queue.empty())
  {
    vertex = queue.front();
    queue.pop();
    for (ChristmasTree son : vertex->relatives)
    {
      if (son != vertex->father)
      {
        vertexSon = &vertices.at(son);
        vertexSon->father = vertex->ID; // assign father vertex
        stack.push_back(vertexSon);
        queue.push(vertexSon);
      }
    }
  }

  // differentiate between options
  if (town.max_group_size < 2)
  { // size of groups of policemen is 1
    // memoziation
    while (!stack.empty())
    {
      vertex = stack.back();
      stack.pop_back();
      for (ChristmasTree son : vertex->relatives)
      {
        if (son != vertex->father) // sum gifts only for sons
        {
          vertexSon = &vertices.at(son);
          vertex->giftsVertexIncluded += vertexSon->giftsVertexExcluded; // option if vertex is included
          vertex->giftsVertexExcluded += vertexSon->maxGifts(); // option if vertex is excluded
        }
      }
    }
    return vertices.at(0).maxGifts(); // return the maximum in root from excluded/included
  }
  else
  { // policemen go in groups of two
    while(!stack.empty())
    {
      vertex = stack.back();
      stack.pop_back();
      giftsOfSonIncluded = 0; // set difference to 0
      for (ChristmasTree son : vertex->relatives)
      {
        if (son != vertex->father)
        {
          vertexSon = &vertices.at(son);
          vertex->giftsVertexIncluded += vertexSon->giftsVertexExcluded; // option vertex is included solo
          vertex->giftsVertexExcluded += vertexSon->maxGifts2(); // option vertex is excluded
          giftsOfSonIncluded = std::max({giftsOfSonIncluded, vertexSon->diff()}); // save the maximal difference if a son is included as well in a group
        }
      }
      vertex->giftsVertexIncluded2 = vertex->giftsVertexIncluded + giftsOfSonIncluded;  // option vertex is in group -> include the son with the highest difference
    }
    return vertices.at(0).maxGifts2(); // choose maximum from options in root
  }
}

#ifndef __PROGTEST__

using TestCase = std::pair<uint64_t, TreeProblem>;

const std::vector<TestCase> BASIC_TESTS = {
    {3, {1, {1, 1, 1, 2}, {{0, 3}, {1, 3}, {2, 3}}}},
    {4, {1, {1, 1, 1, 4}, {{0, 3}, {1, 3}, {2, 3}}}},
    {57, {1, {
                 17,
                 11,
                 5,
                 13,
                 8,
                 12,
                 7,
                 4,
                 2,
                 8,
             },
          {
              {1, 4},
              {6, 1},
              {2, 1},
              {3, 8},
              {8, 0},
              {6, 0},
              {5, 6},
              {7, 2},
              {0, 9},
          }}},
    {85, {1, {
                 10,
                 16,
                 13,
                 4,
                 19,
                 8,
                 18,
                 17,
                 18,
                 19,
                 10,
             },
          {
              {9, 7},
              {9, 6},
              {10, 4},
              {4, 9},
              {7, 1},
              {0, 2},
              {9, 2},
              {3, 8},
              {2, 3},
              {5, 4},
          }}},
    {79, {1, {
                 8,
                 14,
                 11,
                 8,
                 1,
                 13,
                 9,
                 14,
                 15,
                 12,
                 1,
                 11,
             },
          {
              {9, 1},
              {1, 2},
              {1, 4},
              {5, 10},
              {7, 8},
              {3, 7},
              {11, 3},
              {11, 10},
              {6, 8},
              {0, 1},
              {0, 3},
          }}},
    {102, {1, {
                  15,
                  10,
                  18,
                  18,
                  3,
                  4,
                  18,
                  12,
                  6,
                  19,
                  9,
                  19,
                  10,
              },
           {
               {10, 2},
               {11, 10},
               {6, 3},
               {10, 8},
               {5, 3},
               {11, 1},
               {9, 5},
               {0, 4},
               {12, 3},
               {9, 7},
               {11, 9},
               {4, 12},
           }}},
    {93, {1, {
                 1,
                 7,
                 6,
                 18,
                 15,
                 2,
                 14,
                 15,
                 18,
                 8,
                 15,
                 1,
                 5,
                 6,
             },
          {
              {0, 13},
              {6, 12},
              {0, 12},
              {7, 8},
              {8, 3},
              {12, 11},
              {12, 1},
              {10, 12},
              {2, 6},
              {6, 9},
              {12, 7},
              {0, 4},
              {0, 5},
          }}},
};

const std::vector<TestCase> BONUS_TESTS = {
    {3, {2, {1, 1, 1, 2}, {{0, 3}, {1, 3}, {2, 3}}}},
    {5, {2, {1, 1, 1, 4}, {{0, 3}, {1, 3}, {2, 3}}}},
    {7, {2, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {{0, 1}, {2, 1}, {2,3}, {3,4}, {5,3}, {2,6}, {6,7}, {6,8}, {6,9}}}},
    {9, {2, {1, 1, 1, 1, 1, 1, 4, 1, 1, 1}, {{0, 1}, {2, 1}, {2,3}, {3,4}, {5,3}, {2,6}, {6,7}, {6,8}, {6,9}}}},
    {11, {2, {1, 1, 4, 1, 1, 1, 4, 1, 1, 1}, {{0, 1}, {2, 1}, {2,3}, {3,4}, {5,3}, {2,6}, {6,7}, {6,8}, {6,9}}}},
};

void test(const std::vector<TestCase> &T)
{
  int i = 0;
  for (auto &[s, t] : T)
  {
    if (s != solve(t))
      std::cout << "Error in " << i << " (returned " << solve(t) << ")" << std::endl;
    i++;
  }
  std::cout << "Finished" << std::endl;
}

int main()
{
  test(BASIC_TESTS);
  test(BONUS_TESTS);
}

#endif
