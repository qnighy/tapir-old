/**
 * @file RGSSReset.h
 * @brief It defines RGSSReset class.
 * @author Masaki Hara
 */

#ifndef RGSSRESET_H_INCLUDED
#define RGSSRESET_H_INCLUDED

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Corresponding to the Ruby class RGSSReset.
 */
extern VALUE rb_eRGSSReset;

/**
 * @brief Initializes RGSSReset.
 */
extern void Init_RGSSReset(void);

#ifdef __cplusplus
}
#endif

#endif // RGSSRESET_H_INCLUDED
