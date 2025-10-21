# Exercise 9: Tavily Extract - Clean Content Extraction

[← Previous: Tavily Search](08_tavily_search.md) | [Back to Exercises](README.md) | [Next: Tool Use →](10_tool_use.md)

## What You Learned

- Tavily Extract API for content extraction
- Converting web pages to clean markdown
- Image extraction from articles
- Basic vs. advanced extraction depth
- Batch URL processing

## Getting Started

Use the same Tavily API key from Exercise 8:
```bash
export TAVILY_API_KEY='tvly-your-key-here'
```

## Exercise 9.1: Basic vs. Advanced Extraction Depth

Compare extraction quality at different depths:

**Basic extraction (faster):**
```json
{
  "urls": ["https://example.com/article"],
  "extract_depth": "basic"
}
```

**Advanced extraction (more comprehensive):**
```json
{
  "urls": ["https://example.com/article"],
  "extract_depth": "advanced"
}
```

**Experiments to try:**

1. **Technical articles:**
   - Extract from: dev.to, Medium, HackerNoon
   - Compare content completeness between depths

2. **News articles:**
   - Extract from: NYTimes, BBC, Reuters
   - Measure response time difference

3. **Blog posts:**
   - Extract from: personal blogs, Substack
   - Check if basic depth misses content

**What to observe:**
- Response time difference (basic is faster)
- Content completeness (advanced may capture more)
- When is basic "good enough"?

## Exercise 9.2: Batch URL Extraction

Extract content from multiple URLs in one request:

**Multiple articles:**
```json
{
  "urls": [
    "https://example.com/article-1",
    "https://example.com/article-2",
    "https://example.com/article-3"
  ],
  "extract_depth": "basic"
}
```

**Experiments to try:**

1. **Extract a series:**
   - "Python tutorial part 1", "part 2", "part 3"
   - Save each to separate files
   - Combine into one document

2. **Compare sources:**
   - Same news story from 3 different sources
   - Extract all, compare coverage
   - Use AI to summarize differences

3. **Research collection:**
   - Gather 5-10 articles on a topic
   - Extract all content
   - Feed to AI for meta-analysis

**What to learn:**
- Batch processing efficiency
- Error handling (some URLs may fail)
- Organizing extracted content

## Exercise 9.3: Image Extraction and Analysis

Extract images from web content:

**With images:**
```json
{
  "urls": ["https://example.com/visual-guide"],
  "include_images": true
}
```

**Experiments to try:**

1. **Download all images:**
   ```bash
   # Extract image URLs, download with curl/wget
   for img in $(jq -r '.results[0].images[]' response.json); do
     wget "$img"
   done
   ```

2. **Count images per article:**
   - Track image density
   - Compare tech blogs vs. news sites

3. **Feed images to AI (combine with Exercise 3):**
   - Extract images from article
   - Analyze each with Groq vision model
   - Create image catalog with AI descriptions

**What to observe:**
- Image URL formats (some may be data URIs)
- Image relevance (are they all article images?)
- Use cases (archiving, analysis, training data)

## Exercise 9.4: Build a Read-Later Service

**Real-world application:** Create a personal read-later/web clipper service.

**Implementation flow:**
1. User provides URL via command line
2. Extract clean markdown content
3. Save to organized folder structure
4. Optional: Send to email or note-taking app

**Basic version:**

```bash
#!/bin/bash
# save_article.sh - Save articles for offline reading

URL="$1"

if [ -z "$URL" ]; then
  echo "Usage: ./save_article.sh <url>"
  exit 1
fi

# Extract content
RESPONSE=$(curl -s -X POST "https://api.tavily.com/extract" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d '{
    "urls": ["'"$URL"'"],
    "include_images": true,
    "extract_depth": "advanced"
  }')

# Get title from content (first heading)
CONTENT=$(echo "$RESPONSE" | jq -r '.results[0].raw_content')
TITLE=$(echo "$CONTENT" | grep -m 1 "^# " | sed 's/^# //')

# Create filename from title
FILENAME=$(echo "$TITLE" | tr ' ' '_' | tr '[:upper:]' '[:lower:]').md

# Save to reading_list folder
mkdir -p reading_list
echo "$CONTENT" > "reading_list/$FILENAME"

echo "✓ Saved: reading_list/$FILENAME"
```

**Usage:**
```bash
./save_article.sh "https://example.com/interesting-article"
```

**Advanced features to add:**

1. **Organize by date:**
   ```bash
   # Save to: reading_list/2025/10/article.md
   mkdir -p "reading_list/$(date +%Y/%m)"
   ```

2. **Add metadata header:**
   ```markdown
   ---
   title: Article Title
   url: https://example.com
   saved: 2025-10-01
   tags: technology, AI
   ---

   [Article content here]
   ```

3. **Extract to multiple formats:**
   ```bash
   # Also save as plain text
   echo "$CONTENT" | sed 's/#//g' > "reading_list/${FILENAME%.md}.txt"
   ```

4. **Build a searchable index:**
   ```bash
   # Create index.md with all saved articles
   echo "# My Reading List" > reading_list/index.md
   for file in reading_list/*.md; do
     echo "- [$file]($file)" >> reading_list/index.md
   done
   ```

5. **Send to email:**
   ```bash
   # Email the markdown content
   echo "$CONTENT" | mail -s "$TITLE" your@email.com
   ```

## Exercise 9.5: AI-Powered Article Summarization Pipeline

**Real-world application:** Extract article, summarize with AI, send digest.

