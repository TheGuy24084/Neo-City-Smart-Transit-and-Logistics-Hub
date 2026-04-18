<template>
  <div 
    class="bg-slate-900/90 border border-slate-800 p-6 backdrop-blur-xl relative overflow-hidden transition-all duration-500 hover:border-cyan-500/30 group shadow-2xl"
    :class="variants[variant || 'default']"
  >
    <!-- Aesthetic Accent Bar -->
    <div 
      class="absolute top-0 left-0 w-1 h-full shadow-[0_0_15px_rgba(34,211,238,0.5)]"
      :class="accentClasses[variant || 'default']"
    ></div>

    <!-- Header -->
    <div v-if="$slots.title" class="flex justify-between items-center mb-4">
      <h3 class="text-sm font-black text-white uppercase tracking-tighter flex items-center gap-2">
        <slot name="title"></slot>
      </h3>
      <slot name="header-suffix"></slot>
    </div>

    <!-- Content -->
    <div class="relative z-10">
      <slot></slot>
    </div>

    <!-- Functional Decoration -->
    <div class="absolute -right-4 -top-4 w-16 h-16 bg-white/5 rounded-full blur-2xl group-hover:bg-white/10 transition-all"></div>
  </div>
</template>

<script setup lang="ts">
defineProps<{
  variant?: 'cyan' | 'emerald' | 'purple' | 'blue' | 'amber' | 'default'
}>();

const variants = {
  cyan: 'border-cyan-500/30 text-cyan-50',
  emerald: 'border-emerald-500/30 text-emerald-50',
  purple: 'border-purple-500/30 text-purple-50',
  blue: 'border-blue-500/30 text-blue-50',
  amber: 'border-amber-500/30 text-amber-50',
  default: 'border-slate-800'
};

const accentClasses = {
  cyan: 'bg-cyan-500',
  emerald: 'bg-emerald-500',
  purple: 'bg-purple-500',
  blue: 'bg-blue-500',
  amber: 'bg-amber-500',
  default: 'bg-slate-700'
};
</script>
