# Exercise 4: Safety Check (Text) - Content Moderation

[← Previous: Vision](03_vision.md) | [Back to Exercises](README.md) | [Next: Safety Image →](05_safety_image.md)

## What You Learned

- LlamaGuard model
- Safety categories
- Content classification
- Moderation workflows

## Exercise 4.1: Boundary Testing

Test increasingly risky prompts to understand safety thresholds:
- Safe: "How do I bake a cake?"
- Borderline: "How do I build a potato gun for a science project?"
- Risky: "How do I pick a lock?" (locksmith context vs. burglary context)

**What to observe:** Where does LlamaGuard draw the line? How does context affect safety classification?

## Exercise 4.2: False Positive Hunting

Find safe content that LlamaGuard flags as unsafe:
- Medical information (might trigger health misinformation)
- Historical discussions (might trigger violence)
- Fiction writing (might trigger various categories)

**What to learn:** Understanding false positives and safety model limitations.

## Exercise 4.3: Language and Culture Testing

Test the same concept in different languages or cultural contexts:
- English vs. Spanish vs. French
- Direct statements vs. euphemisms
- American vs. British English phrasing

**What to observe:** Is safety detection consistent across languages and cultures?

## Exercise 4.4: Safety Category Analysis

Create test cases for each specific safety category:
- S1: Violent Crimes
- S2: Non-Violent Crimes
- S3: Sex-Related Crimes
- S4: Child Sexual Exploitation
- S5: Defamation
- S6: Specialized Advice
- S7: Privacy
- S8: Intellectual Property
- S9: Indiscriminate Weapons
- S10: Hate
- S11: Suicide & Self-Harm
- S12: Sexual Content
- S13: Elections

**What to learn:** Which categories are most/least sensitive? Create a safety testing suite.

## Exercise 4.5: Two-Stage Safety Pipeline

Build a workflow: User input → Safety check → AI response → Safety check
- First check: Validate user input
- Second check: Validate AI output
- Track: Which stage catches what?

**What to learn:** Implementing production-grade content moderation.

## Exercise 4.6: Gamer Tag Validator - Real-World UI Form Safety

Build a practical mini-application that validates user-submitted gamer tags/usernames using LlamaGuard before allowing them into your system.

**Real-world use case:** Gaming platforms, social networks, forums, and chat applications need to prevent offensive usernames at registration time.

**Application flow:**
1. User enters a gamer tag
2. Check the tag with LlamaGuard (`meta-llama/llama-guard-4-12b`)
3. If safe → Accept and confirm
4. If unsafe → Reject with reason and ask for another name
5. Loop until a safe name is provided

**Basic implementation pattern:**

```javascript
// Pseudocode - adapt to your language
function validateGamerTag(tag) {
  // Call LlamaGuard with the username
  const result = callLlamaGuard({
    model: "meta-llama/llama-guard-4-12b",
    messages: [{
      role: "user",
      content: tag
    }]
  });

  // Parse response: "safe" or "unsafe\nS10" (category code)
  const response = result.choices[0].message.content.trim();

  if (response === "safe") {
    return { safe: true };
  } else {
    // Extract category (e.g., "S10" for Hate)
    const lines = response.split('\n');
    const category = lines[1] || "unknown";
    return { safe: false, category: category, reason: getCategoryName(category) };
  }
}

function getCategoryName(code) {
  const categories = {
    "S1": "Violent Crimes",
    "S5": "Defamation",
    "S10": "Hate Speech",
    "S12": "Sexual Content",
    // ... add all 13 categories
  };
  return categories[code] || "Inappropriate Content";
}
```

**Implementation challenges:**

1. **Command-line version:**
   ```bash
   # Simple bash loop
   while true; do
     read -p "Enter gamer tag: " tag
     # Call LlamaGuard via curl
     # Parse response
     # Accept or reject
   done
   ```

2. **Interactive prompt version (Node.js/Python):**
   - Use `readline` (Node.js) or `input()` (Python)
   - Keep asking until a safe name is provided
   - Display helpful feedback on why a name was rejected

3. **Web form version (bonus):**
   - Create a simple HTML form
   - Add client-side JavaScript to call your API
   - Show real-time validation feedback
   - Display green checkmark or red X with reason

**Test cases to try:**

**Should be SAFE:**
- `"DragonSlayer42"`
- `"CodeNinja"`
- `"PixelMaster"`
- `"SwiftGamer"`

**Should be UNSAFE (test boundary cases):**
- Offensive words (S10: Hate)
- Sexual content (S12: Sexual Content)
- Celebrity names (S8: Intellectual Property - maybe)
- Profanity or slurs (S10: Hate)

**Advanced features to add:**

1. **Rate limiting:**
   - Track API calls (each check costs ~$0.0001)
   - Limit to 3 attempts per minute

