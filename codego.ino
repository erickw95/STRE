#include <NilRTOS.h>

const uint8_t D1_PIN = 2;
const uint8_t D2_PIN = 3;
const uint8_t D3_PIN = 4;
const uint8_t D4_PIN = 5;

const uint8_t A_PIN = 6;
const uint8_t B_PIN = 7;
const uint8_t C_PIN = 8;
const uint8_t D_PIN = 9;
const uint8_t E_PIN = 10;
const uint8_t F_PIN = 11;
const uint8_t G_PIN = 12;

const uint8_t SensorPin    = A0;

// numero de leituras sucessivas no sensor pra tirar a média
const uint8_t NumReadings  = 5; 

// leitura do uv
int UVindex = 0;

int DigitOne = '-'; // digito mais da direita
int DigitTwo = '-'; // digito da esquerda

bool firstDigitActive = true; // indica se é o digito da direita que está aceso - dígito da vez


// Declare a semaphore with an inital counter value of zero.
SEMAPHORE_DECL(sem, 0);
//------------------------------------------------------------------------------
// Declare a stack with 128 bytes beyond context switch and interrupt needs.
NIL_WORKING_AREA(waThread2, 128);

NIL_THREAD(Thread2, arg) {

  pinMode(D1_PIN, OUTPUT);
  pinMode(D2_PIN, OUTPUT);
  pinMode(D3_PIN, OUTPUT);
  pinMode(D4_PIN, OUTPUT);
  
  pinMode(A_PIN , OUTPUT);
  pinMode(B_PIN , OUTPUT);
  pinMode(C_PIN , OUTPUT);
  pinMode(D_PIN , OUTPUT);
  pinMode(E_PIN , OUTPUT);
  pinMode(F_PIN , OUTPUT);
  pinMode(G_PIN , OUTPUT);
  
  while (TRUE) {
    
    // espera o sinal
    nilSemWait(&sem);

    digitalWrite(D1_PIN, !firstDigitActive);  // se o primeiro digito é ativo, bota em LOW
    digitalWrite(D2_PIN, firstDigitActive);   // o contrário do anterior
    digitalWrite(D3_PIN, HIGH);
    digitalWrite(D4_PIN, HIGH);

    // guarda o digito da vez
    int digit = firstDigitActive ? DigitOne : DigitTwo ;

    switch(digit)
    {
      case 0:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , HIGH);
        digitalWrite(F_PIN , HIGH);
        digitalWrite(G_PIN , LOW);
        break;
      case 1:
        digitalWrite(A_PIN , LOW);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , LOW);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , LOW);
        digitalWrite(G_PIN , LOW);
        break;
      case 2:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , LOW);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , HIGH);
        digitalWrite(F_PIN , LOW);
        digitalWrite(G_PIN , HIGH);
        break;
      case 3:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , LOW);
        digitalWrite(G_PIN , HIGH);
        break;
      case 4:
        digitalWrite(A_PIN , LOW);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , LOW);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , HIGH);
        digitalWrite(G_PIN , HIGH);
        break;
      case 5:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , LOW);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , HIGH);
        digitalWrite(G_PIN , HIGH);
        break;
      case 6:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , LOW);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , HIGH);
        digitalWrite(F_PIN , HIGH);
        digitalWrite(G_PIN , HIGH);
        break;
      case 7:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , LOW);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , LOW);
        digitalWrite(G_PIN , LOW);
        break;
      case 8:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , HIGH);
        digitalWrite(F_PIN , HIGH);
        digitalWrite(G_PIN , HIGH);
        break;
      case 9:
        digitalWrite(A_PIN , HIGH);
        digitalWrite(B_PIN , HIGH);
        digitalWrite(C_PIN , HIGH);
        digitalWrite(D_PIN , HIGH);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , HIGH);
        digitalWrite(G_PIN , HIGH);
        break;
      case '-': // tracinho para caso de erro
        digitalWrite(A_PIN , LOW);
        digitalWrite(B_PIN , LOW);
        digitalWrite(C_PIN , LOW);
        digitalWrite(D_PIN , LOW);
        digitalWrite(E_PIN , LOW);
        digitalWrite(F_PIN , LOW);
        digitalWrite(G_PIN , HIGH);
        break;
    }

    firstDigitActive = !firstDigitActive; // muda o digito da vez
  }
}
//------------------------------------------------------------------------------
// Declare a stack with 128 bytes beyond context switch and interrupt needs. 
NIL_WORKING_AREA(waThread1, 128);

// Declare the thread function for thread 1.
NIL_THREAD(Thread1, arg) {
  
  while (TRUE) {

    unsigned int acc = 0; // acumulador p/ sucessivas leituras
    for(int i = 0; i < NumReadings; ++i) {
      acc += analogRead(SensorPin);
    }
    unsigned int sensorReading = acc / NumReadings; // média da leitura
    
    float sensorOutVoltage = (float)sensorReading / 1023 * 5.0; // média p/ volt

    // mapa volt-index
    UVindex = (sensorOutVoltage - 0.99) * (15.0 - 0.0) / (2.9 - 0.99) + 0.0; 

    // descarta leitura ruim
    if(UVindex < 0 || UVindex > 15) {
      DigitOne = '-';
      DigitTwo = '-'; 
    }
    else {
      DigitOne = UVindex % 10;  // digito da direita
      DigitTwo = UVindex / 10;  // digito da esquerda
    }   

    // autalização-controle do display
    for(int i = 0; i < 100; ++i) {
      nilThdSleepMilliseconds(10);
      nilSemSignal(&sem); // sinal pra outra thread
    }
  }
}
//------------------------------------------------------------------------------
/*
 * Threads static table, one entry per thread.  A thread's priority is
 * determined by its position in the table with highest priority first.
 * 
 * These threads start with a null argument.  A thread's name may also
 * be null to save RAM since the name is currently not used.
 */
NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY("thread1", Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY("thread2", Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()
//------------------------------------------------------------------------------
void setup() {
  // Start Nil RTOS.
  nilSysBegin();
}
//------------------------------------------------------------------------------
// Loop is the idle thread.  The idle thread must not invoke any 
// kernel primitive able to change its state to not runnable.
void loop() {
  // Not used.
}