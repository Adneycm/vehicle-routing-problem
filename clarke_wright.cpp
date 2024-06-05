#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm> // for std::find
#include <set>
#include <tuple>

using namespace std;

#define MAX_CAPACITY 15
#define MAX_CITY_VISITS 3
#define filename "grafo_10.txt"


bool contains(const std::vector<int>& vec, int value) {
    if (value == 0) {
        return false;
    }
    return find(vec.begin(), vec.end(), value) != vec.end();
}

int getSingleVisitCost(vector<tuple<int, int>>& single_visits, int node) {
    for (const auto& visit : single_visits) {
        if (get<0>(visit) == node) {
            return get<1>(visit);
        }
    }
    return 0;
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

void clarke_wright(
    int& num_nodes, vector<int>& city_index, vector<int>& demand,
    int& num_edges, map<int, vector<pair<int, int>>>& graph) {

    // Creating single city visits
    vector<tuple<int, int>> single_visits;
    for (const auto& edge : graph[0]) {
        int destination = edge.first;
        int weight = 2*edge.second;
        single_visits.push_back(make_tuple(destination, weight));
    }

    // Check for possible paths between cities without going through the warehouse
    int solution_cost = 0;
    vector<tuple<tuple<int, int>, int>> multiple_visits;
    for (int i = 1; i < city_index.size(); ++i) {
        for (const auto& edge : graph[i]) {
            int destination = edge.first;
            int weight = edge.second;

            if (destination != 0) {
                tuple<int, int> node_tuple = single_visits[i-1];
                int cost_node = get<1>(node_tuple);

                tuple<int, int> destination_tuple = single_visits[destination-1];
                int cost_destination = get<1>(destination_tuple);

                int joint_visits = cost_node/2 + weight + cost_destination/2;
                int isolated_visits = cost_node + cost_destination;

                if (joint_visits < isolated_visits) {
                    multiple_visits.push_back(make_tuple(make_tuple(i, destination), isolated_visits - joint_visits));
                    solution_cost += joint_visits;
                }
            }
        }
    }

    // Sorting the multiple visits based on the economy made by joining the paths (descending order)
    sort(multiple_visits.begin(), multiple_visits.end(),
        [](const tuple<tuple<int, int>, int>& a, const tuple<tuple<int, int>, int>& b) {
            return get<1>(a) > get<1>(b);
        });

    // Construct solution
    vector<int> solution;
    solution.push_back(0);
    for (const auto& item : multiple_visits) {
        // (node 0, node 1)
        tuple<int, int> nodes = get<0>(item);
        int cost = get<1>(item);

        int node_0 = get<0>(nodes);
        int node_1 = get<1>(nodes);


        if (!contains(solution, node_0) || !contains(solution, node_1)){
            solution.push_back(node_0);
            solution.push_back(node_1);
            solution.push_back(0);
        }
    }

    set<int> solution_set(solution.begin(), solution.end());
    for (const int& value : city_index) {
        if (solution_set.find(value) == solution_set.end()) {
            solution.push_back(value);
            solution.push_back(0);

            int cost = getSingleVisitCost(single_visits, value);
            cout << "Cost single visit " << cost << " Node " << value << endl;
            solution_cost += cost;
        }
    }


    cout << "Solution: ";
    for (const int& value : solution) {
        cout << value << " ";
    }
    cout << "Cost: " << solution_cost << endl;
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

    clarke_wright(num_nodes, city_index, demand, num_edges, graph);

    return 0;
}
