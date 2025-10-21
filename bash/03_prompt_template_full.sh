#!/bin/bash
#==============================================================================
# 03_prompt_template_full.sh - Compile prompt templates for dynamic AI interactions
#
# This example demonstrates advanced prompt engineering using templates:
# 1. Load template files with variables and conditional logic
# 2. Remove comment lines (///) for cleaner prompts
# 3. Substitute variables like timestamps and user inputs
# 4. Generate dynamic, context-aware prompts
#
# Template features:
# - Variables: [[.VarName]] replaced with actual values
# - Comments: Lines starting with /// are removed
# - Conditionals: [[if .Category == "X"]] ... [[end]]
# - Timestamps: [[.Now]] replaced with current time
#
# Usage:
#     ./03_prompt_template_full.sh [category] [topic]
#
# Examples:
#     ./03_prompt_template_full.sh "Science" "Quantum Computing"
#     ./03_prompt_template_full.sh "History" "The Industrial Revolution"
#     ./03_prompt_template_full.sh "Technology" "Artificial Intelligence"
#
# Environment:
#     GROQ_API_KEY - Your Groq API key (required)
#
# Dependencies:
#     - curl: HTTP client for API requests
#     - jq: JSON processor for parsing responses
#     - sed: Stream editor for template processing
#     - bc: Calculator for cost calculations
#==============================================================================

set -euo pipefail  # Exit on error, undefined variables, pipe failures

#------------------------------------------------------------------------------
# Configuration
#------------------------------------------------------------------------------

# API configuration
readonly API_URL="https://api.groq.com/openai/v1/chat/completions"
readonly MODEL="meta-llama/llama-4-scout-17b-16e-instruct"

# Template configuration
readonly TEMPLATE_DIR="../templates"
readonly TEMPLATE_FILE="$TEMPLATE_DIR/essay_writer.txt"

# Pricing (per million tokens)
readonly INPUT_PRICE=0.11
readonly OUTPUT_PRICE=0.34

# Colors for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly CYAN='\033[0;36m'
readonly NC='\033[0m' # No Color

#------------------------------------------------------------------------------
# Helper Functions
#------------------------------------------------------------------------------

# Print colored output
print_color() {
    local color=$1
    shift
    echo -e "${color}$*${NC}"
}

# Print section header
print_header() {
    echo
    print_color "$CYAN" "════════════════════════════════════════════════════════════════"
    print_color "$CYAN" " $1"
    print_color "$CYAN" "════════════════════════════════════════════════════════════════"
}

