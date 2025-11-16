# Exercise 10: Tool Use - Building AI Agents with Function Calling

[← Previous: Tavily Extract](09_tavily_extract.md) | [Back to Exercises](README.md) | [Next: Web Search (Groq) →](11_web_search.md)

## What You Learned

- Groq function calling / tool use
- Defining tools for AI models
- Multi-step agent workflows
- Combining Tavily Search and Extract as tools
- Building autonomous AI agents

## What is Tool Use / Function Calling?

Tool use (also called function calling) allows AI models to:
1. **Recognize** when they need external data or capabilities
2. **Request** specific function calls with arguments
3. **Use** the results to provide informed answers

This creates AI "agents" that can interact with the real world through APIs, databases, and custom functions.

## Getting Started

You need both API keys:
```bash
export DEMETERICS_API_KEY='dmt-your-key'
export TAVILY_API_KEY='tvly-your-key'
```

## Exercise 10.1: Single Tool Agent

Build an agent that uses one tool at a time:

**Simple search agent:**
```python
# Define one tool
tools = [{
    "type": "function",
    "function": {
        "name": "search_web",
        "description": "Search for current information",
        "parameters": {
            "type": "object",
            "properties": {
                "query": {"type": "string"}
            },
            "required": ["query"]
        }
    }
}]

# Ask questions that require search
queries = [
    "What's the weather in Tokyo today?",
    "Who won the latest Nobel Prize in Physics?",
    "What are the current stock prices for tech companies?"
]
```

**Experiments to try:**

1. **When does AI use tools?**
   - Ask: "What is 2+2?" (no tool needed)
   - Ask: "What happened in the news today?" (needs search)
   - Observe when AI decides to use vs. not use tools

2. **Tool descriptions matter:**
   - Vague: "Search for stuff"
   - Specific: "Search the web for current, real-time information and news"
   - Which gets better results?

3. **Required parameters:**
   - Try tools with/without `required` fields
   - See if AI provides incomplete arguments

**What to learn:**
- How AI decides when to use tools
- Importance of clear tool descriptions
- Parameter validation

## Exercise 10.2: Multi-Tool Agent

Build an agent with multiple tools and let it choose:

**Search + Extract agent:**
```javascript
const tools = [
    {
        type: "function",
        function: {
            name: "search_web",
            description: "Search for URLs and summaries",
            parameters: {/* ... */}
        }
    },
    {
        type: "function",
        function: {
            name: "extract_content",
            description: "Get full content from a URL",
            parameters: {/* ... */}
        }
    }
];

// Query that might use both:
"Find recent articles about AI safety, then get the full content of the most relevant one"
```

**Experiments to try:**

1. **Tool selection:**
   - Which tool does AI choose for different queries?
   - Can it use both tools in sequence?

2. **Parallel tool calls:**
   - Ask: "Search for Python tutorials and JavaScript tutorials"
   - Does AI make two search calls at once?

3. **Tool chaining:**
   - Search returns URLs
   - AI should extract content from top result
   - Does it work automatically?

**What to observe:**
- AI's tool selection logic
- Ability to chain multiple tools
- Parallel vs. sequential execution

## Exercise 10.3: Custom Tools - Calculator

Add a custom calculation tool:

**Implementation:**
```python
def calculate(expression):
    """Safely evaluate math expressions"""
    # Use ast.literal_eval or safe parser
    try:
        result = eval(expression, {"__builtins__": {}}, {})
        return {"result": result}
    except Exception as e:
        return {"error": str(e)}

tools = [
    {
        "type": "function",
        "function": {
            "name": "calculate",
            "description": "Perform mathematical calculations",
            "parameters": {
                "type": "object",
                "properties": {
                    "expression": {
                        "type": "string",
                        "description": "Math expression like '2+2' or '10*5'"
                    }
                },
                "required": ["expression"]
            }
        }
    }
]

# Test queries:
# "What is 12345 * 67890?"
# "Calculate the compound interest on $1000 at 5% for 10 years"
```

