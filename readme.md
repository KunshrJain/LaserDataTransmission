ESP32 Dual-Laser Synchronous Serial Communicator
This project implements a one-way, wireless optical communication system using two ESP32s and two lasers. It transmits text data (like "Hello") by serializing it into bits and sending it over a custom, reliable protocol.

Unlike simpler one-laser projects, this system uses a synchronous protocol, which makes it far more robust.

Data Laser: Carries the actual bit value (1 or 0).

Clock Laser: Provides a timing pulse to tell the receiver exactly when to read the Data Laser.

This method eliminates errors from timing mismatches (clock drift) that plague asynchronous (single-laser) systems.

🛰️ The Communication Protocol Explained
This is the most important part of the project. The two lasers work like a standard SPI or I2C bus, but with light.

LASER_CLOCK / LDR_CLOCK is the Serial Clock (SCK).

LASER_DATA / LDR_DATA is the Serial Data (MOSI).

The entire transfer is "Receiver-Driven" in the sense that the Receiver only acts when it sees a clock pulse.

How a Single Bit is Transmitted (e.g., sending a '1')
Here is the exact sequence of events for every single bit:

Idle State: Both Transmitter lasers are OFF.

LASER_CLOCK = HIGH (Active-Low)

LASER_DATA = HIGH (Active-Low)

Step 1: Set Data (Transmitter)

The Transmitter wants to send a 1.

It pulls the LASER_DATA line LOW (turning the Data Laser ON).

(If it were sending a 0, it would leave LASER_DATA HIGH / OFF).

Step 2: Setup Delay (Transmitter)

The Transmitter calls delay(setupDelay); (e.g., 200ms).

This pause is crucial. It gives the laser time to fully turn on and gives the LDR on the receiver side time to "react" to the light and for its resistance to stabilize.

Step 3: Clock Pulse (Transmitter)

The Transmitter now pulls the LASER_CLOCK line LOW (turning the Clock Laser ON).

This is the "trigger" signal. It shouts to the Receiver: "READ THE DATA LINE NOW!"

The Transmitter holds this pulse for delay(bitDelay); (e.g., 1000ms).

Step 4: Clock Detect (Receiver)

The Receiver, which is constantly checking analogRead(LDR_CLOCK), sees the light.

The if (clkVal > threshClock) condition becomes true.

Step 5: Data Read (Receiver)

The Receiver's indicator LED turns on.

It waits a very short delay(100); to ensure it's reading in the middle of the clock pulse, not at the very edge.

It then immediately checks the other LDR: int dataVal = analogRead(LDR_DATA);.

Since the Data Laser is ON (from Step 2), dataVal will be high.

int bitVal = (dataVal > threshData) ? 1 : 0; resolves to 1.

This 1 is stored in the bits[] array.

Step 6: End of Pulse (Transmitter)

The Transmitter's delay(bitDelay); finishes.

It sets LASER_CLOCK back to HIGH (Clock Laser OFF).

It also sets LASER_DATA back to HIGH (Data Laser OFF) to be ready for the next bit.

Step 7: Wait for Reset (Receiver)

The Receiver is now in its "debounce" loop: while (analogRead(LDR_CLOCK) > threshClock).

It stays "stuck" in this loop, waiting for the Clock Laser to turn OFF.

As soon as the Clock Laser turns off, analogRead(LDR_CLOCK) drops below threshClock, the loop exits, and the Receiver goes back to Step 4, waiting for the next clock pulse.

This process repeats 8 times to build a full character.

🛠️ Hardware Requirements
2x ESP32 Development Boards

2x Laser Diode Modules (Code is written for Active-Low modules, where a LOW signal turns the laser ON)

2x Light Dependent Resistors (LDRs / Photoresistors)

2x 10kΩ Resistors (for the LDR voltage dividers)

2x Status LEDs (any color)

2x 220Ω Resistors (current-limiting for the LEDs)

2x Breadboards

Jumper Wires

🔌 Wiring Diagrams
Transmitter (ESP32 #1)
The transmitter is a simple digital output setup.

GPIO 26 -> Clock Laser IN pin

GPIO 25 -> Data Laser IN pin

GPIO 2 -> 220Ω Resistor -> LED Anode (+)

Laser VCC -> 3.3V or 5V (check your module)

Laser GND -> GND

LED Cathode (-) -> GND

Receiver (ESP32 #2)
The receiver uses Analog-to-Digital (ADC) pins. LDRs are resistors, so we must use a voltage divider to convert their changing resistance into a voltage the ESP32 can read.

Clock LDR Circuit:

3.3V -> 10kΩ Resistor -> NODE_A

NODE_A -> GPIO 34 (This is an ADC pin)

NODE_A -> LDR (one leg)

LDR (other leg) -> GND

Data LDR Circuit:

3.3V -> 10kΩ Resistor -> NODE_B

NODE_B -> GPIO 35 (This is an ADC pin)

NODE_B -> LDR (one leg)

LDR (other leg) -> GND

Indicator LED:

GPIO 2 -> 220Ω Resistor -> LED Anode (+)

LED Cathode (-) -> GND

Why the voltage divider? The LDR's resistance decreases when light hits it.

No Light: LDR resistance is high. Most of the 3.3V is "used" by the LDR, so the voltage at NODE_A is low. analogRead() returns a low value.

Laser Light: LDR resistance drops to (almost) zero. It becomes a plain wire. All 3.3V now flows to NODE_A. analogRead() returns a high value (e.g., 3000+).
