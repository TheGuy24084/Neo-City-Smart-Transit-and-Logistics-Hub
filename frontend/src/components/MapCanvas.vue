<template>
  <div class="w-full h-screen bg-slate-950 flex flex-col items-center justify-center overflow-hidden font-sans selection:bg-cyan-500/30">
    <!-- Header with Cyberpunk styling -->
    <header class="absolute top-0 w-full p-6 flex justify-between items-center z-10 bg-slate-950/80 backdrop-blur-md border-b border-cyan-500/20">
      <div class="flex flex-col">
        <h1 class="text-3xl font-black tracking-tighter text-cyan-400 uppercase italic">
          Neo-Sector <span class="text-white text-4xl">07</span>
        </h1>
        <p class="text-[10px] text-cyan-500/60 font-mono tracking-widest uppercase">Smart Transit Hub // Route Intelligence v2.0</p>
      </div>
      <div class="flex gap-4 items-center">
        <div v-if="startNodeId !== null && endNodeId === null" class="animate-pulse flex items-center gap-2">
            <div class="w-2 h-2 rounded-full bg-red-500"></div>
            <span class="text-[10px] font-mono text-red-400 uppercase">Awaiting Destination...</span>
        </div>
        <button 
            v-if="shortestPath.length > 0"
            @click="clearRoute"
            class="px-4 py-2 bg-red-500/10 hover:bg-red-500/20 border border-red-500/40 text-red-400 text-xs font-bold uppercase tracking-widest transition-all clip-path-polygon"
        >
            Clear Route
        </button>
        <div class="px-4 py-2 bg-cyan-500/10 border border-cyan-500/30 rounded-sm">
          <span class="text-xs font-mono text-cyan-400 uppercase">System: Online</span>
        </div>
      </div>
    </header>
    
    <!-- Time Travel Indicator -->
    <div 
        v-if="isTimeTraveling"
        class="absolute top-24 left-1/2 -translate-x-1/2 z-30 px-6 py-2 bg-amber-500/90 backdrop-blur-md border border-amber-400 shadow-[0_0_20px_rgba(245,158,11,0.4)] flex items-center gap-3 animate-bounce"
    >
        <div class="w-3 h-3 rounded-full bg-slate-900 animate-pulse"></div>
        <span class="text-xs font-black text-slate-900 uppercase tracking-[0.2em]">Time Travel Active // Historical Data</span>
    </div>

    <!-- Live Traffic Control Panel -->
    <section class="absolute left-8 top-32 w-80 z-20 flex flex-col gap-4">
        <div class="bg-slate-900/90 border border-emerald-500/30 p-6 backdrop-blur-xl relative overflow-hidden">
            <div class="absolute top-0 left-0 w-1 h-full bg-emerald-500 shadow-[0_0_15px_rgba(52,211,153,0.5)]"></div>
            
            <h3 class="text-sm font-black text-white uppercase mb-4 tracking-tighter flex items-center gap-2">
                <span class="text-emerald-400">02.</span> Live Traffic Control
            </h3>

            <div v-if="activeQueue" class="space-y-4">
                <div class="flex justify-between items-center bg-slate-800/50 p-3 border border-emerald-500/10">
                    <span class="text-[10px] text-emerald-500/60 uppercase font-bold">Intersection</span>
                    <span class="text-lg font-mono text-emerald-400">#{{ activeQueue.intersectionId }}</span>
                </div>

                <div class="flex flex-col gap-2">
                    <span class="text-[10px] text-emerald-500/50 uppercase font-bold">Queue Sequence (FIFO)</span>
                    <div class="flex flex-col gap-2">
                        <div 
                            v-for="(car, idx) in activeQueue.cars" 
                            :key="car.id"
                            class="p-2 border transition-all duration-300"
                            :class="idx === 0 ? 'bg-emerald-500/20 border-emerald-400 shadow-[0_0_10px_rgba(52,211,153,0.2)]' : 'bg-slate-800/30 border-slate-700'"
                        >
                            <div class="flex justify-between items-center text-[10px] font-mono">
                                <span :class="idx === 0 ? 'text-emerald-300' : 'text-slate-400'">
                                    {{ idx === 0 ? '[FRONT]' : `[#${idx + 1}]` }}
                                </span>
                                <span class="text-white font-bold">{{ car.model }}</span>
                                <span class="text-slate-500">ID:{{ car.id }}</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <div v-else class="p-4 border border-dashed border-slate-700 text-center">
                <p class="text-[10px] text-slate-500 uppercase">Select an intersection to monitor traffic</p>
            </div>
        </div>
    </section>

    <!-- Navigation Stats Sidebar -->
    <aside 
        class="absolute right-8 top-32 w-72 z-20 transition-all duration-500"
        :class="shortestPath.length > 0 ? 'translate-x-0 opacity-100' : 'translate-x-12 opacity-0 pointer-events-none'"
    >
        <div class="bg-slate-900/90 border border-cyan-500/30 p-6 backdrop-blur-xl relative overflow-hidden group">
            <div class="absolute top-0 left-0 w-1 h-full bg-cyan-500 shadow-[0_0_15px_rgba(34,211,238,0.5)]"></div>
            
            <h3 class="text-sm font-black text-white uppercase mb-4 tracking-tighter flex items-center gap-2">
                <span class="text-cyan-400">01.</span> Navigation Stats
            </h3>

            <div class="space-y-4">
                <div class="flex flex-col">
                    <span class="text-[10px] text-cyan-500/50 uppercase font-bold">Total Distance</span>
                    <span class="text-3xl font-mono text-cyan-400 tracking-tighter">
                        {{ totalDistance.toLocaleString() }}<span class="text-xs text-white ml-1">METERS</span>
                    </span>
                </div>

                <div class="flex flex-col">
                    <span class="text-[10px] text-cyan-500/50 uppercase font-bold">Node Sequence</span>
                    <div class="flex flex-wrap gap-1 mt-2">
                        <span 
                            v-for="(nodeId, idx) in shortestPath" 
                            :key="`seq-${nodeId}`"
                            class="px-2 py-1 bg-cyan-500/10 border border-cyan-500/20 text-[10px] font-mono text-cyan-300"
                        >
                           {{ nodeId }}{{ idx < shortestPath.length - 1 ? ' →' : '' }}
                        </span>
                    </div>
                </div>
                
                <div class="pt-4 border-t border-cyan-500/10">
                    <p class="text-[9px] text-slate-500 font-mono italic leading-tight">
                        Optimized via Dijkstra SSSP algorithm. Latency: < 1ms.
                    </p>
                </div>
            </div>
        </div>
    </aside>

    <!-- Main SVG Canvas -->
    <div ref="container" class="w-full h-full relative cursor-crosshair">
      <svg 
        :viewBox="`0 0 ${width} ${height}`" 
        class="w-full h-full"
      >
        <!-- Background Grid -->
        <defs>
          <pattern id="grid" width="40" height="40" patternUnits="userSpaceOnUse">
            <path d="M 40 0 L 0 0 0 40" fill="none" stroke="rgba(34, 211, 238, 0.05)" stroke-width="1"/>
          </pattern>
          <filter id="neon-glow">
            <feGaussianBlur in="SourceGraphic" stdDeviation="2" result="blur" />
            <feMerge>
              <feMergeNode in="blur" />
              <feMergeNode in="SourceGraphic" />
            </feMerge>
          </filter>
        </defs>
        <rect width="100%" height="100%" fill="url(#grid)" />

        <!-- Road Connections (Edges) -->
        <g v-for="(edges, fromNodeId) in cityMap" :key="`edges-${fromNodeId}`">
          <line
            v-for="edge in edges"
            :key="`${fromNodeId}-${edge.toNode}`"
            :x1="getNodeCoords(Number(fromNodeId)).x"
            :y1="getNodeCoords(Number(fromNodeId)).y"
            :x2="getNodeCoords(edge.toNode).x"
            :y2="getNodeCoords(edge.toNode).y"
            class="stroke-slate-800 hover:stroke-cyan-500/30 transition-all duration-300"
            stroke-width="2"
            stroke-linecap="round"
          />
        </g>

        <!-- Highlighted Path (Neon Green) -->
        <g v-if="pathLines.length > 0">
          <line
            v-for="(line, idx) in pathLines"
            :key="`path-${idx}`"
            v-bind="line"
            class="stroke-emerald-400 path-animated filter drop-shadow-[0_0_8px_rgba(52,211,153,0.8)]"
            stroke-width="4"
            stroke-linecap="round"
            filter="url(#neon-glow)"
          />
        </g>

        <!-- Intersections (Nodes) -->
        <g v-for="node in nodes" :key="node.intersectionId">
          <!-- Selection Glow -->
          <circle 
            v-if="startNodeId === node.intersectionId || endNodeId === node.intersectionId"
            :cx="node.x"
            :cy="node.y"
            r="12"
            class="animate-ping-slow opacity-50"
            :class="startNodeId === node.intersectionId ? 'fill-emerald-400' : 'fill-red-500'"
          />
          
          <circle
            :cx="node.x"
            :cy="node.y"
            :r="hoverNode === node.intersectionId ? 8 : 6"
            class="transition-all duration-300 cursor-pointer"
            :class="getNodeClass(node.intersectionId)"
            @mouseenter="hoverNode = node.intersectionId"
            @mouseleave="hoverNode = null"
            @click="handleNodeClick(node.intersectionId)"
          />
          
          <text 
            v-if="hoverNode === node.intersectionId || startNodeId === node.intersectionId || endNodeId === node.intersectionId"
            :x="node.x + 12" 
            :y="node.y + 4" 
            class="text-[10px] font-mono select-none pointer-events-none uppercase font-bold"
            :class="startNodeId === node.intersectionId ? 'fill-emerald-400' : endNodeId === node.intersectionId ? 'fill-red-400' : 'fill-cyan-400'"
          >
          </text>
        </g>

        <!-- Dynamic Car Rendering (Day 3 Feature) -->
        <g v-for="(pos, carId) in liveCars" :key="`car-${carId}`">
            <circle
                :cx="pos.x"
                :cy="pos.y"
                r="4"
                class="fill-white stroke-cyan-400 stroke-2 transition-all duration-700 ease-in-out"
                filter="url(#neon-glow)"
            />
            <path 
                :d="`M ${pos.x - 6} ${pos.y - 6} L ${pos.x + 6} ${pos.y + 6} M ${pos.x + 6} ${pos.y - 6} L ${pos.x - 6} ${pos.y + 6}`"
                class="stroke-cyan-500/40"
                stroke-width="1"
            />
        </g>
      </svg>

    <!-- Legend Overlay -->
    <div class="absolute bottom-12 left-12 p-4 bg-slate-900/90 border-l-2 border-cyan-500 backdrop-blur-sm z-20">
        <h3 class="text-xs font-bold text-slate-400 uppercase mb-2 tracking-widest">Topology Monitor</h3>
        <div class="flex flex-col gap-1">
            <div class="flex justify-between items-center gap-8">
                <span class="text-[10px] text-slate-500 font-mono uppercase">Intersections</span>
                <span class="text-[10px] text-cyan-400 font-mono tracking-tighter">{{ nodes.length }}</span>
            </div>
            <div class="flex justify-between items-center gap-8">
                <span class="text-[10px] text-slate-500 font-mono uppercase">Structural Edges</span>
                <span class="text-[10px] text-cyan-400 font-mono tracking-tighter">{{ totalEdges }}</span>
            </div>
        </div>
    </div>
    </div>

    <!-- Timeline Scrubber (Day 3 Feature) -->
    <footer class="absolute bottom-0 w-full p-8 z-30 bg-gradient-to-t from-slate-950 to-transparent">
        <div class="max-w-4xl mx-auto space-y-4">
            <div class="flex justify-between items-end px-2">
                <div class="flex flex-col">
                    <span class="text-[10px] text-cyan-500/50 uppercase font-bold tracking-widest">Chronos Engine</span>
                    <span class="text-xl font-mono text-white italic">
                        {{ isTimeTraveling ? 'HISTORICAL_RESTORE' : 'REAL_TIME_STREAM' }}
                    </span>
                </div>
                <div class="text-right">
                    <span class="text-[10px] text-slate-500 font-mono">STEP: {{ currentSnapshotIndex }}/{{ (historySnapshots.length || 1) - 1 }}</span>
                    <p class="text-xs font-mono text-cyan-400">{{ currentSnapshotTimestamp }}</p>
                </div>
            </div>
            
            <div class="relative group">
                <input 
                    type="range" 
                    min="0" 
                    :max="(historySnapshots.length || 1) - 1" 
                    v-model="currentSnapshotIndex"
                    class="w-full h-1 bg-slate-800 rounded-lg appearance-none cursor-pointer accent-cyan-500 hover:accent-cyan-400 transition-all"
                >
                <div 
                    class="absolute -top-1 h-3 bg-cyan-500/20 pointer-events-none transition-all duration-300"
                    :style="{ width: `${(currentSnapshotIndex / ((historySnapshots.length || 1) - 1)) * 100}%` }"
                ></div>
            </div>
        </div>
    </footer>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed, watch } from 'vue';
