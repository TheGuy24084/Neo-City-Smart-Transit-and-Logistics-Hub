<script setup lang="ts">
import { onMounted, provide, ref, watch } from 'vue';
import MapCanvas from '../components/MapCanvas.vue';
import CyberCard from '../components/CyberCard.vue';
import CommandCenter from '../components/CommandCenter.vue';
import { useCityEngine } from '../composables/useCityEngine';
import { useAuthStore } from '../stores/auth';
import { useRouter } from 'vue-router';

const engine = useCityEngine();
provide('cityEngine', engine);

const authStore = useAuthStore();
const router = useRouter();

const showWelcome = ref(true);
const showGuestDenial = ref(false);
const activeToast = ref<{ message: string, type: 'error' | 'info' } | null>(null);

const {
    navigationStack, metrics, activeQueue, 
    isSearching, errorNotification, showSpecs,
    shortestPath, totalDistance, popTo, 
    initHistory
} = engine;

const handleLogout = () => {
    authStore.logout();
    router.push('/login');
};

const handleGuestAction = () => {
    if (authStore.userRole === 'GUEST') {
        showGuestDenial.value = true;
        setTimeout(() => showGuestDenial.value = false, 3000);
    }
};

// Watch for engine errors to show toast
watch(errorNotification, (newVal) => {
    if (newVal) {
        activeToast.value = { message: newVal, type: 'error' };
        setTimeout(() => activeToast.value = null, 4000);
    }
});

onMounted(() => {
    engine.initMap();
    initHistory();
    setTimeout(() => showWelcome.value = false, 5000);
});
</script>

