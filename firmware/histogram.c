/** \file firmware/histogram.c
 * \brief Histogram
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
 * \defgroup histogram Histogram
 * \ingroup firmware
 *
 * Histogram.
 *
 * @{
 */


#include "global.h"
#include "histogram.h"
#include "uart-comm.h"
#include "frame-comm.h"
#include "measurement-timer.h"
#include "data-table.h"


/** \addtogroup firmware_comm
 * @{
 */


#ifdef INVENTED_HISTOGRAM

/** created from binary via objcopy */
extern uint8_t invented_histogram[] asm("_binary_invented_histogram_bin_start") PROGMEM;
/** created from binary via objcopy */
extern uint8_t invented_histogram_size[] asm("_binary_invented_histogram_bin_size") PROGMEM;
/** created from binary via objcopy */
extern uint8_t invented_histogram_end[] asm("_binary_invented_histogram_bin_end") PROGMEM;

#if (ELEMENT_SIZE_IN_BYTES == 3)
/** Simulate a histogram based on the invented histogram data */
static
void invent_histogram(const uint16_t duration)
{
  uint8_t *t8 = (uint8_t *)table;
  for (size_t j=0; j<3*MAX_COUNTER; j+=3) {
    const uint32_t v =
      (((uint32_t)pgm_read_byte(&(invented_histogram[j+0])))<< 0) +
      (((uint32_t)pgm_read_byte(&(invented_histogram[j+1])))<< 8) +
      (((uint32_t)pgm_read_byte(&(invented_histogram[j+2])))<<16);
    const uint32_t r = (v*duration) >> 8;
    t8[j+0] = (r>> 0) & 0xff;
    t8[j+1] = (r>> 8) & 0xff;
    t8[j+2] = (r>>16) & 0xff;
  }
}
#endif

#endif


/** Send histogram packet to controller via serial port (layer 3).
 *
 * \param type The type of histogram we are sending
 *             (#packet_histogram_type_t).  You may also a dummy value
 *             like '?' or -1 or 0xff or 0 until you make use of that
 *             value on the receiver side.
 *
 * Note that send_histogram() might take a significant amount of time.
 * For example, at 9600bps, transmitting a good 3KByte will take a
 * good 3 seconds.  If you disable interrupts for that time and want
 * to continue the measurement later, you will want to properly pause
 * the timer.  We are currently keeping interrupts enabled if we
 * continue measuring, which avoids this issue.
 *
 * Note that for 'I' histograms it is possible that we send fluked
 * values due to overflows.
 */
void send_histogram(const packet_histogram_type_t type)
{
  const uint16_t duration = get_duration();

#ifdef INVENTED_HISTOGRAM
  invent_histogram(duration);
#endif

  packet_histogram_header_t header = {
    ELEMENT_SIZE_IN_BYTES,
    type,
    duration,
    orig_timer_count
  };
  frame_start(FRAME_TYPE_HISTOGRAM, sizeof(header)+sizeof_table);
  uart_putb((const void *)&header, sizeof(header));
  uart_putb((const void *)table, sizeof_table);
  frame_end();
}


/** @} */


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */