#!/usr/bin/env python3
"""
03_prompt_template.py - Compile prompt templates for dynamic AI interactions

This example demonstrates advanced prompt engineering using templates:
1. Load template files with variables and conditional logic
2. Remove comment lines (///) for cleaner prompts
3. Substitute variables like timestamps and user inputs
4. Support basic conditional logic
5. Generate dynamic, context-aware prompts

Template features:
- Variables: [[.VarName]] replaced with actual values
- Comments: Lines starting with /// are removed
- Conditionals: Basic if/else based on category
- Timestamps: [[.Now]] replaced with current time

Usage:
    python 03_prompt_template.py [category] [topic]

Examples:
    python 03_prompt_template.py "Science" "Quantum Computing"
    python 03_prompt_template.py "History" "The Industrial Revolution"
    python 03_prompt_template.py "Technology" "Artificial Intelligence"

Environment:
    DEMETERICS_API_KEY - Your Demeterics Managed LLM Key (required)
"""

import os
import sys
import json
import re
from datetime import datetime, timezone
from pathlib import Path
import http.client
from typing import Dict, Any, Tuple, Optional

# Configuration
API_URL = "api.demeterics.com"
API_PATH = "/groq/v1/chat/completions"
MODEL = "meta-llama/llama-4-scout-17b-16e-instruct"

# Template configuration
TEMPLATE_DIR = Path("../templates")
TEMPLATE_FILE = TEMPLATE_DIR / "essay_writer.txt"

# Pricing (per million tokens)
INPUT_PRICE = 0.11
OUTPUT_PRICE = 0.34

def create_example_template() -> None:
    """Create an example template file if it doesn't exist."""
    TEMPLATE_DIR.mkdir(parents=True, exist_ok=True)

    template_content = '''/// Template for writing educational essays
/// Variables: [[.Now]] = current timestamp, [[.Category]] = essay category, [[.Topic]] = essay topic
/// Comments starting with /// are removed during compilation

# 🎯 OBJECTIVE

Write an engaging, educational essay about [[.Topic]] in the [[.Category]] category.
The essay should be appropriate for students aged 14-18, informative yet accessible.
Current timestamp: [[.Now]]

# 📝 ESSAY REQUIREMENTS

* **Length:** 500-750 words (approximately 5-7 paragraphs)
* **Structure:** Introduction, 3-4 body paragraphs, conclusion
* **Tone:** Educational, engaging, age-appropriate
* **Citations:** Include at least 3 factual references

# 📚 CATEGORY: [[.Category]]

[[if .Category == "History"]]
Focus on:
- Cause and effect relationships
- Specific dates and key figures
- Historical significance and modern relevance
[[else if .Category == "Science"]]
Focus on:
- Fundamental concepts explained simply
- Real-world applications
- Scientific method and evidence
[[else if .Category == "Technology"]]
Focus on:
- Technical concepts in simple terms
- Benefits and challenges
- Future implications
[[else]]
Focus on:
- Comprehensive overview
- Relevant examples
- Balanced perspective
[[end]]

# OUTPUT FORMAT

Provide the essay with:
- Clear, descriptive title
- Well-structured paragraphs
- Key terms defined
- 2-3 discussion questions at the end'''

    TEMPLATE_FILE.write_text(template_content)
    print(f"✅ Created template file: {TEMPLATE_FILE}")

