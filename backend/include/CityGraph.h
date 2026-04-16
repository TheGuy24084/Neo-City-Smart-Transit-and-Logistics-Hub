#ifndef CITY_GRAPH_H
#define CITY_GRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <string>

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

    // Returns the entire graph structure in a format suitable for JSON serialization
    const std::map<int, std::vector<Edge>>& getAdjacencyList() const { return adjacencyList; }

private:
    std::map<int, std::vector<Edge>> adjacencyList;
};

#endif // CITY_GRAPH_H
