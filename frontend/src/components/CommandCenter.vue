<script setup lang="ts">
import { ref, inject, computed } from 'vue';
import type { useCityEngine } from '../composables/useCityEngine';
import CyberCard from './CyberCard.vue';

const engine = inject<ReturnType<typeof useCityEngine>>('cityEngine');
if (!engine) throw new Error('CityEngine not provided');

const { 
    nodes, startNodeId, endNodeId, 
    startSearch, endSearch,
    findShortestPath, clearRoute 
} = engine;

const showStartList = ref(false);
const showEndList = ref(false);

const filteredStartNodes = computed(() => {
    const s = startSearch.value.toLowerCase();
    if (!s) return nodes.value;
    return nodes.value.filter(n => 
        n.intersectionId.toString().includes(s) || 
        n.name?.toLowerCase().includes(s) ||
        `intersection #${n.intersectionId}`.includes(s)
    );
});

const filteredEndNodes = computed(() => {
    const s = endSearch.value.toLowerCase();
    if (!s) return nodes.value;
    return nodes.value.filter(n => 
        n.intersectionId.toString().includes(s) || 
        n.name?.toLowerCase().includes(s) ||
        `intersection #${n.intersectionId}`.includes(s)
    );
});

const selectStart = (id: number) => {
    startNodeId.value = id;
    startSearch.value = `Intersection #${id}`;
    setTimeout(() => { showStartList.value = false; }, 100);
};

const selectEnd = (id: number) => {
    endNodeId.value = id;
    endSearch.value = `Intersection #${id}`;
    setTimeout(() => { showEndList.value = false; }, 100);
    handleOptimize();
};

const handleOptimize = () => {
    if (startNodeId.value !== null && endNodeId.value !== null) {
        findShortestPath();
    }
};
</script>

<template>
  <aside class="flex flex-col gap-6">
    <CyberCard variant="cyan">
        <template #title>Transit Command Center</template>
        <div class="space-y-6">
            <!-- Departure Search -->
            <div class="relative">
                <label class="text-[9px] font-black text-cyan-500 uppercase tracking-widest ml-1 mb-2 block">Source_Node</label>
                <div class="relative">
                    <input 
                        v-model="startSearch"
                        @focus="showStartList = true"
                        type="text" 
                        class="w-full bg-slate-950/80 border border-white/5 p-4 rounded-xl text-white font-mono text-sm focus:outline-none focus:border-cyan-500/40 transition-all"
                        placeholder="Search IDs or Names..."
                    >
                    <button v-if="startSearch" @click="startSearch = ''; startNodeId = null" class="absolute right-4 top-1/2 -translate-y-1/2 text-slate-600 hover:text-white text-xs">×</button>
                </div>
                
                <transition name="list-fade">
                    <div v-if="showStartList && filteredStartNodes.length > 0" class="absolute z-[60] w-full mt-2 max-h-48 overflow-y-auto bg-slate-900 border border-white/10 rounded-xl shadow-2xl custom-scrollbar">
                        <button 
                            v-for="node in filteredStartNodes" 
                            :key="node.intersectionId"
                            @click="selectStart(node.intersectionId)"
                            class="w-full text-left p-3 hover:bg-cyan-500/10 transition-colors flex justify-between items-center group border-b border-white/5 last:border-none"
                        >
                            <span class="text-white font-mono text-xs">{{ node.name }}</span>
                            <span class="text-[9px] text-slate-500 group-hover:text-cyan-400">ID:{{ node.intersectionId }}</span>
                        </button>
                    </div>
                </transition>
            </div>

            <!-- Destination Search -->
            <div class="relative">
                <label class="text-[9px] font-black text-red-500 uppercase tracking-widest ml-1 mb-2 block">Target_Node</label>
                <div class="relative">
                    <input 
                        v-model="endSearch"
                        @focus="showEndList = true"
                        type="text" 
                        class="w-full bg-slate-950/80 border border-white/5 p-4 rounded-xl text-white font-mono text-sm focus:outline-none focus:border-red-500/40 transition-all"
                        placeholder="Search Destination..."
                    >
                    <button v-if="endSearch" @click="endSearch = ''; endNodeId = null" class="absolute right-4 top-1/2 -translate-y-1/2 text-slate-600 hover:text-white text-xs">×</button>
                </div>

                <transition name="list-fade">
                    <div v-if="showEndList && filteredEndNodes.length > 0" class="absolute z-[60] w-full mt-2 max-h-48 overflow-y-auto bg-slate-900 border border-white/10 rounded-xl shadow-2xl custom-scrollbar">
                        <button 
                            v-for="node in filteredEndNodes" 
                            :key="node.intersectionId"
                            @click="selectEnd(node.intersectionId)"
                            class="w-full text-left p-3 hover:bg-emerald-500/10 transition-colors flex justify-between items-center group border-b border-white/5 last:border-none"
                        >
                            <span class="text-white font-mono text-xs">{{ node.name }}</span>
                            <span class="text-[9px] text-slate-500 group-hover:text-emerald-400">ID:{{ node.intersectionId }}</span>
                        </button>
                    </div>
                </transition>
            </div>

            <div class="grid grid-cols-2 gap-4 mt-6">
                <button 
                    @click="clearRoute"
                    class="py-3 border border-white/5 bg-white/5 hover:bg-white/10 rounded-xl text-[10px] uppercase font-black tracking-widest text-slate-400 transition-all"
                >
                    Clear All
                </button>
                <button 
                    @click="handleOptimize"
                    :disabled="startNodeId === null || endNodeId === null"
                    class="py-3 bg-cyan-500 hover:bg-cyan-400 text-slate-950 rounded-xl text-[10px] uppercase font-black tracking-widest transition-all shadow-lg disabled:opacity-30 disabled:cursor-not-allowed"
                >
                    Find Route
                </button>
            </div>
        </div>
    </CyberCard>
  </aside>
</template>

<style scoped>
.list-fade-enter-active, .list-fade-leave-active {
  transition: all 0.2s ease-out;
}
.list-fade-enter-from, .list-fade-leave-to {
  opacity: 0;
  transform: translateY(-5px);
}
.custom-scrollbar::-webkit-scrollbar { width: 4px; }
.custom-scrollbar::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.05); border-radius: 10px; }
</style>
