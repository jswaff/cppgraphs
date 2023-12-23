#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

// the maximum number of vertices in the graph
const int NUM_VERTICES=50;

// a representation of a weighted, undirected graph with n vertices
// using an array of n lists of vertices.  lists are used due to the
// sparse nature (relatively low densities) of the homework assignment.
class Graph
{
public:
   Graph() {
      for (int i=0;i<NUM_VERTICES;i++) {
         vector<pair<int,int>> edge;
         edges[i] = edge;
      }
   }

   Graph(float density, int max_range) {
      for (int i=0;i<NUM_VERTICES;i++) {
         vector<pair<int,int>> edge;
         edges[i] = edge;
      }
      initialize(density, max_range);
   }

   void initialize(float density, int max_range) {
      cout << "initializing with density " << density << " and max_range " << max_range << endl;

      // between any two vertices i and j, determine if an edge should exist using the
      // density, and if so what the range should be
      for (int i=0;i<NUM_VERTICES;i++) {
         for (int j=i+1;j<NUM_VERTICES;j++) {
            if (prob() < density) {
               int range = (rand() % max_range) + 1; // random value [1, max_range]
               edges[i].push_back(make_pair(j,range));
               edges[j].push_back(make_pair(i,range)); // undirected
            }
         }
      }
   }

   void print() {
      for (int i=0;i<NUM_VERTICES;i++) {
         cout << i << ": ";
         for (pair<int,int> p : edges[i]) {
            cout << p.first << "(" << p.second << ") ";
         }
         cout << endl;
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
      }
   }

   // list all vertices y such that there is an edge from x to y
   vector<pair<int,int>> neighbors(int x) {
      return edges[x];
   }

private:

   // the edge lists.  list <i> contains vertex <j> if there is an edge from vertex
   // <i> to <j>.  As this is an undirected graph, that would also imply an edge
   // exists from <j> to <i>.  Also, as this is a weighted graph, each edge has a
   // cost associated with it.  The second element of the pair is the cost.
   vector<pair<int,int>> edges[NUM_VERTICES];

   inline float prob() {
      return ((float)rand()) / RAND_MAX;
   }
};

// find a shortest path between two vertices using Dijkstra's algorithm
class ShortestPath {

public:

   ShortestPath(Graph g) : g(g) { }

   // find the shortest path and cost
   pair<vector<int>,int> path(int s, int d) {
      open.clear();  open_path.clear();
      closed.clear();  closed_path.clear();
      if (s==d) return make_pair(vector<int>(),0);

      // step 1 - include s in the closed set with distance 0
      // and all immediate successors of s with their distance in the open set
      closed.push_back(make_pair(s,0));
      closed_path.push_back(vector<int>());
      for (auto neighbor : g.neighbors(s)) {
         open.push_back(neighbor);
         vector<int> path_to_neighbor;
         path_to_neighbor.push_back(neighbor.first);
         open_path.push_back(path_to_neighbor);
      }

      while (!open.empty()) {

         // find lowest cost element in open set.  move it to the closed set if not already in it.
         // if the lowest cost element is our destination, we found the shortest path.
         unsigned int least_ind = find_least_ind(open);
         pair<int,int> n = open[least_ind];
         vector<int> n_path = open_path[least_ind];
         if (find_ind(closed,n.first)==-1) {
            if (n.first==d) {
               pair<vector<int>,int> retval = make_pair(n_path,n.second);
               return retval;
            }
            closed.push_back(make_pair(n.first,n.second));
            closed_path.push_back(vector<int>(n_path));
         }
         open.erase(open.begin() + least_ind);
         open_path.erase(open_path.begin() + least_ind);

         // get neighbors of n.  for each one:
         // 1. if already in the closed set, do nothing; it is guaranteed shortest distance already.
         // 2. otherwise, if not in the open set, add it
         // 3. otherwise, update the open set if this path is shorter
         for (auto neighbor : g.neighbors(n.first)) {
            if (find_ind(closed,neighbor.first) == -1) {
               int open_ind = find_ind(open,neighbor.first);
               int distance = n.second + neighbor.second;
               vector<int> path = vector<int>(n_path);
               path.push_back(neighbor.first);
               if (open_ind == -1) {
                  open.push_back(make_pair(neighbor.first,distance));
                  open_path.push_back(path);
               } else {
                  if (distance < open[open_ind].second) {
                     open[open_ind].second = distance;
                     open_path[open_ind] = path;
                  }
               }
            }
         }
      }

      // no path
      return make_pair(vector<int>(),-1);
   }

private:
   Graph g;