2. **Profanity filter fallback:**
   - Use a local word list for instant rejection
   - Only call LlamaGuard if it passes basic filter
   - Saves API costs

3. **Logging and analytics:**
   - Log all rejected tags (for improving filters)
   - Track which categories are most common
   - Calculate cost per user registration

4. **Username suggestions:**
   - If rejected, suggest alternatives
   - `"That name isn't safe. Try: CodeWarrior, PixelKnight, SwiftNinja"`

5. **Multi-language support:**
   - Test with non-English characters
   - Does LlamaGuard catch offensive terms in other languages?

**What to learn:**
- Practical application of content moderation APIs
- User experience design for safety features
- Balance between security and user friction
- Cost management in production (how much does username validation cost?)
- Handling edge cases and false positives

**Bonus: Calculate the cost**
- Each LlamaGuard check: ~20 input tokens, ~5 output tokens
- Cost: (20 × $0.20/1M) + (5 × $0.20/1M) = $0.000005 per check
- 1 million user registrations (avg 2 attempts each): 2M checks = **$10 total**

**Challenge:** Can you build this in under 100 lines of code in your language?

## Exercise 4.7: Migrate to OpenAI Moderation API (FREE Alternative)

Learn how to adapt your code to use OpenAI's **omni-moderation-latest** model - a multimodal moderation API that's **completely FREE** for all OpenAI API users!

**Why try OpenAI Moderation?**
- **100% FREE** - No cost, doesn't count toward usage limits
- **Multimodal** - Handles both text and images
- **Built on GPT-4o** - Advanced accuracy, especially for non-English languages
- **Different categories** - Compare with LlamaGuard's approach

**What needs to change:**

| Parameter | Groq (LlamaGuard) | OpenAI (Omni Moderation) |
|-----------|-------------------|--------------------------|
| **Base URL** | `https://api.groq.com/openai/v1/chat/completions` | `https://api.openai.com/v1/moderations` |
| **API Key** | `GROQ_API_KEY` environment variable | `OPENAI_API_KEY` environment variable |
| **Model** | `meta-llama/llama-guard-4-12b` | `omni-moderation-latest` |
| **Request format** | Chat completion (`messages` array) | Moderation (`input` field) |
| **Response format** | `choices[0].message.content` = `"safe"` or `"unsafe\nS10"` | `results[0].flagged` = boolean + category scores |

**Getting an OpenAI API Key:**

1. Go to https://platform.openai.com/signup
2. Create a free account
3. Navigate to https://platform.openai.com/api-keys
4. Click "Create new secret key"
5. Copy the key (starts with `sk-proj-...`)
6. Set environment variable:
   ```bash
   export OPENAI_API_KEY="sk-proj-your-key-here"
   ```

**Request format comparison:**

**Groq (LlamaGuard):**
```bash
curl https://api.groq.com/openai/v1/chat/completions \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "meta-llama/llama-guard-4-12b",
    "messages": [{"role": "user", "content": "DragonSlayer42"}]
  }'
```

**OpenAI (Moderation):**
```bash
curl https://api.openai.com/v1/moderations \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "omni-moderation-latest",
    "input": "DragonSlayer42"
  }'
```

**Response format comparison:**

**Groq response:**
```json
{
  "choices": [{
    "message": {
      "content": "safe"
    }
  }]
}
```
or
```json
{
  "choices": [{
    "message": {
      "content": "unsafe\nS10"
    }
  }]
}
```

**OpenAI response:**
```json
{
  "id": "modr-123",
  "model": "omni-moderation-latest",
  "results": [{
    "flagged": false,
    "categories": {
      "sexual": false,
      "hate": false,
      "harassment": false,
      "self-harm": false,
      "sexual/minors": false,
      "hate/threatening": false,
      "violence/graphic": false,
      "self-harm/intent": false,
      "self-harm/instructions": false,
      "harassment/threatening": false,
      "violence": false
    },
    "category_scores": {
      "sexual": 0.0001,
      "hate": 0.0002,
      "harassment": 0.0001,
      "self-harm": 0.0000,
      "sexual/minors": 0.0000,
      "hate/threatening": 0.0000,
      "violence/graphic": 0.0000,
      "self-harm/intent": 0.0000,
      "self-harm/instructions": 0.0000,
      "harassment/threatening": 0.0000,
      "violence": 0.0001
    }
  }]
}
```

**Category mapping:**

| LlamaGuard | OpenAI Moderation |
|------------|-------------------|
| S1: Violent Crimes | `violence`, `violence/graphic` |
| S3: Sex-Related Crimes | `sexual`, `sexual/minors` |
| S5: Defamation | *(No direct equivalent - covered by `harassment`)* |
| S10: Hate | `hate`, `hate/threatening` |
| S11: Suicide & Self-Harm | `self-harm`, `self-harm/intent`, `self-harm/instructions` |
| S12: Sexual Content | `sexual` |

