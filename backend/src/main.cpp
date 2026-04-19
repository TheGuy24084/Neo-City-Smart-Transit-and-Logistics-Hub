#include "CityGraph.h"
#include "AuthManager.h"
#include <iostream>

/**
 * @brief Helper to check authentication and return 401 if invalid.
 */
bool checkAuth(const std::string& token) {
    if (!AuthManager::getInstance().isTokenValid(token)) {
        std::cout << "HTTP/1.1 401 Unauthorized" << std::endl;
        std::cout << "Content-Type: application/json" << std::endl;
        std::cout << std::endl;
        std::cout << "{ \"error\": \"UNAUTHORIZED\", \"message\": \"INVALID_OR_MISSING_TOKEN\" }" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Helper to check Admin role and return 403 if insufficient.
 */
bool checkAdmin(const std::string& token) {
    if (!checkAuth(token)) return false;
    if (AuthManager::getInstance().getCurrentRole() != "Admin") {
        std::cout << "HTTP/1.1 403 Forbidden" << std::endl;
        std::cout << "Content-Type: application/json" << std::endl;
        std::cout << std::endl;
        std::cout << "{ \"error\": \"FORBIDDEN\", \"message\": \"ADMIN_PRIVILEGES_REQUIRED\" }" << std::endl;
        return false;
    }
    return true;
}

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
 * RBAC: All Authenticated Users
 */
void handleGetMap(const CityGraph &graph, const std::string& token) {
    std::cout << "---  API: GET /api/map ---" << std::endl;
    if (!checkAuth(token)) return;

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
 * RBAC: All Authenticated Users
 */
void handleGetRoute(const CityGraph &graph, AnalyticsEngine &ae, int start,
                    int end, const std::string& token) {
    std::cout << "---  API: GET /api/route ---" << std::endl;
    if (!checkAuth(token)) return;

    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
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
 * MOCK API ROUTE: /api/metrics
 * RBAC: Admin Only
 */
void handleGetMetrics(const CityGraph &graph, TrafficManager &tm,
                      const HistoryEngine &history, const AnalyticsEngine &ae, 
                      const std::string& token) {
    std::cout << "---  API: GET /api/metrics ---" << std::endl;
    if (!checkAdmin(token)) return;

    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

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

/**
 * MOCK API ROUTE: /api/analytics/flux
 * RBAC: Admin Only
 * predictive congestion analysis
 */
void handleGetTemporalFlux(const CityGraph &graph, TrafficManager &tm, 
                           AnalyticsEngine &ae, const std::string& token) {
    std::cout << "--- API: GET /api/analytics/flux ---" << std::endl;
    if (!checkAdmin(token)) return;

    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

    std::cout << "{" << std::endl;
    std::cout << "  \"congestionIndex\": " << ae.getCongestionIndex(tm, graph) << "," << std::endl;
    std::cout << "  \"fluxDelta\": -2.4," << std::endl; 
    std::cout << "  \"systemPulse\": [12, 45, 32, 67, 88, 54, 30]," << std::endl;
    std::cout << "  \"predictionModel\": \"Temporal-RNN-v2\"" << std::endl;
    std::cout << "}" << std::endl;
}

int main() {
    std::cout << "Neo-City Graph Engine (v5.2 Secure) starting..." << std::endl;

    auto hub = std::make_unique<CityGraph>();
    hub->seedMap();

    TrafficManager tm;
    HistoryEngine history;
    AnalyticsEngine ae;

    AuthManager& auth = AuthManager::getInstance();

    std::cout << "\n[SCENARIO 1: Admin Access]" << std::endl;
    handleLogin("admin", "neocity2026");
    std::string adminToken = auth.getSessionToken();
    handleGetMap(*hub, adminToken);
    handleGetMetrics(*hub, tm, history, ae, adminToken);
    handleGetTemporalFlux(*hub, tm, ae, adminToken);
    auth.logout();

    std::cout << "\n[SCENARIO 2: Guest Access]" << std::endl;
    auth.guestLogin();
    std::string guestToken = auth.getSessionToken();
    handleGetMap(*hub, guestToken);
    handleGetRoute(*hub, ae, 0, 7, guestToken);
    handleGetMetrics(*hub, tm, history, ae, guestToken); // Should fail with 403
    handleGetTemporalFlux(*hub, tm, ae, guestToken);   // Should fail with 403
    auth.logout();

    std::cout << "\n[SCENARIO 3: Invalid Access]" << std::endl;
    handleGetMap(*hub, "INVALID_TOKEN_TEST"); // Should fail with 401

    return 0;
}