import type { CityMap, Node, Edge } from '../types/map';
import type { Car, StateSnapshot, QueueStatus } from '../types/traffic';

const cityMap = ref<CityMap>({});
const nodes = ref<Node[]>([]);
const hoverNode = ref<number | null>(null);

// Routing State
const startNodeId = ref<number | null>(null);
const endNodeId = ref<number | null>(null);
const shortestPath = ref<number[]>([]);

// Traffic & History State (Day 3)
const activeQueue = ref<QueueStatus | null>(null);
const historySnapshots = ref<StateSnapshot[]>([]);
const currentSnapshotIndex = ref(0);
const isTimeTraveling = computed(() => currentSnapshotIndex.value < historySnapshots.value.length - 1);

const liveCars = ref<Record<number, { x: number, y: number, id: number }>>({});
const currentSnapshotTimestamp = computed(() => {
    if (!historySnapshots.value.length) return "N/A";
    const date = new Date(historySnapshots.value[currentSnapshotIndex.value].timestamp);
    return date.toLocaleString('en-US', { hour12: false });
});

const width = ref(window.innerWidth);
const height = ref(window.innerHeight);

const totalEdges = computed(() => {
  return Object.values(cityMap.value).reduce((acc, curr) => acc + curr.length, 0) / 2;
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
        lines.push({ x1: u.x, y1: u.y, x2: v.x, y2: v.y });
    }
    return lines;
});