**Implementation challenge:**

Adapt Exercise 4.6 (Gamer Tag Validator) to use OpenAI's moderation API:

1. Change the base URL and endpoint
2. Update request format from `messages` to `input`
3. Parse the new response format (`results[0].flagged`)
4. Map OpenAI categories to user-friendly messages
5. Add confidence score display (from `category_scores`)

**Code changes needed (pseudocode):**

```javascript
// BEFORE (Groq)
const response = await fetch('https://api.groq.com/openai/v1/chat/completions', {
  headers: { 'Authorization': `Bearer ${process.env.GROQ_API_KEY}` },
  body: JSON.stringify({
    model: 'meta-llama/llama-guard-4-12b',
    messages: [{ role: 'user', content: gamerTag }]
  })
});
const result = response.choices[0].message.content;
const isSafe = result === 'safe';

// AFTER (OpenAI)
const response = await fetch('https://api.openai.com/v1/moderations', {
  headers: { 'Authorization': `Bearer ${process.env.OPENAI_API_KEY}` },
  body: JSON.stringify({
    model: 'omni-moderation-latest',
    input: gamerTag
  })
});
const result = response.results[0];
const isSafe = !result.flagged;
const violatedCategories = Object.keys(result.categories).filter(k => result.categories[k]);
```

**Exercises to try:**

1. **Side-by-side comparison:**
   - Run the same gamer tag through both APIs
   - Compare which service flags what
   - Which has more/fewer false positives?

2. **Confidence score analysis:**
   - OpenAI provides 0-1 scores for each category
   - Create a custom threshold (e.g., flag if any score > 0.5)
   - Compare with OpenAI's built-in `flagged` boolean

3. **Cost vs. accuracy:**
   - Groq LlamaGuard: ~$0.000005 per check
   - OpenAI Moderation: **$0.00 (FREE!)**
   - Does "free" mean lower quality? Test and compare!

4. **Multi-language testing:**
   - OpenAI claims better non-English support
   - Test gamer tags in Spanish, French, Japanese
   - Which API performs better?

5. **Image moderation (bonus):**
   - OpenAI's `omni-moderation-latest` supports images
   - Send a base64-encoded image or image URL
   - Compare with [Exercise 5](05_safety_image.md) (LlamaGuard Vision)

**Image moderation request format:**

**Using image URL:**
```bash
curl https://api.openai.com/v1/moderations \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "omni-moderation-latest",
    "input": [
      {
        "type": "image_url",
        "image_url": {
          "url": "https://example.com/image.jpg"
        }
      }
    ]
  }'
```

**Using base64-encoded image:**
```bash
# First encode your image
IMAGE_BASE64=$(base64 -i test_image.jpg)

curl https://api.openai.com/v1/moderations \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "omni-moderation-latest",
    "input": [
      {
        "type": "image_url",
        "image_url": {
          "url": "data:image/jpeg;base64,'"$IMAGE_BASE64"'"
        }
      }
    ]
  }'
```

**Combining text and images:**
```json
{
  "model": "omni-moderation-latest",
  "input": [
    {
      "type": "text",
      "text": "User comment: Check this out!"
    },
    {
      "type": "image_url",
      "image_url": {
        "url": "https://example.com/user_upload.jpg"
      }
    }
  ]
}
```

**What to learn:**
- How to migrate between different AI API providers
- Understanding trade-offs (cost vs. features vs. accuracy)
- Parsing different response formats
- Multimodal input handling (text + images in single request)
- When "free" makes sense in production (OpenAI subsidizes moderation to improve their ecosystem)

**Documentation:**
- https://platform.openai.com/docs/guides/moderation
- https://platform.openai.com/docs/guides/moderation?example=images#quickstart
- https://platform.openai.com/docs/api-reference/moderations

**Reminder: OpenAI Moderation is FREE!** This makes it excellent for high-volume applications like username validation, chat filtering, or UGC (user-generated content) moderation where cost would otherwise be prohibitive.

## Reflection Questions

After completing these exercises, consider:

1. When would you use LlamaGuard vs. OpenAI Moderation?
2. How do you balance false positives vs. false negatives?
3. What's the cost difference for high-volume applications?
4. How do you handle edge cases in production?

## Next Steps

Explore image safety with [Exercise 5: Safety Check (Image)](05_safety_image.md).

---

**Related Examples:**
- [bash/04_safety_check_minimal.sh](../bash/04_safety_check_minimal.sh)
- [bash/04_safety_check_full.sh](../bash/04_safety_check_full.sh)
- [nodejs/04_safety_check.js](../nodejs/04_safety_check.js)
- [python/04_safety_check.py](../python/04_safety_check.py)
- [go/04_safety_check.go](../go/04_safety_check.go)

[← Previous: Vision](03_vision.md) | [Back to Exercises](README.md) | [Next: Safety Image →](05_safety_image.md)
