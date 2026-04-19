<template>
  <div ref="container" class="w-full h-full relative overflow-hidden bg-slate-950/50 rounded-3xl group" :class="{ 'cursor-wait': isSearching }">
    <svg 
      viewBox="0 0 1600 1000"
      class="w-full h-full"
      @mousemove="handleMouseMove"
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
      <g v-memo="[nodes.length, !!cityMap]">
        <g v-for="(edges, fromNodeId) in cityMap" :key="`cluster-${fromNodeId}`">
          <line
            v-for="edge in edges"
            :key="`${fromNodeId}-${edge.toNode}`"
            :x1="getNodeCoords(Number(fromNodeId))?.x || 0"
            :y1="getNodeCoords(Number(fromNodeId))?.y || 0"
            :x2="getNodeCoords(edge.toNode)?.x || 0"
            :y2="getNodeCoords(edge.toNode)?.y || 0"
            class="stroke-slate-800 transition-all duration-300 pointer-events-none"
            stroke-width="5"
            stroke-linecap="round"
          />
        </g>
      </g>

      <!-- Highlighted Path (Neon Emerald) -->
      <g v-if="pathLines.length > 0" v-memo="[pathLines.length, shortestPath.join(',')]">
        <line
          v-for="(line, idx) in pathLines"
          :key="`path-seg-${idx}-${line.x1}`"
          v-bind="line"
          class="stroke-emerald-400 path-animated pointer-events-none"
          stroke-width="6"
          stroke-linecap="round"
          filter="url(#neon-glow)"
        />
      </g>

      <!-- Pulse Rings for Selection (Visual Feedback) -->
      <g v-if="startNodeId !== null" :key="`start-pulse-${startNodeId}`">
        <circle 
          :cx="getNodeCoords(startNodeId)?.x" 
          :cy="getNodeCoords(startNodeId)?.y" 
          r="25" 
          class="fill-emerald-400/20 animate-ping pointer-events-none" 
        />
      </g>
      <g v-if="endNodeId !== null" :key="`end-pulse-${endNodeId}`">
        <circle 
          :cx="getNodeCoords(endNodeId)?.x" 
          :cy="getNodeCoords(endNodeId)?.y" 
          r="25" 
          class="fill-red-500/20 animate-ping pointer-events-none" 
        />
      </g>

      <!-- Intersections (Nodes) -->
      <g v-memo="[nodes.length, hoverNode, startNodeId, endNodeId, shortestPath.length]">
        <g v-for="node in nodes" :key="`node-group-${node.intersectionId}`">
            <!-- Enlarged hit area for better usability -->
            <circle
                :cx="node.x"
                :cy="node.y"
                r="25"
                class="fill-transparent cursor-pointer pointer-events-auto"
                :class="{ 'pointer-events-none': isSearching }"
                @mouseenter="handleHover(node)"
                @mouseleave="handleHoverExit"
                @click.stop="handleClick(node.intersectionId)"
            />
            
            <!-- Visible Node Circle -->
            <circle
                :cx="node.x"
                :cy="node.y"
                :r="hoverNode === node.intersectionId ? 18 : 12"
                class="transition-all duration-300 pointer-events-none"
                :class="getNodeClass(node.intersectionId)"
            />
        </g>
      </g>

      <!-- Dynamic Car Rendering -->
      <g v-for="(pos, carId) in liveCars" :key="`car-render-${carId}`">
          <circle
              :cx="pos.x"
              :cy="pos.y"
              r="6"
              class="fill-white stroke-cyan-400 stroke-2 transition-all duration-700 ease-in-out pointer-events-none shadow-glow"
          />
      </g>
    </svg>

    <!-- Loading Sentinel (Performance) -->
    <transition name="fade">
        <div v-if="isSearching" class="absolute inset-0 bg-slate-950/20 backdrop-blur-[2px] z-50 flex items-center justify-center pointer-events-none">
            <div class="px-6 py-3 bg-slate-900 border border-cyan-500/40 rounded-full shadow-2xl flex items-center gap-4">
                <div class="w-4 h-4 border-2 border-cyan-500 border-t-transparent animate-spin rounded-full"></div>
                <span class="text-[9px] font-black text-white uppercase tracking-[0.3em]">Processing_Neural_Path</span>
            </div>
        </div>
    </transition>

    <!-- Floating Legend (Usability) -->
    <div class="absolute top-6 left-6 p-4 bg-slate-900/80 backdrop-blur-md border border-white/5 rounded-2xl shadow-2xl flex flex-col gap-3 z-30">
        <div class="flex items-center gap-3">
            <div class="w-3 h-3 rounded-full bg-emerald-400 animate-pulse"></div>
            <span class="text-[9px] font-black uppercase tracking-widest text-slate-400">Departure</span>
        </div>
        <div class="flex items-center gap-3">
            <div class="w-3 h-3 rounded-full bg-red-500 animate-pulse"></div>
            <span class="text-[9px] font-black uppercase tracking-widest text-slate-400">Destination</span>
        </div>
        <div class="flex items-center gap-3">
            <div class="w-6 h-1 bg-emerald-400 rounded-full"></div>
            <span class="text-[9px] font-black uppercase tracking-widest text-slate-400">Optimized Path</span>
        </div>
    </div>

    <!-- Node Hover Tooltip (UX Focus) -->
    <div 
        v-if="hoverInfo"
        class="fixed z-[100] pointer-events-none bg-slate-900/95 border border-cyan-500/30 p-4 rounded-xl shadow-[0_0_40px_rgba(34,211,238,0.2)] flex flex-col gap-1 min-w-[120px]"
        :style="{ left: `${mouseX + 15}px`, top: `${mouseY + 15}px` }"
    >
        <span class="text-[8px] font-black text-cyan-500 uppercase tracking-widest leading-none">Intersection_ID</span>
        <span class="text-white font-mono text-sm font-bold">#{{ hoverInfo.intersectionId }}</span>
        <div class="mt-2 h-[2px] bg-slate-800 rounded-full overflow-hidden">
            <div class="h-full bg-cyan-400 w-1/2"></div>
        </div>
        <span class="text-[7px] text-slate-500 uppercase mt-1">Traffic_Level: OPTIMAL</span>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, inject } from 'vue';