class TemplateProcessor:
    """Process templates with variable substitution and basic conditionals."""

    def __init__(self, template_path: Path):
        """Initialize the template processor.

        Args:
            template_path: Path to the template file
        """
        self.template_path = template_path
        self.template_content = self._load_template()

    def _load_template(self) -> str:
        """Load template from file.

        Returns:
            Template content as string

        Raises:
            FileNotFoundError: If template file doesn't exist
        """
        if not self.template_path.exists():
            raise FileNotFoundError(f"Template not found: {self.template_path}")
        return self.template_path.read_text()

    def process(self, variables: Dict[str, Any]) -> str:
        """Process template with variable substitution.

        Args:
            variables: Dictionary of variables to substitute

        Returns:
            Processed template string
        """
        processed = self.template_content

        # Step 1: Remove comment lines (///)
        processed = self._remove_comments(processed)

        # Step 2: Substitute variables
        processed = self._substitute_variables(processed, variables)

        # Step 3: Process conditionals
        processed = self._process_conditionals(processed, variables)

        # Step 4: Clean up any remaining markers
        processed = self._cleanup_markers(processed)

        return processed

    def _remove_comments(self, text: str) -> str:
        """Remove lines starting with ///

        Args:
            text: Template text

        Returns:
            Text with comment lines removed
        """
        lines = text.split('\n')
        filtered_lines = [line for line in lines if not line.strip().startswith('///')]
        return '\n'.join(filtered_lines)

    def _substitute_variables(self, text: str, variables: Dict[str, Any]) -> str:
        """Substitute [[.VarName]] with actual values.

        Args:
            text: Template text
            variables: Variable dictionary

        Returns:
            Text with variables substituted
        """
        for key, value in variables.items():
            # Skip Topic variable as it will be in user message
            if key == "Topic":
                continue
            pattern = f"\\[\\[\\.{key}\\]\\]"
            text = re.sub(pattern, str(value), text)
        return text

    def _process_conditionals(self, text: str, variables: Dict[str, Any]) -> str:
        """Process basic conditional blocks.

        This is a simplified implementation supporting:
        - [[if .Category == "X"]] ... [[end]]
        - [[else if .Category == "Y"]] ... [[end]]
        - [[else]] ... [[end]]

        Args:
            text: Template text
            variables: Variable dictionary

        Returns:
            Text with conditionals processed
        """
        category = variables.get('Category', '')

        # Pattern to match conditional blocks
        pattern = r'\[\[if \.Category == "(.*?)"\]\](.*?)\[\[end\]\]'

        def replace_conditional(match):
            condition_value = match.group(1)
            content = match.group(2)

            # Check if condition matches
            if condition_value == category:
                # Process the content for this condition
                # Remove else blocks
                content = re.sub(r'\[\[else.*?\]\].*?(?=\[\[end\]\]|\Z)', '', content, flags=re.DOTALL)
                return content.strip()
            else:
                # Look for else if or else blocks
                else_pattern = r'\[\[else if \.Category == "' + re.escape(category) + r'"\]\](.*?)(?=\[\[else|\[\[end\]\])'
                else_match = re.search(else_pattern, content, re.DOTALL)
                if else_match:
                    return else_match.group(1).strip()

                # Look for generic else
                else_pattern = r'\[\[else\]\](.*?)(?=\[\[end\]\]|\Z)'
                else_match = re.search(else_pattern, content, re.DOTALL)
                if else_match:
                    return else_match.group(1).strip()

            return ''

        # Process conditionals
        text = re.sub(pattern, replace_conditional, text, flags=re.DOTALL)

        return text

    def _cleanup_markers(self, text: str) -> str:
        """Remove any remaining conditional markers.

        Args:
            text: Template text

        Returns:
            Cleaned text
        """
        # Remove any remaining conditional markers
        markers = [r'\[\[if.*?\]\]', r'\[\[else.*?\]\]', r'\[\[end\]\]']
        for marker in markers:
            text = re.sub(marker, '', text)

        # Clean up multiple blank lines
        text = re.sub(r'\n\n\n+', '\n\n', text)

        return text.strip()

def make_api_request(system_prompt: str, user_prompt: str, api_key: str) -> Dict[str, Any]:
    """Make request to Demeterics Groq proxy.

    Args:
        system_prompt: System message content
        user_prompt: User message content
        api_key: API key for authentication

    Returns:
        API response as dictionary

    Raises:
        Exception: If API request fails
    """
    # Prepare request data
    request_data = {
        "model": MODEL,
        "messages": [
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt}
        ],
        "max_tokens": 2000,
        "temperature": 0.7
    }

    # Convert to JSON
    json_data = json.dumps(request_data)

    # Make HTTPS connection
    conn = http.client.HTTPSConnection(API_URL)

    try:
        # Set headers
        headers = {
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json"
        }

        # Send request
        conn.request("POST", API_PATH, json_data, headers)

        # Get response
        response = conn.getresponse()
        response_data = response.read().decode('utf-8')

        # Parse JSON response
        result = json.loads(response_data)

        # Check for API errors
        if 'error' in result:
            raise Exception(f"API Error: {result['error'].get('message', 'Unknown error')}")

        return result

    finally:
        conn.close()

