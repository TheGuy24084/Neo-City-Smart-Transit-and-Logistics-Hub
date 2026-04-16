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

int main() {
    std::cout << "Neo-City Graph Engine starting..." << std::endl;
    
    CityGraph hub;
    hub.seedMap();
    
    // Simulate API call
    handleGetMap(hub);
    
    return 0;
}
