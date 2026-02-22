#!/usr/bin/env python3
"""
Exercise 18 Helper: Generate Printable Shape Detection Test Sheet

WHAT THIS GENERATES:
    - A single-page PDF with 8 geometric shapes arranged in a 2x4 grid
    - Each shape has a thick black outline and dashed cut lines
    - Students print, cut out shapes, and hold them in front of a camera
    - Used with Exercise 18.2 (Shape Detection with OpenCV)

WHY WE NEED THIS:
    - Shape detection algorithms work best with clean, high-contrast shapes
    - Printed shapes on white paper give consistent results for learning
    - Having standardized test shapes lets all students compare results
    - The variety of shapes (3-8 sides, curves, stars) tests different
      detection techniques: contour approximation, circularity, convexity

SHAPES INCLUDED:
    Circle, Triangle (equilateral), Square, Rectangle,
    Pentagon, Hexagon, Star (5-pointed), Arrow

PREREQUISITES:
    - Python 3.6 or higher
    - matplotlib (pip install matplotlib)
    - numpy (pip install numpy)

EXPECTED OUTPUT:
    - exercises/18_shapes.pdf (single page, ready to print)

Run with: python3 exercises/18_generate_shapes_pdf.py
"""

import numpy as np                        # For math and angle calculations
import matplotlib.pyplot as plt           # For creating the figure and saving PDF
import matplotlib.patches as patches      # For drawing geometric shapes
from matplotlib.patches import FancyArrowPatch  # For the arrow shape
from pathlib import Path                  # For building file paths

# ==============================================================================
# Step 1: Define helper functions to create each shape
# ==============================================================================

def make_regular_polygon(cx, cy, radius, num_sides, rotation_deg=0):
    """
    Generate vertices for a regular polygon.

    A regular polygon has all sides equal and all angles equal.
    We place vertices evenly around a circle of the given radius.

    Args:
        cx, cy: Center coordinates of the polygon
        radius: Distance from center to each vertex (in inches)
        num_sides: Number of sides (3=triangle, 5=pentagon, 6=hexagon, etc.)
        rotation_deg: Rotate the shape (e.g., -90 to point triangle upward)

    Returns:
        List of (x, y) tuples for each vertex
    """
    # Convert rotation from degrees to radians (numpy trig functions use radians)
    rotation_rad = np.radians(rotation_deg)

    # Calculate angle between each vertex (full circle = 2*pi radians)
    angle_step = 2 * np.pi / num_sides

    # Generate each vertex by walking around the circle
    vertices = []
    for i in range(num_sides):
        angle = rotation_rad + i * angle_step
        x = cx + radius * np.cos(angle)  # x = center + radius * cos(angle)
        y = cy + radius * np.sin(angle)  # y = center + radius * sin(angle)
        vertices.append((x, y))

    return vertices


def make_star(cx, cy, outer_radius, inner_radius, num_points=5):
    """
    Generate vertices for a star shape.

    A star alternates between outer points and inner notches.
    We use two radii: outer for the tips, inner for the notches.

    Args:
        cx, cy: Center coordinates
        outer_radius: Distance from center to star tips
        inner_radius: Distance from center to inner notches
        num_points: Number of star points (5 = classic star)

    Returns:
        List of (x, y) tuples alternating between outer and inner vertices
    """
    vertices = []
    # Total vertices = 2 * num_points (alternating outer and inner)
    total_vertices = 2 * num_points
    angle_step = 2 * np.pi / total_vertices

    # Start from the top (90 degrees = pi/2 radians)
    start_angle = np.pi / 2

    for i in range(total_vertices):
        angle = start_angle + i * angle_step
        # Even indices (0, 2, 4...) = outer points (tips)
        # Odd indices (1, 3, 5...) = inner points (notches)
        r = outer_radius if i % 2 == 0 else inner_radius
        x = cx + r * np.cos(angle)
        y = cy + r * np.sin(angle)
        vertices.append((x, y))

    return vertices


# ==============================================================================
# Step 2: Set up the figure (page layout)
# ==============================================================================

# Create a figure sized for standard paper (8.5 x 11 inches = US Letter)
# This also works well on A4 (8.27 x 11.69 inches)
fig, axes = plt.subplots(
    nrows=4,          # 4 rows of shapes
    ncols=2,          # 2 columns of shapes
    figsize=(8.5, 11) # US Letter size in inches
)

