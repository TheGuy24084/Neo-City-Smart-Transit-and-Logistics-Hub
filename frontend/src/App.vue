<script setup lang="ts">
import { onMounted, provide } from 'vue';
import MapCanvas from './components/MapCanvas.vue';
import CyberCard from './components/CyberCard.vue';
import { useCityEngine } from './composables/useCityEngine';

const engine = useCityEngine();
provide('cityEngine', engine);

const {
    navigationStack, metrics, activeQueue, showSpecs, 
    isStressTesting, districtSummary, historySnapshots,
    currentSnapshotIndex, isTimeTraveling, currentSnapshotTimestamp,
    totalDistance, shortestPath, popTo, popView, runStressTest,
    initHistory
} = engine;

onMounted(() => {
    engine.initMap();
    initHistory();
});
</script>

<template>
  <div class="min-h-screen bg-slate-950 font-sans selection:bg-cyan-500/30 flex flex-col p-4 lg:p-10 gap-8 overflow-y-auto custom-scrollbar">
    
    <!-- Header -->
    <header class="w-full flex flex-col lg:flex-row justify-between items-center gap-6 z-40 bg-slate-900/40 backdrop-blur-xl border border-white/5 p-8 rounded-2xl shadow-2xl">
      <div class="flex flex-col">
        <h1 class="text-4xl font-black tracking-tighter text-white uppercase italic leading-none">
          Neo-Sector <span class="text-cyan-400">07</span>
        </h1>
        <nav class="flex items-center gap-2 mt-3">
            <div v-for="(state, idx) in navigationStack" :key="`crumb-${idx}`" class="flex items-center gap-2 group">
                <button 
                    @click="popTo(idx)"
                    class="text-[10px] font-mono tracking-widest transition-all uppercase"
                    :class="idx === navigationStack.length - 1 ? 'text-white cursor-default' : 'text-slate-500 hover:text-cyan-400'"
                >
                    {{ state.label }}
                </button>
                <span v-if="idx < navigationStack.length - 1" class="text-slate-800 text-[10px]">/</span>
            </div>
        </nav>
      </div>
      
      <div class="flex gap-4 items-center">
        <button 
            v-if="navigationStack.length > 1"
            @click="popView"
            class="px-5 py-2 bg-white/5 hover:bg-white/10 border border-white/10 text-[10px] text-white uppercase font-black tracking-widest transition-all flex items-center gap-2 group rounded-lg"
        >
            <span class="group-hover:-translate-x-1 transition-transform">←</span> Back
        </button>
        <button 
            @click="showSpecs = true"
            class="px-6 py-2 bg-cyan-500/10 hover:bg-cyan-500/20 border border-cyan-500/30 text-[10px] text-cyan-400 uppercase font-black tracking-widest transition-all rounded-lg"
        >
            Tech Specs
        </button>
      </div>
    </header>

    <!-- Main Responsive Grid -->
    <main class="flex-1 grid grid-cols-1 lg:grid-cols-12 gap-8 items-start">
      
      <!-- Left Sidebar (Live Traffic) -->
      <aside class="lg:col-span-3 flex flex-col gap-6">
        <CyberCard variant="emerald">
            <template #title>02. Live Traffic Control</template>
            <template #header-suffix>
                <span v-if="activeQueue" class="px-2 py-0.5 bg-emerald-500/10 border border-emerald-500/20 text-[8px] text-emerald-400 rounded-full animate-pulse font-mono tracking-tighter">LIVE_STREAM</span>
            </template>

            <div v-if="activeQueue" class="space-y-6">
                <div class="flex justify-between items-center bg-white/5 p-4 border border-white/5 rounded-lg">
                    <span class="text-[9px] text-slate-500 uppercase font-bold tracking-widest">Intersection</span>
                    <span class="text-xl font-mono text-white tracking-tighter italic">#{{ activeQueue.intersectionId }}</span>
                </div>

                <div class="flex flex-col gap-3">
                    <span class="text-[9px] text-emerald-500/50 uppercase font-bold tracking-widest">FIFO Priority Queue</span>
                    <div class="flex flex-col gap-2">
                        <div 
                            v-for="(car, idx) in activeQueue.cars" 
                            :key="car.id"
                            class="p-3 border transition-all duration-500 rounded-lg"
                            :class="idx === 0 ? 'bg-emerald-500/10 border-emerald-500/30 shadow-[0_0_20px_rgba(16,185,129,0.1)]' : 'bg-white/5 border-white/5'"
                        >
                            <div class="flex justify-between items-center text-[10px] font-mono">
                                <span :class="idx === 0 ? 'text-emerald-400 font-black' : 'text-slate-500'">
                                    {{ idx === 0 ? '[PRIORITY]' : `[WAITING_${idx}]` }}
                                </span>
                                <span class="text-white font-bold opacity-80">{{ car.model }}</span>
                                <span class="text-slate-600 text-[8px]">ID:{{ car.id }}</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <div v-else class="p-8 border border-dashed border-slate-800 text-center opacity-40 rounded-xl">
                <p class="text-[10px] text-slate-500 uppercase tracking-[0.2em]">Select Intersection Node</p>
            </div>
        </CyberCard>

        <CyberCard v-if="activeQueue" variant="amber">
            <template #title>03. Traffic Load</template>
            <div class="space-y-5">
                <div class="flex justify-between text-[9px] font-bold uppercase tracking-widest">
                    <span class="text-amber-500/50">Simulated Congestion</span>
                    <span class="text-amber-400">{{ (activeQueue.cars.length / 5 * 100).toFixed(0) }}%</span>
                </div>
                <div class="h-1 bg-slate-800 rounded-full overflow-hidden p-[1px]">
                    <div 
                        class="h-full bg-gradient-to-r from-amber-600 to-amber-400 transition-all duration-1000"
                        :style="{ width: `${(activeQueue.cars.length / 5) * 100}%` }"
                    ></div>
                </div>
                <button 
                    @click="runStressTest"
                    :disabled="isStressTesting"
                    class="w-full py-3 bg-amber-500/5 hover:bg-amber-500/10 border border-amber-500/20 text-[9px] text-amber-500 uppercase font-black tracking-[3px] transition-all disabled:opacity-30 rounded-lg"
                >
                    {{ isStressTesting ? 'SIMULATING...' : 'Launch Stress Test' }}
                </button>
            </div>
        </CyberCard>
      </aside>

      <!-- Center Content (Map) -->
      <section class="lg:col-span-6 flex flex-col gap-6 h-full min-h-[500px] lg:min-h-[700px]">
        <div class="flex-1 relative bg-slate-900 border border-white/5 rounded-3xl overflow-hidden shadow-[0_0_50px_rgba(0,0,0,0.5)] group">
            <MapCanvas />
            
            <!-- Contextual Overlay -->
            <div v-if="isTimeTraveling" class="absolute top-8 left-1/2 -translate-x-1/2 z-30 px-8 py-3 bg-amber-500 text-slate-950 font-black text-[10px] uppercase tracking-[4px] shadow-2xl rounded-full flex items-center gap-4 animate-pulse">
                <div class="w-2 h-2 rounded-full bg-slate-950 animate-ping"></div>
                Historical Buffer Active
            </div>
        </div>
      </section>

      <!-- Right Sidebar (Metrics & Audit) -->
      <aside class="lg:col-span-3 flex flex-col gap-6">
        <CyberCard variant="purple">
            <template #title>01. Metrics Engine</template>
            <div class="space-y-6">
                <div class="space-y-3">
                    <div class="flex justify-between items-end">
                        <span class="text-[9px] text-purple-400/50 uppercase font-bold tracking-widest">Graph Traversal</span>
                        <span class="text-[9px] font-mono text-purple-500">O(E log V)</span>
                    </div>
                    <div class="flex items-center gap-4">
                        <div class="flex-1 h-1.5 bg-slate-800 rounded-full overflow-hidden">
                            <div 
                                class="h-full bg-gradient-to-r from-purple-600 to-purple-400 transition-all duration-1000"
                                :style="{ width: `${Math.min(100, (metrics.nodesVisited / 20) * 100)}%` }"
                            ></div>
                        </div>
                        <span class="text-xs font-mono text-white leading-none">{{ metrics.nodesVisited }} <span class="text-[8px] text-slate-500 uppercase">Visits</span></span>
                    </div>
                </div>

                <div class="space-y-3">
                    <div class="bg-white/5 p-5 border border-white/5 flex items-center justify-between rounded-xl">
                        <div class="flex flex-col">
                            <span class="text-[8px] text-slate-600 uppercase font-black tracking-widest">State Footprint</span>
                            <span class="text-xl font-mono text-white tracking-tighter italic">{{ (metrics.historyBytes / 1024).toFixed(2) }}KB</span>
                        </div>
                        <div class="text-xs font-mono text-purple-500 opacity-20 group-hover:opacity-100 transition-opacity">0x6F2A</div>
                    </div>
                </div>
            </div>
        </CyberCard>

        <CyberCard v-if="shortestPath.length > 0" variant="cyan">
            <template #title>Navigation Stats</template>
            <div class="space-y-4">
                <div class="flex flex-col">
                    <span class="text-[9px] text-cyan-500/40 uppercase font-bold tracking-widest">Total Path Metric</span>
                    <span class="text-4xl font-mono text-white tracking-tighter italic">
                        {{ totalDistance }}<span class="text-sm text-cyan-500 font-normal ml-2">Units</span>
                    </span>
                </div>
                <div class="flex flex-wrap gap-1.5 pt-4 border-t border-white/5">
                    <span v-for="id in shortestPath" :key="id" class="px-2 py-1 bg-white/5 border border-white/5 text-[8px] font-mono text-slate-400 rounded">
                        #{{ id }}
                    </span>
                </div>
            </div>
        </CyberCard>

        <CyberCard v-if="districtSummary" variant="blue">
            <template #title>District Audit (DFS)</template>
            <div class="grid grid-cols-2 gap-4">
                <div class="p-4 bg-white/5 border border-white/5 rounded-xl">
                    <p class="text-[8px] text-slate-600 uppercase font-bold">Nodes</p>
                    <p class="text-2xl font-mono text-blue-400 tracking-tighter">{{ districtSummary.totalNodes }}</p>
                </div>
                <div class="p-4 bg-white/5 border border-white/5 rounded-xl">
                    <p class="text-[8px] text-slate-600 uppercase font-bold">Edges</p>
                    <p class="text-2xl font-mono text-blue-400 tracking-tighter">{{ districtSummary.totalEdges }}</p>
                </div>
            </div>
        </CyberCard>
      </aside>
    </main>

    <!-- Sticky Footer (Chronos Engine) -->
    <footer class="sticky bottom-0 z-40 mt-auto bg-slate-900/60 backdrop-blur-2xl border border-white/5 p-8 rounded-3xl shadow-[0_-20px_50px_rgba(0,0,0,0.5)]">
        <div class="max-w-7xl mx-auto space-y-6">
            <div class="flex flex-wrap justify-between items-end gap-6">
                <div class="flex flex-col">
                    <span class="text-[9px] text-cyan-500 font-black uppercase tracking-[5px]">Chronos Engine :: Simulation Layer</span>
                    <span class="text-2xl font-mono text-white italic tracking-tighter">
                        {{ isTimeTraveling ? 'HISTORY_BUFFER_SYNC' : 'REAL_TIME_STREAM_LIVE' }}
                    </span>
                </div>
                <div class="text-right flex flex-col items-end gap-1">
                    <div class="flex items-center gap-3">
                        <span class="text-[9px] text-slate-600 font-mono italic">BUFFER_DEPTH</span>
                        <span class="text-sm font-mono text-cyan-500">{{ currentSnapshotIndex }}</span>
                    </div>
                    <p class="text-[10px] font-mono text-slate-400 uppercase tracking-widest">{{ currentSnapshotTimestamp }}</p>
                </div>
            </div>
            
            <div class="relative py-2 group">
                <div class="absolute -top-1 left-0 w-full h-px bg-white/5"></div>
                <input 
                    type="range" 
                    min="0" 
                    :max="(historySnapshots.length || 1) - 1" 
                    v-model="currentSnapshotIndex"
                    class="w-full h-1 bg-slate-800 rounded-full appearance-none cursor-pointer accent-cyan-500 hover:accent-cyan-400 transition-all"
                >
            </div>
        </div>
    </footer>

    <!-- Technical Specs Modal -->
    <div v-if="showSpecs" class="fixed inset-0 z-[100] flex items-center justify-center p-6 bg-slate-950/95 backdrop-blur-3xl">
        <div class="max-w-4xl w-full bg-slate-900 border border-white/10 p-12 shadow-[0_0_100px_rgba(168,85,247,0.15)] relative overflow-hidden rounded-[2.5rem]">
            <button @click="showSpecs = false" class="absolute top-10 right-10 text-slate-500 hover:text-white transition-colors font-mono text-xs tracking-widest">CLOSE_MODULE [X]</button>
            
            <div class="absolute -left-20 -bottom-20 w-64 h-64 bg-purple-500/5 blur-[100px]"></div>
            
            <h2 class="text-4xl font-black text-white uppercase italic tracking-tighter mb-10 border-b border-white/5 pb-6">
                <span class="text-purple-500">ENGINEERING_DOSSIER</span> <span class="text-slate-700 text-2xl mx-4">/</span> TS.24
            </h2>
            <div class="grid grid-cols-1 md:grid-cols-2 gap-12">
                <div class="space-y-8">
                    <div class="group">
                        <h4 class="text-purple-400 text-[10px] font-black uppercase mb-3 tracking-[3px] opacity-60 group-hover:opacity-100 transition-opacity">Graph Adjacency List</h4>
                        <p class="text-slate-400 text-sm leading-relaxed font-mono italic">Optimized for sparse urban topologies. Reduced memory footprint by avoiding O(V²) adjacency matrices.</p>
                    </div>
                    <div class="group">
                        <h4 class="text-purple-400 text-[10px] font-black uppercase mb-3 tracking-[3px] opacity-60 group-hover:opacity-100 transition-opacity">Dijkstra + Priority Heap</h4>
                        <p class="text-slate-400 text-sm leading-relaxed font-mono italic">Real-time traversal at O(E log V). Min-Priority Queue ensures immediate convergence on optimal routes.</p>
                    </div>
                </div>
                <div class="space-y-8">
                    <div class="group">
                        <h4 class="text-purple-400 text-[10px] font-black uppercase mb-3 tracking-[3px] opacity-60 group-hover:opacity-100 transition-opacity">Double Linked History</h4>
                        <p class="text-slate-400 text-sm leading-relaxed font-mono italic">Immutable snapshot recording with O(1) step access. Bidirectional scrubbing enabled via pointer stability.</p>
                    </div>
                    <div class="group">
                        <h4 class="text-purple-400 text-[10px] font-black uppercase mb-3 tracking-[3px] opacity-60 group-hover:opacity-100 transition-opacity">LIFO Navigation Stack</h4>
                        <p class="text-slate-400 text-sm leading-relaxed font-mono italic">Seamless hierarchical UI state management. Decoupled navigation allows O(1) context preservation.</p>
                    </div>
                </div>
            </div>
            
            <div class="mt-12 pt-8 border-t border-white/5 flex justify-between items-center">
                <p class="text-[8px] text-slate-700 font-mono uppercase tracking-[5px]">Security Clearance :: RED_LEVEL</p>
                <div class="flex gap-2">
                    <div class="w-1 h-1 bg-purple-500"></div>
                    <div class="w-1 h-1 bg-purple-500 opacity-50"></div>
                    <div class="w-1 h-1 bg-purple-500 opacity-20"></div>
                </div>
            </div>
        </div>
    </div>

  </div>
</template>

<style>
#app {
  width: 100%;
  height: 100%;
}

.custom-scrollbar::-webkit-scrollbar {
  width: 4px;
}
.custom-scrollbar::-webkit-scrollbar-track {
  background: transparent;
}
.custom-scrollbar::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.05);
  border-radius: 10px;
}
.custom-scrollbar::-webkit-scrollbar-thumb:hover {
  background: rgba(34, 211, 238, 0.2);
}
</style>
