/*
 * ============================================================================
 * Secrets Configuration Template
 * ============================================================================
 *
 * SETUP INSTRUCTIONS:
 *   1. Copy this file and rename it to "secrets.h"
 *   2. Fill in your actual WiFi credentials and Demeterics Managed LLM Key below
 *   3. Never commit secrets.h to GitHub!
 *
 * HOW TO GET A DEMETERICS MANAGED LLM KEY:
 *   1. Go to: https://demeterics.com
 *   2. Sign up for a free account (Google + GitHub login supported)
 *   3. Open the dashboard and click "Managed LLM Keys"
 *   4. Click "Create Key"
 *   5. Copy the key (starts with "dmt_...")
 *   6. Paste it below and keep it secret
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

// Demeterics Managed LLM Key
#define DEMETERICS_API_KEY "dmt_your_api_key_here"  // Replace with your Demeterics Managed LLM Key

/*
 * EXAMPLE (DO NOT USE THESE - THEY WON'T WORK):
 *
 * #define WIFI_SSID "MyHomeWiFi"
 * #define WIFI_PASSWORD "SuperSecret123"
 * #define DEMETERICS_API_KEY "dmt_abc123xyz456..."
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
 *   - Always starts with "dmt_"
 *   - About 50+ characters long
 *   - Copy the entire key including dmt_ prefix
 *   - No spaces or line breaks
 */
