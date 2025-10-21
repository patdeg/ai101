# Exercise 8: Tavily Search - AI-Powered Web Search

[← Previous: Whisper](07_whisper.md) | [Back to Exercises](README.md) | [Next: Tavily Extract →](09_tavily_extract.md)

## What You Learned

- Tavily Search API for AI applications
- AI-generated answer synthesis
- Topic-specific search (general, news, finance)
- Clean, structured search results
- Domain filtering and time restrictions

## Getting Started

**Get your free Tavily API key:**
1. Visit https://tavily.com
2. Sign up (1,000 free searches/month)
3. Copy your API key (starts with `tvly-`)
4. Set environment variable:
   ```bash
   export TAVILY_API_KEY='tvly-your-key-here'
   ```

## Exercise 8.1: News Search with Time Restrictions

Search for recent news with different time windows:

**Basic news search (last 7 days):**
```json
{
  "query": "artificial intelligence breakthroughs",
  "topic": "news",
  "days": 7,
  "max_results": 10,
  "include_answer": true
}
```

**Experiments to try:**

1. **Breaking news (last 24 hours):**
   - Set `"days": 1`
   - Compare answer quality with 7-day search

2. **Weekly digest (last 7 days):**
   - Default setting
   - Get comprehensive AI summary of the week

3. **Monthly overview (last 30 days):**
   - Set `"days": 30`
   - See how answer synthesis handles larger time windows

4. **Custom time windows:**
   - Try 3 days, 14 days, 60 days
   - Find the sweet spot for your use case

**What to observe:**
- How does `days` parameter affect answer quality?
- Are recent results more relevant than older ones?
- Does AI answer change significantly with time range?

## Exercise 8.2: Finance Search with Domain Filtering

Search financial news while controlling sources:

**Include trusted financial sites:**
```json
{
  "query": "stock market analysis technology sector",
  "topic": "finance",
  "include_domains": [
    "bloomberg.com",
    "wsj.com",
    "reuters.com",
    "ft.com"
  ],
  "max_results": 10,
  "include_answer": true
}
```

**Exclude certain domains:**
```json
{
  "query": "cryptocurrency market trends",
  "topic": "finance",
  "exclude_domains": [
    "reddit.com",
    "twitter.com",
    "seeking alpha.com"
  ],
  "max_results": 10
}
```

**Experiments to try:**

1. **Trusted sources only:**
   - Use `include_domains` with reputable financial sites
   - Compare answer quality vs. unrestricted search

2. **Exclude social media:**
   - Use `exclude_domains` to filter out forums/social
   - Does this improve signal-to-noise ratio?

3. **Academic sources:**
   - Include only .edu or research domains
   - Try: `"include_domains": ["*.edu", "arxiv.org", "papers.ssrn.com"]`

4. **Side-by-side comparison:**
   - Run same query with/without domain filtering
   - Count how many results come from specified domains

**What to learn:**
- Impact of source filtering on answer quality
- Trade-offs between breadth and reliability
- Cost of domain filtering (does it reduce result count?)

## Exercise 8.3: Image Search with Descriptions

Enable image search and AI-generated descriptions:

**Basic image search:**
```json
{
  "query": "solar panel installation guide",
  "include_images": true,
  "include_image_descriptions": true,
  "max_results": 5
}
```

**Experiments to try:**

1. **Visual content queries:**
   - "infographic data visualization examples"
   - "architecture modern buildings"
   - "data science workflow diagrams"

2. **Compare with/without descriptions:**
   - Run same query twice
   - Observe: `include_image_descriptions: true` vs. `false`
   - Do AI descriptions add value?

3. **Image relevance scoring:**
   - Check `score` field for each image
   - Are high-scoring images more relevant?

4. **Image-heavy vs. text-heavy queries:**
   - Compare: "chart types" vs. "what are different chart types"
   - Which query returns better images?

**What to observe:**
- Quality of AI-generated image descriptions
- Relevance of images to the query
- Use cases for image search (tutorials, visual refs, diagrams)

