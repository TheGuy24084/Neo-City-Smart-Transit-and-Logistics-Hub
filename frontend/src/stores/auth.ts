import { defineStore } from 'pinia';
import { ref, computed } from 'vue';

export const useAuthStore = defineStore('auth', () => {
    const token = ref(localStorage.getItem('neo_token') || null);
    const userRole = ref(localStorage.getItem('neo_role') || null);
    const operatorId = ref(localStorage.getItem('neo_operator') || 'OP_734');
    const operatorName = ref(localStorage.getItem('neo_name') || '');

    const isAuthenticated = computed(() => !!token.value);
    const isAdmin = computed(() => userRole.value === 'Admin' || userRole.value === 'Operator'); // Operators are also authenticated users

    function login(newToken: string, role: string, name: string) {
        token.value = newToken;
        userRole.value = role;
        operatorName.value = name;
        localStorage.setItem('neo_token', newToken);
        localStorage.setItem('neo_role', role);
        localStorage.setItem('neo_name', name);
    }

    function register(username: string) {
        // MOCK: In real system, this calls backend registerUser
        console.log(`[AuthStore] Registered User: ${username}`);
    }

    function guestLogin() {
        const guestToken = 'GUEST_TOKEN_' + Math.floor(Math.random() * 10000);
        token.value = guestToken;
        userRole.value = 'GUEST';
        operatorName.value = 'Guest Visitor';
        localStorage.setItem('neo_token', guestToken);
        localStorage.setItem('neo_role', 'GUEST');
        localStorage.setItem('neo_name', 'Guest Visitor');
    }

    function logout() {
        token.value = null;
        userRole.value = null;
        operatorName.value = '';
        localStorage.removeItem('neo_token');
        localStorage.removeItem('neo_role');
        localStorage.removeItem('neo_name');
    }

    return {
        token,
        userRole,
        operatorId,
        operatorName,
        isAuthenticated,
        isAdmin,
        login,
        register,
        guestLogin,
        logout
    };
});
