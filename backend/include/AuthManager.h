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
     * @brief Registers a new user in the system.
     * Prevents duplicate usernames.
     */
    bool registerUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(authMtx);
        if (userDatabase.find(username) != userDatabase.end()) {
            return false; // User already exists
        }
        userDatabase[username] = password; // MOCK: In production, hash this!
        return true;
    }

    /**
     * @brief Verifies user credentials against the current database.
     */
    bool verifyUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(authMtx);
        
        auto it = userDatabase.find(username);
        if (it != userDatabase.end() && it->second == password) {
            currentUser = username;
            currentRole = (username == "admin") ? "Admin" : "Operator";
            currentSessionToken = "NEO_TOKEN_" + std::to_string(rand() % 9000 + 1000);
            return true;
        }
        return false;
    }

    /**
     * @brief Initializes a restricted Guest session.
     * 
     * HUMAN TOUCH: Guest vs Admin Security
     * Guest sessions are restricted to non-destructive observational data only.
     * Unlike Admin sessions which can access metrics and control transit flow,
     * Guest tokens are marked with 'GUEST' role to prevent any system mutation
     * at the engine level.
     */
    void guestLogin() {
        std::lock_guard<std::mutex> lock(authMtx);
        currentUser = "Guest_Visitor";
        currentRole = "GUEST";
        currentSessionToken = "GUEST_TOKEN_" + std::to_string(rand() % 9000 + 1000);
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
        srand(time(0));
        // Seed some default users
        userDatabase["admin"] = "neocity2026";
        userDatabase["operator1"] = "secure123";
    }

    std::map<std::string, std::string> userDatabase;
    std::string currentUser;
    std::string currentRole;
    std::string currentSessionToken;
    mutable std::mutex authMtx;
};

#endif // AUTH_MANAGER_H
