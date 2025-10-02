/*
 * ============================================================================
 * Secrets Configuration Template
 * ============================================================================
 *
 * SETUP INSTRUCTIONS:
 *   1. Copy this file and rename it to "secrets.h"
 *   2. Fill in your actual WiFi credentials and Groq API key below
 *   3. Never commit secrets.h to GitHub!
 *
 * HOW TO GET A GROQ API KEY:
 *   1. Go to: https://console.groq.com
 *   2. Sign up for a free account (GitHub or Google login)
 *   3. Click "API Keys" in the left sidebar
 *   4. Click "Create API Key"
 *   5. Copy the key (starts with "gsk_...")
 *   6. Paste it below
 *
 * FREE TIER LIMITS:
 *   - 14,400 requests per day
 *   - Perfect for personal projects!
 *   - That's 1 command every 6 seconds, 24/7
 *
 * SECURITY NOTES:
 *   - Keep this file private (add secrets.h to .gitignore)
 *   - Don't share your API key with anyone
 *   - If you accidentally expose it, delete it and create a new one
 *
 * ============================================================================
 */

// WiFi Credentials
#define WIFI_SSID "YourWiFiNetworkName"       // Replace with your WiFi name
#define WIFI_PASSWORD "YourWiFiPassword"      // Replace with your WiFi password

// Groq API Key
#define GROQ_API_KEY "gsk_your_api_key_here"  // Replace with your Groq API key

/*
 * EXAMPLE (DO NOT USE THESE - THEY WON'T WORK):
 *
 * #define WIFI_SSID "MyHomeWiFi"
 * #define WIFI_PASSWORD "SuperSecret123"
 * #define GROQ_API_KEY "gsk_abc123xyz456..."
 *
 * IMPORTANT NOTES:
 *
 * WiFi Network:
 *   - Must be 2.4 GHz (ESP32 doesn't support 5 GHz)
 *   - WPA2 or WPA3 security is fine
 *   - Guest networks usually work too
 *   - Corporate/enterprise networks with captive portals won't work
 *
 * WiFi Password:
 *   - Include special characters if your password has them
 *   - Use quotes: "My Password!" not My Password!
 *   - Escape backslashes: "My\\Password" for "My\Password"
 *
 * API Key:
 *   - Always starts with "gsk_"
 *   - About 50+ characters long
 *   - Copy the entire key including gsk_ prefix
 *   - No spaces or line breaks
 */
