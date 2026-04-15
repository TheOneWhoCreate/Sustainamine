#include <WiFi.h>
#include <WebServer.h>

// --- Motor Pins ---
const int ENA = 14;
const int IN1 = 27;
const int IN2 = 26;

// --- Ultrasonic Pins ---
const int trigPin = 5;
const int echoPin = 18;

// --- WiFi Credentials ---
const char* ssid = "ESP32_Water";
const char* password = "12345678";

WebServer server(80);

// Variable to track motor state for the web dashboard
bool isMotorOn = false;

// Function to calculate distance
float getDistance() {
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  return distance;
}

// HTML page generator
String getHTML(float distance, bool motorState) {
  String html = "<!DOCTYPE html><html>";
  html += "<head>";
  html += "<meta http-equiv='refresh' content='2'>";  // Refreshes page every 2 seconds
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;background:#f2f2f2;}";
  html += ".card{background:white;padding:20px;margin:50px auto;width:300px;border-radius:10px;box-shadow:0 0 10px #aaa;}";
  // Adds green text if ON, red text if OFF
  String statusColor = motorState ? "green" : "red";
  html += "</style></head>";

  html += "<body>";
  html += "<div class='card'>";
  html += "<h1>System Dashboard</h1>";
  html += "<h2>Distance: " + String(distance) + " cm</h2>";
  html += "<h2>Motor: <span style='color:" + statusColor + "'>" + (motorState ? "ON" : "OFF") + "</span></h2>";
  html += "</div>";
  html += "</body></html>";

  return html;
}

// Handle webpage request
void handleRoot() {
  float distance = getDistance();
  server.send(200, "text/html", getHTML(distance, isMotorOn));
}

void setup() {
  Serial.begin(115200);  // Standard ESP32 baud rate

  // --- Setup Motor ---
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Ensure motor is off when it first boots up
  digitalWrite(ENA, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // --- Setup Ultrasonic Sensor ---
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // --- Start WiFi Access Point ---
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();

  Serial.println("WiFi started");
  Serial.println("Connect to ESP32_Water and go to 192.168.4.1");
}

void loop() {
  // 1. Keep the web server listening for connections
  server.handleClient();

  // 2. Read the current distance
  float currentDistance = getDistance();

  // 3. Motor Control Logic
  // We check if distance > 0 to prevent the motor from triggering if the sensor bugs out and reads 0.0
  if (currentDistance > 0 && currentDistance <= 8.0) {

    // Spin Forward Full Speed
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(ENA, HIGH);
    isMotorOn = true;

  } else {

    // Stop Motor
    digitalWrite(ENA, LOW);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    isMotorOn = false;
  }

  // A tiny delay to keep the ESP32 from crashing due to checking the sensor too rapidly
  delay(50);
}