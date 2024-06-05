#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm> // for std::find
#include <tuple>

using namespace std;

#define MAX_CAPACITY 15
#define MAX_CITY_VISITS 3
#define filename "grafo.txt"


bool contains(const std::vector<int>& vec, int value) {
    if (value == 0) {
        return false;
    }
    return find(vec.begin(), vec.end(), value) != vec.end();
}


void readFile(int& num_nodes, vector<int>& city_index, vector<int>& demand, int& num_edges, map<int, vector<pair<int, int>>>& graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file" << endl;
        return;
    }

    // Read nodes
    file >> num_nodes;
    city_index.resize(num_nodes);
    demand.resize(num_nodes);
    for (int i = 1; i < num_nodes; ++i) {
        file >> city_index[i] >> demand[i];
    }

    // Read edges
    file >> num_edges;
    int from, to, weight;
    for (int i = 0; i < num_edges; ++i) {
        file >> from >> to >> weight;
        graph[from].emplace_back(to, weight);
    }

    file.close();

    cout << "Nodes (" << num_nodes << ") and their demands:" << endl;
    for (int i = 0; i < num_nodes; ++i) {
        cout << "City: " << city_index[i] << ", Demand: " << demand[i] << endl;
    }
    cout << endl;

    cout << "Edges (" << num_edges << ") and their possible routes (destination, cost):" << endl;
    for (const auto& node : graph) {
        cout << "Node " << node.first << ": ";
        for (const auto& edge : node.second) {
            cout << "(" << edge.first << ", " << edge.second << ") ";
        }
        cout << endl;
    }
    cout << endl;

}


void searchRoutes(
    int& num_nodes, vector<int>& city_index, vector<int>& demand,
    int& num_edges, map<int, vector<pair<int, int>>>& graph,
    int cost, int node, int supply, int cities,
    vector<int> visited_cities,
    vector<tuple<vector<int>, int>>& solutions) {

    // Auxiliar variables
    int aux_node = node;
    vector<int> aux_visited_cities;
    vector<int> aux_demand;

    for (const auto& edge : graph[aux_node]) {
        int destination = edge.first;
        int weight = edge.second;

        if (destination!=0 && (cities+1 > MAX_CITY_VISITS || supply-demand[destination] <= 0)) {
          return;
        }

        if (destination == 0) {
          aux_visited_cities = visited_cities;
          aux_visited_cities.push_back(destination);
          
          // Checking for solution
          int sum_demands = 0;
          for (int value : demand) {
              sum_demands += value;
          }
          if (sum_demands == 0) {
              tuple<vector<int>, int> solution_tuple = make_tuple(aux_visited_cities, cost+weight);
              solutions.push_back(solution_tuple);
              return;
          }

          searchRoutes(
          num_nodes, city_index, demand, num_edges, graph,
          cost+weight, destination, MAX_CAPACITY, 0, aux_visited_cities,
          solutions);

        } else {

          if (demand[destination] != 0 && !contains(aux_visited_cities, destination)) {
              aux_visited_cities = visited_cities;
              aux_visited_cities.push_back(destination);
              aux_demand = demand;
              aux_demand[destination] = 0;

              searchRoutes(
              num_nodes, city_index, aux_demand, num_edges, graph,
              cost+weight, destination, supply-demand[destination], cities+1, aux_visited_cities,
              solutions);
          }
          
        }
    }
}

int main() {
    int num_nodes;
    vector<int> city_index;
    vector<int> demand;
    int num_edges;
    map<int, vector<pair<int, int>>> graph;

    readFile(num_nodes, city_index, demand, num_edges, graph);

    // Constraints
    int supply = MAX_CAPACITY;
    int cost, node, cities = 0;
    vector<int> visited_cities = {0};

    vector<tuple<vector<int>, int>> solutions;

    searchRoutes(
      num_nodes, city_index, demand, num_edges, graph,
      cost, node, supply, cities, visited_cities, solutions);

    vector<int> best_route;
    int best_cost = 999999;

    for (const auto& t : solutions) {
        const vector<int>& route = get<0>(t);
        int cost = get<1>(t);

        if (cost <  best_cost) {
          best_route = route;
          best_cost = cost;
        }
    }

    cout << "[BEST SOLUTION] - ";
    cout << "Route: ";
    for (int i : best_route) {
        cout << i << " ";
    }
    cout << "Cost: " << best_cost << endl;
    return 0;
}