   // closed vertices - have known shortest distances.
   // first element is destination vertex
   // second element is distance to vertex
   vector<pair<int,int>> closed;
   vector<vector<int>> closed_path;

   // open vertices
   // first element is destination vertex
   // second element is distance to vertex
   vector<pair<int,int>> open;
   vector<vector<int>> open_path;

   unsigned int find_least_ind(vector<pair<int,int>> vs) {
      int least_ind=0;
      for (unsigned int i=1;i<vs.size();i++) {
         if (vs[i].second < vs[least_ind].second) {
            least_ind = i;
         }
      }
      return least_ind;
   }

   // find the index of v in vs.  if not present, returns -1
   int find_ind(vector<pair<int,int>> vs, int v) {
      int ind=-1;
      for (unsigned int i=0;i<vs.size();i++) {
         if (vs[i].first == v) {
            ind = i;
            break;
         }
      }
      return ind;
   }


};


void print_paths(float density) {

   // for ranges 1.0 to 10.0
   for (float r=1.0;r<=10.0;r+=1.0) {
      Graph g(density, r);
      ShortestPath sp(g);

      // compute path lengths 1 to 2, 1 to 3, 1 to 4, ..., 1 to 50
      int total_distance=0;
      int num_distances=0;
      for (int dest=1;dest<50;dest++) {
         pair<vector<int>,int> s_path = sp.path(0,dest);
         //cout << "\tpath 0->" << dest << ": ";
         //for (int i : s_path.first) {
         //   cout << i << "->";
         //}
         //cout << endl;
         if (s_path.second != -1) {
            total_distance += s_path.second;
            num_distances++;
         }
      }
      cout << "\taverage path length: " << ((float)total_distance) / num_distances << endl;
   }
}

int main() {

   print_paths(0.2);
   print_paths(0.4);

   return 0;
}

/*
SAMPLE OUTPUT:
initializing with density 0.2 and max_range 1
	average path length: 1.93878
initializing with density 0.2 and max_range 2
	average path length: 2.44898
initializing with density 0.2 and max_range 3
	average path length: 3.08163
initializing with density 0.2 and max_range 4
	average path length: 2.97959
initializing with density 0.2 and max_range 5
	average path length: 3.63265
initializing with density 0.2 and max_range 6
	average path length: 4.55102
initializing with density 0.2 and max_range 7
	average path length: 5.02041
initializing with density 0.2 and max_range 8
	average path length: 6.4898
initializing with density 0.2 and max_range 9
	average path length: 5.02041
initializing with density 0.2 and max_range 10
	average path length: 7.73469
initializing with density 0.4 and max_range 1
	average path length: 1.69388
initializing with density 0.4 and max_range 2
	average path length: 1.73469
initializing with density 0.4 and max_range 3
	average path length: 2.10204
initializing with density 0.4 and max_range 4
	average path length: 2.65306
initializing with density 0.4 and max_range 5
	average path length: 2.71429
initializing with density 0.4 and max_range 6
	average path length: 2.97959
initializing with density 0.4 and max_range 7
	average path length: 3.2449
initializing with density 0.4 and max_range 8
	average path length: 3.77551
initializing with density 0.4 and max_range 9
	average path length: 3.57143
initializing with density 0.4 and max_range 10
	average path length: 3.77551
*/


