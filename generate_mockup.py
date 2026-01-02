from PIL import Image, ImageDraw, ImageFont
import math

# Constants
WIDTH = 320
HEIGHT = 240
BG_COLOR = (0, 0, 0)
TEXT_COLOR = (255, 255, 255)
FREQ_COLOR = (255, 255, 0) # Yellow
SLIDER_TRACK = (128, 128, 128) # Dark Grey
SLIDER_KNOB = (255, 165, 0) # Orange
BTN_BLUE = (0, 0, 255)
BTN_NAVY = (0, 0, 128)
BTN_GREEN = (0, 255, 0)
BTN_GREY = (64, 64, 64) # Dark Grey

def draw_rounded_rect(draw, xy, corner_radius, fill=None, outline=None):
    x1, y1, x2, y2 = xy
    draw.rectangle((x1 + corner_radius, y1, x2 - corner_radius, y2), fill=fill, outline=outline)
    draw.rectangle((x1, y1 + corner_radius, x2, y2 - corner_radius), fill=fill, outline=outline)
    draw.pieslice((x1, y1, x1 + corner_radius * 2, y1 + corner_radius * 2), 180, 270, fill=fill, outline=outline)
    draw.pieslice((x2 - corner_radius * 2, y1, x2, y1 + corner_radius * 2), 270, 360, fill=fill, outline=outline)
    draw.pieslice((x1, y2 - corner_radius * 2, x1 + corner_radius * 2, y2), 90, 180, fill=fill, outline=outline)
    draw.pieslice((x2 - corner_radius * 2, y2 - corner_radius * 2, x2, y2), 0, 90, fill=fill, outline=outline)

def create_mockup():
    img = Image.new('RGB', (WIDTH, HEIGHT), BG_COLOR)
    draw = ImageDraw.Draw(img)
    
    # Fonts (using default if custom not available, scaling size roughly)
    # Font 2 approx 16px, Font 4 approx 26px
    try:
        font_title = ImageFont.truetype("arial.ttf", 16)
        font_freq = ImageFont.truetype("arial.ttf", 32)
        font_unit = ImageFont.truetype("arial.ttf", 16)
        font_btn = ImageFont.truetype("arial.ttf", 14)
    except:
        font_title = ImageFont.load_default()
        font_freq = ImageFont.load_default()
        font_unit = ImageFont.load_default()
        font_btn = ImageFont.load_default()

    # Title
    draw.text((160, 5), "Tone Generator", fill=TEXT_COLOR, anchor="mt", font=font_title)
    
    # Frequency Display
    draw.text((160, 30), "1000", fill=FREQ_COLOR, anchor="mt", font=font_freq)
    draw.text((260, 38), "Hz", fill=TEXT_COLOR, anchor="lt", font=font_unit)
    
    # Slider
    slider_x = 20
    slider_y = 75
    slider_w = 280
    slider_h = 20
    
    # Track
    draw.rectangle([slider_x, slider_y + slider_h//2 - 2, slider_x + slider_w, slider_y + slider_h//2 + 2], fill=SLIDER_TRACK)
    
    # Knob (Logarithmic position for 1000Hz)
    # p = log(1000/10) / log(20000/10) = log(100) / log(2000) = 2 / 3.301 = 0.606
    p = 0.606
    knob_x = slider_x + int(p * slider_w)
    knob_y = slider_y + slider_h // 2
    r = 8
    draw.ellipse([knob_x - r, knob_y - r, knob_x + r, knob_y + r], fill=SLIDER_KNOB, outline=TEXT_COLOR)
    
    # Buttons
    buttons = [
        # Label, X, Y, W, H, Color
        ("-100", 10, 115, 90, 35, BTN_BLUE),
        ("+100", 110, 115, 90, 35, BTN_BLUE),
        ("-10", 10, 160, 90, 35, BTN_NAVY),
        ("+10", 110, 160, 90, 35, BTN_NAVY),
        ("START", 210, 115, 100, 80, BTN_GREEN),
        ("Vol -", 10, 205, 145, 30, BTN_GREY),
        ("Vol +", 165, 205, 145, 30, BTN_GREY)
    ]
    
    for label, x, y, w, h, color in buttons:
        draw_rounded_rect(draw, (x, y, x+w, y+h), 5, fill=color)
        # Center text
        draw.text((x + w/2, y + h/2), label, fill=TEXT_COLOR, anchor="mm", font=font_btn)

    # Save
    img.save("ui_mockup.png")
    print("Mockup created: ui_mockup.png")

if __name__ == "__main__":
    create_mockup()
