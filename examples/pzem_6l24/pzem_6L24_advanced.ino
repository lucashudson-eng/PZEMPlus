/*
 * PZEM-6L24 Three-Phase Monitor (ASCII tables + NAN-aware)
 *
 * - Configurable output serial (OUT) and host/bridge serial (HOST)
 * - Hex bridge: send "0A,1F,FF" style hex to HOST; raw bytes go to PZEM and hex response comes back
 * - Clean ASCII tables with fixed widths
 * - Uses NAN to flag read errors; tables display "Err" for any NAN
 */

#include <Arduino.h>
#include "PZEM6L24.h" 

//======================================================================================
// CONFIG: set which serials/ports to use
//======================================================================================

// -------- Output serial (where measurements print) --------
#define OUT_SERIAL        Serial        // change to Serial1/Serial2 if you prefer

// -------- Host/bridge serial (where you type hex for bridge) --------
#define HOST_SERIAL       Serial        // can be same as OUT_SERIAL or different

// -------- PZEM serial port & pins --------
// AVR: uses SoftwareSerial on pins below
// ESP32: uses HardwareSerial(PZEM_UART_NUM) and pins PZEM_RX/PZEM_TX
#if defined(__AVR_ATmega328P__)
// Example: Arduino UNO/Nano-class (change pins as wired)
static const uint8_t PZEM_RX = 5;       // D5 RX from PZEM (RO)
static const uint8_t PZEM_TX = 4;       // D4 TX to PZEM (DI)
SoftwareSerial PZEM_SERIAL(PZEM_RX, PZEM_TX);
PZEM6L24 pzem(PZEM_SERIAL, 0xF8);
#else
// ESP32-class defaults (change as needed)
static const int PZEM_UART_NUM = 1;     // UART1
static const int PZEM_RX = 5;           // GPIO5  (PZEM TX -> ESP32 RX)
static const int PZEM_TX = 4;           // GPIO4  (PZEM RX -> ESP32 TX)
HardwareSerial PZEM_SERIAL(PZEM_UART_NUM);
PZEM6L24 pzem(PZEM_SERIAL, PZEM_RX, PZEM_TX, 0xF8);
#endif

//======================================================================================
// Aliases for readability
//======================================================================================
#define OUT  OUT_SERIAL
#define HOST HOST_SERIAL

//======================================================================================
// Timing
//======================================================================================
static unsigned long lastVoltageMillis = 0;
static const unsigned long MEASUREMENT_INTERVAL = 10UL * 1000UL; // 10s

//======================================================================================
// ----------------------------- Table helpers (ASCII) ---------------------------------
//======================================================================================
struct ColSpec {
  const char *header;
  uint8_t     width; // inner width
  bool        right; // right-align numbers
};

// ASCII cell padding (assumes ASCII; avoids Unicode width issues)
static void printPadded(const String &s, uint8_t width, bool right) {
  int len = s.length();
  int pad = (int)width - len;
  if (pad < 0) pad = 0;
  if (!right) {
    OUT.print(s);
    for (int i = 0; i < pad; i++) OUT.write(' ');
  } else {
    for (int i = 0; i < pad; i++) OUT.write(' ');
    OUT.print(s);
  }
}

static void tableBorder(const ColSpec cols[], uint8_t n) {
  OUT.write('+');
  for (uint8_t i = 0; i < n; i++) {
    for (uint8_t j = 0; j < cols[i].width + 2; j++) OUT.write('-');
    OUT.write('+');
  }
  OUT.println();
}

static void tableHeader(const ColSpec cols[], uint8_t n) {
  tableBorder(cols, n);
  OUT.write('|');
  for (uint8_t i = 0; i < n; i++) {
    OUT.write(' ');
    printPadded(String(cols[i].header), cols[i].width, false);
    OUT.write(' ');
    OUT.write('|');
  }
  OUT.println();
  tableBorder(cols, n);
}

static void tableRow(const ColSpec cols[], uint8_t n, const String cells[]) {
  OUT.write('|');
  for (uint8_t i = 0; i < n; i++) {
    OUT.write(' ');
    printPadded(cells[i], cols[i].width, cols[i].right);
    OUT.write(' ');
    OUT.write('|');
  }
  OUT.println();
}

