#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

// Calibration factor (you need to determine this value by calibration)
float calibration_factor = 1100;  // Example value; adjust as needed

void setup() {
  Serial.begin(115200);
  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Calibration
  Serial.println("Tare... remove any weights from the scale.");
  delay(5000);
  scale.tare();
  Serial.println("Tare done...");
  Serial.println("Place a known weight on the scale for calibration...");
  delay(5000);
  long reading = scale.get_units(10);
  Serial.print("Known weight reading: ");
  Serial.println(reading);

  // Adjust the calibration factor based on the known weight
  float known_weight = 1000.0;  // Known weight in grams (1 kg)
  calibration_factor = reading / known_weight;
  scale.set_scale(calibration_factor);
  Serial.print("Calibration factor set to: ");
  Serial.println(calibration_factor);
}

void loop() {
  if (scale.is_ready()) {
    // Get weight in grams
    float weight = scale.get_units();

    // Map weight to percentage
    float max_weight = 1000.0;  // Max weight for 100%
    float percentage = (weight / max_weight) * 100.0;

    // Ensure percentage is within the range [0, 100]
    if (percentage > 100.0) percentage = 100.0;
    if (percentage < 0.0) percentage = 0.0;

    // Print weight and percentage
    Serial.print("Weight (g): ");
    Serial.print(weight);
    Serial.print(" g, ");
    Serial.print("Percentage: ");
    Serial.print(percentage);
    Serial.println(" %");
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}
