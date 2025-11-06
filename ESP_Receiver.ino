// === ESP32 LASER RECEIVER (LDR based, Debug Mode) ===
// LDR_CLOCK → GPIO 34
// LDR_DATA  → GPIO 35
// LED indicator → GPIO 2

#define LDR_CLOCK 34
#define LDR_DATA  35
#define LED_PIN   2

int threshClock = 1000;
int threshData  = 1000;

int bits[8];
int bitCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LDR Receiver Ready...");
  Serial.println("Waiting for bits...\n");
}

void loop() {
  int clkVal = analogRead(LDR_CLOCK);

  // Detect clock laser ON
  if (clkVal > threshClock) {
    digitalWrite(LED_PIN, HIGH);
    delay(100); // sync delay to match transmitter

    int dataVal = analogRead(LDR_DATA);
    int bitVal = (dataVal > threshData) ? 1 : 0;
    bits[bitCount++] = bitVal;

    // print live bit reception
    Serial.print(bitVal);

    // when full byte (8 bits) is ready
    if (bitCount == 8) {
      char c = bitsToChar(bits);
      Serial.print("  ->  ");

      // print binary representation
      for (int i = 0; i < 8; i++) Serial.print(bits[i]);
      
      Serial.print("  =  '");
      Serial.print(c);
      Serial.println("'");

      bitCount = 0;
    }

    // Wait for clock laser OFF before next bit
    while (analogRead(LDR_CLOCK) > threshClock)
      delay(5);

    digitalWrite(LED_PIN, LOW);
  }
}

// Converts 8-bit array to a character
char bitsToChar(int b[]) {
  int val = 0;
  for (int i = 0; i < 8; i++)
    val = (val << 1) | b[i];
  return (char)val;
}
