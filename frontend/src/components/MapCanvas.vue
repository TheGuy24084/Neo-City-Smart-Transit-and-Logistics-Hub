<template>
  <div class="w-full h-screen bg-slate-900 flex flex-col items-center justify-center overflow-hidden">
    <!-- Header with Cyberpunk styling -->
    <header class="absolute top-0 w-full p-6 flex justify-between items-center z-10 bg-slate-900/80 backdrop-blur-md border-b border-cyan-500/20">
      <div class="flex flex-col">
        <h1 class="text-3xl font-black tracking-tighter text-cyan-400 uppercase">
          Neo-Sector <span class="text-white">07</span>
        </h1>
        <p class="text-[10px] text-cyan-500/60 font-mono tracking-widest uppercase">Smart Transit Hub // Network Diagnostic</p>
      </div>
      <div class="flex gap-4">
        <div class="px-4 py-2 bg-cyan-500/10 border border-cyan-500/30 rounded-sm">
          <span class="text-xs font-mono text-cyan-400">STATUS: ACTIVE</span>
        </div>
      </div>
    </header>

    <!-- Main SVG Canvas -->
    <div ref="container" class="w-full h-full relative cursor-crosshair">
      <svg 
        :viewBox="`0 0 ${width} ${height}`" 
        class="w-full h-full"
        @mousedown="startPan"
        @mousemove="onPan"
        @mouseup="endPan"
        @mouseleave="endPan"
      >
        <!-- Background Grid -->
        <defs>
          <pattern id="grid" width="40" height="40" patternUnits="userSpaceOnUse">
            <path d="M 40 0 L 0 0 0 40" fill="none" stroke="rgba(34, 211, 238, 0.05)" stroke-width="1"/>
          </pattern>
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
            class="stroke-slate-700 hover:stroke-cyan-500/50 transition-all duration-300 pointer-events-auto"
            stroke-width="2"
            stroke-linecap="round"
          />
          <!-- Street Names (Optional/Hover) -->
        </g>

        <!-- Intersections (Nodes) -->
        <g v-for="node in nodes" :key="node.intersectionId">
          <circle
            :cx="node.x"
            :cy="node.y"
            r="6"
            class="fill-cyan-400 filter drop-shadow-[0_0_8px_rgba(34,211,238,0.8)] cursor-pointer hover:r-8 transition-all"
            @mouseenter="hoverNode = node.intersectionId"
            @mouseleave="hoverNode = null"
          />
          <text 
            v-if="hoverNode === node.intersectionId"
            :x="node.x + 12" 
            :y="node.y + 4" 
            class="fill-cyan-400 text-[10px] font-mono select-none pointer-events-none"
          >
            NODE_{{ node.intersectionId }}
          </text>
        </g>
      </svg>

      <!-- UI Overlays -->
      <div class="absolute bottom-8 left-8 p-4 bg-slate-900/90 border-l-2 border-cyan-500 backdrop-blur-sm">
        <h3 class="text-xs font-bold text-slate-400 uppercase mb-2">Network Topology</h3>
        <div class="flex flex-col gap-1">
          <div class="flex justify-between items-center gap-8">
            <span class="text-[10px] text-slate-500">Nodes:</span>
            <span class="text-[10px] text-cyan-400 font-mono">{{ nodes.length }}</span>
          </div>
          <div class="flex justify-between items-center gap-8">
            <span class="text-[10px] text-slate-500">Edges:</span>
            <span class="text-[10px] text-cyan-400 font-mono">{{ totalEdges }}</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed } from 'vue';
import type { CityMap, Node, Edge } from '../types/map';

const cityMap = ref<CityMap>({});
const nodes = ref<Node[]>([]);
const hoverNode = ref<number | null>(null);

const width = ref(window.innerWidth);
const height = ref(window.innerHeight);

const totalEdges = computed(() => {
  return Object.values(cityMap.value).reduce((acc, curr) => acc + curr.length, 0) / 2;
});

const handleResize = () => {
  width.value = window.innerWidth;
  height.value = window.innerHeight;
};

// Simple Layout Engine (since backend just seeds the logic)
const generateLayout = (data: CityMap) => {
  const nodeIds = Object.keys(data).map(Number);
  const padding = 100;
  
  // Create a pseudo-random but repeatable layout for Day 1
  return nodeIds.map((id, index) => {
    // Basic Circle/Grid distribution for seed nodes
    const angle = (index / nodeIds.length) * Math.PI * 2;
    const radius = Math.min(width.value, height.value) * 0.3;
    
    // Spread them out based on ID for a "city-like" feel
    return {
      intersectionId: id,
      x: width.value / 2 + Math.cos(angle) * (radius + (id % 3) * 50),
      y: height.value / 2 + Math.sin(angle) * (radius + (id % 2) * 30),
    };
  });
};

const fetchMapData = async () => {
  try {
    // In a real Day 1 scenario, we might hit the C++ backend
    // For now, we mock the result to ensure the UI works immediately
    const response = {
      0: [{ toNode: 1, weight: 100, streetName: "Neon Blvd" }, { toNode: 4, weight: 80, streetName: "Mainframe" }],
      1: [{ toNode: 0, weight: 100, streetName: "Neon Blvd" }, { toNode: 2, weight: 120, streetName: "Neon Blvd" }, { toNode: 5, weight: 85, streetName: "Silicon St" }, { toNode: 8, weight: 200, streetName: "Central" }],
      2: [{ toNode: 1, weight: 120, streetName: "Neon Blvd" }, { toNode: 3, weight: 110, streetName: "Neon Blvd" }, { toNode: 6, weight: 150, streetName: "Processing" }],
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

// Basic Pan functionality (Skeleton)
const startPan = () => {};
const onPan = () => {};
const endPan = () => {};
</script>

<style scoped>
.filter {
  transition: all 0.3s ease;
}
</style>
