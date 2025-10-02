# Exercise 11: Web Search (groq/compound-mini)

[← Previous: Tool Use](10_tool_use.md) | [Back to Exercises](README.md) | [Next: Code Execution →](12_code_execution.md)

## What You’ll Learn
- Using a built-in web-search-capable model (`groq/compound-mini`)
- Crafting queries that trigger search (fresh, factual, time-bound)
- Comparing model variants and query logic (AND/OR patterns)
- Applying domain/country hints where supported
- Reading optional reasoning and executed tool info

## Getting Started

Run a baseline example in your preferred language:
- Bash: `./bash/11_web_search_full.sh`
- Node: `node nodejs/11_web_search.js`
- Python: `python3 python/11_web_search.py`
- Go: `go run go/11_web_search.go`

Tip: Keep the terminal open so you can quickly rerun with small edits.

## Exercise 11.1: Fresh Events vs. Timeless Facts

Goal: Learn when the model chooses to search.

Try two prompts:
1) “What were the top 3 AI model releases last week? Include links.”
2) “Explain how binary search works.”

Predict first: Which one needs web search? Why?

Success criteria:
- You get up-to-date links for the first prompt.
- The second prompt may not show any search (it’s timeless).

Reflect:
- How do you detect that a search happened? (Look for citations, reasoning, or executed tools.)

## Exercise 11.2: Boolean Logic Queries (AND/OR)

Goal: Practice precise information needs.

Prompt ideas:
- “(OpenAI OR Meta) AND release notes in the last 7 days. Include links and brief summaries.”
- “(NVIDIA OR AMD) AND GPU architecture updates this month.”

Try different operators and time windows.

Success criteria:
- Your answer clearly matches both sides of your boolean logic.
- You receive credible links matching the brands/topics.

Reflect:
- Did the model respect your AND/OR intent? How did you verify correctness?

## Exercise 11.3: Source Control (Include/Exclude Domains)

Goal: Improve signal-to-noise by steering sources. Some deployments allow specifying domain hints; only add these if your docs/account confirm support.

If supported, extend your request (in the payload) with a web search config such as:
```
include_domains: ["arxiv.org", "openai.com", "ai.meta.com"],
exclude_domains: ["reddit.com", "twitter.com"],
```
Or run two versions: one unrestricted, one with manual filtering (ask the model: “prefer primary sources, official blogs, academic sites”).

Success criteria:
- With include/exclude, more results come from the intended sources.
- Without include/exclude, you can still steer the model using instruction-only.

Reflect:
- Which produced higher-quality citations? Any trade-offs (fewer results, narrower scope)?

## Exercise 11.4: Country Hint and Localization

Goal: Observe geographic bias in results.

If supported by your deployment, add a country hint (e.g., `"country": "us"`). If not supported, do an instruction-only version: “Prioritize U.S. sources and spellings.” Then try “Prioritize UK sources and spellings.”

Success criteria:
- Noticeable differences in cited publications and spelling.
- Links align with the requested region.

Reflect:
- When would localization matter in real products? How could you expose it as a user setting?

## Exercise 11.5: Model Comparison (compound-mini vs. compound)

Goal: Evaluate trade-offs between model sizes.

Run the same query on:
- `groq/compound-mini`
- `groq/compound` (larger)

Success criteria:
- You can articulate 2 pros and 2 cons for each (speed, completeness, quality, cost).
- You decide which is better for your use case.

Reflect:
- Does the bigger model always win? When is “mini” good enough?

## Stretch: Build a Mini Research Assistant

Combine what you learned:
- Input: a topic and time window
- Output: 3–5 bullet takeaways with citations (links), plus a 2-sentence executive summary
- Optionally apply domain/country preferences (if supported)

Ship it as a script in your language of choice. Share a screenshot of the output.

## Reflection Questions
- How do you decide when to trust search outputs vs. ask for more verification?
- What signals indicate a good citation (publisher, date, author)?
- How would you handle conflicting sources?

## Related Examples
- [bash/11_web_search_full.sh](../bash/11_web_search_full.sh)
- [nodejs/11_web_search.js](../nodejs/11_web_search.js)
- [python/11_web_search.py](../python/11_web_search.py)
- [go/11_web_search.go](../go/11_web_search.go)

[← Previous: Tool Use](10_tool_use.md) | [Back to Exercises](README.md) | [Next: Code Execution →](12_code_execution.md)

