<script setup lang="ts">
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { useAuthStore } from '../stores/auth';

const router = useRouter();
const authStore = useAuthStore();

const username = ref('admin');
const password = ref('');
const isError = ref(false);
const isBooting = ref(false);
const bootProgress = ref(0);

const handleLogin = async () => {
    isError.value = false;
    
    // Simulating API latency and UX touches
    if (username.value === 'admin' && password.value === 'neocity2026') {
        isBooting.value = true;
        
        // Progress bar simulation
        const interval = setInterval(() => {
            bootProgress.value += 2;
            if (bootProgress.value >= 100) {
                clearInterval(interval);
                authStore.login('NEO_TOKEN_INIT_77', 'Admin');
                router.push('/');
            }
        }, 30);
    } else {
        isError.value = true;
        setTimeout(() => isError.value = false, 500);
    }
};
</script>

<template>
  <div class="min-h-screen w-full bg-slate-950 flex items-center justify-center p-6 relative overflow-hidden font-sans">
    
    <!-- Matrix/Blurred Background Overlay -->
    <div class="absolute inset-0 z-0">
        <div class="absolute inset-0 bg-[url('https://images.unsplash.com/photo-1605142859862-978be7eba909?q=80&w=2070&auto=format&fit=crop')] bg-cover bg-center blur-2xl opacity-20 contrast-150 grayscale"></div>
        <div class="absolute inset-0 bg-gradient-to-br from-slate-950 via-slate-950/90 to-cyan-950/20"></div>
        
        <!-- Animated Scanlines -->
        <div class="absolute inset-0 pointer-events-none bg-[linear-gradient(rgba(18,16,16,0)_50%,rgba(0,0,0,0.25)_50%),linear-gradient(90deg,rgba(255,0,0,0.06),rgba(0,255,0,0.02),rgba(0,0,255,0.06))] z-10 bg-[length:100%_4px,3px_100%]"></div>
    </div>

    <!-- Login Card -->
    <div 
        class="max-w-md w-full z-20 transition-all duration-500"
        :class="[isError ? 'animate-shake' : '', isBooting ? 'scale-95 opacity-50 blur-sm pointer-events-none' : '']"
    >
        <div class="bg-slate-900/60 backdrop-blur-3xl border border-white/10 p-10 rounded-[2.5rem] shadow-[0_0_80px_rgba(0,0,0,0.5)] relative overflow-hidden group">
            
            <!-- Tech Decorative Elements -->
            <div class="absolute top-0 right-0 w-32 h-32 bg-cyan-500/5 blur-[60px]"></div>
            <div class="absolute -left-10 -bottom-10 w-40 h-40 bg-purple-500/5 blur-[80px]"></div>
            
            <div class="text-center mb-10">
                <h1 class="text-4xl font-black italic tracking-tighter text-white uppercase leading-none mb-2">
                    NEO<span class="text-cyan-400">CITY</span>
                </h1>
                <p class="text-[10px] font-mono tracking-[0.4em] text-slate-500 uppercase">Secure Infrastructure Access</p>
            </div>

            <form @submit.prevent="handleLogin" class="space-y-6">
                <div class="space-y-2">
                    <label class="text-[10px] font-black uppercase tracking-widest text-slate-400 ml-1">Operator_ID</label>
                    <input 
                        v-model="username"
                        type="text" 
                        class="w-full bg-slate-950/50 border border-white/5 p-4 rounded-xl text-white font-mono text-sm focus:outline-none focus:border-cyan-500/50 transition-colors"
                        placeholder="SYSTEM_ID"
                    >
                </div>

                <div class="space-y-2">
                    <label class="text-[10px] font-black uppercase tracking-widest text-slate-400 ml-1">Access_Code</label>
                    <input 
                        v-model="password"
                        type="password" 
                        class="w-full bg-slate-950/50 border border-white/5 p-4 rounded-xl text-white font-mono text-sm focus:outline-none focus:border-cyan-500/50 transition-colors"
                        :class="isError ? 'border-red-500/50' : ''"
                        placeholder="••••••••"
                    >
                </div>

                <button 
                    type="submit"
                    class="w-full py-4 bg-cyan-500 text-slate-950 font-black uppercase text-xs tracking-[0.2em] rounded-xl hover:bg-cyan-400 transition-all shadow-[0_20px_40px_rgba(34,211,238,0.2)] hover:shadow-[0_20px_50px_rgba(34,211,238,0.4)] relative overflow-hidden group"
                >
                    <span class="relative z-10 group-hover:scale-110 transition-transform inline-block">Initialize Engine</span>
                    
                    <!-- Flicker Effect -->
                    <div class="absolute inset-0 bg-white opacity-0 group-hover:animate-flicker pointer-events-none"></div>
                </button>
            </form>

            <div class="mt-8 pt-6 border-t border-white/5 flex flex-col gap-2 items-center">
                <p class="text-[8px] font-mono text-slate-600 uppercase tracking-widest italic">Unauthorized access is logged (0x44)</p>
                <div class="flex gap-1">
                    <div class="w-1 h-1 bg-cyan-500"></div>
                    <div class="w-1 h-1 bg-cyan-500 opacity-50"></div>
                </div>
            </div>
        </div>
    </div>

    <!-- System Boot Sequence Overlay -->
    <div v-if="isBooting" class="fixed inset-0 z-[100] flex flex-col items-center justify-center bg-slate-950/80 backdrop-blur-3xl animate-fadeIn">
        <div class="max-w-sm w-full space-y-8 px-10 text-center">
            <div class="space-y-2">
                <h2 class="text-white font-mono text-[10px] uppercase tracking-[0.5em] animate-pulse">Initializing Antigravity Engine...</h2>
                <div class="h-[2px] w-full bg-slate-900 rounded-full overflow-hidden p-[1px]">
                    <div 
                        class="h-full bg-cyan-500 shadow-[0_0_10px_rgba(34,211,238,1)] transition-all duration-300 ease-out"
                        :style="{ width: `${bootProgress}%` }"
                    ></div>
                </div>
            </div>
            
            <div class="grid grid-cols-2 gap-4 text-left">
                <p class="text-[8px] font-mono text-cyan-500/40 uppercase">Memory_Dump [OK]</p>
                <p class="text-[8px] font-mono text-cyan-500/40 uppercase text-right">Graph_Check [OK]</p>
                <p class="text-[8px] font-mono text-cyan-500/40 uppercase">Thread_Sync [OK]</p>
                <p class="text-[8px] font-mono text-cyan-500/40 uppercase text-right">Auth_Grant [OK]</p>
            </div>
        </div>
    </div>

  </div>
</template>

<style scoped>
@keyframes flicker {
  0% { opacity: 0.1; }
  5% { opacity: 0; }
  10% { opacity: 0.15; }
  15% { opacity: 0.05; }
  25% { opacity: 0.1; }
  30% { opacity: 0; }
  50% { opacity: 0.2; }
  70% { opacity: 0.1; }
  90% { opacity: 0; }
  100% { opacity: 0.1; }
}

.animate-flicker {
  animation: flicker 0.1s infinite;
}

@keyframes shake {
  0%, 100% { transform: translateX(0); }
  25% { transform: translateX(-10px); }
  75% { transform: translateX(10px); }
}

.animate-shake {
  animation: shake 0.1s 3;
}

@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

.animate-fadeIn {
  animation: fadeIn 0.5s ease-out forwards;
}
</style>
