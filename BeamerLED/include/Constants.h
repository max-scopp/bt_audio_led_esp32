
/////////////
/// Common
/////////////

#define BUZZER_PIN 27
#define LOOP_DEBUG_PIN 33

/////////////
/// Dave's remainings
/////////////

// #define FAN_SIZE 60      // How many pixels per fan
// #define NUM_FANS 1       // Number of fans in the strans
// #define LED_FAN_OFFSET 0 // How far from 12 o'clock first pixel is

/////////////
/// LEDs
/////////////

#define TARGET_FPS 60
#define SHOW_FPS 0

#define DEBUG_TO_SERIAL 0

/////////////
/// Audio
/////////////

#define PEAK2_DECAY_PER_SECOND 1.4f

/// ============================================================

#define BAND_COUNT 16      // Choices are 8, 16, 24, or 32.  Only 16 is "pretty" and hand-tuned, but you could fix others
#define MATRIX_WIDTH 48    // Number of pixels wide
#define MATRIX_HEIGHT 16   // Number of pixels tall
#define GAIN_DAMPEN 2      // Higher values cause auto gain to react more slowly (default 2)
#define INPUT_PIN 4        // Audio line input
#define COLOR_SPEED_PIN 33 // How fast palette rotates in spectrum bars
#define MAX_COLOR_SPEED 64 //    ...and the max allowed

#define BRIGHTNESS_PIN 25   // Pin for brightness pot read
#define PEAK_DECAY_PIN 26   // Pin for peak decay pot read
#define COLOR_SCHEME_PIN 27 // Pin for controlling color scheme

#define SUPERSAMPLES 2                                    // How many supersamples to take
#define SAMPLE_BITS 12                                    // Sample resolution (0-4095, default 12)
#define MAX_ANALOG_IN ((1 << SAMPLE_BITS) * SUPERSAMPLES) // What our max analog input value is on all analog pins (4096 is default 12 bit resolution)
#define MAX_VU 10000                                      // How high our VU could max out at.  Arbitarily tuned.
#define ONSCREEN_FPS 0                                    // Debugging display of FPS count on LED screen
#define MS_PER_SECOND 1000                                // 1000 milliseconds per second
#define STACK_SIZE 4096                                   // Stack size for each new thread

#define BLACK 0x0000 // Color definitions in 16-bit 5-6-5 space
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