## Exercise 8.4: Search Depth Comparison

Compare "basic" vs. "advanced" search depth:

**Basic search (faster, less comprehensive):**
```json
{
  "query": "quantum computing applications",
  "search_depth": "basic",
  "include_answer": true,
  "max_results": 10
}
```

**Advanced search (slower, more comprehensive):**
```json
{
  "query": "quantum computing applications",
  "search_depth": "advanced",
  "include_answer": true,
  "max_results": 10
}
```

**Experiments to try:**

1. **Response time measurement:**
   - Record `response_time` for both depths
   - Calculate average difference over 5 queries

2. **Answer quality assessment:**
   - Compare AI-generated answers
   - Is "advanced" noticeably better?

3. **Result diversity:**
   - Count unique domains in results
   - Does "advanced" find more diverse sources?

4. **Cost-benefit analysis:**
   - Both depths cost the same per search
   - When is extra time worth it?

**What to learn:**
- When to use basic vs. advanced depth
- Trade-offs between speed and comprehensiveness
- Use cases for each depth level

## Exercise 8.5: Build an AI News Digest with Groq + ntfy.sh

**Real-world application:** Create a personalized news digest that searches for topics you care about, summarizes them with AI, and sends you a notification.

**Implementation flow:**
1. Search for news on your topic (Tavily)
2. Extract search results and answer
3. Send to Groq AI for executive summary
4. Push summary to your phone via ntfy.sh

**Step 1: Search for news**

Pick a topic you're interested in:
- Technology: "AI breakthroughs", "space exploration"
- Finance: "renewable energy stocks", "tech IPOs"
- Sports: "Olympics 2024", "World Cup qualifiers"
- Science: "climate change solutions", "medical discoveries"

```bash
# Search Tavily for recent news
TAVILY_RESPONSE=$(curl -s -X POST "https://api.tavily.com/search" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d '{
    "query": "artificial intelligence breakthroughs",
    "topic": "news",
    "days": 7,
    "max_results": 10,
    "include_answer": true
  }')
```

**Step 2: Extract key information**

```bash
# Extract AI answer and top results
ANSWER=$(echo "$TAVILY_RESPONSE" | jq -r '.answer')
RESULTS=$(echo "$TAVILY_RESPONSE" | jq -r '.results[] | "- \(.title) (\(.url))"' | head -5)
```

**Step 3: Build context for Groq**

```bash
# Combine Tavily results into prompt
CONTEXT="Here are recent news articles about AI breakthroughs:

AI Summary from Tavily:
$ANSWER

Top Articles:
$RESULTS

Please create a concise executive summary (3-4 sentences) highlighting the most important developments."
```

**Step 4: Get executive summary from Groq**

```bash
# Send to Groq for summarization
SUMMARY=$(curl -s -X POST "https://api.groq.com/openai/v1/chat/completions" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "system",
        "content": "You are a news analyst. Create concise, executive-level summaries of news topics."
      },
      {
        "role": "user",
        "content": "'"$(echo "$CONTEXT" | jq -Rs .)"'"
      }
    ],
    "temperature": 0.3,
    "max_completion_tokens": 200
  }' | jq -r '.choices[0].message.content')
```

**Step 5: Send notification via ntfy.sh**

```bash
# Push to your phone via ntfy.sh
curl -s -X POST "https://ntfy.sh/your-unique-topic-name" \
  -H "Title: AI News Digest - $(date +%Y-%m-%d)" \
  -H "Priority: default" \
  -H "Tags: newspaper,robot" \
  -d "$SUMMARY"
```

**Full script example:**

