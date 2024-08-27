// Include the necessary libraries
#define BLYNK_TEMPLATE_ID "TMPL3Tv8whDlm"
#define BLYNK_TEMPLATE_NAME "patient monitoring"
#define BLYNK_AUTH_TOKEN "pizyEk3MS9xiaEBabLairbeg-nBIj5Jv"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HX711.h>

// Blynk credentials
char auth[] = "pizyEk3MS9xiaEBabLairbeg-nBIj5Jv";
char ssid[] = "Galaxy S10plus";
char pass[] = "qchahiye";

// Load Cell and HX711 settings
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;
float calibration_factor = 1100;  // Example value; adjust as needed

BlynkTimer timer;

// Pin definitions
#define Buzzer 23

void setup() {
  Serial.begin(115200);

  // Initialize HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Blynk and WiFi initialization
  Blynk.begin(auth, ssid, pass);

  // Calibration
  Serial.println("Tare... remove any weights from the scale.");
  delay(5000);
  scale.tare();
  Serial.println("Tare done...");
  
  // Set the interval to measure and send the weight percentage
  timer.setInterval(1000L, sendWeightPercentage);

  // Initialize buzzer pin
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
}

// Function to measure and send weight percentage
void sendWeightPercentage() {
  if (scale.is_ready()) {
    // Get weight in grams
    float weight = scale.get_units();
    
    // Map weight to percentage
    float max_weight = 1000.0;  // Max weight for 100%
    float percentage = (weight / max_weight) * 100.0;

    // Ensure percentage is within the range [0, 100]
    if (percentage > 100.0) percentage = 100.0;
    if (percentage < 0.0) percentage = 0.0;

    // Send percentage to Blynk
    Blynk.virtualWrite(V0, percentage);

    // Check for alert condition
    if (percentage < 10.0) {
      Blynk.logEvent("saline_alert", "Saline level is below 10%");
      digitalWrite(Buzzer, HIGH); // Activate buzzer
    } else {
      digitalWrite(Buzzer, LOW); // Deactivate buzzer
    }

    // Debug print
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.print(" g, ");
    Serial.print("Percentage: ");
    Serial.print(percentage);
    Serial.println(" %");
  } else {
    Serial.println("HX711 not found.");
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
