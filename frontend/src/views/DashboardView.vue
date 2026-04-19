<script setup lang="ts">
import { onMounted, provide, ref } from 'vue';
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

const handleGuestAction = () => {
    if (authStore.userRole === 'GUEST') {
        showGuestDenial.value = true;
        setTimeout(() => showGuestDenial.value = false, 3000);
    }
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
            @click="handleLogout"
            class="px-4 py-2 bg-red-500/10 hover:bg-red-500/20 border border-red-500/30 text-[10px] text-red-500 uppercase font-black tracking-widest transition-all rounded-lg"
        >
            Logout
        </button>
      </div>
    </header>

    <!-- Main Responsive Grid -->
    <main class="flex-1 grid grid-cols-1 lg:grid-cols-12 gap-8 items-start relative">
      
      <!-- Left Sidebar (Command Center) -->
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
                    
                    <div class="flex flex-col gap-3">
                        <span class="text-[9px] text-emerald-500/50 uppercase font-bold tracking-widest">FIFO Priority Queue</span>
                        <div class="flex flex-col gap-2">
                            <div v-for="(car, idx) in activeQueue.cars" :key="car.id" class="p-3 border border-white/5 bg-white/5 rounded-lg text-[10px] font-mono flex justify-between items-center">
                                <span class="text-slate-500">CAR_{{ car.id }}</span>
                                <span class="text-white opacity-80 uppercase">{{ car.model }}</span>
                            </div>
                        </div>
                    </div>
                </div>
                <div v-else class="p-8 border border-dashed border-slate-800 text-center opacity-40 rounded-xl">
                    <p class="text-[10px] text-slate-500 uppercase tracking-[0.2em]">Select Intersection Node</p>
                </div>
            </CyberCard>
        </div>
      </aside>

      <!-- Center Content (Map) -->
      <section class="lg:col-span-6 flex flex-col gap-6 h-full min-h-[500px] lg:min-h-[750px]">
        <MapCanvas />
      </section>

      <!-- Right Sidebar (Metrics & Stats) -->
      <aside class="lg:col-span-3 flex flex-col gap-6">
        <div class="relative group cursor-pointer" @click="handleGuestAction">
            <CyberCard variant="purple">
                <template #title>01. Metrics Engine</template>
                <template #header-suffix>
                    <span v-if="authStore.userRole === 'GUEST'" class="px-2 py-0.5 bg-slate-500/20 border border-white/10 text-[7px] text-slate-400 rounded-full font-mono font-black">VIEW_ONLY</span>
                </template>
                <div class="space-y-6">
                    <div class="space-y-3">
                        <div class="flex justify-between items-end">
                            <span class="text-[9px] text-purple-400/50 uppercase font-bold tracking-widest">Graph Traversal</span>
                            <span class="text-[9px] font-mono text-purple-500 italic">SECURE_LEVEL</span>
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
                    
                    <div v-if="authStore.userRole === 'GUEST'" class="bg-slate-950/80 p-6 border border-white/5 rounded-2xl text-center">
                        <p class="text-[8px] font-mono text-slate-600 uppercase tracking-widest leading-relaxed">
                            System metrics are encrypted for restricted clearance accounts. <br/> Access level 0x05 required.
                        </p>
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
                    <div class="flex flex-wrap gap-1.5 pt-4 border-t border-white/5">
                        <span v-for="id in shortestPath" :key="id" class="px-2 py-1 bg-white/5 border border-white/5 text-[8px] font-mono text-slate-400 rounded">
                            #{{ id }}
                        </span>
                    </div>
                </div>
            </CyberCard>
        </transition>
      </aside>
    </main>

    <!-- Footer -->
    <footer class="bg-slate-900/40 backdrop-blur-2xl border border-white/5 p-8 rounded-3xl mt-auto z-40">
        <div class="max-w-7xl mx-auto flex justify-between items-center">
            <div class="flex flex-col">
                <span class="text-[9px] text-cyan-500 font-black uppercase tracking-[5px]">NEO_Simulation :: Layer_07</span>
                <span class="text-xs font-mono text-slate-500 uppercase">{{ currentSnapshotTimestamp }}</span>
            </div>
            <div class="flex items-center gap-6">
                <div class="w-2 h-2 rounded-full bg-emerald-500 animate-pulse shadow-[0_0_10px_rgba(16,185,129,1)]"></div>
                <span class="text-[9px] text-white font-black uppercase tracking-widest group">System_Optimal</span>
            </div>
        </div>
    </footer>
  </div>
</template>

<style scoped>
.side-slide-enter-active, .side-slide-leave-active {
  transition: all 0.6s cubic-bezier(0.16, 1, 0.3, 1);
}
.side-slide-enter-from { opacity: 0; transform: translateX(50px); }
.side-slide-leave-to { opacity: 0; transform: translateX(50px); }

.custom-scrollbar::-webkit-scrollbar { width: 4px; }
.custom-scrollbar::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.05); border-radius: 10px; }
.custom-scrollbar::-webkit-scrollbar-thumb:hover { background: rgba(34, 211, 238, 0.2); }
</style>