// Format a float with fixed decimals and a unit into ASCII
static String fval(float v, uint8_t decimals, const char *unit) {
  char num[24];
  dtostrf(v, 0, decimals, num);
  String s(num);
  if (unit && unit[0]) { s += ' '; s += unit; }
  return s;
}

// NAN-aware wrapper: returns "Err" if v is NAN, otherwise fval(...)
static inline String safe(float v, uint8_t decimals, const char *unit) {
  return isnan(v) ? String("Err") : fval(v, decimals, unit);
}

//======================================================================================
// --------- Table printers (all NAN-aware via safe()) --------------------------------
//======================================================================================
static void printVoltageTable(float a, float b, float c) {
  const ColSpec cols[] = {
    {"Method", 10, false}, {"Phase A", 10, true}, {"Phase B", 10, true}, {"Phase C", 10, true}
  };
  const uint8_t N = sizeof(cols)/sizeof(cols[0]);
  tableHeader(cols, N);
  String row[N] = { "Multi", safe(a,1,"V"), safe(b,1,"V"), safe(c,1,"V") };
  tableRow(cols, N, row);
  tableBorder(cols, N);
}

static void printPowerTable(float pA, float pB, float pC,
                            float qA, float qB, float qC,
                            float sA, float sB, float sC) {
  const ColSpec cols[] = {
    {"Type", 10, false}, {"Phase A", 10, true}, {"Phase B", 10, true}, {"Phase C", 10, true}
  };
  const uint8_t N = sizeof(cols)/sizeof(cols[0]);
  tableHeader(cols, N);
  String r1[N] = {"Active",   safe(pA,1,"W"),   safe(pB,1,"W"),   safe(pC,1,"W")};
  String r2[N] = {"Reactive", safe(qA,1,"VAR"), safe(qB,1,"VAR"), safe(qC,1,"VAR")};
  String r3[N] = {"Apparent", safe(sA,1,"VA"),  safe(sB,1,"VA"),  safe(sC,1,"VA")};
  tableRow(cols, N, r1);
  tableRow(cols, N, r2);
  tableRow(cols, N, r3);
  tableBorder(cols, N);
}

static void printPFTable(float pfA, float pfB, float pfC) {
  const ColSpec cols[] = { {"Phase", 8, false}, {"Factor", 8, true} };
  const uint8_t N = sizeof(cols)/sizeof(cols[0]);
  tableHeader(cols, N);
  String r1[N] = {"Phase A", safe(pfA,3,"")};
  String r2[N] = {"Phase B", safe(pfB,3,"")};
  String r3[N] = {"Phase C", safe(pfC,3,"")};
  tableRow(cols, N, r1);
  tableRow(cols, N, r2);
  tableRow(cols, N, r3);
  tableBorder(cols, N);
}

static void printEnergyTable(float eA, float eB, float eC) {
  const ColSpec cols[] = { {"Phase", 8, false}, {"Energy", 10, true} };
  const uint8_t N = sizeof(cols)/sizeof(cols[0]);
  tableHeader(cols, N);
  String r1[N] = {"Phase A", safe(eA,2,"kWh")};
  String r2[N] = {"Phase B", safe(eB,2,"kWh")};
  String r3[N] = {"Phase C", safe(eC,2,"kWh")};
  tableRow(cols, N, r1);
  tableRow(cols, N, r2);
  tableRow(cols, N, r3);
  tableBorder(cols, N);
}

static void printCombined(float p, float q, float s, float pf, float e) {
  const ColSpec cols[] = { {"Type", 14, false}, {"Value", 12, true} };
  const uint8_t N = sizeof(cols)/sizeof(cols[0]);
  tableHeader(cols, N);
  String r1[N] = {"Active Pwr",   safe(p,  1,"W")};
  String r2[N] = {"Reactive Pwr", safe(q,  1,"VAR")};
  String r3[N] = {"Apparent Pwr", safe(s,  1,"VA")};
  String r4[N] = {"Power Factor", safe(pf, 3,"")};
  String r5[N] = {"Energy",       safe(e,  2,"kWh")};
  tableRow(cols, N, r1);
  tableRow(cols, N, r2);
  tableRow(cols, N, r3);
  tableRow(cols, N, r4);
  tableRow(cols, N, r5);
  tableBorder(cols, N);
}

