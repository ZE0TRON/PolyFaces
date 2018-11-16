#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>
#include "main.h"
#include <set>
using namespace std;

// returns 3d distance between two nodes
float distanceCalc(Node *a, Node *b)
{
    return sqrt(pow((a->x - b->x), 2) + pow((a->y - b->y), 2) + pow((a->z - b->z), 2));
}

void updateNeighbors(Node *a, int deletedIndex)
{
    for (int i = 0; i < a->edges.size(); i++)
    {
        Node *neighbour = a->edges.at(i).b;
        for (int j = 0; j < neighbour->edges.size(); j++)
        {
            if (neighbour->edges.at(j).b->index == deletedIndex)
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
    if(a->index == -1 ){
        newNode->index = b->index;
    }
    else if(b->index == -1){
        newNode->index = a->index;
    }
    else {
        newNode->index = a->index > b->index ? b->index : a->index;
    }
    vector<Edge> * newEdges = new vector<Edge>;
    map<int, int> unionEd;
    for (int i = 0; i < a->edges.size(); i++)
    {
      try
        {
             unionEd.at(a->edges.at(i).b->index);
        }
        catch (out_of_range e)
        {
            if (a->edges.at(i).length != 0)
            {
                Edge *tempEdge = &(a->edges.at(i));
                tempEdge->a = newNode;
                if(tempEdge->b->index!= newNode->index){
                    newEdges->push_back(*tempEdge);
                    //cout << "I am adding  : "
                    // << " A : " << tempEdge->a->index << " B : " << tempEdge->b->index << endl;
                     unionEd.insert(pair<int, int>(tempEdge->b->index, 0));
                }
            }
        }
    }
    for (int i = 0; i < b->edges.size(); i++)
    {
        try
        {
            //cout << "Duplicate : " << b->edges.at(i).b->index << endl;
             unionEd.at(b->edges.at(i).b->index);
        }
        catch (out_of_range e)
         {
            if (b->edges.at(i).length != 0)
            {
                Edge *tempEdge = &(b->edges.at(i));
                tempEdge->a = newNode;
                if(tempEdge->b->index!= newNode->index){
                    //cout << "Couldnt find : " << tempEdge->b->index << endl;
                    newEdges->push_back(*tempEdge);
                    unionEd.insert(pair<int, int>(b->edges.at(i).b->index, 0));
                }
           }
        }
    }
    newNode->edges = *newEdges;
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
int main(int argc, char * argv[])
{
    if(argc <4){
        cout<< "Usage ./poly <inputfile> <outputfile> <reduction_percentage>"<<endl;
        return 0;
    }
    
    string inputfile = argv[1];
    string inpSlashed;
    stringstream inpfileStream(inputfile);
    cout<<inputfile;
    string fileName;
    while(getline(inpfileStream,inpSlashed,'/')){
        fileName = inpSlashed;
    }
    //Clearing the texture and normalize vectors from file
    string grepQuery ="grep -v vt "+inputfile+" | grep -v vn | grep -v s | grep -v vp | grep -v m > /tmp/simplified"+fileName;
    string simplifiedInput = "/tmp/simplified"+fileName;
    string cSimplifiedInput = simplifiedInput.c_str();
    const char * cGrepQuery = grepQuery.c_str();
    int grepStatus;
    grepStatus=system(cGrepQuery);
    cout<<"Grep Status : "<<grepStatus<<endl;
    string outputfile = argv[2];
    float ratio = ((float)atoi(argv[3]))/100;
    ifstream infile(cSimplifiedInput);
    set<int> markDeleted;
    map<int, int> nodeDict;
    vector<Node> nodes;
    map<float, Edge> edges;
    vector<vector<int> > faces;
    char a;
    float x, y, z;
    int vCounter = 1;
    Node *tempNode = new Node;
    // Processing The Obj File to the Graph
    for (string line; getline(infile, line);)
    {
        stringstream linestream(line);
        linestream >> a;
        switch (a)
        {
            case 'v': // Parsing the vertices in the file
            {
                linestream >> x >> y >> z;
                tempNode->x = x;
                tempNode->y = y;
                tempNode->z = z;
                tempNode->index = vCounter++;
                nodes.push_back(*tempNode);
                break;
            }
            case 'f': // Parsing the faces in the file
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
                try{
                    for (int i = 0; i < face.size() - 1; i++)
                    {
                        // Creating a chained edge from first node to  last node (1-2-3-....k-1)
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
                }
                catch(out_of_range e){
                    continue;
                }
                if (face.size() > 2)
                {
                    // Connecting the first node with last node (1 - k)
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
                faces.push_back(face);
                break;
            }
            default:
                continue;
                break;
        }
    } // End of parsing file
    infile.close();
    int numberOfNodes = static_cast<int>(nodes.size());
    int *deleteds = (int*)calloc(numberOfNodes,sizeof(int));

    int deletedNodeCounter=0;
    map<float,Edge>::iterator myIt;
    // Finding the lowest weighted edge and deleting it
    while((deletedNodeCounter < (int)(((float)numberOfNodes)*ratio))){
        myIt = edges.begin();
       // cout<<"Deleted Node Counter : "<<deletedNodeCounter<<endl;
        Edge gonnaDeleted = myIt ->second;
      //  cout<<"The edges length is : "<<gonnaDeleted.length<<endl;
        int oldA = gonnaDeleted.a->index;
        int oldB = gonnaDeleted.b->index;
        int deletedIndex = oldA>oldB ? oldA:oldB;
        int keepedIndex = oldA<oldB ? oldA:oldB;
        if(keepedIndex==-1){
            keepedIndex = deletedIndex;
            deletedIndex = -1;
        }
        else if(keepedIndex==-1 && deletedIndex==-1){
            if(myIt!=edges.end()){
                myIt++;
                continue;
            }
        }
        else{
            if(myIt->first<0.00000005){
                if(edges.size()>5){
                    edges.erase(myIt);
                    continue;
                }
                else{
                    break;
                }
               
            }
            Node *newNode = createAverage(gonnaDeleted.a, gonnaDeleted.b);
            updateNeighbors(newNode, deletedIndex);
            
            nodes.at(keepedIndex) = *newNode;
            deleteds[deletedIndex]= 1;
            nodes.at(deletedIndex).index=-1;
//            if(edges.size()>5){
                edges.erase(myIt);
//            }
//            else{
//                break;
//            }
            deletedNodeCounter++;
            
            for(map<float,Edge>::iterator loopIt=edges.begin();loopIt !=edges.end();){
                
                if(loopIt->second.a->index == -1 ){
                    loopIt->second.a = newNode;
                    loopIt->second.length = distanceCalc(loopIt->second.a, loopIt->second.b);
                }
                else if(loopIt->second.b->index == -1){
                    loopIt->second.b = newNode;
                }
                else if(loopIt->second.a->index == -1 && loopIt->second.b->index == -1){

                    if(edges.size()>5){
                        edges.erase(myIt);;
                    }
                    else{
                        break;
                    }
                    loopIt = edges.begin();
                    //cout<<"Edge is "<<loopIt->first<<endl;
                }
                else{
                    
                     loopIt++;
                }
            }
        }
     }
    int increment=0;
    for(int i=0;i < numberOfNodes;i++){
        increment+=deleteds[i];
        deleteds[i]=increment;
    }
    
    for(int i=0;i<faces.size();i++){
        int currentNumberOfFaces = static_cast<int>(faces.at(i).size());
        int currentFaceCounter=0;
        vector<int> * newVector = new vector<int>;
        for(int j=0;j<currentNumberOfFaces;j++){
            if(nodes.at(faces.at(i).at(j)-1).index== -1 && currentFaceCounter >0){
                  faces.erase(faces.begin()+i);
                currentFaceCounter+=1;
                break;
            }
            else if(nodes.at(faces.at(i).at(j)-1).index== -1 ){
                  faces.erase(faces.begin()+i);
                currentFaceCounter+=1;
                break;
                
            }
            else{
                newVector->push_back(faces.at(i).at(j));
            }
        }
        if(currentNumberOfFaces <= 3){
            faces.erase(faces.begin()+i);
        }
    }
    
    for(int i=0;i<numberOfNodes;i++){
        if(nodes.at(i).index== -1){
            nodes.erase(nodes.begin()+i);
            i--;
            numberOfNodes--;
        }
    }
    // Writing to file
    ofstream outFile;
    outFile.open(outputfile);
    for(int i=0;i<numberOfNodes;i++){
        if(nodes.at(i).index != -1){
            outFile<<"v"<<" "<<nodes.at(i).x<<" "<<nodes.at(i).y<<" "<<nodes.at(i).z<<"\n";
        }
        else{
            cout<<"Doesnt work for me  Index : "<<nodes.at(i).index <<" X : " <<nodes.at(i).x<<" Y : " <<nodes.at(i).y<<" Z : " <<nodes.at(i).z<<endl;
        }
    }
    for(int i=0;i<faces.size();i++){
        outFile<<"f";
        int currentNumberOfFaces = static_cast<int>(faces.at(i).size());
        for(int j=0;j<currentNumberOfFaces;j++){
            outFile<<" "<<faces.at(i).at(j)-deleteds[faces.at(i).at(j)];
        }
        outFile<<"\n";
    }
    
    outFile.close();

    return 0;
}

