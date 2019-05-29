#include "graph.h"

using namespace fl;
using namespace std;

Edge::Edge(Vertex * _v0, Vertex * _v1) : v0(_v0), v1(_v1)
{

}


Graph::Graph()
{

}


Edge * Graph::add(Vertex * v0, Vertex * v1)
{
  Edge * e = new Edge(v0, v1);
  edges.push_back(e);
  v0->edges.push_back(e);
  v1->edges.push_back(e);
  return e;
}