static void printAngles(float vA, float vB, float vC, float iA, float iB, float iC) {
  const ColSpec cols[] = {
    {"Type", 8, false}, {"Phase A", 10, true}, {"Phase B", 10, true}, {"Phase C", 10, true}
  };
  const uint8_t N = sizeof(cols)/sizeof(cols[0]);
  tableHeader(cols, N);
  String r1[N] = {"V Angle", safe(vA,1,"deg"), safe(vB,1,"deg"), safe(vC,1,"deg")};
  String r2[N] = {"I Angle", safe(iA,1,"deg"), safe(iB,1,"deg"), safe(iC,1,"deg")};
  tableRow(cols, N, r1);
  tableRow(cols, N, r2);
  tableBorder(cols, N);
}

//======================================================================================
// Hex bridge: HOST <-> PZEM_SERIAL  (comma-separated hex like "0A,1F,FF")
//======================================================================================
static void pzemSerialBridge() {
  if (!HOST) return;

  if (HOST.available()) {
    String cmd;
    unsigned long start = millis();
    // accumulate quick burst
    while (HOST.available() && (millis() - start) < 200) {
      char c = (char)HOST.read();
      cmd += c;
      delay(2);
      start = millis();
    }

    if (cmd.length() > 0) {
      // Parse comma-separated HEX tokens
      const size_t len = cmd.length();
      char *buf = (char*)malloc(len + 1);
      if (!buf) {
        OUT.println(F("Memory alloc failed"));
        return;
      }
      memcpy(buf, cmd.c_str(), len + 1);

      uint8_t outBuf[128];
      size_t outLen = 0;

      char *tok = strtok(buf, ",");
      while (tok && outLen < sizeof(outBuf)) {
        // trim whitespace
        while (*tok && isspace((unsigned char)*tok)) tok++;
        char *endp = tok + strlen(tok) - 1;
        while (endp >= tok && isspace((unsigned char)*endp)) { *endp = '\0'; endp--; }
        if (*tok == '\0') { tok = strtok(NULL, ","); continue; }

        long v = strtol(tok, NULL, 16);  // allow 0x prefix
        if (v < 0 || v > 255) {
          OUT.print(F("Invalid hex value: "));
          OUT.println(tok);
        } else {
          outBuf[outLen++] = (uint8_t)v;
        }
        tok = strtok(NULL, ",");
      }
      free(buf);

      if (outLen == 0) {
        OUT.println(F("No valid hex bytes found"));
        return;
      }

      // Echo what we're sending (hex)
      OUT.print(F("Sending bytes (hex): "));
      for (size_t i = 0; i < outLen; ++i) {
        if (outBuf[i] < 16) OUT.print('0');
        OUT.print(outBuf[i], HEX);
        OUT.print(' ');
      }
      OUT.println();

      // Send to PZEM
      PZEM_SERIAL.write(outBuf, outLen);
      PZEM_SERIAL.flush();

      // Read response (hex) with timeout
      unsigned long respStart = millis();
      OUT.print(F("Response (hex): "));
      while (millis() - respStart < 500) {
        while (PZEM_SERIAL.available()) {
          int b = PZEM_SERIAL.read();
          if (b < 0) break;
          uint8_t ub = (uint8_t)b;
          if (ub < 16) OUT.print('0');
          OUT.print(ub, HEX);
          OUT.print(' ');
          respStart = millis();
        }
      }
      OUT.println();
    }
  }
}

//======================================================================================
// Setup / Loop
//======================================================================================
void setup() {
  // Bring up serials
  OUT.begin(115200);
  while (!OUT) { /* wait for native USB */ }

  if (&HOST != &OUT) {
    HOST.begin(115200);
    while (!HOST) { /* optional wait if HOST is native USB */ }
  }

#if defined(__AVR_ATmega328P__)
  PZEM_SERIAL.begin(9600);
#else
  PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
#endif

  // Init driver
  pzem.begin(9600);

  OUT.println(F("PZEM-6L24 started"));
  OUT.println(F("Waiting for measurements..."));
}

