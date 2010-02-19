#ifndef REGISTERS_H
#define REGISTERS_H

/* Safe registers to reserve for special purposes are r2..r7, apparently:
 * http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_regbind
 */

#ifdef __ASSEMBLER__

/* Define the same special use registers as the C compiler uses below */
# define sreg_save r7

#else

/** Reserve register for special use by assembly language ISR.
 *
 * The C compiler will not touch this register then! */
register uint8_t sreg_save asm("r7");

#endif /* !__ASSEMBLER__ */

#endif /* !REGISTERS_H */
