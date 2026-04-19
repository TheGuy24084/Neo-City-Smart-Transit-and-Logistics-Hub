<script setup lang="ts">
import { ref, inject, onMounted, onUnmounted } from 'vue';
import type { useCityEngine } from '../composables/useCityEngine';
import CyberCard from './CyberCard.vue';

const engine = inject<ReturnType<typeof useCityEngine>>('cityEngine');
if (!engine) throw new Error('CityEngine not provided');

const { metrics, hotZones, quickRewind } = engine;

const pulsePath = ref('');
const generatePulse = () => {
    let path = 'M 0 50';
    for (let i = 0; i <= 20; i++) {
        const x = (i / 20) * 300;
        const y = 50 + (Math.random() - 0.5) * 40;
        path += ` L ${x} ${y}`;
    }
    pulsePath.value = path;
};

let pulseInterval: any;
onMounted(() => {
    generatePulse();
    pulseInterval = setInterval(generatePulse, 2000);
});

onUnmounted(() => {
    clearInterval(pulseInterval);
});
</script>

<template>
    <CyberCard variant="purple" class="flex-1">
        <template #title>04. PREDICTIVE FLOW ANALYSIS</template>
        <template #header-suffix>
            <span class="text-[7px] font-mono text-purple-400 bg-purple-400/10 px-2 py-0.5 rounded-full border border-purple-400/20">LIVE_FLUX</span>
        </template>

        <div class="flex flex-col h-full gap-6">
            <!-- Waveform Animation -->
            <div class="h-24 bg-slate-950/50 rounded-xl border border-white/5 relative overflow-hidden group">
                <svg viewBox="0 0 300 100" class="w-full h-full preserve-3d">
                    <defs>
                        <linearGradient id="flux-grad" x1="0%" y1="0%" x2="100%" y2="0%">
                            <stop offset="0%" stop-color="#a855f7" />
                            <stop offset="100%" stop-color="#10b981" />
                        </linearGradient>
                    </defs>
                    <path 
                        :d="pulsePath" 
                        fill="none" 
                        stroke="url(#flux-grad)" 
                        stroke-width="2" 
                        class="transition-all duration-1000 ease-in-out"
                    />
                </svg>
                <div class="absolute inset-0 bg-gradient-to-t from-slate-950 to-transparent opacity-40"></div>
                <div class="absolute top-2 left-2 flex flex-col">
                    <span class="text-[7px] font-black text-purple-400 uppercase tracking-widest">System_Pulse</span>
                    <span class="text-[10px] font-mono text-white">{{ metrics.congestionIndex.toFixed(1) }}% LOAD</span>
                </div>
            </div>

            <!-- Hot-Zones -->
            <div class="flex-1 space-y-4">
                <div class="flex justify-between items-center">
                    <span class="text-[9px] font-black text-slate-500 uppercase tracking-widest">Congestion_Hot_Zones</span>
                    <span class="text-[8px] font-mono text-emerald-400 uppercase">Top_3</span>
                </div>
                
                <div class="space-y-3">
                    <div v-for="(zone, idx) in hotZones" :key="zone.intersectionId" class="flex items-center gap-4 bg-white/5 p-3 rounded-xl border border-white/5 hover:border-purple-500/20 transition-all">
                        <div class="w-8 h-8 rounded-lg bg-slate-950 flex items-center justify-center border border-white/10">
                            <span class="text-[10px] font-black text-purple-400 italic">0{{ idx + 1 }}</span>
                        </div>
                        <div class="flex-1 flex flex-col">
                            <span class="text-[10px] font-bold text-white">{{ zone.name }}</span>
                            <div class="flex items-center gap-2">
                                <div class="flex-1 h-1 bg-slate-800 rounded-full overflow-hidden">
                                    <div class="h-full bg-emerald-500" :style="{ width: `${zone.intensity}%` }"></div>
                                </div>
                                <span class="text-[8px] font-mono text-slate-500">{{ zone.intensity }}%</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Quick Rewind & Disclaimer -->
            <div class="space-y-4">
                <button 
                    @click="quickRewind"
                    class="w-full py-3 bg-gradient-to-r from-purple-600 to-emerald-600 hover:from-purple-500 hover:to-emerald-500 text-white rounded-xl text-[10px] font-black uppercase tracking-widest transition-all shadow-lg flex items-center justify-center gap-3 active:scale-95"
                >
                    <svg class="w-3 h-3" fill="currentColor" viewBox="0 0 24 24"><path d="M11 18V6l-8.5 6 8.5 6zm.5-6l8.5 6V6l-8.5 6z"/></svg>
                    Quick Rewind (-5 Steps)
                </button>
                
                <p class="text-[7px] text-slate-600 font-medium leading-relaxed italic text-center">
                    Neural Link Active - Processing Temporal Snapshots via DLL.
                </p>
            </div>
        </div>
    </CyberCard>
</template>

<style scoped>
.preserve-3d { transform-style: preserve-3d; }
</style>
