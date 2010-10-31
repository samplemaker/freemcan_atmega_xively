/** \file firmware/measurement-timer-adc-trigger.c
 * \brief Timer hardware directly triggering ADC
 *
 * \author Copyright (C) 2010 samplemaker
 * \author Copyright (C) 2010 Hans Ulrich Niedermann <hun@n-dimensional.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 * \defgroup measurement_timer_adc_trigger Timer hardware directly triggering ADC
 * \ingroup firmware
 *
 * Timer hardware directly triggering ADC
 *
 * @{
 */


/** Make sure we use the sub 1 second timer resolution */
#define TIMER_SUB_1SEC

#include "global.h"
#include "measurement-timer-adc-trigger.h"
#include "packet-comm.h"
#include "wdt-softreset.h"


/** timer counter
 *
 * Initialized once by main() with value received from host
 * controller. Never touched by main() again after starting the timer
 * interrupt.
 *
 * Timer interrupt handler has exclusive access to read/writes
 * timer_count to decrement, once the timer ISR has been enabled.
 */
volatile uint16_t timer_count;


/** timer multiple
  *
  * Is send by hostware. Number of dropped analog samples (downsampling of
  * analog signal sampled with timer1 time base)
  */
volatile uint16_t timer_multiple;


/** Last value of timer counter
 *
 * Used for pseudo synchronized reading of the timer_count multi-byte
 * variable in the main program, while timer_count may be written to
 * by the timer ISR.
 *
 * \see get_duration, ISR(TIMER1_COMPA_vect)
 */
volatile uint16_t last_timer_count = 1;


/** Original timer count received in the command.
 *
 * Used later for determining how much time has elapsed yet. Written
 * once only, when the command has been received.
 */
volatile uint16_t orig_timer_count;


/** Configure 16 bit timer to trigger an ISR every 0.1 second
 *
 * Configure "measurement in progress toggle LED-signal"
 */
void timer_init(const uint8_t timer0, const uint8_t timer1)
{
  /** Set up timer with the combined value we just got the bytes of.
   *
   * For some reasons, the following line triggers a bug with
   * the avr-gcc 4.4.2 and 4.5.0 we have available on Fedora
   * 12 and Fedora 13. Debian Lenny (5.05)'s avr-gcc 4.3.2
   * does not exhibit the buggy behaviour, BTW. So we do the
   * assignments manually here.
   *
   * orig_timer_count = (((uint16_t)timer1)<<8) | timer0;
   * timer_count = orig_timer_count;
   */
  asm("\n\t"
      "sts orig_timer_count,   %[timer0]\n\t"
      "sts orig_timer_count+1, %[timer1]\n\t"
      "sts timer_count,   %[timer0]\n\t"
      "sts timer_count+1, %[timer1]\n\t"
      : /* output operands */
      : /* input operands */
        [timer0] "r" (timer0),
        [timer1] "r" (timer1)
      );

  /** Safeguard: We cannot handle 0 or 1 count measurements. */
  if (orig_timer_count <= 1) {
    send_text_P(PSTR("Unsupported timer value <= 1"));
    wdt_soft_reset();
  }

  /* Prepare timer 0 control register A and B for
     clear timer on compare match (CTC)                           */
  TCCR1A = 0;
  TCCR1B =  _BV(WGM12);

  /* Configure "measurement in progress LED"                      */
  /* configure pin 19 as an output */
  DDRD |= (_BV(DDD5));
  /* toggle LED pin 19 on compare match automatically             */
  TCCR1A |= _BV(COM1A0);

  /* Toggle pin on port PD4 on compare match B.  This is ATmega644
   * DIP40 pin 18.  Conflicts with Pollin board usage for switch 3!
   */
  DDRD |= (_BV(DDD4));
  TCCR1A |= _BV(COM1B0);

  /* Prescaler settings on timer conrtrol reg. B                  */
  TCCR1B |=  ((((TIMER_PRESCALER >> 2) & 0x1)*_BV(CS12)) |
              (((TIMER_PRESCALER >> 1) & 0x1)*_BV(CS11)) |
              ((TIMER_PRESCALER & 0x01)*_BV(CS10)));

  /* Derive sample rate (time base) as a multiple of the base
     compare match value for 0.1sec. Write to output compare
     reg. A                                                       */
  OCR1A = (TIMER_COMPARE_MATCH_VAL);

  /* The ADC can only be triggered via compare register B.
     Set the trigger point (compare match B) to 50% of
     compare match A                                              */
  OCR1B = (TIMER_COMPARE_MATCH_VAL >> 1);

  /* we do not need to jump to any ISRs since we do everything
     inside the ADC callback function                             */

  /* output compare match B interrupt enable                      */
  //TIMSK1 |= BIT(OCIE1B);

  /* output compare match A interrupt enable                      */
  //TIMSK1 |= _BV(OCIE1A);
}


/** \todo Should give out a reasonable value */
uint16_t get_duration(void)
{
  return 0;
}


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
