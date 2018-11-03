#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>
#include "main.h"
using namespace std;

float distanceCalc(Node *a, Node *b)
{
  return sqrt(pow((a->x - b->x), 2) + pow((a->y - b->y), 2) + pow((a->z - b->z), 2)); // 3d distance between two points.
}

void updateNeighbors(Node *a, int oldA, int oldB)
{
  for (int i = 0; i < a->edges.size(); i++)
  {
    Node *neighbour = a->edges.at(i).b;
    for (int j = 0; j < neighbour->edges.size(); j++)
    {
      if (neighbour->edges.at(j).b->index == oldA || neighbour->edges.at(j).b->index == oldB)
      {
        Edge *tempEdge = &neighbour->edges.at(j);
        tempEdge->b = a;
      }
    }
  }
}

Node *createAverage(Node *a, Node *b)
{
  Node *newNode = new Node;
  newNode->x = (a->x + b->x) / 2;
  newNode->y = (a->y + b->y) / 2;
  newNode->z = (a->z + b->z) / 2;
  newNode->index = a->index > b->index ? b->index : a->index;
  vector<Edge> newEdges;
  map<int, int> unionEd;
  for (int i = 0; i < a->edges.size(); i++)
  {
    try
    {
      int h = unionEd.at(a->edges.at(i).b->index);
    }
    catch (out_of_range e)
    {
      if (a->edges.at(i).length != 0)
      {
        Edge *tempEdge = &(b->edges.at(i));
        tempEdge->a = newNode;
        newEdges.push_back(*tempEdge);
        cout << "I am adding  : "
             << " A : " << tempEdge->a->index << " B : " << tempEdge->b->index << endl;
        unionEd.insert(pair<int, int>(tempEdge->b->index, 0));
      }
    }
  }
  for (int i = 0; i < b->edges.size(); i++)
  {
    try
    {
      cout << "Duplicate : " << b->edges.at(i).b->index << endl;
      int h = unionEd.at(b->edges.at(i).b->index);
    }
    catch (out_of_range e)
    {
      if (a->edges.at(i).length != 0)
      {
        Edge *tempEdge = &(b->edges.at(i));
        tempEdge->a = newNode;
        cout << "Couldnt find : " << tempEdge->b->index << endl;
        newEdges.push_back(*tempEdge);
        unionEd.insert(pair<int, int>(b->edges.at(i).b->index, 0));
      }
    }
  }
  newNode->edges = *(new vector<Edge>);
  newNode->edges.swap(newEdges);
  return newNode;
}

