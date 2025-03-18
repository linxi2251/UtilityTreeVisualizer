#!/usr/bin/env python3
from PIL import Image, ImageDraw, ImageFont
import os

# Create icons directory if it doesn't exist
os.makedirs("icons", exist_ok=True)

# Icon size
width, height = 32, 32

# Icon colors
colors = {
    "system": (80, 80, 200),    # Blue
    "folder": (255, 180, 0),    # Yellow/Orange
    "generic": (200, 200, 0),   # Yellow
    "water": (0, 120, 200),     # Blue
    "telecom": (200, 0, 200),   # Purple
    "pipeline": (200, 100, 0),  # Brown
    "gas": (200, 0, 0),         # Red
    "pipe": (0, 200, 0),        # Green
    "point": (0, 200, 200),     # Cyan
    "area": (120, 120, 120),    # Gray
    "feature": (150, 150, 150), # Gray
    "default": (100, 100, 100)  # Dark Gray
}

# Generate each icon
for name, color in colors.items():
    img = Image.new('RGBA', (width, height), (255, 255, 255, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw different shapes based on icon type
    if name in ["system", "folder", "generic", "water", "telecom", "pipeline", "gas"]:
        # Folder-like icons for categories
        draw.rectangle([(4, 8), (28, 26)], fill=color)
        draw.rectangle([(8, 4), (24, 8)], fill=color)
    elif name in ["pipe"]:
        # Pipe icon
        draw.rectangle([(8, 8), (24, 24)], fill=color)
        draw.rectangle([(12, 4), (20, 28)], fill=color)
    elif name in ["point"]:
        # Point icon (circle)
        draw.ellipse([(6, 6), (26, 26)], fill=color)
    elif name in ["area"]:
        # Area icon (rectangle with border)
        draw.rectangle([(6, 6), (26, 26)], fill=color)
    else:
        # Default icon (square)
        draw.rectangle([(8, 8), (24, 24)], fill=color)
    
    # Save the icon
    img.save(f"icons/{name}.png")

print("Icons generated successfully!") 