# Exercise 5: Safety Check (Image) - Vision Moderation

[← Previous: Safety Text](04_safety_text.md) | [Back to Exercises](README.md) | [Next: Prompt Guard →](06_prompt_guard.md)

## What You Learned

- Vision-based safety
- Multimodal moderation
- Image content classification
- Context-aware moderation

## Exercise 5.1: Image Category Testing

Test images from different categories:
- Completely safe (nature, food, objects)
- Borderline (beach photos, art, medical diagrams)
- Test the model's handling of artistic content vs. inappropriate content

**What to observe:** How does vision safety differ from text safety?

## Exercise 5.2: Context Matters

Test the same image with different text prompts:
- Image: Kitchen knife
  - Text: "Is this safe to use for cooking?"
  - Text: "Can I use this as a weapon?"

**What to learn:** How text context influences image safety classification.

## Exercise 5.3: Artistic Content

Test classical art, medical diagrams, and educational content:
- Renaissance paintings (may contain nudity)
- Anatomy textbooks
- News photos from conflict zones

**What to observe:** Can the AI distinguish educational/artistic content from inappropriate content?

## Exercise 5.4: Cropping and Framing

Take the same image and crop differently:
- Full context visible
- Tight crop on specific element
- Multiple crops focusing on different parts

**What to learn:** How framing affects safety classification.

## Exercise 5.5: Combined Text + Image Safety

Build a full moderation pipeline:
- Check text prompt ([Exercise 4](04_safety_text.md))
- Check image content (Example 5)
- Check AI response about image ([Exercise 4](04_safety_text.md))

**What to learn:** Implementing comprehensive multimodal content moderation.

## Exercise 5.6: OpenAI Omni-Moderation Comparison

Compare LlamaGuard Vision with OpenAI's free omni-moderation-latest model for image safety:

**OpenAI Image Moderation:**
```bash
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

**Image moderation response:**
```json
{
  "id": "modr-123",
  "model": "omni-moderation-latest",
  "results": [{
    "flagged": true,
    "categories": {
      "violence": true,
      "violence/graphic": false,
      "self-harm": false,
      "sexual": false
    },
    "category_scores": {
      "violence": 0.89,
      "violence/graphic": 0.12,
      "self-harm": 0.01,
      "sexual": 0.03
    },
    "category_applied_input_types": {
      "violence": ["image"],
      "violence/graphic": [],
      "self-harm": [],
      "sexual": []
    }
  }]
}
```

**Note:** Image moderation currently supports 6 categories: `violence`, `violence/graphic`, `self-harm`, `self-harm/intent`, `self-harm/instructions`, and `sexual` (but not `sexual/minors` for images).

**Comparison exercises:**

1. **Accuracy comparison:**
   - Run the same images through both APIs
   - Which catches more violations?
   - Which has more false positives?

2. **Cost comparison:**
   - LlamaGuard Vision: ~$0.0001 per image
   - OpenAI Omni-Moderation: **FREE**
   - Is free worth any potential accuracy trade-offs?

3. **Category coverage:**
   - LlamaGuard: 13 categories (S1-S13)
   - OpenAI: 6 image categories
   - Which categories are missing from each?

4. **Combined text + image:**
   - OpenAI supports both in one request (see [Exercise 4.7](04_safety_text.md))
   - Compare vs. separate calls to LlamaGuard (text) and LlamaGuard Vision (image)

## Reflection Questions

After completing these exercises, consider:

1. How does image moderation differ from text moderation?
2. When is context (accompanying text) important for image safety?
3. How do you handle artistic or educational content?
4. Which API is better for your use case: LlamaGuard Vision or OpenAI Omni-Moderation?

## Next Steps

Learn about prompt security with [Exercise 6: Prompt Guard](06_prompt_guard.md).

---

**Related Examples:**
- [bash/05_image_safety_check_minimal.sh](../bash/05_image_safety_check_minimal.sh)
- [bash/05_image_safety_check_full.sh](../bash/05_image_safety_check_full.sh)

**See also:**
- [Exercise 4: Safety Check (Text)](04_safety_text.md) - Text moderation
- [Exercise 4.7](04_safety_text.md#exercise-47-migrate-to-openai-moderation-api-free-alternative) - OpenAI Moderation API

[← Previous: Safety Text](04_safety_text.md) | [Back to Exercises](README.md) | [Next: Prompt Guard →](06_prompt_guard.md)
