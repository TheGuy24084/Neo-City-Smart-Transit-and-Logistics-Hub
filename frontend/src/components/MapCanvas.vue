<template>
  <div ref="container" class="w-full h-full relative cursor-crosshair overflow-hidden">
    <svg 
      viewBox="0 0 1600 1000"
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
      <rect width="100%" height="100%" fill="url(#grid)" class="pointer-events-none" />

      <!-- Road Connections (Edges) -->
      <g v-for="(edges, fromNodeId) in cityMap" :key="`edges-${fromNodeId}`">
        <line
          v-for="edge in edges"
          :key="`${fromNodeId}-${edge.toNode}`"
          :x1="getNodeCoords(Number(fromNodeId))?.x || 0"
          :y1="getNodeCoords(Number(fromNodeId))?.y || 0"
          :x2="getNodeCoords(edge.toNode)?.x || 0"
          :y2="getNodeCoords(edge.toNode)?.y || 0"
          class="stroke-slate-800 hover:stroke-cyan-500/30 transition-all duration-300 pointer-events-none"
          stroke-width="5"
          stroke-linecap="round"
        />
      </g>

      <!-- Highlighted Path (Neon Green) -->
      <g v-if="pathLines.length > 0">
        <line
          v-for="(line, idx) in pathLines"
          :key="`path-${idx}`"
          v-bind="line"
          class="stroke-emerald-400 path-animated filter drop-shadow-[0_0_8px_rgba(52,211,153,0.8)] pointer-events-none"
          stroke-width="4"
          stroke-linecap="round"
          filter="url(#neon-glow)"
        />
      </g>

      <!-- Intersections (Nodes) -->
      <g v-for="node in nodes" :key="node.intersectionId">
        <circle 
          v-if="startNodeId === node.intersectionId || endNodeId === node.intersectionId"
          :cx="node.x"
          :cy="node.y"
          r="20"
          class="opacity-50"
          :class="startNodeId === node.intersectionId ? 'fill-emerald-400 animate-pulse' : 'fill-red-500 animate-pulse'"
        />
        
        <circle
          :cx="node.x"
          :cy="node.y"
          :r="hoverNode === node.intersectionId ? 18 : 14"
          class="transition-all duration-300 cursor-pointer pointer-events-auto"
          :class="getNodeClass(node.intersectionId)"
          @mouseenter="hoverNode = node.intersectionId"
          @mouseleave="hoverNode = null"
          @click.stop="handleClick(node.intersectionId)"
        />
      </g>

      <!-- Dynamic Car Rendering -->
      <g v-for="(pos, carId) in liveCars" :key="`car-${carId}`">
          <circle
              :cx="pos.x"
              :cy="pos.y"
              r="8"
              class="fill-white stroke-cyan-400 stroke-2 transition-all duration-700 ease-in-out pointer-events-none"
              filter="url(#neon-glow)"
          />
      </g>
    </svg>

    <!-- Internal Map Overlays -->
    <div class="absolute bottom-6 left-6 p-4 bg-slate-900/90 border-l-2 border-cyan-500 backdrop-blur-sm z-10 hidden lg:block">
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
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, inject } from 'vue';
import type { useCityEngine } from '../composables/useCityEngine';

const engine = inject<ReturnType<typeof useCityEngine>>('cityEngine');
if (!engine) throw new Error('CityEngine not provided');

const {
    cityMap, nodes, hoverNode, startNodeId, endNodeId, 
    shortestPath, liveCars, pathLines, totalEdges,
    getNodeCoords
} = engine;

const width = ref(window.innerWidth);
const height = ref(window.innerHeight);

const getNodeClass = (id: number) => {
    if (startNodeId.value === id) return 'fill-emerald-400 drop-shadow-[0_0_10px_rgba(52,211,153,0.8)]';
    if (endNodeId.value === id) return 'fill-red-500 drop-shadow-[0_0_10px_rgba(239,68,68,0.8)]';
    if (shortestPath.value.includes(id)) return 'fill-emerald-400';
    return 'fill-cyan-400 drop-shadow-[0_0_5px_rgba(34,211,238,0.4)] hover:fill-white';
};

const handleClick = (id: number) => {
    console.log(`[MapCanvas] Node clicked: ${id}`);
    engine.handleNodeClick(id);
};

const handleResize = () => {
    width.value = window.innerWidth;
    height.value = window.innerHeight;
};

onMounted(async () => {
    window.addEventListener('resize', handleResize);
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
</style>
