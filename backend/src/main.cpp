#include <iostream>
#include "CityGraph.h"

/**
 * MOCK ANTIGRAVITY API ROUTE: /api/map
 * In a real production environment, this would be hooked into a C++ web framework
 * like Crow, Droogon, or a custom internal server.
 */
void handleGetMap(const CityGraph& graph) {
    std::cout << "--- Antigravity API: GET /api/map ---" << std::endl;
    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;
    
    // Manual JSON serialization for demonstration
    std::cout << "{" << std::endl;
    const auto& adj = graph.getAdjacencyList();
    bool firstNode = true;
    for (auto const& [nodeId, edges] : adj) {
        if (!firstNode) std::cout << "," << std::endl;
        std::cout << "  \"" << nodeId << "\": [" << std::endl;
        
        bool firstEdge = true;
        for (const auto& edge : edges) {
            if (!firstEdge) std::cout << "," << std::endl;
            std::cout << "    { \"toNode\": " << edge.toNode 
                      << ", \"weight\": " << edge.weight 
                      << ", \"streetName\": \"" << edge.streetName << "\" }";
            firstEdge = false;
        }
        std::cout << std::endl << "  ]";
        firstNode = false;
    }
    std::cout << std::endl << "}" << std::endl;
}

/**
 * MOCK ANTIGRAVITY API ROUTE: /api/route?start=X&end=Y
 */
void handleGetRoute(const CityGraph& graph, int start, int end) {
    std::cout << "--- Antigravity API: GET /api/route?start=" << start << "&end=" << end << " ---" << std::endl;
    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

    std::vector<int> path = graph.findShortestPath(start, end);
    
    std::cout << "[";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i] << (i == path.size() - 1 ? "" : ", ");
    }
    std::cout << "]" << std::endl;
}

int main() {
    std::cout << "Neo-City Graph Engine starting..." << std::endl;
    
    CityGraph hub;
    hub.seedMap();
    
    // Day 1 Simulation: Fetch Map
    handleGetMap(hub);
    
    std::cout << std::endl;

    // Day 2 Simulation: Requesting path from Node 0 to Node 7
    handleGetRoute(hub, 0, 7);
    
    return 0;
}