<template>
  <div class="min-h-screen bg-slate-950 font-sans flex flex-col p-4 lg:p-10 gap-8 overflow-y-auto custom-scrollbar relative">
    
    <!-- Tech Specs Modal (Restored) -->
    <transition name="fade">
        <div v-if="showSpecs" class="fixed inset-0 z-[200] flex items-center justify-center p-6 bg-slate-950/90 backdrop-blur-3xl">
            <div class="max-w-2xl w-full bg-slate-900 border border-cyan-500/30 rounded-3xl overflow-hidden shadow-[0_0_100px_rgba(34,211,238,0.2)]">
                <div class="p-8 border-b border-white/5 flex justify-between items-center">
                    <div>
                        <h2 class="text-2xl font-black text-white uppercase italic tracking-tighter">Engineering_Dossier</h2>
                        <p class="text-[9px] font-mono text-cyan-500/60 uppercase tracking-widest mt-1">Algorithm_Complexity_Metrics</p>
                    </div>
                    <button @click="showSpecs = false" class="w-10 h-10 rounded-full border border-white/10 flex items-center justify-center text-slate-500 hover:text-white transition-all">✕</button>
                </div>
                <div class="p-8 grid grid-cols-1 md:grid-cols-2 gap-8">
                    <div class="p-6 bg-slate-950/50 rounded-2xl border border-white/5 hover:border-cyan-500/20 transition-all group">
                        <span class="text-[9px] font-black text-cyan-500 uppercase tracking-widest block mb-4">Path_Optimizer</span>
                        <h3 class="text-white font-bold text-lg mb-2 italic">Dijkstra's Engine</h3>
                        <p class="text-slate-400 text-xs leading-relaxed mb-4 font-medium italic">Calculates shortest hub-to-hub transit routes with high precision.</p>
                        <div class="px-3 py-2 bg-cyan-500/10 rounded-lg border border-cyan-500/20 inline-flex items-center gap-3">
                            <span class="text-[8px] font-black text-cyan-400 uppercase">Worst_Case:</span>
                            <span class="text-xs font-mono font-bold text-white tracking-widest">O(E log V)</span>
                        </div>
                    </div>
                    
                    <div class="p-6 bg-slate-950/50 rounded-2xl border border-white/5 hover:border-purple-500/20 transition-all group">
                        <span class="text-[9px] font-black text-purple-400 uppercase tracking-widest block mb-4">Traffic_Controller</span>
                        <h3 class="text-white font-bold text-lg mb-2 italic">FIFO Scheduling</h3>
                        <p class="text-slate-400 text-xs leading-relaxed mb-4 font-medium italic">Manages real-time vehicle entry prioritized by neural timestamp.</p>
                        <div class="px-3 py-2 bg-purple-500/10 rounded-lg border border-purple-500/20 inline-flex items-center gap-3">
                            <span class="text-[8px] font-black text-purple-400 uppercase">Complexity:</span>
                            <span class="text-xs font-mono font-bold text-white tracking-widest">O(1)</span>
                        </div>
                    </div>

                    <div class="p-6 bg-slate-950/50 rounded-2xl border border-white/5 hover:border-emerald-500/20 transition-all group">
                        <span class="text-[9px] font-black text-emerald-400 uppercase tracking-widest block mb-4">Temporal_Persistence</span>
                        <h3 class="text-white font-bold text-lg mb-2 italic">Time Travel DB</h3>
                        <p class="text-slate-400 text-xs leading-relaxed mb-4 font-medium italic">Uses doubly-linked snapshots for instantaneous history state access.</p>
                        <div class="px-3 py-2 bg-emerald-500/10 rounded-lg border border-emerald-500/20 inline-flex items-center gap-3">
                            <span class="text-[8px] font-black text-emerald-400 uppercase">Access_Lag:</span>
                            <span class="text-xs font-mono font-bold text-white tracking-widest">O(1)</span>
                        </div>
                    </div>

                    <div class="p-6 bg-slate-950/50 rounded-2xl border border-white/5 hover:border-amber-500/20 transition-all group">
                        <span class="text-[9px] font-black text-amber-500 uppercase tracking-widest block mb-4">Data_Structure</span>
                        <h3 class="text-white font-bold text-lg mb-2 italic">Graph Adjacency</h3>
                        <p class="text-slate-400 text-xs leading-relaxed mb-4 font-medium italic">Hyper-efficient storage of topological hub connections.</p>
                        <div class="px-3 py-2 bg-amber-500/10 rounded-lg border border-amber-500/20 inline-flex items-center gap-3">
                            <span class="text-[8px] font-black text-amber-400 uppercase">Memory:</span>
                            <span class="text-xs font-mono font-bold text-white tracking-widest">O(V + E)</span>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </transition>

    <!-- Toast Notification -->
    <transition name="side-slide">
        <div v-if="activeToast" class="fixed bottom-10 left-1/2 -translate-x-1/2 z-[150] min-w-[320px] p-[1px] bg-gradient-to-r from-red-500/50 to-red-400/50 rounded-xl overflow-hidden shadow-[0_0_50px_rgba(239,68,68,0.3)]">
            <div class="bg-slate-950 p-4 rounded-[11px] flex items-center gap-4 border border-red-500/10">
                <div class="w-8 h-8 rounded-lg bg-red-500/20 flex items-center justify-center text-red-500 font-black text-sm">!</div>
                <div class="flex flex-col">
                    <span class="text-[8px] font-black uppercase tracking-[0.2em] text-red-400">System_Alert</span>
                    <span class="text-white text-xs font-bold leading-tight uppercase">{{ activeToast.message }}</span>
                </div>
                <button @click="activeToast = null" class="ml-auto text-slate-600 hover:text-white transition-colors">✕</button>
            </div>
        </div>
    </transition>

    <!-- Welcome Notification -->
    <transition name="side-slide">
        <div v-if="showWelcome" class="fixed top-10 right-10 z-[100] bg-cyan-500 p-6 rounded-2xl shadow-[0_20px_60px_rgba(34,211,238,0.4)] flex items-center gap-6 border border-white/20">
            <div class="w-12 h-12 bg-slate-950 rounded-xl flex items-center justify-center text-cyan-400 font-black text-xl italic select-none">!</div>
            <div class="flex flex-col">
                <p class="text-[8px] font-black uppercase tracking-[0.3em] text-slate-950 mb-1">Neural_Link_Established</p>
                <p class="text-slate-950 font-bold text-lg leading-tight">Welcome, {{ authStore.operatorName }}</p>
                <p class="text-[9px] font-mono text-slate-950/60 uppercase mt-1 italic">{{ authStore.userRole }}_CLEARANCE_ACTIVE</p>
            </div>
        </div>
    </transition>

    <!-- Guest Denial Popup -->
    <transition name="side-slide">
        <div v-if="showGuestDenial" class="fixed bottom-10 right-10 z-[100] bg-red-500 p-6 rounded-2xl shadow-2xl flex items-center gap-6 border border-white/20">
            <div class="w-12 h-12 bg-slate-950 rounded-xl flex items-center justify-center text-red-500 font-black text-xl italic select-none">✕</div>
            <div class="flex flex-col">
                <p class="text-[8px] font-black uppercase tracking-[0.3em] text-slate-950 mb-1">Access_Restricted</p>
                <p class="text-slate-950 font-bold text-lg leading-tight">Administrative Privileges Req.</p>
                <p class="text-[9px] font-mono text-slate-950/60 uppercase mt-1 italic">Guest mode restricted to observer status.</p>
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
            @click="showSpecs = true"
            class="px-4 py-2 bg-cyan-500/10 hover:bg-cyan-500/20 border border-cyan-500/30 text-[10px] text-cyan-400 uppercase font-black tracking-widest transition-all rounded-lg"
        >
            Tech_Specs
        </button>
        <button 
            @click="handleLogout"
            class="px-4 py-2 bg-red-500/10 hover:bg-red-500/20 border border-red-500/30 text-[10px] text-red-500 uppercase font-black tracking-widest transition-all rounded-lg"
        >
            Logout
        </button>
      </div>
    </header>

    <!-- Main Grid -->
    <main class="flex-1 grid grid-cols-1 lg:grid-cols-12 gap-8 items-start relative">
      <aside class="lg:col-span-3 flex flex-col gap-6">
        <CommandCenter />
        
        <div class="relative group cursor-pointer" @click="handleGuestAction">
            <CyberCard variant="emerald">
                <template #title>02. Live Traffic Control</template>
                <template #header-suffix>
                    <span v-if="authStore.userRole === 'GUEST'" class="px-2 py-0.5 bg-slate-500/20 border border-white/10 text-[7px] text-slate-400 rounded-full font-mono font-black">VIEW_ONLY</span>
                </template>

                <div v-if="activeQueue" class="space-y-6">
                    <div class="flex justify-between items-center bg-white/5 p-4 border border-white/5 rounded-lg">
                        <span class="text-[9px] text-slate-500 uppercase font-bold tracking-widest">Intersection</span>
                        <span class="text-xl font-mono text-white tracking-tighter italic">#{{ activeQueue.intersectionId }}</span>
                    </div>
                </div>
                <div v-else class="p-8 border border-dashed border-slate-800 text-center opacity-40 rounded-xl">
                    <p class="text-[10px] text-slate-500 uppercase tracking-[0.2em]">Select Intersection Node</p>
                </div>
            </CyberCard>
        </div>
      </aside>

      <section class="lg:col-span-6 flex flex-col gap-6 h-full min-h-[500px] lg:min-h-[750px]">
        <MapCanvas />
      </section>

      <aside class="lg:col-span-3 flex flex-col gap-6">
        <div class="relative group cursor-pointer" @click="handleGuestAction">
            <CyberCard variant="purple">
                <template #title>01. Metrics Engine</template>
                <div class="space-y-6">
                    <div class="space-y-3">
                        <div class="flex justify-between items-end">
                            <span class="text-[9px] text-purple-400/50 uppercase font-bold tracking-widest">Graph Traversal</span>
                        </div>
                        <div class="flex items-center gap-4">
                            <div class="flex-1 h-1.5 bg-slate-800 rounded-full overflow-hidden">
                                <div 
                                    class="h-full bg-gradient-to-r from-purple-600 to-purple-400 transition-all duration-1000"
                                    :style="{ width: authStore.userRole === 'GUEST' ? '0%' : '65%' }"
                                ></div>
                            </div>
                            <span class="text-xs font-mono text-white leading-none">{{ authStore.userRole === 'GUEST' ? '???' : metrics.nodesVisited }}</span>
                        </div>
                    </div>
                </div>
            </CyberCard>
        </div>

        <transition name="side-slide">
            <CyberCard v-if="shortestPath.length > 0" variant="cyan">
                <template #title>Route Overview</template>
                <div class="space-y-4">
                    <div class="flex flex-col">
                        <span class="text-[9px] text-cyan-500/40 uppercase font-bold tracking-widest">Total Path Metric</span>
                        <span class="text-4xl font-mono text-white tracking-tighter italic">
                            {{ totalDistance }}<span class="text-sm text-cyan-500 font-normal ml-2">UNITS</span>
                        </span>
                    </div>
                </div>
            </CyberCard>
        </transition>
      </aside>
    </main>
  </div>
</template>

<style scoped>
.fade-enter-active, .fade-leave-active {
  transition: opacity 0.4s ease;
}
.fade-enter-from, .fade-leave-to {
  opacity: 0;
}

.side-slide-enter-active, .side-slide-leave-active {
  transition: all 0.6s cubic-bezier(0.16, 1, 0.3, 1);
}
.side-slide-enter-from { opacity: 0; transform: translate(50px, 0); }
.side-slide-leave-to { opacity: 0; transform: translate(50px, 0); }

.custom-scrollbar::-webkit-scrollbar { width: 4px; }
.custom-scrollbar::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.05); border-radius: 10px; }
</style>
