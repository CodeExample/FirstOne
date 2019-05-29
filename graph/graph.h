#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <list>

using namespace std;

namespace fl
{

class Edge;

class Vertex
{
public:
  list<Edge*> edges;
  void * data=nullptr;
};

class Edge
{
public:
  Edge(Vertex*, Vertex*);

  Vertex * v0, *v1;
  void * data=nullptr;
};


class Graph
{
public:
  Graph();

  list<Vertex*> vertexes;
  list<Edge*> edges;

  Edge * add(Vertex * v0, Vertex * v1);

};

}

#endif // GRAPH_H
