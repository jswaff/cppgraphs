#include <cstdlib>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

// the maximum number of vertices in the graph
const int MAX_VERTICES=50;

// a representation of a weighted, undirected graph with n vertices
// using an array of n lists of vertices.  lists are used due to the
// sparse nature (relatively low densities) of the homework assignment.
class Graph
{
public:
   Graph() {
      for (int i=0;i<MAX_VERTICES;i++) {
         vector<pair<int,int>> edge;
         edges[i] = edge;
      }
   }

   // test whether there is an edge from x to y
   bool is_adjacent(int x,int y) {
      for (pair<int,int> x_neighbor : edges[x]) {
         if (x_neighbor.first == y) return true;
      }
      return false;
   }

   // add an edge from x to y with cost d, if it is not there
   void add(int x,int y,int d) {
      if (!is_adjacent(x,y)) {
         edges[x].push_back(make_pair(y,d));
         edges[y].push_back(make_pair(x,d));
         vertices.insert(x);
         vertices.insert(y);
      }
   }

   // list all vertices y such that there is an edge from x to y
   vector<pair<int,int>> neighbors(int x) {
      return edges[x];
   }

   int num_vertices() { return vertices.size(); }

private:

   set<int> vertices;

   // the edge lists.  list <i> contains vertex <j> if there is an edge from vertex
   // <i> to <j>.  As this is an undirected graph, that would also imply an edge
   // exists from <j> to <i>.  Also, as this is a weighted graph, each edge has a
   // cost associated with it.  The second element of the pair is the cost.
   vector<pair<int,int>> edges[MAX_VERTICES];

};

// find a minimum spanning tree using Prim's algorithm
class MST {

public:

   MST(Graph g) : g(g) { }

   // find the MST and cost
   pair<vector<pair<int,int>>,int> mst(int start_node) {

      vector<int> closed;
      vector<pair<int,int>> mst;
      int total_cost=0;

      // add the starting node to the closed set
      closed.push_back(start_node);

      // find the nearest neighbor to the starting node
      vector<pair<int,int>> start_node_neighbors = g.neighbors(start_node);
      if (start_node_neighbors.size() == 0) {
         return make_pair(mst,-1);
      }
      unsigned int least_ind = find_least_ind(start_node_neighbors);
      pair<int,int> least_neighbor = start_node_neighbors[least_ind];
      mst.push_back(make_pair(start_node,least_neighbor.first));
      total_cost += least_neighbor.second;
      closed.push_back(least_neighbor.first);

      while(closed.size() < g.num_vertices()) {

         // find the nearest node not already in the closed set
         int closest_node_distance;
         pair<int,int> shortest_edge = make_pair(-1,-1);
         for (auto closed_node : closed) {
            vector<pair<int,int>> closed_node_neighbors = g.neighbors(closed_node);
            for (auto closed_node_neighbor : closed_node_neighbors) {
               // is this node already in the closed set?
               bool in_closed = find(closed.begin(), closed.end(), closed_node_neighbor.first) != closed.end();
               if (!in_closed) {
                  if (shortest_edge.first == -1 || closed_node_neighbor.second < closest_node_distance) {
                     closest_node_distance = closed_node_neighbor.second;
                     shortest_edge = make_pair(closed_node, closed_node_neighbor.first);
                  }
               }
            }
         }

         if (shortest_edge.first == -1) {
            cerr << "could not find shortest_edge!\n";
            return make_pair(mst,-1);
         }

         // we found the nearest neighbor
         mst.push_back(shortest_edge);
         total_cost += closest_node_distance;
         closed.push_back(shortest_edge.second);
      }

      return make_pair(mst,total_cost);
   }

private:
   Graph g;

   unsigned int find_least_ind(vector<pair<int,int>> vs) {
      int least_ind=0;
      for (unsigned int i=1;i<vs.size();i++) {
         if (vs[i].second < vs[least_ind].second) {
            least_ind = i;
         }
      }
      return least_ind;
   }

};


int main() {

   // read the data file.  the initial integer is the number of vertices in the graph, followed
   // by integer triplets i,j,cost
   ifstream data_file("sample_data.txt");
   vector<vector<int>> data;
   string line;
   while (getline(data_file, line)) {
      istringstream ss(line);
      vector<int> line_vec;
      int v;
      while (ss >> v) {
         line_vec.push_back(v);
      }
      data.push_back(line_vec);
   }
   data_file.close();

   // build the graph
   int num_vertices = data[0][0];
   cout << "num vertices: " << num_vertices << endl;

   Graph g;
   for (int i=1;i<data.size();i++) {
      g.add(data[i][0],data[i][1],data[i][2]);
   }

   // compute the MST using Prim's algorithm
   MST mst(g);
   pair<vector<pair<int,int>>,int> mst_out = mst.mst(0);
   for (auto edge : mst_out.first) {
      cout << edge.first << "->" << edge.second << endl;
   }
   cout << "total cost: " << mst_out.second << endl;

//// Output from sample_data.txt
/*
num vertices: 20
0->2
2->9
9->8
8->4
4->7
4->15
7->10
15->19
9->12
12->3
12->11
12->17
11->14
17->1
14->18
1->6
18->5
5->16
9->13
total cost: 30
*/


   return 0;
}



