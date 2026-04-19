<script setup lang="ts">
import { onMounted, provide, ref } from 'vue';
import MapCanvas from '../components/MapCanvas.vue';
import CyberCard from '../components/CyberCard.vue';
import { useCityEngine } from '../composables/useCityEngine';
import { useAuthStore } from '../stores/auth';
import { useRouter } from 'vue-router';

const engine = useCityEngine();
provide('cityEngine', engine);

const authStore = useAuthStore();
const router = useRouter();

const showWelcome = ref(true);

const {
    navigationStack, metrics, activeQueue, showSpecs, 
    isStressTesting, districtSummary, historySnapshots,
    currentSnapshotIndex, isTimeTraveling, currentSnapshotTimestamp,
    totalDistance, shortestPath, popTo, popView, runStressTest,
    initHistory
} = engine;

const handleLogout = () => {
    authStore.logout();
    router.push('/login');
};

onMounted(() => {
    engine.initMap();
    initHistory();
    setTimeout(() => showWelcome.value = false, 5000);
});
</script>

<template>
  <div class="min-h-screen bg-slate-950 font-sans selection:bg-cyan-500/30 flex flex-col p-4 lg:p-10 gap-8 overflow-y-auto custom-scrollbar relative">
    
    <!-- Welcome Notification -->
    <transition name="notif">
        <div v-if="showWelcome" class="fixed top-10 right-10 z-[60] bg-cyan-500 p-6 rounded-2xl shadow-[0_20px_60px_rgba(34,211,238,0.4)] flex items-center gap-6 border border-white/20">
            <div class="w-12 h-12 bg-slate-950 rounded-xl flex items-center justify-center text-cyan-400 font-black text-xl italic select-none">!</div>
            <div class="flex flex-col">
                <p class="text-[8px] font-black uppercase tracking-[0.3em] text-slate-950 mb-1">Neural_Link_Established</p>
                <p class="text-slate-950 font-bold text-lg leading-tight">Welcome, {{ authStore.operatorName }}</p>
                <p class="text-[9px] font-mono text-slate-950/60 uppercase mt-1 italic">{{ authStore.userRole }}_CLEARANCE_ACTIVE</p>
            </div>
        </div>
    </transition>

    <!-- Header -->
    <header class="w-full flex flex-col lg:flex-row justify-between items-center gap-6 z-40 bg-slate-900/40 backdrop-blur-xl border border-white/5 p-8 rounded-2xl shadow-2xl">
      <div class="flex flex-col">
        <h1 class="text-4xl font-black tracking-tighter text-white uppercase italic leading-none">
          Neo-Sector <span class="text-cyan-400">07</span>
        </h1>
        <div class="flex items-center gap-4 mt-3">
            <span class="text-[8px] font-mono text-cyan-500/60 uppercase tracking-widest bg-cyan-500/5 px-2 py-1 rounded border border-cyan-500/10">
                {{ authStore.userRole === 'GUEST' ? 'GUEST_MODE' : `ID: ${authStore.operatorId}` }}
            </span>
            <span class="text-slate-800 text-[10px]">|</span>
            <nav class="flex items-center gap-2">
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
        <button 
            @click="handleLogout"
            class="px-4 py-2 bg-red-500/10 hover:bg-red-500/20 border border-red-500/30 text-[10px] text-red-500 uppercase font-black tracking-widest transition-all rounded-lg"
        >
            Logout
        </button>
      </div>
    </header>

    <!-- Main Responsive Grid -->
    <main class="flex-1 grid grid-cols-1 lg:grid-cols-12 gap-8 items-start">
      
      <!-- Left Sidebar (Live Traffic) -->
      <aside class="lg:col-span-3 flex flex-col gap-6">
        <div class="relative group">
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
            
            <!-- Guest Lock Overlay -->
            <div v-if="authStore.userRole === 'GUEST'" class="absolute inset-0 z-50 bg-slate-950/60 backdrop-blur-[2px] rounded-[2rem] flex flex-col items-center justify-center gap-4 transition-all opacity-0 group-hover:opacity-100 cursor-not-allowed">
                <div class="w-12 h-12 rounded-full bg-slate-900 border border-white/10 flex items-center justify-center text-xl shadow-2xl">🔒</div>
                <p class="text-[9px] font-mono text-white uppercase tracking-[0.3em] font-black">Admin_Access_Only</p>
                <p class="text-[8px] text-slate-400 max-w-[150px] text-center uppercase tracking-widest italic opacity-60">Control modules restricted for observer status.</p>
            </div>
        </div>

        <div class="relative group">
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
                        :disabled="isStressTesting || authStore.userRole === 'GUEST'"
                        class="w-full py-3 bg-amber-500/5 hover:bg-amber-500/10 border border-amber-500/20 text-[9px] text-amber-500 uppercase font-black tracking-[3px] transition-all disabled:opacity-30 rounded-lg"
                    >
                        {{ isStressTesting ? 'SIMULATING...' : 'Launch Stress Test' }}
                    </button>
                </div>
            </CyberCard>
        </div>
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
        <div class="relative group">
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
                        </div>
                    </div>
                </div>
            </CyberCard>
            
            <!-- Guest Lock Overlay -->
            <div v-if="authStore.userRole === 'GUEST'" class="absolute inset-0 z-50 bg-slate-950/60 backdrop-blur-[2px] rounded-[2rem] flex flex-col items-center justify-center gap-4 transition-all opacity-0 group-hover:opacity-100 cursor-not-allowed">
                <div class="w-10 h-10 rounded-full bg-slate-900 border border-white/10 flex items-center justify-center text-lg">🔒</div>
                <p class="text-[9px] font-mono text-white uppercase tracking-[0.2em] font-black">Metrics_Restricted</p>
            </div>
        </div>

        <CyberCard v-if="shortestPath.length > 0" variant="cyan">
            <template #title>Navigation Stats</template>
            <div class="space-y-4">
                <div class="flex flex-col">
                    <span class="text-[9px] text-cyan-500/40 uppercase font-bold tracking-widest">Total Path Metric</span>
                    <span class="text-4xl font-mono text-white tracking-tighter italic">
                        {{ totalDistance }}<span class="text-sm text-cyan-500 font-normal ml-2">Units</span>
                    </span>
                </div>
            </div>
        </CyberCard>
      </aside>
    </main>

    <!-- Sticky Footer -->
    <footer class="sticky bottom-0 z-40 mt-auto bg-slate-900/40 backdrop-blur-2xl border border-white/5 p-8 rounded-3xl">
        <div class="max-w-7xl mx-auto flex justify-between items-center">
            <span class="text-[9px] text-cyan-500 font-black uppercase tracking-[5px]">Chronos Engine :: Simulation Layer</span>
            <p class="text-[10px] font-mono text-slate-400 uppercase tracking-widest">{{ currentSnapshotTimestamp }}</p>
        </div>
    </footer>

    <!-- Technical Specs Modal (Same as before) -->
    <div v-if="showSpecs" class="fixed inset-0 z-[100] flex items-center justify-center p-6 bg-slate-950/95 backdrop-blur-3xl">
        ...
    </div>

  </div>
</template>

<style scoped>
.notif-enter-active, .notif-leave-active {
  transition: all 0.5s cubic-bezier(0.16, 1, 0.3, 1);
}
.notif-enter-from { opacity: 0; transform: translateY(-40px) scale(0.9); }
.notif-leave-to { opacity: 0; transform: translateY(-20px) scale(0.95); }

.custom-scrollbar::-webkit-scrollbar { width: 4px; }
.custom-scrollbar::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.05); border-radius: 10px; }
.custom-scrollbar::-webkit-scrollbar-thumb:hover { background: rgba(34, 211, 238, 0.2); }
</style>
