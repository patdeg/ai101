/**
 * ============================================================================
 * Example 11: Tool Use - AI Agents with Function Calling
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Giving AI the ability to call functions/tools
 *   - Building an AI agent that can take actions
 *   - Tool definition and execution loop
 *
 * WHAT YOU'LL LEARN:
 *   - OpenAI-compatible tool calling format
 *   - Agent loop pattern
 *   - Combining AI decisions with external actions
 *
 * COMPILE:
 *   gcc -o 11_tool_use 11_tool_use.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./11_tool_use
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include "cJSON.h"

/* Response buffer */
struct ResponseBuffer {
    char *data;
    size_t size;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct ResponseBuffer *buf = (struct ResponseBuffer *)userp;
    char *ptr = realloc(buf->data, buf->size + realsize + 1);
    if (ptr == NULL) return 0;
    buf->data = ptr;
    memcpy(&(buf->data[buf->size]), contents, realsize);
    buf->size += realsize;
    buf->data[buf->size] = '\0';
    return realsize;
}

/* ============================================================================
 * TOOL IMPLEMENTATIONS
 * ============================================================================
 * These are the actual functions the AI can call.
 */

/* Get current time */
char *get_current_time(void) {
    static char buffer[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

/* Simple calculator */
double calculate(double a, double b, const char *operation) {
    if (strcmp(operation, "add") == 0) return a + b;
    if (strcmp(operation, "subtract") == 0) return a - b;
    if (strcmp(operation, "multiply") == 0) return a * b;
    if (strcmp(operation, "divide") == 0) {
        if (b == 0) return 0;  /* Avoid division by zero */
        return a / b;
    }
    return 0;
}

/* Get weather (simulated) */
char *get_weather(const char *location) {
    static char buffer[256];
    /* In real app, this would call a weather API */
    snprintf(buffer, sizeof(buffer),
        "Weather in %s: Sunny, 22C (72F), humidity 45%%",
        location);
    return buffer;
}

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Initialize */
    curl_global_init(CURL_GLOBAL_ALL);

    printf("=== AI AGENT WITH TOOLS ===\n");
    printf("The AI can call these tools:\n");
    printf("  - get_current_time: Get current date/time\n");
    printf("  - calculate: Perform math operations\n");
    printf("  - get_weather: Get weather for a location\n\n");

    /* Step 3: Define the tools (functions) the AI can use */
    const char *tools_json = R"([
        {
            "type": "function",
            "function": {
                "name": "get_current_time",
                "description": "Get the current date and time",
                "parameters": {
                    "type": "object",
                    "properties": {},
                    "required": []
                }
            }
        },
        {
            "type": "function",
            "function": {
                "name": "calculate",
                "description": "Perform a mathematical calculation",
                "parameters": {
                    "type": "object",
                    "properties": {
                        "a": {"type": "number", "description": "First number"},
                        "b": {"type": "number", "description": "Second number"},
                        "operation": {
                            "type": "string",
                            "enum": ["add", "subtract", "multiply", "divide"],
                            "description": "The operation to perform"
                        }
                    },
                    "required": ["a", "b", "operation"]
                }
            }
        },
        {
            "type": "function",
            "function": {
                "name": "get_weather",
                "description": "Get current weather for a location",
                "parameters": {
                    "type": "object",
                    "properties": {
                        "location": {"type": "string", "description": "City name"}
                    },
                    "required": ["location"]
                }
            }
        }
    ])";

    cJSON *tools = cJSON_Parse(tools_json);

    /* Step 4: User's request */
    const char *user_query = "What time is it, and what's 42 multiplied by 17?";
    printf("User: %s\n\n", user_query);

    /* Step 5: Build initial request */
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        cJSON_Delete(tools);
        curl_global_cleanup();
        return 1;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();

    cJSON *user_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(user_msg, "role", "user");
    cJSON_AddStringToObject(user_msg, "content", user_query);
    cJSON_AddItemToArray(messages, user_msg);

    cJSON_AddStringToObject(root, "model", "meta-llama/llama-4-scout-17b-16e-instruct");
    cJSON_AddItemToObject(root, "messages", cJSON_Duplicate(messages, 1));
    cJSON_AddItemToObject(root, "tools", cJSON_Duplicate(tools, 1));
    cJSON_AddStringToObject(root, "tool_choice", "auto");

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Response buffer */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    /* Configure and send */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    printf("Sending request to AI...\n");
    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        cJSON *response_json = cJSON_Parse(response.data);

        if (response_json) {
            cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
            if (choices && cJSON_GetArraySize(choices) > 0) {
                cJSON *first = cJSON_GetArrayItem(choices, 0);
                cJSON *message = cJSON_GetObjectItem(first, "message");
                cJSON *tool_calls = cJSON_GetObjectItem(message, "tool_calls");

                if (tool_calls && cJSON_IsArray(tool_calls)) {
                    printf("\nAI wants to call %d tool(s):\n\n",
                           cJSON_GetArraySize(tool_calls));

                    /* Process each tool call */
                    int num_calls = cJSON_GetArraySize(tool_calls);
                    for (int i = 0; i < num_calls; i++) {
                        cJSON *call = cJSON_GetArrayItem(tool_calls, i);
                        cJSON *function = cJSON_GetObjectItem(call, "function");
                        cJSON *name = cJSON_GetObjectItem(function, "name");
                        cJSON *args = cJSON_GetObjectItem(function, "arguments");

                        printf("Tool: %s\n", name->valuestring);
                        printf("Args: %s\n", args->valuestring);

                        /* Execute the tool */
                        char result[512] = "";
                        cJSON *args_json = cJSON_Parse(args->valuestring);

                        if (strcmp(name->valuestring, "get_current_time") == 0) {
                            strcpy(result, get_current_time());
                        }
                        else if (strcmp(name->valuestring, "calculate") == 0) {
                            double a = cJSON_GetObjectItem(args_json, "a")->valuedouble;
                            double b = cJSON_GetObjectItem(args_json, "b")->valuedouble;
                            const char *op = cJSON_GetObjectItem(args_json, "operation")->valuestring;
                            double res_val = calculate(a, b, op);
                            snprintf(result, sizeof(result), "%.2f", res_val);
                        }
                        else if (strcmp(name->valuestring, "get_weather") == 0) {
                            const char *loc = cJSON_GetObjectItem(args_json, "location")->valuestring;
                            strcpy(result, get_weather(loc));
                        }

                        printf("Result: %s\n\n", result);
                        cJSON_Delete(args_json);
                    }
                } else {
                    /* No tool calls - direct response */
                    cJSON *content = cJSON_GetObjectItem(message, "content");
                    if (content && cJSON_IsString(content)) {
                        printf("\nAI Response: %s\n", content->valuestring);
                    }
                }
            }
            cJSON_Delete(response_json);
        }
    } else {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    }

    /* Clean up */
    free(response.data);
    free(json_payload);
    cJSON_Delete(root);
    cJSON_Delete(messages);
    cJSON_Delete(tools);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    printf("=== TOOL USE PATTERN ===\n\n");
    printf("1. Define tools with JSON schema\n");
    printf("2. Send user query + tool definitions\n");
    printf("3. AI returns tool_calls if needed\n");
    printf("4. Execute tools locally\n");
    printf("5. Send results back to AI\n");
    printf("6. AI generates final response\n\n");

    printf("USE CASES:\n");
    printf("  - Web search agents\n");
    printf("  - Database query agents\n");
    printf("  - API integration\n");
    printf("  - Code execution\n");
    printf("  - Multi-step reasoning\n");

    return 0;
}

/*
 * TOOL CALLING NOTES:
 *
 * TOOL DEFINITION SCHEMA:
 *   - name: Function identifier
 *   - description: What the function does
 *   - parameters: JSON Schema for arguments
 *
 * AI DECIDES:
 *   - Which tool to call (or none)
 *   - What arguments to pass
 *   - Can call multiple tools
 *
 * AGENT LOOP:
 *   User query → AI (with tools) → Tool calls → Execute → AI (with results) → Response
 */
