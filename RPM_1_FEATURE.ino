

//   RPM LED UNTUK MOTOR VER. 1 NEW (JUNI 2020)
//   MENGGUNAKAN IC ATMEGA SERIES DENGAN BASIC ARDUINO IDE
//   JUMLAH LED BEBAS, SEMAMPU ANDA MEMBUAT PCB NYA
//   INPUT SENSOR PADA PIN DIGITAL D2. => (KONEK SENSOR PADA KABEL MINUS(-) KOIL BUSI UNTUK MOTOR INJEKSI DAN KONEK KE KABEL PLUS(+)
//                                        UNTUK MOTOR KARBU)
//   UNTUK FEATURE SHIFT LIGHT DAN LAIN-LAIN BELUM DIMASUK KAN KE PROGRAM INI
//   UNTUK KALKULASI RPM DIAMBIL DARI CHANEL YOUTUBE "InterlinkKnight" VIDEO  https://www.youtube.com/watch?v=u2uJMJWsfsg
//   JANGAN LUPA SUBSCRIBE CHANNEL YOUTUBE KAMI https://youtu.be/gY7U8oPt_HA



//.......................................................SEMOGA SUKSES...........................................................



const byte PulsesPerRevolution = 60;  // UBAH NILAI UNTUK KALIBRASI, MINIMAL 2. SESUAIKAN NYALA LED DENGAN ACTUAL RPM PADA MOTOR
// JIKA DIMOTOR TIDAK ADA TACHOMETER MAKA DI KIRA _ KIRA SAJA

const int ledCount = 11;    // JUMLAH TOTAL LED (ledPins[])
int ledPins[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}; // PIN IC ATMEGA ATAU ARDUINO YANG TERHUBUNG KE LED
// NOTE: SEMUA PIN DIGITAL DAN ANALOG BOLEH DIPAKAI UNTUK LED KECUALI PIN NO.2 (HARAM)

const unsigned long ZeroTimeout = 100000;  // UNTUK HIGH RESPON ISI NILAINYA 100000
// JIKA YANG DIBACA LOW RPM MAKA ISI NILAINYA 300000.

// Calibration for smoothing RPM:
const byte numReadings = 2;  //NILAI GANTI 1 UNTUK PEMBACAAN SMOOTH, DEFAULT 2


/////////////
// Variables:
/////////////

volatile unsigned long LastTimeWeMeasured;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long FrequencyReal;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;
unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;
unsigned long CurrentMicros = micros();
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;
unsigned long readings[numReadings];
unsigned long readIndex;
unsigned long total;
float average;

// VARIABLE RPM YANG DITAMPILKAN
int RPM_minimal = 1000;  // UBAH NILAI UNTUK BATAS BAWAH PEMBACAAN RPM DI MOTOR ANDA
int RPM_maximal = 9000;  // UBAH NILAI UNTUK PEMBACAAN BATAS ATAS RPM DI MOTOR ANDA

void setup()  // Start of setup:
{


  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);
  delay(1000);
}

void loop()  // Start of loop:
{
  LastTimeCycleMeasure = LastTimeWeMeasured;
  CurrentMicros = micros();
  if (CurrentMicros < LastTimeCycleMeasure)
  {
    LastTimeCycleMeasure = CurrentMicros;
  }
  FrequencyRaw = 10000000000 / PeriodAverage;

  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra)
  {
    FrequencyRaw = 0;
    ZeroDebouncingExtra = 2000;
  }
  else
  {
    ZeroDebouncingExtra = 0;
  }
  FrequencyReal = FrequencyRaw / 10000;
  RPM = FrequencyRaw / PulsesPerRevolution * 60;
  RPM = RPM / 10000;

  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings)
  {
    readIndex = 0;
  }


  average = total / numReadings;

  int ledLevel = map(average, RPM_minimal, RPM_maximal, ledCount, 0);


  for (int thisLed = 0; thisLed < ledCount; thisLed++) {


    if (thisLed < ledLevel) {
      digitalWrite(ledPins[thisLed], LOW);
    }

    else {
      digitalWrite(ledPins[thisLed], HIGH);
    }
  }
}

void Pulse_Event()
{
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;
  LastTimeWeMeasured = micros();

  if (PulseCounter >= AmountOfReadings) {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;
    PeriodSum = PeriodBetweenPulses;
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);
    AmountOfReadings = RemapedAmountOfReadings;
  }
  else
  {
    PulseCounter++;
    PeriodSum = PeriodSum + PeriodBetweenPulses;
  }
} // SEMOGA SUKSES............................

// Ganbatte
