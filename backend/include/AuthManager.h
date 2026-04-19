#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>
#include <mutex>
#include <map>
#include <iostream>

/**
 * @class AuthManager
 * @brief Singleton for managing system authentication and session state.
 * 
 * HUMAN TOUCH: Why a Singleton?
 * We use a Singleton for the AuthManager to ensure there is exactly one
 * source of truth for user sessions and security context across the entire
 * Antigravity engine. This prevents synchronization issues between multiple
 * instances and ensures that thread-safe access to the "current operator"
 * is maintained throughout the system lifecycle.
 */
class AuthManager {
public:
    // Delete copy constructor and assignment operator to enforce Singleton
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;

    /**
     * @brief Access the global instance of AuthManager.
     */
    static AuthManager& getInstance() {
        static AuthManager instance;
        return instance;
    }

    /**
     * @brief Verifies user credentials.
     * MOCK: Hardcoded logic for admin / neocity2026.
     */
    bool verifyUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(authMtx);
        
        // Simple mock check
        if (username == "admin" && password == "neocity2026") {
            currentUser = "admin";
            currentRole = "Admin";
            currentSessionToken = "NEO_TOKEN_" + std::to_string(rand() % 9000 + 1000);
            return true;
        }
        return false;
    }

    /**
     * @brief Simple token verification for other API routes.
     */
    bool isTokenValid(const std::string& token) {
        std::lock_guard<std::mutex> lock(authMtx);
        return !currentSessionToken.empty() && token == currentSessionToken;
    }

    std::string getCurrentUser() const { return currentUser; }
    std::string getCurrentRole() const { return currentRole; }
    std::string getSessionToken() const { return currentSessionToken; }

    void logout() {
        std::lock_guard<std::mutex> lock(authMtx);
        currentUser = "";
        currentRole = "";
        currentSessionToken = "";
    }

private:
    AuthManager() : currentUser(""), currentRole(""), currentSessionToken("") {
        // Seed random for token generation
        srand(time(0));
    }

    std::string currentUser;
    std::string currentRole;
    std::string currentSessionToken;
    mutable std::mutex authMtx;
};

#endif // AUTH_MANAGER_H
