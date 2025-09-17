#ifndef MAX7219_ANIMATION_H
#define MAX7219_ANIMATION_H

#include <MD_MAX72xx.h>

// Define hardware type and number of devices
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  // Adjust based on your MAX7219 module type
#define MAX_DEVICES 1  // Number of MAX7219 modules

class Max7219Animation {
private:
  MD_MAX72XX mx;  // MAX7219 object
  // Processing animation (reversed as requested)
  static const uint64_t PROCESSING_IMAGES[];
  static const int PROCESSING_IMAGES_LEN;
  // Idle animation
  static const uint64_t IDLE_IMAGES[];
  static const int IDLE_IMAGES_LEN;
  // Static bitmaps
  static const uint64_t TWO_ARROWS = 0xe7c3a51800000000;
  static const uint64_t ARROW_DOWN = 0x183c7e1818181818;
  static const uint64_t LEFT_ARROW = 0x0703050800000000;
  static const uint64_t RIGHT_ARROW = 0xe0c0a01000000000;
  // Animation state
  int processingFrame = 0;
  int idleFrame = 0;
  unsigned long lastFrameTime = 0;

public:
  Max7219Animation(uint8_t dataPin, uint8_t clkPin, uint8_t csPin) 
    : mx(HARDWARE_TYPE, dataPin, clkPin, csPin, MAX_DEVICES) {}
  
  void begin() {
    mx.begin();
    mx.control(MD_MAX72XX::INTENSITY, 3);  // Set brightness (0-15)
    mx.clear();
    displayFrame(PROCESSING_IMAGES[0]);  // Display first processing frame on startup
  }

  void updateProcessingAnimation(int frameDelay) {
    unsigned long currentTime = millis();
    if (currentTime - lastFrameTime >= frameDelay) {
      processingFrame = (processingFrame + 1) % PROCESSING_IMAGES_LEN;  // Loop through frames
      displayFrame(PROCESSING_IMAGES[processingFrame]);
      lastFrameTime = currentTime;
    }
  }

  void updateIdleAnimation(int frameDelay) {
    unsigned long currentTime = millis();
    if (currentTime - lastFrameTime >= frameDelay) {
      idleFrame = (idleFrame + 1) % IDLE_IMAGES_LEN;  // Loop through frames
      displayFrame(IDLE_IMAGES[idleFrame]);
      lastFrameTime = currentTime;
    }
  }

  void stopAnimation() {
    mx.clear();  // Clear the display
    processingFrame = 0;  // Reset processing animation
    idleFrame = 0;  // Reset idle animation
    lastFrameTime = millis();  // Reset timing
  }

  void showTwoArrows() {
    stopAnimation();  // Clear and reset before displaying
    displayFrame(TWO_ARROWS);
  }

  void showArrowDown() {
    stopAnimation();  // Clear and reset before displaying
    displayFrame(ARROW_DOWN);
  }

  void showLeftArrow() {
    stopAnimation();  // Clear and reset before displaying
    displayFrame(LEFT_ARROW);
  }

  void showRightArrow() {
    stopAnimation();  // Clear and reset before displaying
    displayFrame(RIGHT_ARROW);
  }

  void showCustomBitmap(uint64_t bitmap) {
    stopAnimation();  // Clear and reset before displaying
    displayFrame(bitmap);
  }

private:
  void displayFrame(uint64_t frame) {
    uint8_t buffer[8];
    for (int row = 0; row < 8; row++) {
      buffer[row] = (frame >> (row * 8)) & 0xFF;  // Extract each row
    }
    mx.clear();
    for (int row = 0; row < 8; row++) {
      mx.setRow(0, row, buffer[row]);  // Set row data
    }
  }
};

// Define the processing animation array and length
const uint64_t Max7219Animation::PROCESSING_IMAGES[] = {
  0x003c7e66661e1c00,
  0x003c7e66666e2c00,
  0x003c7e6666763400,
  0x003c7e6666783800,
  0x003c7e66607e3c00,
  0x003c7e60667e3c00,
  0x00387866667e3c00,
  0x00347666667e3c00,
  0x002c6e66667e3c00,
  0x001c1e66667e3c00,
  0x003c7e06667e3c00,
  0x003c7e66067e3c00
};
const int Max7219Animation::PROCESSING_IMAGES_LEN = sizeof(PROCESSING_IMAGES) / 8;

// Define the idle animation array and length
const uint64_t Max7219Animation::IDLE_IMAGES[] = {
  0x003c421a3a221c00,
  0x0004325a5a423c00,
  0x0038445c58423c00,
  0x003c425a5a4c2000
};
const int Max7219Animation::IDLE_IMAGES_LEN = sizeof(IDLE_IMAGES) / 8;

#endif