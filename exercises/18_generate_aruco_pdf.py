#!/usr/bin/env python3
"""
ArUco Marker PDF Generator
===========================

What are ArUco markers?
-----------------------
ArUco markers are like simplified QR codes designed specifically for computer
vision applications. Each marker is a small square grid of black and white
cells that encodes a unique ID number. Unlike QR codes (which store text or
URLs), ArUco markers are optimized for:

  - Fast, real-time detection in video streams
  - Precise pose estimation (position + orientation in 3D space)
  - Rotation-invariant identification (works at any angle)
  - Robustness to partial occlusion and lighting changes

They are widely used in robotics, augmented reality, camera calibration,
and object tracking.

What does "DICT_4X4_50" mean?
-----------------------------
  - 4X4  = each marker is a 4x4 grid of black/white cells (plus a black border)
  - 50   = the dictionary contains 50 unique marker patterns (IDs 0-49)

Larger dictionaries (e.g., DICT_6X6_250) have more cells and more unique IDs,
but are harder to detect at small sizes or long distances. For classroom
exercises, 4x4 markers work great.

What this script generates
--------------------------
A printable PDF page with 12 ArUco markers (4 rows x 3 columns), each about
5-6 cm in size with its ID number printed below. Students print this page
and cut out individual markers for Exercise 18.3 (ArUco Marker Detection).

Dependencies: opencv-python, matplotlib, numpy
"""

import os
import numpy as np
import cv2
import matplotlib
matplotlib.use("Agg")  # Non-interactive backend for PDF generation
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.backends.backend_pdf import PdfPages


def get_marker_grid(aruco_dict, marker_id):
    """
    Generate a minimal ArUco marker grid (just the cell values).

    We request the smallest possible image from OpenCV -- one pixel per cell.
    For DICT_4X4_50, the marker has a 4x4 inner grid plus a 1-cell black
    border on each side, giving a 6x6 binary grid.

    Parameters
    ----------
    aruco_dict : cv2.aruco.Dictionary
        The ArUco dictionary to use.
    marker_id : int
        The unique ID for this marker.

    Returns
    -------
    numpy.ndarray
        2D array of 0s and 255s, shape (grid_size, grid_size).
    """
    # Request 1 pixel per cell to get the raw grid structure.
    # For 4X4 dict: 4 inner cells + 2 border cells = 6x6 grid.
    grid_size = 6  # 4x4 inner + 1 border on each side
    if hasattr(cv2.aruco, "generateImageMarker"):
        grid = cv2.aruco.generateImageMarker(aruco_dict, marker_id, grid_size)
    else:
        grid = cv2.aruco.drawMarker(aruco_dict, marker_id, grid_size)
    return grid


def draw_marker_on_axes(ax, grid):
    """
    Draw a marker as vector rectangles on a matplotlib Axes.

    Instead of using imshow (which causes resampling artifacts in PDF output),
    we draw each cell as a filled Rectangle patch. This produces perfectly
    crisp vector graphics that look sharp at any zoom level or print size.

    Parameters
    ----------
    ax : matplotlib.axes.Axes
        The axes to draw on.
    grid : numpy.ndarray
        2D binary grid from get_marker_grid() (0 = black, 255 = white).
    """
    n = grid.shape[0]  # Grid size (6 for DICT_4X4_50)

    # Set axes limits to match the grid dimensions
    ax.set_xlim(0, n)
    ax.set_ylim(0, n)
    ax.set_aspect("equal")

    # Start with a black background (the border and black cells)
    ax.set_facecolor("black")

    # Draw white rectangles for each white cell.
    # Grid row 0 is the top of the marker, but in axes coordinates y=0 is
    # the bottom, so we flip: axes_y = (n - 1) - row.
    for row in range(n):
        for col in range(n):
            if grid[row, col] == 255:
                # White cell: draw a white filled rectangle
                rect = Rectangle(
                    (col, (n - 1) - row),  # bottom-left corner (x, y)
                    1, 1,                   # width, height (one cell)
                    facecolor="white",
                    edgecolor="white",      # Match face to avoid thin gaps
                    linewidth=0,
                )
                ax.add_patch(rect)

    # Remove axis ticks (distracting on a printed sheet)
    ax.set_xticks([])
    ax.set_yticks([])


