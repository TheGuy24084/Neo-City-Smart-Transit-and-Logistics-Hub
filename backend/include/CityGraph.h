#ifndef CITY_GRAPH_H
#define CITY_GRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <limits>
#include <list>
#include <chrono>

/**
 * @struct Car
 * @brief Represents a vehicle in the simulation.
 */
struct Car {
    int id;
    int currentIntersection;
    int targetIntersection;
    double progress; // 0.0 to 1.0 along the road
    std::string model;
};

/**
 * @struct StateSnapshot
 * @brief Captured state of the city at a specific moment.
 */
struct StateSnapshot {
    long long timestamp; // epoch ms
    std::map<int, std::pair<double, double>> carPositions; // carId -> (x, y)
    std::map<int, bool> trafficLights; // intersectionId -> isGreen
};

/**
 * @class TrafficManager
 * @brief Handles real-time traffic flow using FIFO queues.
 */
class TrafficManager {
public:
    void enqueueCar(int intersectionId, Car car) {
        intersectionQueues[intersectionId].push(car);
    }

    Car dequeueCar(int intersectionId) {
        if (intersectionQueues[intersectionId].empty()) return {-1, -1, -1, 0, ""};
        Car car = intersectionQueues[intersectionId].front();
        intersectionQueues[intersectionId].pop();
        return car;
    }

    size_t getQueueLength(int intersectionId) {
        if (intersectionQueues.find(intersectionId) == intersectionQueues.end()) return 0;
        return intersectionQueues[intersectionId].size();
    }

    const std::queue<Car>& getQueue(int intersectionId) {
        return intersectionQueues[intersectionId];
    }

private:
    std::map<int, std::queue<Car>> intersectionQueues;
};

/**
 * @class HistoryEngine
 * @brief Manages the "Time Travel" system using a Doubly Linked List.
 * 
 * HUMAN TOUCH: Why std::list (DLL) over std::vector for History?
 * 1. O(1) insertion/deletion at both ends: We constantly append new snapshots.
 * 2. Pointer Stability: Iterators remain valid even as we add/remove nodes.
 * 3. Bidirectional Navigation: Essential for "scrubbing" forward and backward
 *    smoothly through the city's state history.
 */
class HistoryEngine {
public:
    void recordState(const StateSnapshot& state) {
        history.push_back(state);
        // Keep history limited to last 100 snapshots for performance demo
        if (history.size() > 100) history.pop_front();
    }

    const std::list<StateSnapshot>& getHistory() const {
        return history;
    }

private:
    std::list<StateSnapshot> history;
};

/**
 * @struct Edge
 * @brief Represents a road connecting two intersections.
 */
struct Edge {
    int toNode;         // The ID of the target intersection.
    int weight;         // Distance or cost (e.g., meters).
    std::string streetName;
};

/**
 * @class CityGraph
 * @brief Neo-City Graph Engine handles the transit network structure.
 * 
 * HUMAN TOUCH: Why an Adjacency List?
 * We use a std::map<int, std::vector<Edge>> for our city map because urban networks are
 * inherently sparse. An Adjacency Matrix would waste O(V^2) space on non-existent roads.
 * The list format ensures we only store data for actual connections, making it more
 * space-efficient and faster to iterate over neighboring intersections.
 */
class CityGraph {
public:
    CityGraph();
    ~CityGraph();

    void addIntersection(int intersectionId);
    void addRoad(int fromNode, int toNode, int weight, const std::string& streetName);
    
    // Populates the graph with a small city district (10-15 nodes)
    void seedMap();

    // Dijkstra's Algorithm: returns sequence of node IDs
    std::vector<int> findShortestPath(int startId, int endId) const;

    // Returns the entire graph structure in a format suitable for JSON serialization
    const std::map<int, std::vector<Edge>>& getAdjacencyList() const { return adjacencyList; }

private:
    std::map<int, std::vector<Edge>> adjacencyList;
};

#endif // CITY_GRAPH_H
