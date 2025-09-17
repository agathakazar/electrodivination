#ifndef MAX7219_ANIMATION_H
#define MAX7219_ANIMATION_H

#include <MD_MAX72xx.h>
#include <freertos/FreeRTOS.h>  // For tasks (ESP32 only)
#include <freertos/task.h>

// Define hardware type and number of devices
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define ANIMATION_TASK_PRIORITY 1  // Low priority to not interfere with main tasks
#define ANIMATION_TASK_STACK_SIZE 2048  // Bytes; adjust if needed

class Max7219Animation {
public:
  enum Mode { PROCESSING, IDLE, STATIC };

private:
  MD_MAX72XX mx;
  static const uint64_t PROCESSING_IMAGES[];
  static const size_t PROCESSING_IMAGES_LEN;
  static const uint64_t IDLE_IMAGES[];
  static const size_t IDLE_IMAGES_LEN;
  static const uint64_t TWO_ARROWS = 0xe7c3a51800000000ULL;
  static const uint64_t ARROW_DOWN = 0x183c7e1818181818ULL;
  static const uint64_t LEFT_ARROW = 0x0703050800000000ULL;
  static const uint64_t RIGHT_ARROW = 0xe0c0a01000000000ULL;

  // Task state (volatile for thread-safety)
  volatile Mode currentMode = PROCESSING;
  volatile int processingFrame = 0;
  volatile int idleFrame = 0;
  volatile unsigned long processingLastTime = 0;
  volatile unsigned long idleLastTime = 0;
  volatile unsigned long processingStartTime = 0;
  volatile uint64_t staticBitmap = 0;
  volatile int frameDelayMs = 150;  // Default; changeable via setFrameDelay()

  TaskHandle_t animationTaskHandle = nullptr;  // Handle for the task

  // Static task function (must be static for FreeRTOS)
  static void animationTask(void* param) {
    Max7219Animation* self = static_cast<Max7219Animation*>(param);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(self->frameDelayMs);  // Delay in ticks

    while (true) {
      unsigned long now = millis();  // Safe to call in task
      uint64_t frame = 0;

      switch (self->currentMode) {
        case PROCESSING:
          if (now - self->processingLastTime >= self->frameDelayMs) {
            self->processingFrame = (self->processingFrame + 1) % self->PROCESSING_IMAGES_LEN;
            frame = self->PROCESSING_IMAGES[self->processingFrame];
            self->processingLastTime = now;
            // Optional auto-switch
            if (now - self->processingStartTime > 5000) {
              self->setModeInternal(IDLE);
            }
          }
          break;
        case IDLE:
          if (now - self->idleLastTime >= self->frameDelayMs) {
            self->idleFrame = (self->idleFrame + 1) % self->IDLE_IMAGES_LEN;
            frame = self->IDLE_IMAGES[self->idleFrame];
            self->idleLastTime = now;
          }
          break;
        case STATIC:
          frame = self->staticBitmap;
          break;
      }

      if (frame != 0) {  // Only update if frame changed (reduces flicker)
        self->displayFrame(frame);
      }

      vTaskDelayUntil(&xLastWakeTime, xFrequency);  // Precise delay, yields CPU
    }
  }

public:
  Max7219Animation(uint8_t dataPin, uint8_t clkPin, uint8_t csPin) 
    : mx(HARDWARE_TYPE, dataPin, clkPin, csPin, MAX_DEVICES) {}
  
  void begin(Mode startMode = IDLE, int initialDelayMs = 150) {
    mx.begin();
    mx.control(MD_MAX72XX::INTENSITY, 3);
    mx.clear();
    currentMode = startMode;
    frameDelayMs = initialDelayMs;
    resetAnimation();
    displayFrame(PROCESSING_IMAGES[0]);  // Initial frame
  }

  // Start the background task (call once in setup())
  void startTask() {
    if (animationTaskHandle == nullptr) {
      xTaskCreatePinnedToCore(
        animationTask,           // Task function
        "AnimationTask",         // Name
        ANIMATION_TASK_STACK_SIZE / sizeof(StackType_t),  // Stack size in words
        this,                    // Parameter
        ANIMATION_TASK_PRIORITY, // Priority
        &animationTaskHandle,    // Handle
        1                        // Pin to core 1 (leave core 0 for main loop/WiFi)
      );
    }
  }

  // Thread-safe mode setter (call from main loop anytime)
  void setMode(Mode mode, uint64_t customBitmap = 0) {
    if (currentMode == mode) return;
    currentMode = mode;
    resetAnimation();
    if (mode == STATIC) {
      staticBitmap = customBitmap;
      displayFrame(customBitmap);
    } else {
      displayFrame(getFirstFrame());
    }
  }

  void setFrameDelay(int delayMs) {
    frameDelayMs = delayMs;  // Task will pick it up next cycle
  }

  // Convenience static shows (thread-safe)
  void showTwoArrows() { setMode(STATIC, TWO_ARROWS); }
  void showArrowDown() { setMode(STATIC, ARROW_DOWN); }
  void showLeftArrow() { setMode(STATIC, LEFT_ARROW); }
  void showRightArrow() { setMode(STATIC, RIGHT_ARROW); }
  void showCustomBitmap(uint64_t bitmap) { setMode(STATIC, bitmap); }

  void stop() {
    setMode(STATIC, 0);
    mx.clear();
    if (animationTaskHandle != nullptr) {
      vTaskDelete(animationTaskHandle);
      animationTaskHandle = nullptr;
    }
  }

  Mode getMode() const { return currentMode; }

private:
  void resetAnimation() {
    processingFrame = 0;
    idleFrame = 0;
    processingLastTime = millis();
    idleLastTime = millis();
    processingStartTime = millis();
  }

  uint64_t getFirstFrame() const {
    switch (currentMode) {
      case PROCESSING: return PROCESSING_IMAGES[0];
      case IDLE: return IDLE_IMAGES[0];
      default: return 0;
    }
  }

  void setModeInternal(Mode mode) {  // Internal, no check
    currentMode = mode;
    resetAnimation();
    displayFrame(getFirstFrame());
  }

  void displayFrame(uint64_t frame) {
    mx.clear();
    for (int row = 0; row < 8; row++) {
      uint8_t rowData = (frame >> (row * 8)) & 0xFF;
      mx.setRow(0, row, rowData);
    }
  }
};

// Array definitions (unchanged)
const uint64_t Max7219Animation::PROCESSING_IMAGES[] = {
  0x003c7e66661e1c00ULL, 0x003c7e66666e2c00ULL, 0x003c7e6666763400ULL,
  0x003c7e6666783800ULL, 0x003c7e66607e3c00ULL, 0x003c7e60667e3c00ULL,
  0x00387866667e3c00ULL, 0x00347666667e3c00ULL, 0x002c6e66667e3c00ULL,
  0x001c1e66667e3c00ULL, 0x003c7e06667e3c00ULL, 0x003c7e66067e3c00ULL
};
const size_t Max7219Animation::PROCESSING_IMAGES_LEN = sizeof(PROCESSING_IMAGES) / sizeof(PROCESSING_IMAGES[0]);

const uint64_t Max7219Animation::IDLE_IMAGES[] = {
  0x003c421a3a221c00ULL, 0x0004325a5a423c00ULL,
  0x0038445c58423c00ULL, 0x003c425a5a4c2000ULL
};
const size_t Max7219Animation::IDLE_IMAGES_LEN = sizeof(IDLE_IMAGES) / sizeof(IDLE_IMAGES[0]);

#endif