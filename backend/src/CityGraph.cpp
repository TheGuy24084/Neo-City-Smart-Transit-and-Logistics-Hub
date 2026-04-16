#include "CityGraph.h"

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