void loop() {
  const unsigned long now = millis();

  // Keep the hex bridge responsive
  pzemSerialBridge();

  if (now - lastVoltageMillis >= MEASUREMENT_INTERVAL) {
    lastVoltageMillis = now;

    OUT.println();
    OUT.println(F("=== PZEM-6L24 Three-Phase Measurements ==="));

    // 1) Individual Phase Measurements
    OUT.println();
    OUT.println(F("1. Individual Phase Measurements:"));
    {
      const ColSpec cols[] = {
        {"Phase", 7, false}, {"Voltage", 10, true}, {"Current", 10, true}, {"Freq", 8, true}
      };
      const uint8_t N = sizeof(cols)/sizeof(cols[0]);
      tableHeader(cols, N);
      for (uint8_t phase = 0; phase < 3; phase++) {
        const float v = pzem.readVoltage(phase);
        const float c = pzem.readCurrent(phase);
        const float f = pzem.readFrequency(phase);
        String row[N] = {
          String("Phase ") + char('A' + phase),
          safe(v, 1, "V"),
          safe(c, 3, "A"),
          safe(f, 1, "Hz")
        };
        tableRow(cols, N, row);
      }
      tableBorder(cols, N);
    }

    delay(200);

    // 2) Multi-Phase Voltage Reading
    OUT.println();
    OUT.println(F("2. Multi-Phase Voltage Reading:"));
    {
      float vA, vB, vC;
      pzem.readVoltage(vA, vB, vC);
      printVoltageTable(vA, vB, vC);
    }

    delay(200);

    // 3) Power Measurements
    OUT.println();
    OUT.println(F("3. Power Measurements:"));
    {
      float pA, pB, pC, qA, qB, qC, sA, sB, sC;
      pzem.readActivePower(pA, pB, pC);
      pzem.readReactivePower(qA, qB, qC);
      pzem.readApparentPower(sA, sB, sC);
      printPowerTable(pA, pB, pC, qA, qB, qC, sA, sB, sC);
    }

    delay(200);

    // 4) Power Factor Measurements
    OUT.println();
    OUT.println(F("4. Power Factor Measurements:"));
    {
      float pfA, pfB, pfC;
      pzem.readPowerFactor(pfA, pfB, pfC);
      printPFTable(pfA, pfB, pfC);
    }

    delay(200);

    // 5) Energy Measurements
    OUT.println();
    OUT.println(F("5. Energy Measurements:"));
    {
      float eA, eB, eC;
      pzem.readActiveEnergy(eA, eB, eC);
      printEnergyTable(eA, eB, eC);
    }

    delay(200);

    // 6) Combined Measurements
    OUT.println();
    OUT.println(F("6. Combined Measurements:"));
    {
      const float p  = pzem.readActivePower();
      const float q  = pzem.readReactivePower();
      const float s  = pzem.readApparentPower();
      const float pf = pzem.readPowerFactor();
      const float e  = pzem.readActiveEnergy();
      printCombined(p, q, s, pf, e);
    }

    delay(200);

    // 7) Phase Angle Measurements
    OUT.println();
    OUT.println(F("7. Phase Angle Measurements:"));
    {
      float vA, vB, vC, iA, iB, iC;
      pzem.readVoltagePhaseAngle(vA, vB, vC);
      pzem.readCurrentPhaseAngle(iA, iB, iC);
      printAngles(vA, vB, vC, iA, iB, iC);
    }

    delay(200);

    // 8) Reset Energy Example (commented out)
    OUT.println();
    OUT.println(F("8. Reset Energy Example:"));
    OUT.println(F("   (Uncomment the code below to test energy reset)"));
    OUT.println(F("   pzem.resetEnergy(PZEM_RESET_ENERGY_ALL);"));

    OUT.println();
    OUT.println(F("=========================================="));
    OUT.println(F("End of measurement cycle"));
    OUT.println(F("=========================================="));
  }
}