const handleResize = () => {
  width.value = window.innerWidth;
  height.value = window.innerHeight;
};

const getNodeClass = (id: number) => {
    if (startNodeId.value === id) return 'fill-emerald-400 drop-shadow-[0_0_10px_rgba(52,211,153,0.8)]';
    if (endNodeId.value === id) return 'fill-red-500 drop-shadow-[0_0_10px_rgba(239,68,68,0.8)]';
    if (shortestPath.value.includes(id)) return 'fill-emerald-400';
    return 'fill-cyan-400 drop-shadow-[0_0_5px_rgba(34,211,238,0.4)] hover:fill-white';
};

const getNodeLabel = (id: number) => {
    if (startNodeId.value === id) return 'START_NODE';
    if (endNodeId.value === id) return 'DESTINATION';
    return `NODE_${id}`;
};

const handleNodeClick = (id: number) => {
    // Select intersection for traffic monitoring
    fetchTrafficStatus(id);

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

const fetchTrafficStatus = async (id: number) => {
    try {
        console.log(`Fetching traffic status for intersection: ${id}`);
        // Mock data for the demo
        activeQueue.value = {
            intersectionId: id,
            queueLength: 3,
            cars: [
                { id: 101, currentIntersection: 0, targetIntersection: id, progress: 0.9, model: "CyberSedan" },
                { id: 102, currentIntersection: 0, targetIntersection: id, progress: 0.7, model: "NeonTruck" },
                { id: 103, currentIntersection: 0, targetIntersection: id, progress: 0.5, model: "VoltRider" }
            ]
        };
    } catch (error) {
        console.error("Traffic fetch error:", error);
    }
};

// Simulation Loop for History
const initHistory = () => {
    const snapshots: StateSnapshot[] = [];
    const baseTime = Date.now() - 100000;
    
    for (let i = 0; i < 50; i++) {
        const positions: Record<number, { x: number, y: number }> = {};
        // Randomly place cars for history demo
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

watch(currentSnapshotIndex, (newIdx) => {
    if (historySnapshots.value[newIdx]) {
        liveCars.value = historySnapshots.value[newIdx].carPositions;
    }
});

const clearRoute = () => {
    startNodeId.value = null;
    endNodeId.value = null;
    shortestPath.value = [];
};

const findShortestPath = async () => {
    if (startNodeId.value === null || endNodeId.value === null) return;
    
    try {
        console.log(`Calculating route: ${startNodeId.value} -> ${endNodeId.value}`);
        // simulate Antigravity API route
        const response = await fetch(`/api/route?start=${startNodeId.value}&end=${endNodeId.value}`);
        
        // Since we are mocking the backend in main.cpp, we simulate the result here for the UI
        // In a real environment, this line would be: const path = await response.json();
        
        // MOCK LOGIC for demo (matches C++ implementation)
        // We'll use a local mock for now to ensure the UI is wowed immediately
        const path = await simulateBackendDijkstra(startNodeId.value, endNodeId.value);
        shortestPath.value = path;
    } catch (error) {
        console.error("Pathfinding error:", error);
    }
};

// Frontend side Dijkstra mock for immediate visual feedback if backend isn't real-time
const simulateBackendDijkstra = async (start: number, end: number) => {
    // This replicates the backend's logic for Day 2 demonstration PURPOSES
    // In final delivery, standard fetch would used.
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
        path.push(curr);
        curr = prev[curr] ?? null;
        if (curr === start) {
            path.push(start);
            break;
        }
    }
    return path.reverse();
};

// Simple Layout Engine
const generateLayout = (data: CityMap) => {
  const nodeIds = Object.keys(data).map(Number);
  
  return nodeIds.map((id, index) => {
    const angle = (index / nodeIds.length) * Math.PI * 2;
    const radius = Math.min(width.value, height.value) * 0.35;
    
    return {
      intersectionId: id,
      x: width.value / 2 + Math.cos(angle) * (radius + (id % 3) * 60),
      y: height.value / 2 + Math.sin(angle) * (radius + (id % 2) * 40),
    };
  });
};

const fetchMapData = async () => {
  try {
    const response = {
      0: [{ toNode: 1, weight: 100, streetName: "Neon Blvd" }, { toNode: 4, weight: 80, streetName: "Mainframe" }],
      1: [{ toNode: 0, weight: 100, streetName: "Neon Blvd" }, { toNode: 2, weight: 120, streetName: "Neon Blvd" }, { toNode: 5, weight: 85, streetName: "Silicon St" }, { toNode: 8, weight: 200, streetName: "Central" }],
      2: [{ toNode: 1, weight: 120, streetName: "Neon Blvd" }, { toNode: 3, weight: 110, streetName: "Neon Blvd" }, { toNode: 6, weight: 150, streetName: "Processing" }, { toNode: 9, weight: 250, streetName: "Bypass" }],
      3: [{ toNode: 2, weight: 110, streetName: "Neon Blvd" }, { toNode: 7, weight: 95, streetName: "Logic Loop" }],
      4: [{ toNode: 0, weight: 80, streetName: "Mainframe" }, { toNode: 5, weight: 90, streetName: "Cyber Way" }, { toNode: 11, weight: 180, streetName: "Skyline" }],
      5: [{ toNode: 4, weight: 90, streetName: "Cyber Way" }, { toNode: 1, weight: 85, streetName: "Silicon St" }, { toNode: 8, weight: 110, streetName: "Shortcut 01" }],
      6: [{ toNode: 2, weight: 150, streetName: "Processing" }, { toNode: 7, weight: 70, streetName: "Data Stream" }],
      7: [{ toNode: 6, weight: 70, streetName: "Data Stream" }, { toNode: 3, weight: 95, streetName: "Logic Loop" }],
      8: [{ toNode: 1, weight: 200, streetName: "Central" }, { toNode: 9, weight: 130, streetName: "Transit Hub" }, { toNode: 5, weight: 110, streetName: "Shortcut 01" }],
      9: [{ toNode: 8, weight: 130, streetName: "Transit Hub" }, { toNode: 10, weight: 140, streetName: "Binary Alley" }, { toNode: 2, weight: 250, streetName: "Bypass" }],
      10: [{ toNode: 9, weight: 140, streetName: "Binary Alley" }, { toNode: 11, weight: 60, streetName: "Node Conn" }],
      11: [{ toNode: 10, weight: 60, streetName: "Node Conn" }, { toNode: 4, weight: 180, streetName: "Skyline" }],
    };
    
    cityMap.value = response;
    nodes.value = generateLayout(response);
    
    // Initialize history after nodes are generated
    setTimeout(initHistory, 500);
  } catch (error) {
    console.error("Failed to load Neo-City Map:", error);
  }
};

const getNodeCoords = (id: number) => {
  return nodes.value.find(n => n.intersectionId === id) || { x: 0, y: 0 };
};

onMounted(() => {
  window.addEventListener('resize', handleResize);
  fetchMapData();
});

onUnmounted(() => {
  window.removeEventListener('resize', handleResize);
});
</script>

<style scoped>
.path-animated {
  stroke-dasharray: 10;
  animation: dash 20s linear infinite;
}

@keyframes dash {
  from { stroke-dashoffset: 1000; }
  to { stroke-dashoffset: 0; }
}

@keyframes ping-slow {
  0% { transform: scale(1); opacity: 0.8; }
  100% { transform: scale(3); opacity: 0; }
}

.animate-ping-slow {
    transform-origin: center;
    animation: ping-slow 2s cubic-bezier(0, 0, 0.2, 1) infinite;
}

.clip-path-polygon {
    clip-path: polygon(10% 0, 100% 0, 90% 100%, 0% 100%);
}
</style>