// Find minumum weighted edge in the graph
// delete the node a and node b in the edge
// create a new node between them
// update all the edges and update the edges in the other nodes they were connected
// The next edge set will be the union of both edges of nodes.
// Update the indexes
// Replace the faces with new indexes
//
// Delete 2 elemented faces
int main()
{
  ifstream infile("sSimple.obj");
  map<int, int> nodeDict;
  vector<Node> nodes;
  map<float, Edge> edges;

  char a;
  float x, y, z;
  int c;
  int vCounter = 0;
  Node *tempNode = new Node;
  for (string line; getline(infile, line);)
  {
    stringstream linestream(line);
    linestream >> a;
    switch (a)
    {
    case 'v':
    {
      linestream >> x >> y >> z;
      //cout<<x<<" "<<y<<" "<<z<<endl;
      tempNode->x = x;
      tempNode->y = y;
      tempNode->z = z;
      tempNode->index = vCounter++;
      nodes.push_back(*tempNode);
      break;
    }
    case 'f':
    {
      vector<int> face;
      string spaced;
      while (linestream >> spaced)
      {
        string slashed;
        int faceNo;
        stringstream ss(spaced);

        getline(ss, slashed, '/');
        stringstream slashStream(slashed);
        slashStream >> faceNo;
        //cout<<faceNo<<" ";
        face.push_back(faceNo);
      }
      //cout<<endl;
      for (int i = 0; i < face.size() - 1; i++)
      {
        Edge *tempEdge = new Edge;
        tempEdge->a = &(nodes.at(face.at(i) - 1));
        tempEdge->b = &(nodes.at(face.at(i + 1) - 1));
        tempEdge->length = distanceCalc(tempEdge->a, tempEdge->b);
        nodes.at(face.at(i) - 1).edges.push_back(*tempEdge);
        edges.insert(pair<float, Edge>(tempEdge->length, *tempEdge));
        float tempLength = tempEdge->length;
        tempEdge = new Edge;
        tempEdge->b = &(nodes.at(face.at(i) - 1));
        tempEdge->a = &(nodes.at(face.at(i + 1) - 1));
        tempEdge->length = tempLength;
        nodes.at(face.at(i + 1) - 1).edges.push_back(*tempEdge);
      }
      if (face.size() > 2)
      {
        Edge *tempEdge = new Edge;
        tempEdge->a = &(nodes.at(face.at(0) - 1));
        tempEdge->b = &(nodes.at(face.at(face.size() - 1) - 1));
        tempEdge->length = distanceCalc(tempEdge->a, tempEdge->b);
        nodes.at(face.at(0) - 1).edges.push_back(*tempEdge);
        edges.insert(pair<float, Edge>(tempEdge->length, *tempEdge));
        float tempLength = tempEdge->length;
        tempEdge = new Edge;
        tempEdge->b = &(nodes.at(face.at(0) - 1));
        tempEdge->a = &(nodes.at(face.at(face.size() - 1) - 1));
        tempEdge->length = tempLength;
        nodes.at(face.at(face.size() - 1) - 1).edges.push_back(*tempEdge);
      }
      //Process it
      break;
    }
    default:
      continue;
      break;
    }
  } // End of parsing file
  for (int i = 0; i < nodes.size(); i++)
  {
    cout << "X : " << nodes.at(i).x << " Y : " << nodes.at(i).y << " Z : " << nodes.at(i).z << endl;
    cout << "My edges" << endl;
    for (int z = 0; z < nodes.at(i).edges.size(); z++)
    {
      Edge *currentEdge = &(nodes.at(i).edges.at(z));
      cout << "A : " << currentEdge->a->index << " B : " << currentEdge->b->index << " Length :" << currentEdge->length << endl;
    }
  }
  cout << "Node size : " << nodes.size() << endl;
  //
  int *deleteds = (int*)calloc(nodes.size(),sizeof(int));
  // Iterate the edges
  map<float, Edge>::iterator mi = edges.end();
  while (mi != edges.begin())
  {
    cout << "A : " << mi->second.a->index << " B : " << mi->second.b->index << " Length : " << mi->first << endl;
    mi--;
  }
  Edge gonnaDeleted = edges.begin()->second;
  int oldA = gonnaDeleted.a->index;
  int oldB = gonnaDeleted.b->index;
  Node *newNode = createAverage(gonnaDeleted.a, gonnaDeleted.b);
  updateNeighbors(newNode, oldA, oldB);
  cout << "X : " << newNode->x << " Y : " << newNode->y << " Z : " << newNode->z << endl;
  cout << "My edges" << endl;
  for (int z = 0; z < newNode->edges.size(); z++)
  {
    Edge *currentEdge = &(newNode->edges.at(z));
    cout << "A : " << currentEdge->a->index << " B : " << currentEdge->b->index << " Length :" << currentEdge->length << endl;
  }
  nodes.at(oldA<oldB ? oldA:oldB) = *newNode;
  deleteds[oldA>oldB ? oldA:oldB]= 1;


  //Update all indexes
  //int decrementCounter =0;
  for(int Y=0;Y<nodes.size();Y++){
    //decrementCounter+=deleteds[i];
    //nodes.at(i).index-=decrementCounter;
  }



  return 0;
}