**Experiments to try:**

1. **Build a toolbox:**
   - Calculator
   - Web search
   - Content extraction
   - Date/time tool

2. **Complex queries:**
   - "Search for current Bitcoin price and calculate value of 10 BTC"
   - Requires: search → extract price → calculate

3. **Error handling:**
   - What if tool returns an error?
   - Can AI recover and try again?

**What to learn:**
- Creating custom tool functions
- Multi-step reasoning with tools
- Error handling in agents

## Exercise 10.4: Research Agent

**Real-world application:** Build an autonomous research agent.

**Goal:** Given a topic, research it thoroughly and create a report.

**Agent flow:**
1. Search for topic overview
2. Extract top 3 articles
3. Summarize each article
4. Combine into final report

**Implementation:**

```bash
#!/bin/bash
# research_agent.sh

TOPIC="$1"

# Define tools
TOOLS='[
  {
    "type": "function",
    "function": {
      "name": "search_web",
      "description": "Search for information",
      "parameters": {
        "type": "object",
        "properties": {
          "query": {"type": "string"}
        },
        "required": ["query"]
      }
    }
  },
  {
    "type": "function",
    "function": {
      "name": "extract_article",
      "description": "Extract full article content from URL",
      "parameters": {
        "type": "object",
        "properties": {
          "url": {"type": "string"}
        },
        "required": ["url"]
      }
    }
  }
]'

# Initial query
QUERY="Research the topic '$TOPIC' by:
1. Searching for overview articles
2. Getting full content from top 3 results
3. Creating a comprehensive summary with key points"

# Send to Groq with tools
# AI will make multiple tool calls
# Execute each tool
# Send results back
# Get final report
```

**Usage:**
```bash
./research_agent.sh "quantum computing applications"
./research_agent.sh "climate change solutions"
./research_agent.sh "machine learning best practices"
```

**Features to add:**

1. **Source tracking:**
   - Keep list of all URLs used
   - Create bibliography

2. **Depth control:**
   - Quick research: 3 sources
   - Deep research: 10+ sources

3. **Output formats:**
   - Markdown report
   - HTML page
   - PDF export

4. **Fact checking:**
   - Cross-reference multiple sources
   - Flag conflicting information

## Exercise 10.5: Interactive Agent Loop

**Goal:** Build a chat agent that can use tools during conversation.

**Concept:**
```
User: "What's the latest news on SpaceX?"
Agent: [searches] → "Here's what I found..."
User: "Tell me more about their Starship program"
Agent: [searches for Starship] → "Starship is..."
User: "Extract the full article from that first link"
Agent: [extracts content] → "Here's the article..."
```

**Implementation pattern:**

```python
conversation_history = []

while True:
    user_input = input("You: ")
    if user_input.lower() == 'exit':
        break

    # Add to history
    conversation_history.append({
        "role": "user",
        "content": user_input
    })

    # Call Groq with tools
    response = groq_call(conversation_history, tools)

    # Handle tool calls
    while has_tool_calls(response):
        tool_results = execute_tools(response)
        conversation_history.extend(tool_results)
        response = groq_call(conversation_history, tools)

    # Display AI response
    ai_message = response['choices'][0]['message']['content']
    print(f"AI: {ai_message}")

    # Add to history
    conversation_history.append({
        "role": "assistant",
        "content": ai_message
    })
```

**Features:**

1. **Context retention:**
   - AI remembers previous tool uses
   - Can reference earlier searches

2. **Multi-turn tool use:**
   - User guides the research process
   - AI suggests next steps

3. **Command shortcuts:**
   - `/search <query>` - force search
   - `/extract <url>` - force extraction
   - `/summarize` - summarize conversation

## Exercise 10.6: Agent Cost Optimization

**Goal:** Minimize API costs while maximizing usefulness.

**Cost breakdown:**
- Groq call with tools: ~500 tokens
- Tool execution: varies (Tavily ~100 tokens)
- Final response: ~200 tokens
- **Total per query: ~$0.0003**

