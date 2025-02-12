/*
  Code to modulate an interupter for a tesla coil

  fout = 16e6 / (N * (1 + TOP))
  TOP = (16e6 / (fout * N)) - 1

  where N is the prescaler value

  TOP is constrained to 0 and 65535,
  so the output frequency can be adjusted from 16e6 to 244 Hz (for N = 1),
  although at low frequencies the PWM resolution is generally poor

  Increasing N will improve the low frequency resolution and reduce the 
  maximum PWM frequency by the same amount

  The PWM resolution is log(TOP + 1) / log(2), so maximising TOP is critical for good PWM resolution  
*/

uint32_t clock_frequency = 16e6;
uint32_t prescalers[] = {1, 8, 64, 256, 1024};

int prescaler_bits(int n) {
  switch (n) {
    case 8:
      return _BV(CS11);
    case 64:
      return _BV(CS10) | _BV(CS11);
    case 256:
      return _BV(CS12);
    case 1024:
      return _BV(CS10) | _BV(CS12);
  }
  return _BV(CS10);
}

void initialize_pwm(int prescaler, uint16_t comp_reg_a, uint16_t comp_reg_b) {
  // enable timer1 control pins
  DDRB = _BV(PCINT1) | _BV(PCINT2);  
  TCCR1A = 0;
  TCCR1B = 0;
  // TOP
  OCR1A = comp_reg_a;
  // Duty cycle = OCR1B / OCR1A
  OCR1B = comp_reg_b;
  // fast PWM mode
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | prescaler_bits(prescaler);
}

void set_pwm_frequency(unsigned int frequency) {
  // Optimize PWM resolution
  uint16_t top = 0;
  int prescale = 1;
  for (auto n : prescalers) {
    float test = (float)clock_frequency / (float)((float)frequency * (float)n) - (float)1;
    if (test <= 0xffff && test > top) {
      prescale = n;
      top = test;
    }
  }
  // initialize to 50% duty cycle
  initialize_pwm(prescale, top, (float)top * 0.5);
}

void set_pwm_duty_cycle(float d) {
  // TODO clamp d to 0 and 1
  OCR1B = (float)OCR1A * d;
}


int main(void) {

  //pinMode(9, OUTPUT);
  //pinMode(10, OUTPUT);
  //pinMode(3, OUTPUT);

  //pdat_dir1();

  set_pwm_frequency(20);
  set_pwm_duty_cycle(0.15);

  for (;;) {

  }

  return 0;
}