# Add the page title at the very top of the figure
fig.suptitle(
    "Shape Detection Test Sheet - Cut along dashed lines",
    fontsize=14,
    fontweight='bold',
    y=0.97  # Position near top (1.0 = very top edge)
)

# Add subtitle with exercise reference
fig.text(
    0.5, 0.945,  # Centered horizontally, just below title
    "Hold shapes in front of your camera for Exercise 18.2",
    ha='center',        # Horizontal alignment
    fontsize=10,
    style='italic',
    color='#444444'     # Dark gray for subtitle
)

# Adjust spacing between subplots to leave room for title and labels
fig.subplots_adjust(
    top=0.91,    # Leave space for title at top
    bottom=0.03, # Small margin at bottom
    left=0.08,   # Small margin on left
    right=0.92,  # Small margin on right
    hspace=0.35, # Vertical space between rows (for shape labels)
    wspace=0.30  # Horizontal space between columns
)

# ==============================================================================
# Step 3: Define the 8 shapes to draw
# ==============================================================================

# Each shape is defined as: (name, row, col)
# We'll draw each one in its corresponding subplot
shape_definitions = [
    ("Circle",    0, 0),
    ("Triangle",  0, 1),
    ("Square",    1, 0),
    ("Rectangle", 1, 1),
    ("Pentagon",  2, 0),
    ("Hexagon",   2, 1),
    ("Star",      3, 0),
    ("Arrow",     3, 1),
]

# Shape size in data coordinates (each subplot goes from 0 to 1)
# We'll center shapes at (0.5, 0.55) with radius ~0.38 to fill the cell
center_x = 0.5
center_y = 0.55
shape_radius = 0.38

# Common style for all shape outlines
outline_style = dict(
    linewidth=3.0,         # Thick black outline (~3pt) for easy detection
    edgecolor='black',     # Black border
    facecolor='white',     # White fill (high contrast on white paper)
    zorder=2               # Draw shapes above the dashed border
)

# ==============================================================================
# Step 4: Draw each shape in its subplot
# ==============================================================================

