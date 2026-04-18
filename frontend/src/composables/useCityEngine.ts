import { ref, reactive, computed, watch } from 'vue';
import type { CityMap, Node, NavState } from '../types/map';
import type { StateSnapshot, QueueStatus } from '../types/traffic';

export function useCityEngine() {
    const cityMap = ref<CityMap>({});
    const nodes = ref<Node[]>([]);
    const hoverNode = ref<number | null>(null);

    // Routing State
    const startNodeId = ref<number | null>(null);
    const endNodeId = ref<number | null>(null);
    const shortestPath = ref<number[]>([]);

    // Navigation Stack (LIFO)
    const navigationStack = ref<NavState[]>([
        { type: 'city', label: 'Neo-Sector 07' }
    ]);

    // Analytics Metrics
    const metrics = reactive({
        nodesVisited: 0,
        historyBytes: 0,
        avgWaitTime: 4.2,
        systemStatus: 'OPTIMAL'
    });

    // Day 5 Features
    const showSpecs = ref(false);
    const isStressTesting = ref(false);
    const districtSummary = ref<any>(null);

    // Traffic & History State
    const activeQueue = ref<QueueStatus | null>(null);
    const historySnapshots = ref<StateSnapshot[]>([]);
    const currentSnapshotIndex = ref(0);
    const isTimeTraveling = computed(() => currentSnapshotIndex.value < historySnapshots.value.length - 1);

    const liveCars = ref<Record<number, { x: number, y: number }>>({});
    
    const currentSnapshotTimestamp = computed(() => {
        if (!historySnapshots.value.length) return "N/A";
        const date = new Date(historySnapshots.value[currentSnapshotIndex.value].timestamp);
        return date.toLocaleString('en-US', { hour12: false });
    });

    const totalEdges = computed(() => {
        return Object.values(cityMap.value).reduce((acc, curr: any) => acc + curr.length, 0) / 2;
    });

    const totalDistance = computed(() => {
        if (shortestPath.value.length < 2) return 0;
        let dist = 0;
        for (let i = 0; i < shortestPath.value.length - 1; i++) {
            const from = shortestPath.value[i];
            const to = shortestPath.value[i+1];
            const edge = cityMap.value[from]?.find(e => e.toNode === to);
            if (edge) dist += edge.weight;
        }
        return dist;
    });

    const pathLines = computed(() => {
        if (shortestPath.value.length < 2) return [];
        const lines = [];
        for (let i = 0; i < shortestPath.value.length - 1; i++) {
            const u = getNodeCoords(shortestPath.value[i]);
            const v = getNodeCoords(shortestPath.value[i+1]);
            if (u && v) {
                lines.push({ x1: u.x, y1: u.y, x2: v.x, y2: v.y });
            }
        }
        return lines;
    });

    const getNodeCoords = (id: number) => {
        return nodes.value.find(n => n.intersectionId === id);
    };

    const clearRoute = () => {
        startNodeId.value = null;
        endNodeId.value = null;
        shortestPath.value = [];
    };

    const popTo = (index: number) => {
        navigationStack.value = navigationStack.value.slice(0, index + 1);
        if (index === 0) {
            clearRoute();
            activeQueue.value = null;
        }
    };

    const pushView = (state: NavState) => {
        const current = navigationStack.value[navigationStack.value.length - 1];
        if (current.type === state.type && current.id === state.id) return;
        if (navigationStack.value.length < 5) {
            navigationStack.value.push(state);
        }
    };

    const popView = () => {
        if (navigationStack.value.length > 1) {
            navigationStack.value.pop();
            const current = navigationStack.value[navigationStack.value.length - 1];
            if (current.type === 'city') {
                clearRoute();
                activeQueue.value = null;
            }
        }
    };

    watch(currentSnapshotIndex, (newIdx) => {
        if (historySnapshots.value[newIdx]) {
            liveCars.value = historySnapshots.value[newIdx].carPositions;
        }
    });

    const runStressTest = () => {
        isStressTesting.value = true;
        let count = 0;
        const interval = setInterval(() => {
            const id = 500 + count;
            const node = nodes.value[Math.floor(Math.random() * nodes.value.length)];
            if (node) {
                liveCars.value[id] = { 
                    x: node.x + (Math.random() - 0.5) * 60, 
                    y: node.y + (Math.random() - 0.5) * 60
                };
            }
            count++;
            if (count >= 50) {
                clearInterval(interval);
                setTimeout(() => {
                    isStressTesting.value = false;
                }, 5000);
            }
        }, 100);
    };

    const fetchTrafficStatus = async (id: number) => {
        try {
            activeQueue.value = {
                intersectionId: id,
                queueLength: 3,
                cars: [
                    { id: 101, currentIntersection: 0, targetIntersection: id, progress: 0.9, model: "CyberSedan" },
                    { id: 102, currentIntersection: 0, targetIntersection: id, progress: 0.7, model: "NeonTruck" },
                    { id: 103, currentIntersection: 0, targetIntersection: id, progress: 0.5, model: "VoltRider" }
                ]
            };
            fetchDistrictSummary(id);
        } catch (error) {
            console.error("Traffic fetch error:", error);
        }
    };

    const fetchDistrictSummary = async (_id: number) => {
        try {
            districtSummary.value = {
                totalNodes: nodes.value.length,
                totalEdges: totalEdges.value,
                streets: ["Neon Blvd", "Mainframe Ave", "Cyber Way", "Silicon St", "Logic Loop"]
            };
        } catch (error) {
            console.error("DFS summary error:", error);
        }
    };

    const findShortestPath = async () => {
        if (startNodeId.value === null || endNodeId.value === null) return;
        try {
            // MOCK LOGIC for Day 4 Demo: Update metrics
            const path = await simulateBackendDijkstra(startNodeId.value, endNodeId.value);
            shortestPath.value = path;
            metrics.nodesVisited = Math.floor(path.length * 1.5 + Math.random() * 5);
            metrics.historyBytes = historySnapshots.value.length * 1024;
        } catch (error) {
            console.error("Pathfinding error:", error);
        }
    };

    const simulateBackendDijkstra = async (start: number, end: number) => {
        const dists: Record<number, number> = {};
        const prev: Record<number, number | null> = {};
        const unvisited = new Set<number>();

        Object.keys(cityMap.value).forEach(id => {
            dists[Number(id)] = Infinity;
            unvisited.add(Number(id));
        });

        dists[start] = 0;

        while (unvisited.size > 0) {
            let u: number | null = null;
            for (const candidate of unvisited) {
                if (u === null || dists[candidate] < dists[u]) u = candidate;
            }

            if (u === null || dists[u] === Infinity || u === end) break;
            unvisited.delete(u);

            for (const edge of cityMap.value[u] || []) {
                const alt = dists[u] + edge.weight;
                if (alt < dists[edge.toNode]) {
                    dists[edge.toNode] = alt;
                    prev[edge.toNode] = u;
                }
            }
        }

        const path = [];
        let curr: number | null = end;
        while (curr !== null) {
            path.unshift(curr);
            curr = prev[curr];
        }
        return path[0] === start ? path : [];
    };

    const initHistory = () => {
        const snapshots: StateSnapshot[] = [];
        const baseTime = Date.now() - 100000;
        for (let i = 0; i < 50; i++) {
            const positions: Record<number, { x: number, y: number }> = {};
            [101, 102, 103].forEach(carId => {
                const node = nodes.value[Math.floor(Math.random() * nodes.value.length)];
                if (node) {
                    positions[carId] = { 
                        x: node.x + (Math.random() - 0.5) * 40, 
                        y: node.y + (Math.random() - 0.5) * 40 
                    };
                }
            });
            snapshots.push({
                timestamp: baseTime + i * 2000,
                carPositions: positions,
                trafficLights: {},
                carCount: 3
            });
        }
        historySnapshots.value = snapshots;
        currentSnapshotIndex.value = snapshots.length - 1;
    };

    const initMap = () => {
        const backendMap: CityMap = {
            "0": [{ "toNode": 1, "weight": 100, "streetName": "Neon Boulevard" }, { "toNode": 4, "weight": 150, "streetName": "Industrial Bypass" }],
            "1": [{ "toNode": 0, "weight": 100, "streetName": "Neon Boulevard" }, { "toNode": 2, "weight": 80, "streetName": "Central Junction" }, { "toNode": 5, "weight": 120, "streetName": "Processing Lane" }],
            "2": [{ "toNode": 1, "weight": 80, "streetName": "Central Junction" }, { "toNode": 3, "weight": 90, "streetName": "Binary Alley" }, { "toNode": 6, "weight": 110, "streetName": "Data Stream Drive" }],
            "3": [{ "toNode": 2, "weight": 90, "streetName": "Binary Alley" }, { "toNode": 7, "weight": 130, "streetName": "Shortcut 01" }],
            "4": [{ "toNode": 0, "weight": 150, "streetName": "Industrial Bypass" }, { "toNode": 5, "weight": 70, "streetName": "Logic Loop" }, { "toNode": 11, "weight": 180, "streetName": "Skyline Bridge" }],
            "5": [{ "toNode": 1, "weight": 120, "streetName": "Processing Lane" }, { "toNode": 4, "weight": 70, "streetName": "Logic Loop" }, { "toNode": 6, "weight": 100, "streetName": "Central Junction" }, { "toNode": 8, "weight": 90, "streetName": "Silicon Street" }],
            "6": [{ "toNode": 2, "weight": 110, "streetName": "Data Stream Drive" }, { "toNode": 5, "weight": 100, "streetName": "Central Junction" }, { "toNode": 7, "weight": 85, "streetName": "Shortcut 01" }, { "toNode": 9, "weight": 115, "streetName": "Cyber Way" }],
            "7": [{ "toNode": 3, "weight": 130, "streetName": "Shortcut 01" }, { "toNode": 6, "weight": 85, "streetName": "Shortcut 01" }, { "toNode": 10, "weight": 140, "streetName": "Transit Hub Express" }],
            "8": [{ "toNode": 5, "weight": 90, "streetName": "Silicon Street" }, { "toNode": 9, "weight": 100, "streetName": "Mainframe Ave" }],
            "9": [{ "toNode": 6, "weight": 115, "streetName": "Cyber Way" }, { "toNode": 8, "weight": 100, "streetName": "Mainframe Ave" }, { "toNode": 10, "weight": 75, "streetName": "Logic Loop" }],
            "10": [{ "toNode": 9, "weight": 75, "streetName": "Logic Loop" }, { "toNode": 7, "weight": 140, "streetName": "Transit Hub Express" }, { "toNode": 11, "weight": 60, "streetName": "Node Connector" }],
            "11": [{ "toNode": 10, "weight": 60, "streetName": "Node Connector" }, { "toNode": 4, "weight": 180, "streetName": "Skyline Bridge" }]
        };
        cityMap.value = backendMap;
        nodes.value = generateLayout(backendMap);
        metrics.nodesVisited = 13;
        metrics.historyBytes = 42 * 1024;
    };

    const generateLayout = (map: CityMap) => {
        const newNodes: Node[] = [];
        const nodeIds = Object.keys(map).map(Number).sort((a, b) => a - b);
        
        nodeIds.forEach((id, idx) => {
            // Layout nodes in a organic, circular flow for Neo-City aesthetic
            const angle = (idx / nodeIds.length) * 2 * Math.PI;
            const radius = 400; // Adjusted for 1600x1000 viewBox safety
            const x = 800 + Math.cos(angle) * radius;
            const y = 500 + Math.sin(angle) * (radius * 0.82);
            newNodes.push({ intersectionId: id, x, y, name: `Node ${id}` });
        });
        return newNodes;
    };

    const handleNodeClick = (id: number) => {
        console.log(`[CityEngine] Processing click for Node #${id}`);
        fetchTrafficStatus(id);
        pushView({ 
            type: 'intersection', 
            id, 
            label: `Intersection #${id}` 
        });

        if (startNodeId.value === null) {
            startNodeId.value = id;
        } else if (endNodeId.value === null && startNodeId.value !== id) {
            endNodeId.value = id;
            findShortestPath();
        } else if (startNodeId.value !== null && endNodeId.value !== null) {
            clearRoute();
            startNodeId.value = id;
        }
    };

    return {
        cityMap, nodes, hoverNode,
        startNodeId, endNodeId, shortestPath,
        navigationStack, metrics,
        showSpecs, isStressTesting, districtSummary,
        activeQueue, historySnapshots, currentSnapshotIndex,
        isTimeTraveling, liveCars, currentSnapshotTimestamp,
        totalEdges, totalDistance, pathLines,
        getNodeCoords, clearRoute, popTo, pushView, popView,
        runStressTest, fetchTrafficStatus, fetchDistrictSummary,
        findShortestPath, initHistory, generateLayout, handleNodeClick, initMap
    };
}
