#include "CityGraph.h"
#include <iostream>

/**
 * MOCK API ROUTE: /api/map
 * In a real production environment, this would be hooked into a C++ web
 * framework like Crow, Droogon, or a custom internal server.
 */
void handleGetMap(const CityGraph &graph) {
  std::cout << "---  API: GET /api/map ---" << std::endl;
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
                    int end) {
  std::cout << "---  API: GET /api/route?start=" << start << "&end=" << end
            << " ---" << std::endl;
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
 * MOCK  API ROUTE: /api/metrics
 */
void handleGetMetrics(const CityGraph &graph, TrafficManager &tm,
                      const HistoryEngine &history, const AnalyticsEngine &ae) {
  std::cout << "---  API: GET /api/metrics ---" << std::endl;
  std::cout << "HTTP/1.1 200 OK" << std::endl;
  std::cout << "Content-Type: application/json" << std::endl;
  std::cout << std::endl;

  std::cout << "{" << std::endl;
  std::cout << "  \"dijkstraNodesVisited\": "
            << ae.getLastDijkstraNodesVisited() << "," << std::endl;
  std::cout << "  \"historyMemoryBytes\": "
            << (history.getHistory().size() * 1024) << ","
            << std::endl; // Simulated footprint
  std::cout << "  \"avgQueueWaitTime\": 4.2," << std::endl;
  std::cout << "  \"systemStatus\": \"OPTIMAL\"" << std::endl;
  std::cout << "}" << std::endl;
}

/**
 * MOCK  API ROUTE: /api/traffic/status?intersection=X
 */
void handleGetTrafficStatus(TrafficManager &tm, int intersectionId) {
  std::cout << "---  API: GET /api/traffic/status?intersection="
            << intersectionId << " ---" << std::endl;
  std::cout << "HTTP/1.1 200 OK" << std::endl;
  std::cout << "Content-Type: application/json" << std::endl;
  std::cout << std::endl;

  std::cout << "{ \"intersectionId\": " << intersectionId
            << ", \"queueLength\": " << tm.getQueueLength(intersectionId)
            << " }" << std::endl;
}

/**
 * MOCK  API ROUTE: /api/district/summary?start=X
 */
void handleGetDistrictSummary(const CityGraph &graph, int startNodeId) {
  std::cout << "---  API: GET /api/district/summary?start=" << startNodeId
            << " ---" << std::endl;
  std::cout << "HTTP/1.1 200 OK" << std::endl;
  std::cout << "Content-Type: application/json" << std::endl;
  std::cout << std::endl;

  DistrictSummary summary = graph.getDistrictSummary(startNodeId);

  std::cout << "{" << std::endl;
  std::cout << "  \"totalNodes\": " << summary.totalNodes << "," << std::endl;
  std::cout << "  \"totalEdges\": " << summary.totalEdges << "," << std::endl;
  std::cout << "  \"streets\": [";
  for (size_t i = 0; i < summary.streets.size(); ++i) {
    std::cout << "\"" << summary.streets[i] << "\""
              << (i == summary.streets.size() - 1 ? "" : ", ");
  }
  std::cout << "]" << std::endl;
  std::cout << "}" << std::endl;
}

/**
 * MOCK  API ROUTE: /api/history
 */
void handleGetHistory(const HistoryEngine &history) {
  std::cout << "---  API: GET /api/history ---" << std::endl;
  std::cout << "HTTP/1.1 200 OK" << std::endl;
  std::cout << "Content-Type: application/json" << std::endl;
  std::cout << std::endl;

  std::cout << "[" << std::endl;
  bool first = true;
  for (const auto &snapshot : history.getHistory()) {
    if (!first)
      std::cout << "," << std::endl;
    std::cout << "  { \"timestamp\": " << snapshot.timestamp
              << ", \"carCount\": " << snapshot.carPositions.size() << " }";
    first = false;
  }
  std::cout << std::endl << "]" << std::endl;
}

int main() {
  std::cout << "Neo-City Graph Engine (v5.0 Final) starting..." << std::endl;

  // Using std::unique_ptr for managed lifecycle (no raw pointer leaks)
  auto hub = std::make_unique<CityGraph>();
  hub->seedMap();

  TrafficManager tm;
  HistoryEngine history;
  AnalyticsEngine ae;

  std::cout << "Day 5: Concurrent System Ready." << std::endl;

  // Day 1-4 Simulations...
  handleGetMap(*hub);
  handleGetRoute(*hub, ae, 0, 7);
  handleGetMetrics(*hub, tm, history, ae);

  // Day 5 New Feature: District Summary
  handleGetDistrictSummary(*hub, 1);

  return 0;
}