**Optimization strategies:**

1. **Caching search results:**
   ```python
   search_cache = {}

   def search_with_cache(query):
       if query in search_cache:
           return search_cache[query]
       result = tavily_search(query)
       search_cache[query] = result
       return result
   ```

2. **Limit tool calls:**
   - Set `max_tool_calls` parameter
   - Prevent infinite loops

3. **Batch operations:**
   - Instead of: search("A"), search("B"), search("C")
   - Try: search("A, B, and C")

4. **Result truncation:**
   - Extract only first 1000 chars
   - Summarize before sending to AI

**Cost tracking:**
```python
total_groq_tokens = 0
total_tavily_searches = 0

# Track each call
total_groq_tokens += response['usage']['total_tokens']
total_tavily_searches += 1

# Calculate
groq_cost = total_groq_tokens * 0.11 / 1000000  # approx
tavily_cost = 0  # free tier
print(f"Session cost: ${groq_cost:.4f}")
```

## Exercise 10.7: Multi-Agent System

**Advanced:** Build multiple specialized agents that work together.

**Agents:**
1. **Researcher** - Searches and extracts
2. **Analyzer** - Summarizes and finds patterns
3. **Writer** - Creates final report

**Flow:**
```
User Query
  → Researcher Agent (tools: search, extract)
    → Analyzer Agent (tools: none, pure reasoning)
      → Writer Agent (tools: none, formatting)
        → Final Output
```

**Implementation:**
```python
def researcher_agent(query):
    # Has access to search/extract tools
    return raw_data

def analyzer_agent(raw_data):
    # Analyzes without tools
    return insights

def writer_agent(insights):
    # Formats into report
    return formatted_report

# Coordinate
data = researcher_agent("quantum computing")
analysis = analyzer_agent(data)
report = writer_agent(analysis)
```

**Benefits:**
- Specialized prompts for each agent
- Parallel execution possible
- Easier to debug

## Reflection Questions

After completing these exercises, consider:

1. When should you use tool calling vs. direct API calls?
2. How do you prevent infinite tool-calling loops?
3. What's the trade-off between agent autonomy and control?
4. How do you validate tool results before using them?
5. What security concerns exist with tool execution?

## Security Considerations

**Important:** Tool use introduces security risks:

1. **Validate tool inputs:**
   - Sanitize user-provided URLs
   - Limit calculation expressions
   - Prevent code injection

2. **Rate limiting:**
   - Limit tool calls per session
   - Prevent API abuse

3. **Tool permissions:**
   - Only enable safe tools
   - Never give AI file system write access
   - Sandbox code execution

4. **Cost controls:**
   - Set spending limits
   - Monitor usage
   - Alert on anomalies

## Next Steps

Congratulations! You've completed all 10 core exercises. You now know how to:
- Use AI APIs (Groq, Tavily)
- Handle multimodal content (text, images, audio)
- Implement safety and security
- Build autonomous AI agents

**What's next?**
- Combine multiple examples into full applications
- Explore streaming responses
- Build production-ready systems
- Contribute your own examples!

---

**Related Examples:**
- [bash/10_tool_use_minimal.sh](../bash/10_tool_use_minimal.sh)
- [bash/10_tool_use_full.sh](../bash/10_tool_use_full.sh)
- [nodejs/10_tool_use.js](../nodejs/10_tool_use.js)
- [python/10_tool_use.py](../python/10_tool_use.py)
- [go/10_tool_use.go](../go/10_tool_use.go)

**See also:**
- [Exercise 8: Tavily Search](08_tavily_search.md) - Web search tool
- [Exercise 9: Tavily Extract](09_tavily_extract.md) - Content extraction tool
- [Exercise 1: Basic Chat](01_basic_chat.md) - Demeterics Groq proxy fundamentals

[← Previous: Tavily Extract](09_tavily_extract.md) | [Back to Exercises](README.md) | [Next: Web Search (Groq) →](11_web_search.md)
