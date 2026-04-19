import { defineStore } from 'pinia';
import { ref, computed } from 'vue';

export const useAuthStore = defineStore('auth', () => {
    const token = ref(localStorage.getItem('neo_token') || null);
    const userRole = ref(localStorage.getItem('neo_role') || null);
    const operatorId = ref(localStorage.getItem('neo_operator') || 'OP_734');

    const isAuthenticated = computed(() => !!token.value);

    function login(newToken: string, role: string) {
        token.value = newToken;
        userRole.value = role;
        localStorage.setItem('neo_token', newToken);
        localStorage.setItem('neo_role', role);
    }

    function logout() {
        token.value = null;
        userRole.value = null;
        localStorage.removeItem('neo_token');
        localStorage.removeItem('neo_role');
    }

    return {
        token,
        userRole,
        operatorId,
        isAuthenticated,
        login,
        logout
    };
});
