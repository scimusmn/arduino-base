/* @INCLUDE */
#include <avr/interrupt.h>


/* @HEADER */

#ifdef SMM_ARCH_MEGA
struct TimerInterrupt {
  void (*fn)(void*);
  void *ptr;
  uint8_t *tccra, *tccrb, *timsk;
  uint16_t *ocra, *tcnt;

  unsigned long ocrSize(unsigned long us, unsigned int prescaler);
  bool trySetRegisters(unsigned long us, int prescaler, int mask);
  bool setInterval(unsigned long us);
  bool beginInterval(void (*fn)(void*), unsigned long us, void *ptr = nullptr);
  void clearInterval();
};


struct TimerInterruptManager {
  void setup();
  TimerInterrupt * beginInterval(void (*fn)(void*), unsigned long us, void *ptr = nullptr);

  TimerInterrupt timer1, timer3, timer4, timer5;
};

class IntervalTimer {
  public:
  bool begin(void (*fn)(void*), unsigned long us, void *ptr = nullptr);
  void end();
  private:
  TimerInterrupt *interrupt = nullptr;
};
#endif

/* @EXTERN */

#ifdef SMM_ARCH_MEGA
extern smm::TimerInterruptManager SmmTimers;
#endif

/* @IMPLEMENTATION */

#ifdef SMM_ARCH_MEGA

smm::TimerInterruptManager SmmTimers;

unsigned long smm::TimerInterrupt::ocrSize(unsigned long us, unsigned int prescaler) {
  // this calculation is 2x that of the one from section 17.9.2 because the datasheet
  // equation is for a frequency (which requires two interrupts to complete one cycle)
  double ocr = us;
  ocr *= SMM_SYSTEM_CLOCK_FREQ;
  ocr /= prescaler;
  return ocr-1;
}


bool smm::TimerInterrupt::trySetRegisters(unsigned long us, int prescaler, int mask) {
  unsigned long ocr = ocrSize(us, prescaler);
  if (ocr <= 0xffff) {
    cli();
    // see the ATMEGA2560 datasheet section 17.11 for details
    *tcnt = 0;
    *tccra = 0;           // clear a-control register;
    *tccrb = (1<<WGM12);  // enable CTC mode (this is the same bit on all four registers)
    *tccrb |= mask;       // set the prescaler
    *timsk = (1<<OCIE1A); // enable the interrupt (this is also the same bit on all four registers)
    *ocra = ocr;          // set proper timing
    sei();
    return true;
  } else {
    return false;
  }
}



bool smm::TimerInterrupt::setInterval(unsigned long us) {
  if (trySetRegisters(us, 1, 0b001)) { return true; }
  if (trySetRegisters(us, 8, 0b010)) { return true; }
  if (trySetRegisters(us, 64, 0b011)) { return true; }
  if (trySetRegisters(us, 256, 0b100)) { return true; }
  if (trySetRegisters(us, 1024, 0b101)) { return true; }
  return false;
}



bool smm::TimerInterrupt::beginInterval(void (*fn)(void*), unsigned long us, void *ptr) {
  if (this->fn != nullptr) {
    return false;
  }
  this->fn = fn;
  this->ptr = ptr;
  return setInterval(us);
}


void smm::TimerInterrupt::clearInterval() {
  // disable the interrupt
  cli();
  *timsk = 0;
  sei();

  // clear the function
  fn = nullptr;
  ptr = nullptr;
}


void smm::TimerInterruptManager::setup() {
  timer1.fn = nullptr;
  timer1.ptr = nullptr;
  timer1.tccra = &(TCCR1A);
  timer1.tccrb = &(TCCR1B);
  timer1.timsk = &(TIMSK1);
  timer1.ocra = &(OCR1A);
  timer1.tcnt = &(TCNT1);

  timer3.fn = nullptr;
  timer3.ptr = nullptr;
  timer3.tccra = &TCCR3A;
  timer3.tccrb = &TCCR3B;
  timer3.timsk = &TIMSK3;
  timer3.ocra = &OCR3A;
  timer3.tcnt = &TCNT3;

  timer4.fn = nullptr;
  timer4.ptr = nullptr;
  timer4.tccra = &TCCR4A;
  timer4.tccrb = &TCCR4B;
  timer4.timsk = &TIMSK4;
  timer4.ocra = &OCR4A;
  timer4.tcnt = &TCNT4;

  timer5.fn = nullptr;
  timer5.ptr = nullptr;
  timer5.tccra = &TCCR5A;
  timer5.tccrb = &TCCR5B;
  timer5.timsk = &TIMSK5;
  timer5.ocra = &OCR5A;
  timer5.tcnt = &TCNT5;
}


smm::TimerInterrupt * smm::TimerInterruptManager::beginInterval(void (*fn)(void*), unsigned long us, void *ptr) {
  if (timer1.beginInterval(fn, us, ptr)) { return &timer1; }
  if (timer3.beginInterval(fn, us, ptr)) { return &timer3; }
  if (timer4.beginInterval(fn, us, ptr)) { return &timer4; }
  if (timer5.beginInterval(fn, us, ptr)) { return &timer5; }
  return nullptr;
}


bool smm::IntervalTimer::begin(void (*fn)(void*), unsigned long us, void *ptr = nullptr) {
  if (this->interrupt == nullptr) {
    smm::TimerInterrupt *interrupt = SmmTimers.beginInterval(fn, us, ptr);
    if (interrupt != nullptr) {
      this->interrupt = interrupt;
      return true;
    } else {
      return false;
    }
  } else {
    return this->interrupt->beginInterval(fn, us, ptr);
  }
}


void smm::IntervalTimer::end() {
  if (interrupt != nullptr) {
    interrupt->clearInterval();
  }
}


ISR(TIMER1_COMPA_vect) {
  SmmTimers.timer1.fn(SmmTimers.timer1.ptr);
}

ISR(TIMER3_COMPA_vect) {
  SmmTimers.timer3.fn(SmmTimers.timer3.ptr);
}

ISR(TIMER4_COMPA_vect) {
  SmmTimers.timer4.fn(SmmTimers.timer4.ptr);
}

ISR(TIMER5_COMPA_vect) {
  SmmTimers.timer5.fn(SmmTimers.timer5.ptr);
}
#endif

/* @SETUP */

  #ifdef SMM_ARCH_MEGA
  SmmTimers.setup();
  #endif

/* @END */
