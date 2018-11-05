//
//  main.h
//  poly
//
//  Created by Bilge Cimen on 3.11.2018.
//  Copyright © 2018 Bilge Cimen. All rights reserved.
//

#ifndef main_h
#define main_h


#endif /* main_h */
#include <vector>
using namespace std;
struct Node;

struct Edge {
    Node *a;
    Node *b;
    float length;
};

struct Node {
    int index;
    float x;
    float y;
    float z;
    vector<Edge> edges;
};

class Graph {
public:
    Node *nodes;
};
