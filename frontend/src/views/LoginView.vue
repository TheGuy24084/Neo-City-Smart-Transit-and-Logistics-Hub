<script setup lang="ts">
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { useAuthStore } from '../stores/auth';

const router = useRouter();
const authStore = useAuthStore();

const mode = ref<'login' | 'register'>('login');
const username = ref('');
const password = ref('');
const isError = ref(false);
const isBooting = ref(false);
const bootProgress = ref(0);

// Initialize with default for convenience
if (mode.value === 'login') {
    username.value = 'admin';
}

const handleAuth = async () => {
    isError.value = false;
    
    if (mode.value === 'login') {
        if (authStore.verify(username.value, password.value)) {
            const role = username.value === 'admin' ? 'Admin' : 'Operator';
            startBootSequence(role, username.value);
        } else {
            triggerError();
        }
    } else {
        // REGISTER FLOW
        if (username.value && password.value) {
            authStore.register(username.value, password.value);
            // Automatic Entry after Registration
            startBootSequence('Operator', username.value);
        } else {
            triggerError();
        }
    }
};

const handleGuest = () => {
    startBootSequence('GUEST', 'Guest Visitor');
};

const triggerError = () => {
    isError.value = true;
    setTimeout(() => isError.value = false, 500);
};

const startBootSequence = (role: string, name: string) => {
    isBooting.value = true;
    bootProgress.value = 0;
    const interval = setInterval(() => {
        bootProgress.value += 5;
        if (bootProgress.value >= 100) {
            clearInterval(interval);
            if (role === 'GUEST') authStore.guestLogin();
            else authStore.login('NEO_TOKEN_' + Math.random(), role, name);
            router.push('/');
        }
    }, 50);
};
</script>