**Implementation flow:**
1. Extract article content (Tavily)
2. Summarize with AI (Groq)
3. Generate key takeaways
4. Send via notification/email

**Full script:**

```bash
#!/bin/bash
# summarize_article.sh - Extract and summarize any article

URL="$1"

# 1. Extract content from Tavily
EXTRACT_RESPONSE=$(curl -s -X POST "https://api.tavily.com/extract" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TAVILY_API_KEY" \
  -d '{
    "urls": ["'"$URL"'"],
    "extract_depth": "advanced"
  }')

CONTENT=$(echo "$EXTRACT_RESPONSE" | jq -r '.results[0].raw_content')

# Check content length (Groq has token limits)
CONTENT_LENGTH=$(echo "$CONTENT" | wc -c)

if [ $CONTENT_LENGTH -gt 20000 ]; then
  # Truncate to first 20000 chars if too long
  CONTENT=$(echo "$CONTENT" | head -c 20000)
fi

# 2. Send to Groq for summarization
SUMMARY_RESPONSE=$(curl -s -X POST "https://api.groq.com/openai/v1/chat/completions" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "system",
        "content": "You are an expert at summarizing articles. Create a concise summary with: 1) One-sentence overview, 2) Three key takeaways (bullets), 3) Target audience. Keep under 150 words."
      },
      {
        "role": "user",
        "content": "Summarize this article:\n\n'"$(echo "$CONTENT" | jq -Rs .)"'"
      }
    ],
    "temperature": 0.3,
    "max_completion_tokens": 300
  }')

SUMMARY=$(echo "$SUMMARY_RESPONSE" | jq -r '.choices[0].message.content')

# 3. Display summary
echo "=========================================="
echo "Article Summary"
echo "=========================================="
echo ""
echo "URL: $URL"
echo ""
echo "$SUMMARY"
echo ""

# 4. Optional: Send via ntfy.sh
read -p "Send notification? (y/n): " SEND_NOTIF

if [ "$SEND_NOTIF" = "y" ]; then
  curl -s -X POST "https://ntfy.sh/my-article-summaries" \
    -H "Title: Article Summary" \
    -H "Tags: book,memo" \
    -d "$SUMMARY" > /dev/null
  echo "✓ Notification sent to ntfy.sh/my-article-summaries"
fi
```

**Usage:**
```bash
./summarize_article.sh "https://example.com/long-article"
```

**Advanced enhancements:**

1. **Auto-categorization:**
   ```bash
   # Ask AI to categorize: tech, business, science, etc.
   CATEGORY=$(curl ... -d '{"prompt": "Categorize: $SUMMARY"}')
   ```

2. **Multi-language support:**
   ```bash
   # Detect language, translate summary to English
   LANG=$(echo "$CONTENT" | head -100 | detect-language)
   ```

3. **Save to database:**
   ```bash
   # SQLite storage for article history
   sqlite3 articles.db "INSERT INTO summaries VALUES ('$URL', '$SUMMARY', datetime())"
   ```

4. **Generate audio summary:**
   ```bash
   # Use text-to-speech API to create audio version
   echo "$SUMMARY" | tts-api > summary.mp3
   ```

5. **Comparison mode:**
   ```bash
   # Extract and compare 2+ articles on same topic
   ./summarize_article.sh url1 url2
   # AI prompt: "Compare and contrast these articles"
   ```

**Cost calculation:**
- Tavily Extract: 1 call (free tier)
- Groq completion: ~500 tokens = $0.0002
- **Total per summary: ~$0.0002**

## Exercise 9.6: Build a Personal Knowledge Base

**Goal:** Extract and organize web content into a searchable knowledge base.

**Structure:**
```
knowledge_base/
├── technology/
│   ├── ai/
│   │   ├── article1.md
│   │   └── article2.md
│   └── web_dev/
├── science/
└── business/
```

**Features to implement:**

1. **Auto-categorization:**
   - Use Groq to suggest category from content
   - Organize into folder structure

2. **Tagging system:**
   - Extract keywords with AI
   - Add as frontmatter tags

3. **Full-text search:**
   - Use `grep -r` or `ripgrep`
   - Build search script

4. **Related articles:**
   - Find similar content based on keywords
   - Suggest further reading

5. **Export to Obsidian/Notion:**
   - Format for note-taking apps
   - Include backlinks

## Reflection Questions

After completing these exercises, consider:

1. When would you use `basic` vs. `advanced` extraction depth?
2. How do you handle failed extractions in batch processing?
3. What's the best way to organize extracted content?
4. How can you validate extraction quality?
5. What other services could you build with content extraction?

## Next Steps

Learn about AI tool use with [Exercise 10: Tool Use](10_tool_use.md) - combining Tavily Search and Extract with Groq function calling.

---

**Related Examples:**
- [bash/09_tavily_extract_minimal.sh](../bash/09_tavily_extract_minimal.sh)
- [bash/09_tavily_extract_full.sh](../bash/09_tavily_extract_full.sh)
- [nodejs/09_tavily_extract.js](../nodejs/09_tavily_extract.js)
- [python/09_tavily_extract.py](../python/09_tavily_extract.py)
- [go/09_tavily_extract.go](../go/09_tavily_extract.go)

**See also:**
- [Exercise 8: Tavily Search](08_tavily_search.md) - Web search with AI
- [Exercise 3: Vision](03_vision.md) - Analyze extracted images

[← Previous: Tavily Search](08_tavily_search.md) | [Back to Exercises](README.md) | [Next: Tool Use →](10_tool_use.md)
