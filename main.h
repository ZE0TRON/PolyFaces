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