def create_marker_sheet(output_path, rows=4, cols=3):
    """
    Create a PDF page with a grid of ArUco markers.

    Parameters
    ----------
    output_path : str
        Where to save the PDF file.
    rows : int
        Number of marker rows on the page.
    cols : int
        Number of marker columns on the page.
    """
    # --- Step 1: Create the ArUco dictionary ---
    # getPredefinedDictionary loads one of OpenCV's built-in marker sets.
    # DICT_4X4_50 gives us 50 unique markers with a 4x4 internal grid.
    aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)

    total_markers = rows * cols  # 12 markers (IDs 0-11)

    # --- Step 2: Generate all marker grids ---
    # We get the minimal binary grid for each marker (6x6 for DICT_4X4_50).
    grids = []
    for marker_id in range(total_markers):
        grid = get_marker_grid(aruco_dict, marker_id)
        grids.append(grid)

    # --- Step 3: Layout the page with matplotlib ---
    # US Letter size in inches (8.5 x 11), portrait orientation.
    # Each marker will be roughly 5-6 cm (~2 inches) on the printed page.
    fig, axes = plt.subplots(rows, cols, figsize=(8.5, 11))

    # Add the page title and subtitle
    fig.suptitle(
        "ArUco Marker Sheet - Print at 100% scale (no scaling)",
        fontsize=14,
        fontweight="bold",
        y=0.97,
    )
    fig.text(
        0.5, 0.945,
        "Cut out individual markers for Exercise 18.3",
        ha="center",
        fontsize=11,
        style="italic",
        color="gray",
    )

    # Adjust spacing so markers are well-separated and have room for labels
    fig.subplots_adjust(
        left=0.1, right=0.9,
        top=0.89, bottom=0.05,
        hspace=0.45, wspace=0.3,
    )

    # --- Step 4: Draw each marker as vector graphics ---
    # Using Rectangle patches instead of imshow avoids the resampling
    # artifacts that matplotlib's PDF backend introduces with raster images.
    for idx, (grid, ax) in enumerate(zip(grids, axes.flat)):
        draw_marker_on_axes(ax, grid)

        # Label each marker with its ID so students know which is which
        ax.set_title(f"ID: {idx}", fontsize=12, fontweight="bold", pad=8)

        # Add a thin border around each marker for easier cutting
        for spine in ax.spines.values():
            spine.set_visible(True)
            spine.set_linewidth(0.5)
            spine.set_color("lightgray")

    # --- Step 5: Save as PDF ---
    # Since we drew vector rectangles (not raster images), the PDF will be
    # perfectly crisp at any zoom level or print size. No DPI concerns.
    with PdfPages(output_path) as pdf:
        pdf.savefig(fig)

    plt.close(fig)
    print(f"Saved marker sheet to: {output_path}")
    print(f"  - {total_markers} markers (IDs 0-{total_markers - 1})")
    print(f"  - Dictionary: DICT_4X4_50")
    print(f"  - Print at 100% scale for ~5-6 cm markers")


# ---------------------------------------------------------------------------
# Customization tips for students:
#
#   - Change `rows` and `cols` to generate more or fewer markers
#   - Use a different dictionary for more unique IDs:
#       cv2.aruco.DICT_4X4_100   -> 100 markers
#       cv2.aruco.DICT_5X5_250   -> 250 markers (5x5 grid, harder to detect small)
#       cv2.aruco.DICT_6X6_1000  -> 1000 markers (6x6 grid)
#   - For larger dictionaries, adjust grid_size in get_marker_grid():
#       DICT_5X5_*  -> grid_size = 7  (5 inner + 2 border)
#       DICT_6X6_*  -> grid_size = 8  (6 inner + 2 border)
#   - Adjust `figsize` for A4 paper: use (8.27, 11.69) instead of (8.5, 11)
# ---------------------------------------------------------------------------

if __name__ == "__main__":
    # Save the PDF in the same directory as this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_file = os.path.join(script_dir, "18_aruco_markers.pdf")

    create_marker_sheet(output_file)
