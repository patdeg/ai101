# Exercise 3: Vision - Image Analysis

[← Previous: System Prompt](02_system_prompt.md) | [Back to Exercises](README.md) | [Next: Safety Text →](04_safety_text.md)

## What You Learned

- Multimodal AI
- Base64 encoding
- Vision capabilities
- Image processing

## Exercise 3.1: Image Resolution Testing

Take the same photo at different resolutions and test:
- Original (e.g., 4000×3000 pixels)
- Medium (1920×1080 pixels)
- Small (640×480 pixels)
- Tiny (320×240 pixels)

**Question:** "Describe this image in detail."

**What to observe:** At what resolution does the AI start missing details? Is there a quality/cost sweet spot?

## Exercise 3.2: Image Compression Experiment

Save the same image with different JPEG quality levels:
- Quality 100% (highest, largest file)
- Quality 75% (high, medium file)
- Quality 50% (medium, smaller file)
- Quality 25% (low, tiny file)

**What to learn:** How much can you compress images before vision accuracy degrades? Track file size vs. description quality.

## Exercise 3.3: Multi-Image Comparison

Send 2-5 images in one request (API supports up to 5):

```json
{
  "content": [
    {"type": "text", "text": "Compare these images. What's different?"},
    {"type": "image_url", "image_url": {"url": "data:image/jpeg;base64,..."}},
    {"type": "image_url", "image_url": {"url": "data:image/jpeg;base64,..."}}
  ]
}
```

**Ideas to try:**
- Two photos of same object from different angles
- Before/after photos
- Spot-the-difference images

**What to learn:** How well AI compares and contrasts multiple images.

## Exercise 3.4: OCR and Text Detection

Test the AI's text recognition abilities:
- Photo of a book page
- Screenshot of code
- Picture of a handwritten note
- Photo of a street sign
- Image with text in different languages

**Question:** "Extract all text from this image."

**What to observe:** Accuracy with different fonts, handwriting, languages, and text sizes.

## Exercise 3.5: Specific Question Types

Test different vision tasks:
- **Counting:** "How many people are in this image?"
- **Color detection:** "What are the dominant colors?"
- **Object location:** "Where is the cat in this image?"
- **Emotion:** "What emotion is the person expressing?"
- **Scene understanding:** "Is this indoors or outdoors?"

**What to learn:** Which visual tasks the AI handles well vs. poorly.

## Reflection Questions

After completing these exercises, consider:

1. What's the minimum image resolution needed for accurate analysis?
2. How does compression affect AI vision performance?
3. When would multi-image comparison be useful?
4. What are the limits of OCR with this model?

## Next Steps

Learn about content safety with [Exercise 4: Safety Check (Text)](04_safety_text.md).

---

**Related Examples:**
- [bash/03_vision_minimal.sh](../bash/03_vision_minimal.sh)
- [bash/03_vision_full.sh](../bash/03_vision_full.sh)
- [nodejs/03_vision.js](../nodejs/03_vision.js)
- [python/03_vision.py](../python/03_vision.py)
- [go/03_vision.go](../go/03_vision.go)
- [arduino/03_vision/03_vision.ino](../arduino/03_vision/03_vision.ino)

[← Previous: System Prompt](02_system_prompt.md) | [Back to Exercises](README.md) | [Next: Safety Text →](04_safety_text.md)