for shape_name, row, col in shape_definitions:
    # Get the axes (subplot) for this shape
    ax = axes[row, col]

    # Set the coordinate range for each subplot (0 to 1 in both dimensions)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal')  # Keep shapes from being stretched

    # Remove the default axes ticks and labels (we don't need them)
    ax.set_xticks([])
    ax.set_yticks([])

    # --- Draw dashed cut-line border around the subplot ---
    # This rectangle marks where students should cut with scissors
    cut_border = patches.FancyBboxPatch(
        (0.03, 0.03),        # Bottom-left corner (with small margin)
        0.94, 0.94,          # Width and height (almost full subplot)
        boxstyle="round,pad=0.02",  # Slightly rounded corners
        linewidth=1.2,
        edgecolor='#888888', # Gray dashed line
        facecolor='none',    # No fill (transparent)
        linestyle='--',      # Dashed line style
        zorder=1             # Behind the shapes
    )
    ax.add_patch(cut_border)

    # --- Draw the shape itself ---
    if shape_name == "Circle":
        # Circle: use matplotlib's Circle patch
        # A circle is defined by center point and radius
        circle = patches.Circle(
            (center_x, center_y),  # Center of the circle
            shape_radius,          # Radius
            **outline_style
        )
        ax.add_patch(circle)

    elif shape_name == "Triangle":
        # Equilateral triangle: 3-sided regular polygon, pointed upward
        # rotation_deg=90 puts one vertex at the top
        verts = make_regular_polygon(center_x, center_y, shape_radius, 3, rotation_deg=90)
        triangle = patches.Polygon(
            verts,
            closed=True,  # Connect last vertex back to first
            **outline_style
        )
        ax.add_patch(triangle)

    elif shape_name == "Square":
        # Square: 4-sided regular polygon, rotated 45 degrees so sides are flat
        verts = make_regular_polygon(center_x, center_y, shape_radius, 4, rotation_deg=45)
        square = patches.Polygon(
            verts,
            closed=True,
            **outline_style
        )
        ax.add_patch(square)

    elif shape_name == "Rectangle":
        # Rectangle: not a regular polygon (width != height)
        # We draw it manually with a width:height ratio of about 1.5:1
        rect_w = shape_radius * 2 * 1.1  # Wider than tall
        rect_h = shape_radius * 2 * 0.65
        rect = patches.Rectangle(
            (center_x - rect_w / 2, center_y - rect_h / 2),  # Bottom-left corner
            rect_w, rect_h,  # Width, height
            **outline_style
        )
        ax.add_patch(rect)

    elif shape_name == "Pentagon":
        # Pentagon: 5-sided regular polygon, one vertex at the top
        verts = make_regular_polygon(center_x, center_y, shape_radius, 5, rotation_deg=90)
        pentagon = patches.Polygon(
            verts,
            closed=True,
            **outline_style
        )
        ax.add_patch(pentagon)

    elif shape_name == "Hexagon":
        # Hexagon: 6-sided regular polygon, flat side at top
        # rotation_deg=0 gives pointy-top; 30 gives flat-top
        verts = make_regular_polygon(center_x, center_y, shape_radius, 6, rotation_deg=30)
        hexagon = patches.Polygon(
            verts,
            closed=True,
            **outline_style
        )
        ax.add_patch(hexagon)

    elif shape_name == "Star":
        # 5-pointed star: alternating outer tips and inner notches
        # Inner radius ~40% of outer gives a classic star shape
        verts = make_star(center_x, center_y, shape_radius, shape_radius * 0.4)
        star = patches.Polygon(
            verts,
            closed=True,
            **outline_style
        )
        ax.add_patch(star)

    elif shape_name == "Arrow":
        # Arrow: a right-pointing arrow built from polygon vertices
        # We define the arrow shape manually as a 7-sided polygon:
        #   - A rectangular shaft on the left
        #   - A triangular head on the right
        r = shape_radius
        cx, cy = center_x, center_y
        shaft_w = r * 1.4     # Length of the shaft
        shaft_h = r * 0.5     # Height of the shaft
        head_w = r * 0.7      # Depth of the arrowhead triangle
        head_h = r * 1.0      # Full height of the arrowhead

        # Build arrow vertices going clockwise from bottom-left of shaft
        arrow_verts = [
            (cx - shaft_w / 2, cy - shaft_h / 2),              # Bottom-left of shaft
            (cx - shaft_w / 2, cy + shaft_h / 2),              # Top-left of shaft
            (cx + shaft_w / 2 - head_w, cy + shaft_h / 2),     # Top-right of shaft (where head starts)
            (cx + shaft_w / 2 - head_w, cy + head_h / 2),      # Top of arrowhead base
            (cx + shaft_w / 2, cy),                             # Tip of arrowhead (rightmost point)
            (cx + shaft_w / 2 - head_w, cy - head_h / 2),      # Bottom of arrowhead base
            (cx + shaft_w / 2 - head_w, cy - shaft_h / 2),     # Bottom-right of shaft
        ]
        arrow = patches.Polygon(
            arrow_verts,
            closed=True,
            **outline_style
        )
        ax.add_patch(arrow)

    # --- Add shape name label below each shape ---
    ax.text(
        center_x, 0.08,   # Centered horizontally, near bottom of subplot
        shape_name,
        ha='center',       # Horizontal alignment: centered
        va='center',       # Vertical alignment: centered
        fontsize=11,
        fontweight='bold',
        color='#333333'
    )

# ==============================================================================
# Step 5: Save the figure as a PDF file
# ==============================================================================

# Build the output path: same directory as this script, named 18_shapes.pdf
script_dir = Path(__file__).resolve().parent
output_path = script_dir / "18_shapes.pdf"

# Save as PDF (vector format - shapes stay crisp at any print resolution)
plt.savefig(
    str(output_path),
    format='pdf',
    dpi=300,           # High DPI for any rasterized elements
    bbox_inches=None,  # Use the figure size as-is (Letter paper)
    pad_inches=0       # No extra padding beyond what we already set
)

# Close the figure to free memory
plt.close(fig)

# Print confirmation message
print(f"PDF generated: {output_path}")
print(f"Shapes included: Circle, Triangle, Square, Rectangle, Pentagon, Hexagon, Star, Arrow")
print(f"Print this page and cut along the dashed lines.")
print(f"Then hold each shape in front of your camera for Exercise 18.2!")
