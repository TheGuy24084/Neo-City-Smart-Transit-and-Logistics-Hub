#include "CityGraph.h"
#include "AuthManager.h"
#include <iostream>

/**
 * MOCK API ROUTE: /api/auth/login
 */
void handleLogin(const std::string& username, const std::string& password) {
    std::cout << "---  API: POST /api/auth/login ---" << std::endl;
    AuthManager& auth = AuthManager::getInstance();
    
    if (auth.verifyUser(username, password)) {
        std::cout << "HTTP/1.1 200 OK" << std::endl;
        std::cout << "Content-Type: application/json" << std::endl;
        std::cout << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "  \"sessionToken\": \"" << auth.getSessionToken() << "\"," << std::endl;
        std::cout << "  \"userRole\": \"" << auth.getCurrentRole() << "\"," << std::endl;
        std::cout << "  \"message\": \"AUTHENTICATION_SUCCESSFUL\"" << std::endl;
        std::cout << "}" << std::endl;
    } else {
        std::cout << "HTTP/1.1 401 Unauthorized" << std::endl;
        std::cout << "Content-Type: application/json" << std::endl;
        std::cout << std::endl;
        std::cout << "{ \"error\": \"ACCESS_DENIED\", \"message\": \"INVALID_CREDENTIALS\" }" << std::endl;
    }
}

/**
 * MOCK API ROUTE: /api/map
 */
void handleGetMap(const CityGraph &graph, const std::string& token) {
    std::cout << "---  API: GET /api/map ---" << std::endl;
    if (!AuthManager::getInstance().isTokenValid(token)) {
        std::cout << "HTTP/1.1 403 Forbidden" << std::endl;
        return;
    }

    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

    // Manual JSON serialization for demonstration
    std::cout << "{" << std::endl;
    const auto &adj = graph.getAdjacencyList();
    bool firstNode = true;
    for (auto const &[nodeId, edges] : adj) {
        if (!firstNode)
            std::cout << "," << std::endl;
        std::cout << "  \"" << nodeId << "\": [" << std::endl;

        bool firstEdge = true;
        for (const auto &edge : edges) {
            if (!firstEdge)
                std::cout << "," << std::endl;
            std::cout << "    { \"toNode\": " << edge.toNode
                      << ", \"weight\": " << edge.weight << ", \"streetName\": \""
                      << edge.streetName << "\" }";
            firstEdge = false;
        }
        std::cout << std::endl << "  ]";
        firstNode = false;
    }
    std::cout << std::endl << "}" << std::endl;
}

/**
 * MOCK API ROUTE: /api/route?start=X&end=Y
 */
void handleGetRoute(const CityGraph &graph, AnalyticsEngine &ae, int start,
                    int end, const std::string& token) {
    std::cout << "---  API: GET /api/route ---" << std::endl;
    if (!AuthManager::getInstance().isTokenValid(token)) {
        std::cout << "HTTP/1.1 403 Forbidden" << std::endl;
        return;
    }

    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << std::endl;

    int nodesVisited = 0;
    std::vector<int> path = graph.findShortestPath(start, end, nodesVisited);
    ae.trackDijkstra(nodesVisited);

    std::cout << "[";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i] << (i == path.size() - 1 ? "" : ", ");
    }
    std::cout << "]" << std::endl;
}

/**
 * MOCK  API ROUTE: /api/metrics
 */
void handleGetMetrics(const CityGraph &graph, TrafficManager &tm,
                      const HistoryEngine &history, const AnalyticsEngine &ae, 
                      const std::string& token) {
    std::cout << "---  API: GET /api/metrics ---" << std::endl;
    AuthManager& auth = AuthManager::getInstance();
    
    if (!auth.isTokenValid(token)) {
        std::cout << "HTTP/1.1 403 Forbidden" << std::endl;
        return;
    }

    // Role-based verification
    if (auth.getCurrentRole() == "GUEST") {
        std::cout << "HTTP/1.1 401 Unauthorized [GUEST_RESTRICTION]" << std::endl;
        std::cout << "{ \"error\": \"INSUFFICIENT_PRIVILEGES\", \"required\": \"ADMIN\" }" << std::endl;
        return;
    }

    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << std::endl;
    // ... logic ...

    std::cout << "{" << std::endl;
    std::cout << "  \"dijkstraNodesVisited\": "
              << ae.getLastDijkstraNodesVisited() << "," << std::endl;
    std::cout << "  \"historyMemoryBytes\": "
              << (history.getHistory().size() * 1024) << ","
              << std::endl; 
    std::cout << "  \"avgQueueWaitTime\": 4.2," << std::endl;
    std::cout << "  \"systemStatus\": \"OPTIMAL\"" << std::endl;
    std::cout << "}" << std::endl;
}

int main() {
    std::cout << "Neo-City Graph Engine (v5.1 Auth) starting..." << std::endl;

    auto hub = std::make_unique<CityGraph>();
    hub->seedMap();

    TrafficManager tm;
    HistoryEngine history;
    AnalyticsEngine ae;

    std::cout << "Checking Authentication Subsystem..." << std::endl;
    
    // Simulate Login Attempt
    handleLogin("admin", "neocity2026");
    std::string validToken = AuthManager::getInstance().getSessionToken();

    // Day 1-4 Simulations with Auth...
    handleGetMap(*hub, validToken);
    handleGetRoute(*hub, ae, 0, 7, validToken);
    handleGetMetrics(*hub, tm, history, ae, validToken);

    // Simulate Access with Invalid Token
    handleGetMap(*hub, "MALICIOUS_TOKEN_XYZ");

    return 0;
}
