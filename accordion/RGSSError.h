/**
 * @file RGSSError.h
 * @brief It defines RGSSError class.
 * @author Masaki Hara
 */

#ifndef RGSSERROR_H_INCLUDED
#define RGSSERROR_H_INCLUDED

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Corresponding to the Ruby class RGSSError.
 */
extern VALUE rb_eRGSSError;

/**
 * @brief Initializes RGSSError.
 */
extern void Init_RGSSError(void);

#ifdef __cplusplus
}
#endif

#endif // RGSSERROR_H_INCLUDED
