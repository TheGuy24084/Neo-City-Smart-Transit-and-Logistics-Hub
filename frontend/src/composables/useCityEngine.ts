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
    const isSearching = ref(false);
    
    // UI Interaction State (Shared with Sidebar)
    const startSearch = ref('');
    const endSearch = ref('');

    // UI Feedback
    const errorNotification = ref<string | null>(null);

    // Navigation Stack (LIFO)
    const navigationStack = ref<NavState[]>([
        { type: 'city', label: 'Neo-City Hub' }
    ]);

    // Analytics Metrics
    const metrics = reactive({
        nodesVisited: 0,
        historyBytes: 0,
        avgWaitTime: 4.2,
        systemStatus: 'OPTIMAL',
        congestionIndex: 12.4,
        fluxDelta: -1.2
    });

    const showSpecs = ref(false);
    const isStressTesting = ref(false);
    const districtSummary = ref<any>(null);

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

    // Predictive Flow Analysis (Zone 04)
    const hotZones = computed(() => {
        // Mocking congestion based on node IDs for demo purposes
        // In a real app, this would use activeQueue maps from backend
        return nodes.value
            .map(n => ({ 
                ...n, 
                intensity: Math.floor(Math.random() * 80) + 20 
            }))
            .sort((a, b) => b.intensity - a.intensity)
            .slice(0, 3);
    });

    const quickRewind = () => {
        currentSnapshotIndex.value = Math.max(0, currentSnapshotIndex.value - 5);
    };

    const getNodeCoords = (id: number) => {
        return nodes.value.find(n => n.intersectionId === id);
    };

    const clearRoute = () => {
        startNodeId.value = null;
        endNodeId.value = null;
        shortestPath.value = [];
        isSearching.value = false;
        errorNotification.value = null;
        startSearch.value = '';
        endSearch.value = '';
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
        if (current.type === state.type && (state.id !== undefined && current.id === state.id)) return;
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

    let searchTimeout: any = null;

    const findShortestPath = async () => {
        if (startNodeId.value === null || endNodeId.value === null) return;
        
        shortestPath.value = [];
        isSearching.value = true;
        errorNotification.value = null;

        if (searchTimeout) clearTimeout(searchTimeout);

        searchTimeout = setTimeout(async () => {
            try {
                if (startNodeId.value === null || endNodeId.value === null) {
                    isSearching.value = false;
                    return;
                }

                const path = await simulateBackendDijkstra(startNodeId.value, endNodeId.value);
                
                if (!path || path.length === 0) {
                    errorNotification.value = "No direct route found between these sectors.";
                } else {
                    shortestPath.value = path;
                    metrics.nodesVisited = Math.floor(path.length * 1.5 + Math.random() * 5);
                    metrics.historyBytes = historySnapshots.value.length * 1024;
                }
            } catch (error) {
                console.error("Pathfinding process failed:", error);
                errorNotification.value = "Navigation sub-system offline: check neural link.";
            } finally {
                isSearching.value = false;
            }
        }, 300);
    };

    const simulateBackendDijkstra = async (start: number, end: number) => {
        if (start === end) return [start];

        const dists: Record<number, number> = {};
        const prev: Record<number, number | null> = {};
        const unvisited = new Set<number>();

        Object.keys(cityMap.value).forEach(id => {
            const nid = Number(id);
            dists[nid] = Infinity;
            unvisited.add(nid);
        });

        if (dists[start] === undefined || dists[end] === undefined) return [];

        dists[start] = 0;

        while (unvisited.size > 0) {
            let u: number | null = null;
            for (const candidate of unvisited) {
                if (u === null || dists[candidate] < dists[u]) u = candidate;
            }

            if (u === null || dists[u] === Infinity || u === end) break;
            unvisited.delete(u);

            const neighbors = cityMap.value[u];
            for (const edge of neighbors || []) {
                const alt = dists[u] + edge.weight;
                if (alt < dists[edge.toNode]) {
                    dists[edge.toNode] = alt;
                    prev[edge.toNode] = u;
                }
            }
        }

        const path: number[] = [];
        let curr: number | null | undefined = end;
        
        if (dists[end] === Infinity) return [];

        while (curr !== null && curr !== undefined) {
            path.unshift(curr);
            if (curr === start) break;
            curr = prev[curr];
            if (path.length > nodes.value.length + 1) break; 
        }
        
        return (path.length > 0 && path[0] === start) ? path : [];
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
            const angle = (idx / nodeIds.length) * 2 * Math.PI;
            const radius = 400;
            const x = 800 + Math.cos(angle) * radius;
            const y = 380 + Math.sin(angle) * (radius * 0.82);
            newNodes.push({ intersectionId: id, x, y, name: `Node ${id}` });
        });
        return newNodes;
    };

    const handleNodeClick = (id: number) => {
        if (isSearching.value) return;

        console.log(`[CityEngine] Processing click for Node #${id}`);
        fetchTrafficStatus(id);
        pushView({ 
            type: 'intersection', 
            id, 
            label: `Intersection #${id}` 
        });

        if (startNodeId.value === null) {
            startNodeId.value = id;
            startSearch.value = `Intersection #${id}`;
        } else if (endNodeId.value === null && startNodeId.value !== id) {
            endNodeId.value = id;
            endSearch.value = `Intersection #${id}`;
            findShortestPath();
        } else if (startNodeId.value !== null && endNodeId.value !== null) {
            clearRoute();
            startNodeId.value = id;
            startSearch.value = `Intersection #${id}`;
        }
    };

    return {
        cityMap, nodes, hoverNode,
        startNodeId, endNodeId, shortestPath, isSearching,
        startSearch, endSearch,
        errorNotification, navigationStack, metrics,
        showSpecs, isStressTesting, districtSummary,
        activeQueue, historySnapshots, currentSnapshotIndex,
        isTimeTraveling, liveCars, currentSnapshotTimestamp,
        totalEdges, totalDistance, pathLines,
        hotZones, quickRewind,
        getNodeCoords, clearRoute, popTo, pushView, popView,
        runStressTest, fetchTrafficStatus, fetchDistrictSummary,
        findShortestPath, initHistory, generateLayout, handleNodeClick, initMap
    };
}