import type { useCityEngine } from '../composables/useCityEngine';
import type { Node } from '../types/map';

const engine = inject<ReturnType<typeof useCityEngine>>('cityEngine');
if (!engine) throw new Error('CityEngine not provided');

const {
    cityMap, nodes, hoverNode, startNodeId, endNodeId, 
    shortestPath, liveCars, pathLines, isSearching,
    getNodeCoords, handleNodeClick
} = engine;

const hoverInfo = ref<Node | null>(null);
const mouseX = ref(0);
const mouseY = ref(0);

const handleMouseMove = (e: MouseEvent) => {
    mouseX.value = e.clientX;
    mouseY.value = e.clientY;
};

const handleHover = (node: Node) => {
    hoverNode.value = node.intersectionId;
    hoverInfo.value = node;
};

const handleHoverExit = () => {
    hoverNode.value = null;
    hoverInfo.value = null;
};

const getNodeClass = (id: number) => {
    if (startNodeId.value === id) return 'fill-emerald-400 drop-shadow-[0_0_15px_rgba(52,211,153,0.8)] z-50';
    if (endNodeId.value === id) return 'fill-red-500 drop-shadow-[0_0_15px_rgba(239,68,68,0.8)] z-50';
    if (shortestPath.value.includes(id)) return 'fill-emerald-400 scale-110';
    return 'fill-cyan-400 drop-shadow-[0_0_5px_rgba(34,211,238,0.3)] hover:fill-white';
};

const handleClick = (id: number) => {
    handleNodeClick(id);
};
</script>

<style scoped>
.path-animated {
  stroke-dasharray: 10;
  animation: move-dash 1s linear infinite;
}
@keyframes move-dash {
  to { stroke-dashoffset: -20; }
}
.shadow-glow {
    filter: drop-shadow(0 0 4px rgba(34, 211, 238, 0.6));
}
.fade-enter-active, .fade-leave-active {
    transition: opacity 0.3s ease;
}
.fade-enter-from, .fade-leave-to {
    opacity: 0;
}
</style>