<template>
  <div class="min-h-screen w-full bg-slate-950 flex items-center justify-center p-6 relative overflow-hidden font-sans">
    
    <!-- Cyberpunk Background Canvas -->
    <div class="absolute inset-0 z-0">
        <div class="absolute inset-0 bg-[url('https://images.unsplash.com/photo-1605142859862-978be7eba909?q=80&w=2070&auto=format&fit=crop')] bg-cover bg-center blur-[100px] opacity-10 grayscale"></div>
        <div class="absolute inset-0 bg-gradient-to-br from-slate-950 via-slate-950 to-cyan-950/20"></div>
        
        <!-- Grid/Pattern Overlay -->
        <div class="absolute inset-0 opacity-20 pointer-events-none bg-[radial-gradient(circle_at_center,rgba(34,211,238,0.1)_1px,transparent_1px)] bg-[length:40px_40px]"></div>
    </div>

    <!-- Login/Register Card -->
    <div 
        class="max-w-md w-full z-20 transition-all duration-700 ease-out"
        :class="[isError ? 'animate-shake' : '', isBooting ? 'scale-90 opacity-0 blur-xl pointer-events-none' : '']"
    >
        <div class="bg-slate-900/40 backdrop-blur-3xl border border-white/5 p-12 rounded-[3.5rem] shadow-[0_0_100px_rgba(0,0,0,0.6)] relative overflow-hidden group">
            
            <!-- Glow Accents -->
            <div class="absolute -top-24 -right-24 w-48 h-48 bg-cyan-500/10 blur-[80px]"></div>
            <div class="absolute -bottom-24 -left-24 w-48 h-48 bg-purple-500/10 blur-[80px]"></div>
            
            <div class="text-center mb-12">
                <h1 class="text-5xl font-black italic tracking-tighter text-white uppercase leading-none mb-6">
                    NEO<span class="text-cyan-400">CITY</span>
                </h1>
                
                <div class="inline-flex bg-slate-950/50 p-1 rounded-full border border-white/5">
                    <button 
                        @click="mode = 'login'"
                        class="px-8 py-2 rounded-full text-[9px] uppercase font-black tracking-widest transition-all"
                        :class="mode === 'login' ? 'bg-cyan-500 text-slate-950 shadow-lg' : 'text-slate-500 hover:text-white'"
                    >
                        Access
                    </button>
                    <button 
                        @click="mode = 'register'"
                        class="px-8 py-2 rounded-full text-[9px] uppercase font-black tracking-widest transition-all"
                        :class="mode === 'register' ? 'bg-cyan-500 text-slate-950 shadow-lg' : 'text-slate-500 hover:text-white'"
                    >
                        Register
                    </button>
                </div>
            </div>

            <transition name="form-slide" mode="out-in">
                <form :key="mode" @submit.prevent="handleAuth" class="space-y-8">
                    <div class="space-y-3">
                        <label class="text-[9px] font-black uppercase tracking-[0.3em] text-slate-500 ml-1">Identity_Handle</label>
                        <input 
                            v-model="username"
                            type="text" 
                            class="w-full bg-slate-950/80 border border-white/5 p-5 rounded-2xl text-white font-mono text-sm focus:outline-none focus:border-cyan-500/40 transition-all focus:shadow-[0_0_30px_rgba(34,211,238,0.1)]"
                            placeholder="OPERATOR_CODE"
                            autocomplete="username"
                            required
                        >
                    </div>

                    <div class="space-y-3">
                        <label class="text-[9px] font-black uppercase tracking-[0.3em] text-slate-500 ml-1">Access_Code</label>
                        <input 
                            v-model="password"
                            type="password" 
                            class="w-full bg-slate-950/80 border border-white/5 p-5 rounded-2xl text-white font-mono text-sm focus:outline-none focus:border-cyan-500/40 transition-all focus:shadow-[0_0_30px_rgba(34,211,238,0.1)]"
                            placeholder="••••••••"
                            autocomplete="current-password"
                            required
                        >
                    </div>

                    <div class="pt-4 flex flex-col gap-4">
                        <button 
                            type="submit"
                            class="w-full py-5 bg-cyan-500 text-slate-950 font-black uppercase text-[10px] tracking-[0.25em] rounded-2xl hover:bg-cyan-400 transition-all shadow-[0_20px_40px_rgba(34,211,238,0.2)] active:scale-[0.98]"
                        >
                            {{ mode === 'login' ? 'Initialize Engine' : 'Confirm Registration' }}
                        </button>
                        
                        <button 
                            type="button"
                            @click="handleGuest"
                            class="w-full py-3 text-[9px] text-emerald-500/70 hover:text-emerald-400 uppercase font-black tracking-[0.4em] transition-all border border-emerald-500/10 hover:border-emerald-500/30 rounded-xl"
                        >
                            <span class="opacity-50">—</span> Enter as Guest <span class="opacity-50">—</span>
                        </button>
                    </div>
                </form>
            </transition>

            <div class="mt-12 pt-8 border-t border-white/5 text-center relative">
                <p class="text-[8px] font-mono text-slate-700 uppercase tracking-[0.5em] italic">System clearance: LEVEL_01_REQUIRED</p>
                <div class="flex justify-center gap-1.5 mt-3">
                    <div class="w-1 h-1 rounded-full bg-cyan-500/20"></div>
                    <div class="w-1 h-1 rounded-full bg-cyan-500/40"></div>
                    <div class="w-1 h-1 rounded-full bg-cyan-500/20"></div>
                </div>
            </div>
        </div>
    </div>

    <!-- Initialization Overlay -->
    <div v-if="isBooting" class="fixed inset-0 z-[100] flex flex-col items-center justify-center bg-slate-950/90 backdrop-blur-3xl animate-fade-in">
        <div class="max-w-xs w-full space-y-10 text-center">
            <div class="space-y-4">
                <h2 class="text-white font-mono text-[10px] uppercase tracking-[0.6em] animate-pulse">Establishing Neural Link...</h2>
                <div class="h-[2px] w-full bg-slate-900 rounded-full overflow-hidden">
                    <div 
                        class="h-full bg-cyan-500 shadow-[0_0_15px_rgba(34,211,238,1)] transition-all duration-300 ease-out"
                        :style="{ width: `${bootProgress}%` }"
                    ></div>
                </div>
            </div>
            
            <div class="flex flex-col gap-2 items-center opacity-40">
                <p class="text-[8px] font-mono text-cyan-500 uppercase">Synchronizing local graph...</p>
                <p class="text-[8px] font-mono text-cyan-500 uppercase">Verifying sector tokens...</p>
            </div>
        </div>
    </div>

  </div>
</template>

<style scoped>
.form-slide-enter-active, .form-slide-leave-active {
  transition: all 0.5s cubic-bezier(0.16, 1, 0.3, 1);
}
.form-slide-enter-from { opacity: 0; transform: translateY(20px); filter: blur(10px); }
.form-slide-leave-to { opacity: 0; transform: translateY(-20px); filter: blur(10px); }

@keyframes shake {
  0%, 100% { transform: translateX(0); }
  25% { transform: translateX(-10px); }
  75% { transform: translateX(10px); }
}
.animate-shake { animation: shake 0.1s 3; }

@keyframes fade-in {
  from { opacity: 0; }
  to { opacity: 1; }
}
.animate-fade-in { animation: fade-in 0.8s ease-out forwards; }
</style>
