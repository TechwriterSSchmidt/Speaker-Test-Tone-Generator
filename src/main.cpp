#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// --- Hardware Definitions ---
TFT_eSPI tft = TFT_eSPI();

// Touch Screen Pins (CYD)
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// Use a separate SPI instance for Touch
SPIClass touchSpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// Audio
#define SPEAKER_PIN 26
#define PWM_CHANNEL 0
#define PWM_RES 8

// --- UI Constants ---
#define SLIDER_X 20
#define SLIDER_Y 75
#define SLIDER_W 280
#define SLIDER_H 20
#define SLIDER_KNOB_R 8

// --- State Variables ---
int frequency = 1000;
int volume = 127; 
bool isPlaying = false;
unsigned long lastTouchTime = 0;

// --- Forward Declarations ---
void updateFreq();
void updateVolume();
void drawSlider();
void toggleSound();
void increaseFreq100();
void decreaseFreq100();
void increaseFreq10();
void decreaseFreq10();
void increaseVol();
void decreaseVol();

// --- Button Structure ---
struct Button {
  int x, y, w, h;
  String label;
  uint16_t color;
  void (*action)();
};

// --- Button Layout ---
Button buttons[] = {
  // Freq Controls
  {10, 115, 90, 35, "-100", TFT_BLUE, decreaseFreq100},
  {110, 115, 90, 35, "+100", TFT_BLUE, increaseFreq100},
  {10, 160, 90, 35, "-10", TFT_NAVY, decreaseFreq10},
  {110, 160, 90, 35, "+10", TFT_NAVY, increaseFreq10},
  
  // Start/Stop
  {210, 115, 100, 80, "START", TFT_GREEN, toggleSound},

  // Volume Controls (Bottom Row)
  {10, 205, 145, 30, "Vol -", TFT_DARKGREY, decreaseVol},
  {165, 205, 145, 30, "Vol +", TFT_DARKGREY, increaseVol}
};

const int numButtons = sizeof(buttons) / sizeof(Button);

// --- Helper Functions ---

void drawButton(int index) {
  Button b = buttons[index];
  tft.fillRoundRect(b.x, b.y, b.w, b.h, 5, b.color);
  tft.setTextColor(TFT_WHITE, b.color);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1); 
  tft.drawString(b.label, b.x + b.w / 2, b.y + b.h / 2, 2); 
}

void drawSlider() {
  tft.fillRect(0, SLIDER_Y - 12, 320, SLIDER_H + 24, TFT_BLACK);
  tft.fillRect(SLIDER_X, SLIDER_Y + SLIDER_H/2 - 2, SLIDER_W, 4, TFT_DARKGREY);
  
  float p = log((float)frequency / 10.0) / log(20000.0 / 10.0);
  if (p < 0) p = 0;
  if (p > 1) p = 1;
  
  int knobX = SLIDER_X + (int)(p * SLIDER_W);
  tft.fillCircle(knobX, SLIDER_Y + SLIDER_H/2, SLIDER_KNOB_R, TFT_ORANGE);
  tft.drawCircle(knobX, SLIDER_Y + SLIDER_H/2, SLIDER_KNOB_R, TFT_WHITE);
}

void updateSoundOutput() {
  if (isPlaying) {
    ledcSetup(PWM_CHANNEL, frequency, PWM_RES);
    ledcWrite(PWM_CHANNEL, volume); 
  } else {
    ledcWrite(PWM_CHANNEL, 0);
  }
}

void updateFreq() {
  tft.fillRect(0, 30, 320, 35, TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawNumber(frequency, 160, 30, 4);
  tft.setTextSize(1);
  tft.drawString("Hz", 260, 38, 2); 
  
  drawSlider();
  updateSoundOutput();
}

void updateVolume() {
  Serial.printf("Volume: %d\n", volume);
  updateSoundOutput();
}

void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  tft.drawString("Tone Generator", 160, 5, 2); 
  
  updateFreq();
  
  for (int i = 0; i < numButtons; i++) {
    drawButton(i);
  }
}

