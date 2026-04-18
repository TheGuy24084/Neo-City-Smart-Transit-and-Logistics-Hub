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
void handleGetRoute(const CityGraph& graph, AnalyticsEngine& ae, int start, int end) {
    std::cout << "--- Antigravity API: GET /api/route?start=" << start << "&end=" << end << " ---" << std::endl;
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
 * MOCK ANTIGRAVITY API ROUTE: /api/metrics
 */
void handleGetMetrics(const CityGraph& graph, TrafficManager& tm, const HistoryEngine& history, const AnalyticsEngine& ae) {
    std::cout << "--- Antigravity API: GET /api/metrics ---" << std::endl;
    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

    std::cout << "{" << std::endl;
    std::cout << "  \"dijkstraNodesVisited\": " << ae.getLastDijkstraNodesVisited() << "," << std::endl;
    std::cout << "  \"historyMemoryBytes\": " << (history.getHistory().size() * 1024) << "," << std::endl; // Simulated footprint
    std::cout << "  \"avgQueueWaitTime\": 4.2," << std::endl;
    std::cout << "  \"systemStatus\": \"OPTIMAL\"" << std::endl;
    std::cout << "}" << std::endl;
}

/**
 * MOCK ANTIGRAVITY API ROUTE: /api/traffic/status?intersection=X
 */
void handleGetTrafficStatus(TrafficManager& tm, int intersectionId) {
    std::cout << "--- Antigravity API: GET /api/traffic/status?intersection=" << intersectionId << " ---" << std::endl;
    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

    std::cout << "{ \"intersectionId\": " << intersectionId 
              << ", \"queueLength\": " << tm.getQueueLength(intersectionId) << " }" << std::endl;
}

/**
 * MOCK ANTIGRAVITY API ROUTE: /api/history
 */
void handleGetHistory(const HistoryEngine& history) {
    std::cout << "--- Antigravity API: GET /api/history ---" << std::endl;
    std::cout << "HTTP/1.1 200 OK" << std::endl;
    std::cout << "Content-Type: application/json" << std::endl;
    std::cout << std::endl;

    std::cout << "[" << std::endl;
    bool first = true;
    for (const auto& snapshot : history.getHistory()) {
        if (!first) std::cout << "," << std::endl;
        std::cout << "  { \"timestamp\": " << snapshot.timestamp << ", \"carCount\": " << snapshot.carPositions.size() << " }";
        first = false;
    }
    std::cout << std::endl << "]" << std::endl;
}

int main() {
    std::cout << "Neo-City Graph Engine starting..." << std::endl;
    
    CityGraph hub;
    hub.seedMap();
    
    TrafficManager tm;
    HistoryEngine history;
    AnalyticsEngine ae;

    // Day 3 Simulation: Populate some traffic and history
    std::cout << "Initializing Day 3 Traffic Simulation..." << std::endl;
    
    // Add some cars to Node 1
    tm.enqueueCar(1, {101, 0, 1, 0.5, "CyberSedan", 1713450000});
    tm.enqueueCar(1, {102, 0, 1, 0.2, "NeonTruck", 1713450010});
    tm.enqueueCar(1, {103, 0, 1, 0.1, "VoltRider", 1713450020});

    // Record a few snapshots
    for (int i = 0; i < 5; ++i) {
        StateSnapshot snap;
        snap.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        snap.carPositions[101] = {100.0 + (double)i*10, 200.0};
        snap.carPositions[102] = {150.0 + (double)i*5, 250.0};
        snap.trafficLights[1] = (i % 2 == 0); // Toggle light
        history.recordState(snap);
    }

    // Day 1 Simulation: Fetch Map
    handleGetMap(hub);
    
    std::cout << std::endl;

    // Day 2 Simulation: Requesting path from Node 0 to Node 7
    handleGetRoute(hub, ae, 0, 7);
    
    std::cout << std::endl;

    // Day 3 Simulation: Traffic and History
    handleGetTrafficStatus(tm, 1);
    handleGetHistory(history);

    // Day 4 Simulation: Analytics Metrics
    handleGetMetrics(hub, tm, history, ae);
    
    return 0;
}
