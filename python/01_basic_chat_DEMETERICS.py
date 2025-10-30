#!/usr/bin/env python3
"""
Example 1: Basic Chat with Demeterics Proxy

WHAT THIS DEMONSTRATES:
    - Using Demeterics observability proxy for AI calls
    - Automatic instrumentation without code changes
    - Analytics and monitoring for production AI
    - Transparent proxy pattern for any provider
    - Using Groq models through Demeterics

WHAT YOU'LL LEARN:
    - Routing API calls through observability layers
    - Production monitoring best practices
    - Analytics without modifying application code
    - Understanding proxy architectures
    - Getting insights into AI usage patterns

DEMETERICS PROVIDES:
    - Universal token usage tracking
    - Performance metrics (latency, queue times)
    - Cost analysis across all providers
    - Error tracking and debugging
    - Compliance audit trails
    - A/B testing capabilities
    - Usage analytics dashboard

PREREQUISITES:
    - Python 3.6 or higher
    - DEMETERICS_API_KEY environment variable set
    - Internet connection

EXPECTED OUTPUT:
    - Full JSON response (same as direct Groq)
    - AI's answer to your question
    - Token usage (tracked by Demeterics)
    - Visit Demeterics dashboard for analytics!

Run with: python3 01_basic_chat_DEMETERICS.py
"""

import http.client
import json
import os

# ==============================================================================
# Step 1: Load Demeterics API credentials
# ==============================================================================

api_key = os.environ.get('DEMETERICS_API_KEY')

if not api_key:
    print("Error: DEMETERICS_API_KEY environment variable not set")
    print("Run: export DEMETERICS_API_KEY='...'")
    print("Get your key from: https://demeterics.com")
    exit(1)

# Optional: Combined key format for advanced features
# If you have both Demeterics and provider keys:
# api_key = f"{demeterics_key};{groq_key}"

# ==============================================================================
# Step 2: Build the request payload (same as Groq)
# ==============================================================================

payload = {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",  # Groq model
    "messages": [
        {
            "role": "user",
            "content": "What is the capital of Switzerland?"
        }
    ],
    "temperature": 0.7,
    "max_tokens": 100
}

payload_json = json.dumps(payload)

# ==============================================================================
# Step 3: Connect to Demeterics proxy
# ==============================================================================

# Demeterics proxy endpoint (routes to Groq)
conn = http.client.HTTPSConnection("demeterics.uc.r.appspot.com")

# ==============================================================================
# Step 4: Send request through Demeterics
# ==============================================================================

headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'  # Demeterics API key
}

try:
    # Use Groq endpoint path, but through Demeterics proxy
    conn.request("POST", "/api/groq/v1/chat/completions", payload_json, headers)

    # Get the response
    response = conn.getresponse()
    response_data = response.read().decode('utf-8')

    # Parse JSON response
    response_json = json.loads(response_data)

    # ==============================================================================
    # Step 5: Display the results
    # ==============================================================================

    print("Full Response (via Demeterics proxy):")
    print(json.dumps(response_json, indent=2))

    # Extract and display the answer
    if 'choices' in response_json and len(response_json['choices']) > 0:
        answer = response_json['choices'][0]['message']['content']
        print("\n" + "="*50)
        print("AI's Answer (monitored by Demeterics):")
        print(answer)

    # Show token usage (tracked automatically)
    if 'usage' in response_json:
        usage = response_json['usage']
        print("\n" + "="*50)
        print("Token Usage (tracked):")
        print(f"  Prompt tokens: {usage.get('prompt_tokens', 0)}")
        print(f"  Completion tokens: {usage.get('completion_tokens', 0)}")
        print(f"  Total tokens: {usage.get('total_tokens', 0)}")
        print("\n📊 Analytics available at: https://demeterics.com/dashboard")

    # Check for Demeterics metadata
    if 'demeterics_metadata' in response_json:
        print("\n" + "="*50)
        print("Demeterics Metadata:")
        metadata = response_json['demeterics_metadata']
        print(f"  Request ID: {metadata.get('request_id')}")
        print(f"  Latency: {metadata.get('latency_ms')}ms")
        print(f"  Provider: {metadata.get('provider')}")

    # Check for errors
    if 'error' in response_json:
        print("\nAPI Error:")
        print(json.dumps(response_json['error'], indent=2))

except Exception as e:
    print(f"Request failed: {e}")
    print("\nTroubleshooting:")
    print("1. Check your internet connection")
    print("2. Verify your Demeterics API key")
    print("3. Check Demeterics service status")

finally:
    conn.close()

# ==============================================================================
# Demeterics Benefits:
# ==============================================================================
#
# Observability:
#   - Track every API call automatically
#   - Monitor token usage and costs
#   - Analyze performance metrics
#   - Debug production issues
#   - Generate compliance reports
#
# Architecture:
#   Your App → Demeterics Proxy → AI Provider
#                ↓
#          Analytics DB
#
# Features:
#   - Zero code changes required
#   - Works with existing integrations
#   - Universal format across providers
#   - Real-time dashboards
#   - Cost optimization insights
#   - Audit trails for compliance
#
# Use cases:
#   - Production monitoring
#   - Cost tracking and optimization
#   - Performance analysis
#   - A/B testing different models
#   - Compliance reporting
#   - Debugging and troubleshooting
#
# Dashboard features:
#   - Token usage by model/user/time
#   - Cost breakdown and projections
#   - Response time distributions
#   - Error rates and patterns
#   - Custom analytics queries
#