// --- Actions ---

void toggleSound() {
  isPlaying = !isPlaying;
  if (isPlaying) {
    buttons[4].label = "STOP";
    buttons[4].color = TFT_RED;
    ledcSetup(PWM_CHANNEL, frequency, PWM_RES);
    ledcAttachPin(SPEAKER_PIN, PWM_CHANNEL);
    updateSoundOutput();
  } else {
    buttons[4].label = "START";
    buttons[4].color = TFT_GREEN;
    ledcDetachPin(SPEAKER_PIN);
  }
  drawButton(4);
}

void increaseFreq100() { frequency += 100; if (frequency > 20000) frequency = 20000; updateFreq(); }
void decreaseFreq100() { frequency -= 100; if (frequency < 10) frequency = 10; updateFreq(); }
void increaseFreq10() { frequency += 10; if (frequency > 20000) frequency = 20000; updateFreq(); }
void decreaseFreq10() { frequency -= 10; if (frequency < 10) frequency = 10; updateFreq(); }

void increaseVol() { 
  volume += 10; 
  if (volume > 127) volume = 127; 
  updateVolume(); 
}
void decreaseVol() { 
  volume -= 10; 
  if (volume < 0) volume = 0; 
  updateVolume(); 
}

// --- Setup & Loop ---

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("--- Speaker Test Tone Generator (XPT2046 Custom SPI) ---");

  // 1. Init Display (First to claim HSPI)
  tft.init();
  tft.setRotation(1);

  // 2. Init Touch SPI (VSPI)
  touchSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  
  // 3. Init Touch
  if (!ts.begin(touchSpi)) {
    Serial.println("Failed to start touchscreen controller");
  } else {
    Serial.println("Touchscreen started");
  }
  ts.setRotation(1);
  
  // 4. Init Audio
  ledcSetup(PWM_CHANNEL, frequency, PWM_RES);
  
  // 5. Draw UI
  drawUI();
  
  Serial.println("Setup Complete");
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    
    // Map Touch Coordinates
    // CYD Landscape Mapping (approximate, might need fine tuning)
    // X: 200 (Left) -> 3700 (Right)
    // Y: 240 (Top) -> 3800 (Bottom)
    
    int touchX = map(p.x, 200, 3700, 0, 320);
    int touchY = map(p.y, 240, 3800, 0, 240);
    
    // Constrain
    if (touchX < 0) touchX = 0;
    if (touchX > 320) touchX = 320;
    if (touchY < 0) touchY = 0;
    if (touchY > 240) touchY = 240;

    // Serial.printf("Raw: %d, %d -> Map: %d, %d\n", p.x, p.y, touchX, touchY);

    // Check Slider
    if (touchY > SLIDER_Y - 15 && touchY < SLIDER_Y + SLIDER_H + 15) {
       float pos = (float)(touchX - SLIDER_X) / (float)SLIDER_W;
       if (pos < 0) pos = 0;
       if (pos > 1) pos = 1;
       
       frequency = 10.0 * pow(20000.0 / 10.0, pos);
       
       if (frequency < 10) frequency = 10;
       if (frequency > 20000) frequency = 20000;
       
       updateFreq();
       delay(20); 
       return; 
    }

    // Check Buttons
    if (millis() - lastTouchTime > 200) { 
      for (int i = 0; i < numButtons; i++) {
        if (touchX > buttons[i].x && touchX < buttons[i].x + buttons[i].w &&
            touchY > buttons[i].y && touchY < buttons[i].y + buttons[i].h) {
          
          Serial.printf("Button %s pressed\n", buttons[i].label.c_str());
          
          tft.drawRoundRect(buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, 5, TFT_WHITE);
          buttons[i].action();
          delay(100); 
          tft.drawRoundRect(buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, 5, buttons[i].color);
          
          lastTouchTime = millis();
          break; 
        }
      }
    }
  }
}
