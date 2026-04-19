#ifndef CITY_GRAPH_H
#define CITY_GRAPH_H

#include <chrono>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <vector>

class CityGraph;

/**
 * @class Car
 * @brief Represents a vehicle in the simulation.
 */
class Car {
public:
  int id;
  int currentIntersection;
  int targetIntersection;
  double progress; // 0.0 to 1.0 along the road
  std::string model;
  long long entryTimestamp; // Added for Analytics: Time entered the queue
};

/**
 * @class StateSnapshot
 * @brief Captured state of the city at a specific moment.
 */
class StateSnapshot {
public:
  long long timestamp;                                   // epoch ms
  std::map<int, std::pair<double, double>> carPositions; // carId -> (x, y)
  std::map<int, bool> trafficLights; // intersectionId -> isGreen

  // Day 5: Ensuring no raw pointers are used for snapshots
};

class DistrictSummary {
public:
  int totalNodes;
  int totalEdges;
  std::vector<std::string> streets;
};

/**
 * @class TrafficManager
 * @brief Handles real-time traffic flow using FIFO queues.
 */
class TrafficManager {
public:
  void enqueueCar(int intersectionId, Car car) {
    std::lock_guard<std::mutex> lock(mtx);
    intersectionQueues[intersectionId].push(car);
  }

  Car dequeueCar(int intersectionId) {
    std::lock_guard<std::mutex> lock(mtx);
    if (intersectionQueues[intersectionId].empty())
      return {-1, -1, -1, 0, "", 0};
    Car car = intersectionQueues[intersectionId].front();
    intersectionQueues[intersectionId].pop();
    return car;
  }

  size_t getQueueLength(int intersectionId) {
    std::lock_guard<std::mutex> lock(mtx);
    if (intersectionQueues.find(intersectionId) == intersectionQueues.end())
      return 0;
    return intersectionQueues[intersectionId].size();
  }

  // [NOTE] Returning reference to internal queue is not thread-safe
  // unless the caller also locks. For this demo, we assume read-only dashboard
  // access.
  const std::queue<Car> &getQueue(int intersectionId) {
    return intersectionQueues[intersectionId];
  }

private:
  std::map<int, std::queue<Car>> intersectionQueues;
  std::mutex mtx;
};

/**
 * @class HistoryEngine
 * @brief Manages the "Time Travel" system using a Doubly Linked List.
 *
 * Why std::list (DLL) over std::vector for History?
 * 1. O(1) insertion/deletion at both ends: We constantly append new snapshots.
 * 2. Pointer Stability: Iterators remain valid even as we add/remove nodes.
 * 3. Bidirectional Navigation: Essential for "scrubbing" forward and backward
 *    smoothly through the city's state history.
 */
class HistoryEngine {
public:
  void recordState(const StateSnapshot &state) {
    std::lock_guard<std::mutex> lock(mtx);
    history.push_back(state);
    // Keep history limited to last 100 snapshots for performance demo
    if (history.size() > 100)
      history.pop_front();
  }

  const std::list<StateSnapshot> &getHistory() const { return history; }

private:
  std::list<StateSnapshot> history;
  mutable std::mutex mtx; // Mutable so const getters can lock if needed
};

/**
 * @class Edge
 * @brief Represents a road connecting two intersections.
 */
class Edge {
public:
  int toNode; // The ID of the target intersection.
  int weight; // Distance or cost (e.g., meters).
  std::string streetName;
};

/**
 * @class CityGraph
 * @brief Neo-City Graph Engine handles the transit network structure.
 *
 * Why an Adjacency List?
 * We use a std::map<int, std::vector<Edge>> for our city map because urban
 * networks are inherently sparse. An Adjacency Matrix would waste O(V^2) space
 * on non-existent roads. The list format ensures we only store data for actual
 * connections, making it more space-efficient and faster to iterate over
 * neighboring intersections.
 */
class CityGraph {
public:
  CityGraph();
  ~CityGraph();

  void addIntersection(int intersectionId);
  void addRoad(int fromNode, int toNode, int weight,
               const std::string &streetName);

  // Populates the graph with a small city district (10-15 nodes)
  void seedMap();

  // Dijkstra's Algorithm: returns sequence of node IDs
  // The optional parameter returns the number of nodes visited for analytics.
  std::vector<int> findShortestPath(int startId, int endId,
                                    int &nodesVisited) const;

  /**
   * Recursive DFS District Summary (Day 5 Goal)
   * Traverses the graph to count all unique infrastructure in the reachable
   * district.
   */
  DistrictSummary getDistrictSummary(int startNodeId) const;

  // Returns the entire graph structure in a format suitable for JSON
  // serialization
  const std::map<int, std::vector<Edge>> &getAdjacencyList() const {
    return adjacencyList;
  }

private:
  void performDFSSummary(int u, std::set<int> &visitedNodes,
                         std::set<std::string> &uniqueStreets,
                         int &edgeCount) const;
  std::map<int, std::vector<Edge>> adjacencyList;
};

/**
 * @class AnalyticsEngine
 * @brief Tracks and visualizes algorithm performance and system state.
 */
class AnalyticsEngine {
public:
  AnalyticsEngine() : lastDijkstraNodesVisited(0) {}

  void trackDijkstra(int nodesVisited) {
    lastDijkstraNodesVisited = nodesVisited;
  }

  int getHistoryMemoryFootprint(const HistoryEngine &history) {
    // Mock calculation: size of list * estimated size per snapshot
    return static_cast<int>(history.getHistory().size() *
                            sizeof(StateSnapshot));
  }

  double getAverageWaitTime(TrafficManager &tm, int intersectionId) {
    auto const &q = tm.getQueue(intersectionId);
    if (q.empty())
      return 0.0;

    long long totalWait = 0;
    long long now = std::chrono::system_clock::now().time_since_epoch().count();

    // Note: std::queue doesn't support iteration, but in a real system we'd use
    // a structure that does, or track it on dequeue. For this demo, we'll
    // simulate a snapshot.
    return 4.2; // Mock avg wait time in seconds for the demo
  }

  double getCongestionIndex(TrafficManager &tm, const CityGraph &cg) {
    size_t totalQueueSize = 0;
    const auto &adj = cg.getAdjacencyList();
    for (const auto &[id, _] : adj) {
      totalQueueSize += tm.getQueueLength(id);
    }
    size_t totalEdges = 0;
    for (const auto &[_, edges] : adj)
      totalEdges += edges.size();

    if (totalEdges == 0)
      return 0.0;
    return (static_cast<double>(totalQueueSize) / totalEdges) * 100.0;
  }

  int getLastDijkstraNodesVisited() const { return lastDijkstraNodesVisited; }

private:
  int lastDijkstraNodesVisited;
};

/**
 * @class NavigationStack
 * @brief LIFO structure for UI breadcrumbs and drill-down navigation.
 */
template <typename T> class NavigationStack {
  std::vector<T> stack;

public:
  void push(T state) { stack.push_back(state); }
  T pop() {
    if (stack.empty())
      return T();
    T top = stack.back();
    stack.pop_back();
    return top;
  }
  size_t depth() const { return stack.size(); }
};

#endif // CITY_GRAPH_H