```bash
#!/bin/bash
# ai_news_digest.sh - Daily AI news summary to your phone

# 1. Search Tavily
TAVILY_RESULTS=$(curl -s -X POST "https://api.tavily.com/search" \
  -H "Content-Type": application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d '{
    "query": "'"$1"'",
    "topic": "news",
    "days": 7,
    "max_results": 5,
    "include_answer": true
  }')

# 2. Extract data
ANSWER=$(echo "$TAVILY_RESULTS" | jq -r '.answer')
ARTICLES=$(echo "$TAVILY_RESULTS" | jq -r '.results[] | "- \(.title)"' | head -3)

# 3. Build prompt
PROMPT="Recent news summary: $ANSWER\n\nKey articles:\n$ARTICLES\n\nCreate a 3-sentence executive summary."

# 4. Get Groq summary
SUMMARY=$(curl -s -X POST "https://api.groq.com/openai/v1/chat/completions" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {"role": "system", "content": "Create concise news summaries."},
      {"role": "user", "content": "'"$PROMPT"'"}
    ],
    "temperature": 0.3,
    "max_completion_tokens": 150
  }' | jq -r '.choices[0].message.content')

# 5. Send notification
curl -X POST "https://ntfy.sh/my-ai-news-$(whoami)" \
  -H "Title: News: $1" \
  -H "Tags: newspaper" \
  -d "$SUMMARY"

echo "✓ Digest sent to ntfy.sh/my-ai-news-$(whoami)"
```

**Usage:**
```bash
chmod +x ai_news_digest.sh
./ai_news_digest.sh "quantum computing"
./ai_news_digest.sh "renewable energy"
./ai_news_digest.sh "space exploration"
```

**Subscribe on your phone:**
1. Install ntfy app (iOS/Android)
2. Subscribe to: `my-ai-news-yourusername`
3. Get instant news digests!

**Advanced features to add:**

1. **Scheduled digest (cron job):**
   ```bash
   # Run every morning at 8am
   0 8 * * * /path/to/ai_news_digest.sh "your topic"
   ```

2. **Multiple topics:**
   ```bash
   for topic in "AI" "climate change" "space"; do
     ./ai_news_digest.sh "$topic"
   done
   ```

3. **Priority filtering:**
   - Set `"Priority: urgent"` for breaking news
   - Set `"Priority: low"` for weekly digests

4. **Rich formatting:**
   ```bash
   curl -X POST "https://ntfy.sh/topic" \
     -H "Title: Breaking News" \
     -H "Priority: high" \
     -H "Tags: warning,newspaper" \
     -H "Actions: view, Open article, https://example.com" \
     -d "$SUMMARY"
   ```

5. **Error handling:**
   - Check if Tavily returned results
   - Validate Groq response
   - Retry on failures

**What to learn:**
- Chaining multiple AI services (Tavily → Groq → ntfy.sh)
- Building practical automation with APIs
- Cost-effective news monitoring ($0.01 per digest)
- Push notification integration
- Scheduled automation with cron

**Cost calculation:**
- Tavily search: 1 search (free tier: 1,000/month)
- Groq completion: ~300 tokens = $0.0001
- ntfy.sh: Free
- **Total per digest: ~$0.0001** (basically free!)

## Reflection Questions

After completing these exercises, consider:

1. When would you use `topic: "news"` vs. `topic: "general"`?
2. How does domain filtering affect search quality and diversity?
3. What's the cost-benefit of `include_images` and `include_image_descriptions`?
4. How would you build a production news monitoring service?
5. What other APIs could you chain with Tavily + Groq?

## Next Steps

Learn about content extraction with [Exercise 9: Tavily Extract](09_tavily_extract.md).

---

**Related Examples:**
- [bash/08_tavily_search_minimal.sh](../bash/08_tavily_search_minimal.sh)
- [bash/08_tavily_search_full.sh](../bash/08_tavily_search_full.sh)
- [nodejs/08_tavily_search.js](../nodejs/08_tavily_search.js)
- [python/08_tavily_search.py](../python/08_tavily_search.py)
- [go/08_tavily_search.go](../go/08_tavily_search.go)

**See also:**
- [Exercise 1: Basic Chat](01_basic_chat.md) - Using Groq for AI completions
- [Exercise 10: Tool Use](10_tool_use.md) - Combining Tavily with Groq tool calling

[← Previous: Whisper](07_whisper.md) | [Back to Exercises](README.md) | [Next: Tavily Extract →](09_tavily_extract.md)