# Check dependencies
check_dependencies() {
    local deps=("curl" "jq" "sed" "bc")
    local missing=()

    for dep in "${deps[@]}"; do
        if ! command -v "$dep" >/dev/null 2>&1; then
            missing+=("$dep")
        fi
    done

    if [ ${#missing[@]} -gt 0 ]; then
        print_color "$RED" "Error: Missing required dependencies: ${missing[*]}"
        print_color "$YELLOW" "Install with: apt-get install ${missing[*]} (or equivalent)"
        exit 1
    fi
}

# Validate environment
validate_environment() {
    if [ -z "${GROQ_API_KEY:-}" ]; then
        print_color "$RED" "Error: GROQ_API_KEY environment variable not set"
        print_color "$YELLOW" "Set it with: export GROQ_API_KEY='your-api-key-here'"
        exit 1
    fi

    if [ ! -f "$TEMPLATE_FILE" ]; then
        print_color "$RED" "Error: Template file not found: $TEMPLATE_FILE"
        print_color "$YELLOW" "Creating example template..."
        create_example_template
    fi
}

# Create example template if missing
create_example_template() {
    mkdir -p "$TEMPLATE_DIR"
    cat > "$TEMPLATE_FILE" << 'TEMPLATE_EOF'
/// Template for writing educational essays
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

Focus on making the content relevant and engaging for young learners.
Use examples and analogies that resonate with teenagers.

# OUTPUT FORMAT

Provide the essay with:
- Clear title
- Structured paragraphs
- Key terms defined
- Discussion questions at the end
TEMPLATE_EOF
    print_color "$GREEN" "Created template file: $TEMPLATE_FILE"
}

# Process template with variable substitution
process_template() {
    local category=$1
    local topic=$2
    local now=$3

    # Read template and process it:
    # 1. Remove lines starting with ///
    # 2. Replace [[.Now]] with current timestamp
    # 3. Replace [[.Category]] with category
    # 4. Replace [[.Topic]] with topic
    # 5. Handle simple conditionals (basic implementation)

    local processed
    processed=$(sed \
        -e '/^\/\/\//d' \
        -e "s/\[\[\.Now\]\]/$now/g" \
        -e "s/\[\[\.Category\]\]/$category/g" \
        -e "s/\[\[\.Topic\]\]/$topic/g" \
        "$TEMPLATE_FILE")

    # Simple conditional processing (basic if/else)
    # This is a simplified version - production would need proper template engine
    if [[ "$category" == "History" ]]; then
        processed=$(echo "$processed" | sed \
            -e '/\[\[if \.Category == "History"\]\]/,/\[\[else/{ /\[\[if/d; /\[\[else/d; p; }' \
            -e '/\[\[else/,/\[\[end\]\]/d' \
            -e '/\[\[end\]\]/d')
    elif [[ "$category" == "Science" ]]; then
        processed=$(echo "$processed" | sed \
            -e '/\[\[else if \.Category == "Science"\]\]/,/\[\[else/{ /\[\[else if/d; /\[\[else/d; p; }' \
            -e '/\[\[if/,/\[\[else if \.Category == "Science"\]\]/d' \
            -e '/\[\[else if \.Category/,/\[\[else/d' \
            -e '/\[\[else\]\]/,/\[\[end\]\]/d' \
            -e '/\[\[end\]\]/d')
    fi

    # Clean up any remaining conditional markers
    processed=$(echo "$processed" | sed \
        -e '/\[\[if/d' \
        -e '/\[\[else/d' \
        -e '/\[\[end\]\]/d')

    echo "$processed"
}

# Make API request
make_api_request() {
    local system_prompt=$1
    local user_prompt=$2

    # Escape the prompts for JSON
    system_prompt=$(echo "$system_prompt" | jq -Rs .)
    user_prompt=$(echo "$user_prompt" | jq -Rs .)

    # Create request JSON
    local request_json
    request_json=$(cat <<EOF
{
    "model": "$MODEL",
    "messages": [
        {
            "role": "system",
            "content": $system_prompt
        },
        {
            "role": "user",
            "content": $user_prompt
        }
    ],
    "max_tokens": 2000,
    "temperature": 0.7
}
EOF
)

    # Make API request
    local response
    response=$(curl -s -X POST "$API_URL" \
        -H "Authorization: Bearer $GROQ_API_KEY" \
        -H "Content-Type: application/json" \
        -d "$request_json")

    echo "$response"
}

# Parse and display response
display_response() {
    local response=$1

    # Check for errors
    if echo "$response" | jq -e '.error' >/dev/null 2>&1; then
        local error_msg
        error_msg=$(echo "$response" | jq -r '.error.message // .error')
        print_color "$RED" "API Error: $error_msg"
        return 1
    fi

    # Extract the essay content
    local content
    content=$(echo "$response" | jq -r '.choices[0].message.content // "No response generated"')

    print_header "GENERATED ESSAY"
    echo "$content"

    # Display usage statistics
    if echo "$response" | jq -e '.usage' >/dev/null 2>&1; then
        print_header "USAGE STATISTICS"

        local prompt_tokens
        local completion_tokens
        local total_tokens

        prompt_tokens=$(echo "$response" | jq -r '.usage.prompt_tokens // 0')
        completion_tokens=$(echo "$response" | jq -r '.usage.completion_tokens // 0')
        total_tokens=$(echo "$response" | jq -r '.usage.total_tokens // 0')

        echo "Prompt tokens:     $(printf "%'d" "$prompt_tokens")"
        echo "Completion tokens: $(printf "%'d" "$completion_tokens")"
        echo "Total tokens:      $(printf "%'d" "$total_tokens")"

        # Calculate cost
        local input_cost
        local output_cost
        local total_cost

        input_cost=$(echo "scale=6; $prompt_tokens * $INPUT_PRICE / 1000000" | bc)
        output_cost=$(echo "scale=6; $completion_tokens * $OUTPUT_PRICE / 1000000" | bc)
        total_cost=$(echo "scale=6; $input_cost + $output_cost" | bc)

        echo
        echo "Cost breakdown:"
        printf "  Input:  \$%.6f\n" "$input_cost"
        printf "  Output: \$%.6f\n" "$output_cost"
        printf "  Total:  \$%.6f\n" "$total_cost"
    fi
}

#------------------------------------------------------------------------------
# Main Function
#------------------------------------------------------------------------------

main() {
    # Parse command line arguments
    local category=${1:-"Science"}
    local topic=${2:-"Climate Change"}

    # Display banner
    print_header "PROMPT TEMPLATE COMPILATION DEMO"
    echo "Demonstrating dynamic prompt generation using templates"
    echo

    # Check dependencies and environment
    check_dependencies
    validate_environment

    # Display input parameters
    print_color "$BLUE" "Parameters:"
    echo "  Category: $category"
    echo "  Topic: $topic"
    echo "  Template: $TEMPLATE_FILE"
    echo

    # Get current timestamp
    local now
    now=$(date -u +"%Y-%m-%d %H:%M:%S UTC")
    print_color "$BLUE" "Timestamp: $now"

    # Process template
    print_header "PROCESSING TEMPLATE"
    echo "Loading template from: $TEMPLATE_FILE"
    echo "Removing comment lines (///)"
    echo "Substituting variables:"
    echo "  [[.Category]] → $category"
    echo "  [[.Topic]] → $topic"
    echo "  [[.Now]] → $now"

    local processed_prompt
    processed_prompt=$(process_template "$category" "$topic" "$now")

    # Show processed prompt (first 500 chars)
    print_header "COMPILED PROMPT (PREVIEW)"
    echo "${processed_prompt:0:500}..."
    echo
    print_color "$YELLOW" "(Showing first 500 characters of compiled prompt)"

    # Create user message
    local user_message="Please write the essay about $topic as specified in the instructions."

    # Make API request
    print_header "SENDING TO AI"
    echo "Model: $MODEL"
    echo "Making API request..."
    echo

    local response
    response=$(make_api_request "$processed_prompt" "$user_message")

    # Display response
    display_response "$response"

    # Educational notes
    print_header "💡 EDUCATIONAL NOTES"
    cat << NOTES
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

Best practices:
- Keep templates focused on one task
- Use clear variable names
- Document template structure
- Test with various inputs
- Consider edge cases

Advanced usage:
- Chain multiple templates
- Use template inheritance
- Implement complex conditionals
- Add loops for repetitive sections
- Include external data sources
NOTES
}

#------------------------------------------------------------------------------
# Script Entry Point
#------------------------------------------------------------------------------

# Run main function with all arguments
main "$@"