def display_response(response: Dict[str, Any]) -> None:
    """Display the API response.

    Args:
        response: API response dictionary
    """
    # Extract content
    if 'choices' in response and response['choices']:
        content = response['choices'][0]['message']['content']

        print("\n" + "="*70)
        print("GENERATED ESSAY")
        print("="*70)
        print(content)
    else:
        print("No response generated")

    # Display usage statistics
    if 'usage' in response:
        usage = response['usage']
        prompt_tokens = usage.get('prompt_tokens', 0)
        completion_tokens = usage.get('completion_tokens', 0)
        total_tokens = usage.get('total_tokens', 0)

        # Calculate costs
        input_cost = prompt_tokens * INPUT_PRICE / 1_000_000
        output_cost = completion_tokens * OUTPUT_PRICE / 1_000_000
        total_cost = input_cost + output_cost

        print("\n" + "="*70)
        print("USAGE STATISTICS")
        print("="*70)
        print(f"Prompt tokens:     {prompt_tokens:,}")
        print(f"Completion tokens: {completion_tokens:,}")
        print(f"Total tokens:      {total_tokens:,}")
        print(f"\nCost breakdown:")
        print(f"  Input:  ${input_cost:.6f}")
        print(f"  Output: ${output_cost:.6f}")
        print(f"  Total:  ${total_cost:.6f}")

def main():
    """Main function to demonstrate template processing."""

    # Parse command line arguments
    category = sys.argv[1] if len(sys.argv) > 1 else "Science"
    topic = sys.argv[2] if len(sys.argv) > 2 else "Climate Change"

    print("="*70)
    print("PROMPT TEMPLATE COMPILATION DEMO")
    print("="*70)
    print(f"Category: {category}")
    print(f"Topic: {topic}")

    # Check for API key
    api_key = os.getenv('DEMETERICS_API_KEY')
    if not api_key:
        print("\n❌ Error: DEMETERICS_API_KEY environment variable not set")
        print("Set it with: export DEMETERICS_API_KEY='your-api-key-here'")
        sys.exit(1)

    # Create template if it doesn't exist
    if not TEMPLATE_FILE.exists():
        print(f"\n⚠️ Template not found at {TEMPLATE_FILE}")
        print("Creating example template...")
        create_example_template()

    # Get current timestamp
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")
    print(f"Timestamp: {now}")

    # Prepare variables for template
    variables = {
        "Category": category,
        "Topic": topic,
        "Now": now
    }

    try:
        # Process template
        print("\n" + "="*70)
        print("PROCESSING TEMPLATE")
        print("="*70)
        print(f"Loading template from: {TEMPLATE_FILE}")
        print("Processing steps:")
        print("  1. Removing comment lines (///)")
        print("  2. Substituting variables")
        print("  3. Processing conditionals")
        print("  4. Cleaning up markers")

        processor = TemplateProcessor(TEMPLATE_FILE)
        processed_prompt = processor.process(variables)

        # Show preview of processed prompt
        print("\n" + "="*70)
        print("COMPILED PROMPT (PREVIEW)")
        print("="*70)
        preview_length = min(500, len(processed_prompt))
        print(processed_prompt[:preview_length] + "...")
        print(f"\n(Showing first {preview_length} characters of {len(processed_prompt)} total)")

        # Create user message
        user_message = f"Please write the essay about {topic} as specified in the instructions."

        # Make API request
        print("\n" + "="*70)
        print("SENDING TO AI")
        print("="*70)
        print(f"Model: {MODEL}")
        print("Making API request...")

        response = make_api_request(processed_prompt, user_message, api_key)

        # Display response
        display_response(response)

        # Educational notes
        print("\n" + "="*70)
        print("💡 EDUCATIONAL NOTES")
        print("="*70)
        print("""
Template-based prompting offers several advantages:

1. **Consistency**: Ensures uniform output format across requests
2. **Maintainability**: Templates can be updated without changing code
3. **Reusability**: Same template works for different inputs
4. **Clarity**: Separates prompt logic from application logic
5. **Version Control**: Templates can be tracked and versioned

Key techniques demonstrated:
- Variable substitution ([[.VarName]])
- Comment stripping (/// lines)
- Conditional logic (if/else blocks)
- Timestamp injection for context
- Category-specific instructions

Python-specific features used:
- pathlib for cross-platform file handling
- Regular expressions for pattern matching
- String formatting for variable substitution
- datetime for timestamp generation
        """)

    except FileNotFoundError as e:
        print(f"\n❌ Error: {e}")
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()