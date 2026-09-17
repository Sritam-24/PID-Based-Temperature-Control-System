/*
=====================================================
 Temperature Control System using PID Controller
 Arduino UNO + LM35 + IRF520 MOSFET + DC Bulb
 Setpoint = 32°C
 Bulb Always ON
=====================================================
*/

const int sensorPin = A0;
const int bulbPin   = 9;

// Desired Temperature
float setPoint = 32.0;

// ---------- PID Gains ----------
float Kp = 15.0;
float Ki = 0.30;
float Kd = 1.00;

// ---------- PID Variables ----------
float error = 0;
float previousError = 0;
float integral = 0;
float derivative = 0;
float output = 0;

// Time
unsigned long previousTime = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(bulbPin, OUTPUT);

  previousTime = millis();
}

void loop()
{
  //-------------------------------
  // Read LM35
  //-------------------------------
  int adcValue = analogRead(sensorPin);

  float voltage = adcValue * (5.0 / 1023.0);

  // LM35 Conversion
  float temperature = voltage * 100.0;

  //-------------------------------
  // Calculate Sampling Time
  //-------------------------------
  unsigned long currentTime = millis();

  float dt = (currentTime - previousTime) / 1000.0;

  if (dt <= 0)
    dt = 0.001;

  //-------------------------------
  // PID Controller
  //-------------------------------
  error = setPoint - temperature;

  // Integral
  integral += error * dt;

  // Anti-Windup
  integral = constrain(integral, -100, 100);

  // Derivative
  derivative = (error - previousError) / dt;

  // PID Output
  output = Kp * error +
           Ki * integral +
           Kd * derivative;

  //-------------------------------
  // PWM Limits
  //-------------------------------

  // Maximum brightness
  if (output > 255)
    output = 255;

  // Minimum brightness
  if (output < 30)
    output = 30;

  //-------------------------------
  // Drive MOSFET
  //-------------------------------
  analogWrite(bulbPin, (int)output);

  //-------------------------------
  // Serial Monitor
  //-------------------------------
  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.print(" C");

  Serial.print(" | Setpoint : ");
  Serial.print(setPoint);

  Serial.print(" | Error : ");
  Serial.print(error);

  Serial.print(" | PWM : ");
  Serial.println((int)output);

  previousError = error;
  previousTime = currentTime;

  delay(100);
}