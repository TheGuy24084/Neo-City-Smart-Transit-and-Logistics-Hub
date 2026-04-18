#include "CityGraph.h"
#include <algorithm>

CityGraph::CityGraph() {
    // Constructor logic
}

CityGraph::~CityGraph() {
    // Destructor logic
}

void CityGraph::addIntersection(int intersectionId) {
    if (adjacencyList.find(intersectionId) == adjacencyList.end()) {
        adjacencyList[intersectionId] = std::vector<Edge>();
    }
}

void CityGraph::addRoad(int fromNode, int toNode, int weight, const std::string& streetName) {
    // Ensure both nodes exist
    addIntersection(fromNode);
    addIntersection(toNode);
    
    adjacencyList[fromNode].push_back({toNode, weight, streetName});
    // Assuming undirected graph for a standard city map (bidirectional roads)
    adjacencyList[toNode].push_back({fromNode, weight, streetName});
}

void CityGraph::seedMap() {
    /**
     * Seed data for a small city district: "Neo-Sector 7"
     * Nodes: 0-11 (12 nodes)
     */
    
    // Main Arteries
    addRoad(0, 1, 100, "Neon Boulevard");
    addRoad(1, 2, 120, "Neon Boulevard");
    addRoad(2, 3, 110, "Neon Boulevard");
    
    // Residential Grid
    addRoad(0, 4, 80, "Mainframe Ave");
    addRoad(4, 5, 90, "Cyber Way");
    addRoad(5, 1, 85, "Silicon Street");
    
    // Industrial District
    addRoad(2, 6, 150, "Processing Lane");
    addRoad(6, 7, 70, "Data Stream Drive");
    addRoad(7, 3, 95, "Logic Loop");
    
    // Crossroads
    addRoad(1, 8, 200, "Central Junction");
    addRoad(8, 9, 130, "Transit Hub Express");
    addRoad(9, 10, 140, "Binary Alley");
    addRoad(10, 11, 60, "Node Connector");
    addRoad(11, 4, 180, "Skyline Bridge");
    
    // Extra connections for a more complex graph
    addRoad(5, 8, 110, "Shortcut 01");
    addRoad(9, 2, 250, "Industrial Bypass");
}

std::vector<int> CityGraph::findShortestPath(int startId, int endId, int& nodesVisited) const {
    // HUMAN TOUCH: Why Priority Queue?
    // We use std::priority_queue (Min-Heap) to always extract the node with the minimum 
    // current distance. This reduces the search complexity from O(V) to O(log V) 
    // for each extraction, leading to an overall O(E log V) efficiency, 
    // which is critical for real-time city-scale pathfinding.
    
    nodesVisited = 0;
    std::map<int, int> distances;
    std::map<int, int> previous;
    
    // Using a greater comparator to make it a Min-Heap
    typedef std::pair<int, int> NodeDist; // {distance, nodeId}
    std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<NodeDist>> pq;

    for (auto const& [nodeId, _] : adjacencyList) {
        distances[nodeId] = std::numeric_limits<int>::max();
    }

    if (distances.find(startId) == distances.end() || distances.find(endId) == distances.end()) {
        return {}; // Start or end node doesn't exist
    }

    distances[startId] = 0;
    pq.push({0, startId});

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        nodesVisited++; // [ANALYTICS] Increment each time we extract a node from PQ

        if (d > distances[u]) continue;
        if (u == endId) break; // Optimization: we found the shortest path to target

        if (adjacencyList.count(u)) {
            for (const auto& edge : adjacencyList.at(u)) {
                int v = edge.toNode;
                int weight = edge.weight;

                if (distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    previous[v] = u;
                    pq.push({distances[v], v});
                }
            }
        }
    }

    // Reconstruct Path
    std::vector<int> path;
    if (distances[endId] == std::numeric_limits<int>::max()) {
        return {}; // No path found
    }

    for (int at = endId; at != startId; at = previous[at]) {
        path.push_back(at);
        if (previous.find(at) == previous.end()) break; 
    }
    path.push_back(startId);
    
    std::reverse(path.begin(), path.end());
    return path;
}
