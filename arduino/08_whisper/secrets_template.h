/*
 * ============================================================================
 * Secrets Configuration Template
 * ============================================================================
 *
 * SETUP INSTRUCTIONS:
 *   1. Copy this file and rename it to "secrets.h"
 *   2. Fill in your actual WiFi credentials and OpenRouter API key below
 *   3. Never commit secrets.h to GitHub!
 *
 * HOW TO GET AN OPENROUTER API KEY:
 *   1. Go to: https://openrouter.ai
 *   2. Sign up for a free account (Google + GitHub login supported)
 *   3. Open the dashboard and click "Keys"
 *   4. Click "Create Key"
 *   5. Copy the key (starts with "sk-or-v1-...")
 *   6. Paste it below and keep it secret
 *
 * FREE TIER:
 *   - Free-tier (":free") models have no per-request cost
 *   - Perfect for personal projects!
 *   - Browse current free models at https://openrouter.ai/models
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

// OpenRouter API Key
#define OPENROUTER_API_KEY "sk-or-v1-your_api_key_here"  // Replace with your OpenRouter API key

/*
 * EXAMPLE (DO NOT USE THESE - THEY WON'T WORK):
 *
 * #define WIFI_SSID "MyHomeWiFi"
 * #define WIFI_PASSWORD "SuperSecret123"
 * #define OPENROUTER_API_KEY "sk-or-v1-abc123xyz456..."
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
 *   - Always starts with "sk-or-v1-"
 *   - About 60+ characters long
 *   - Copy the entire key including the sk-or-v1- prefix
 *   - No spaces or line breaks
 */
