from PIL import Image, ImageDraw, ImageFont
import os

# Constants
WIDTH = 320
HEIGHT = 240
BG_COLOR = (0, 0, 0)
TEXT_COLOR = (255, 255, 255)
ACCENT_COLOR = (0, 255, 255) # Cyan
BUTTON_COLOR = (0, 0, 128) # Navy
BUTTON_BORDER = (255, 255, 255)
SLIDER_BG = (100, 100, 100)
SLIDER_KNOB = (255, 165, 0) # Orange

def create_mockup():
    img = Image.new('RGB', (WIDTH, HEIGHT), BG_COLOR)
    draw = ImageDraw.Draw(img)
    
    # Title
    draw.text((100, 10), "Speaker Test Tone", fill=TEXT_COLOR)
    
    # Frequency Display
    draw.text((130, 40), "1000 Hz", fill=ACCENT_COLOR)
    
    # Slider
    slider_y = 80
    draw.rectangle([20, slider_y, 300, slider_y + 10], fill=SLIDER_BG)
    draw.ellipse([150, slider_y - 5, 170, slider_y + 15], fill=SLIDER_KNOB)
    
    # Buttons Row 1 (-100, -10, +10, +100)
    btn_y = 120
    btn_w = 60
    btn_h = 30
    spacing = 10
    x = 25
    
    labels = ["-100", "-10", "+10", "+100"]
    for label in labels:
        draw.rectangle([x, btn_y, x + btn_w, btn_y + btn_h], fill=BUTTON_COLOR, outline=BUTTON_BORDER)
        draw.text((x + 15, btn_y + 8), label, fill=TEXT_COLOR)
        x += btn_w + spacing
        
    # Start/Stop Button
    draw.rectangle([110, 170, 210, 210], fill=(0, 128, 0), outline=BUTTON_BORDER)
    draw.text((135, 180), "START", fill=TEXT_COLOR)
    
    # Volume Bar
    draw.rectangle([60, 220, 260, 230], fill=(50, 50, 50), outline=BUTTON_BORDER)
    draw.rectangle([60, 220, 200, 230], fill=(0, 255, 0)) # Partial fill
    
    # Save
    img.save("ui_mockup.png")
    print("Mockup created: ui_mockup.png")

if __name__ == "__main__":
    create_mockup()
