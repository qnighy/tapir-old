/**
 * @file Tone.h
 * @brief It defines Tone class.
 * @author Masaki Hara
 */

#ifndef TONE_H_INCLUDED
#define TONE_H_INCLUDED

#include <stdbool.h>

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef VALUE ToneVALUE;

/**
 * @brief Corresponding to the Ruby class Tone.
 */
extern VALUE rb_cTone;

/**
 * @brief Initializes Tone.
 */
extern void Init_Tone(void);

/**
 * @brief Returns a new tone.
 * @param[in] red The red field of the tone.
 * @param[in] green The green field of the tone.
 * @param[in] blue The blue field of the tone.
 * @param[in] gray The gray field of the tone.
 * @return The generated tone.
 */
ToneVALUE rb_tone_new(double red, double green, double blue, double gray);

/**
 * @brief Returns a new tone.
 * @return The generated tone, with all fields initialized by 0.0.
 */
ToneVALUE rb_tone_new2(void);

/**
 * @brief Compares a tone to another tone.
 * @return true if they're equal, false if not.
 */
bool rb_tone_equal(ToneVALUE self, VALUE other);

/**
 * @brief Sets all fields.
 * @param[out] self The receiver of the method.
 * @param[in] newred The new red value to be set.
 * @param[in] newgreen The new green value to be set.
 * @param[in] newblue The new blue value to be set.
 * @param[in] newgray The new gray value to be set.
 */
void rb_tone_set(
    ToneVALUE self, double newred, double newgreen, double newblue,
    double newgray);

/**
 * @brief Copies all fields from <code>other</code>.
 * @param[out] self The receiver of the method.
 * @param[in] other The new tone to be set.
 */
void rb_tone_set2(ToneVALUE self, ToneVALUE other);

/**
 * @brief Returns red value.
 * @param[in] self The receiver of the method.
 * @return The red value of the tone.
 */
double rb_tone_red(ToneVALUE self);

/**
 * @brief Sets red field.
 * @param[out] self The receiver of the method.
 * @param[in] newred The new red value to be set.
 */
void rb_tone_set_red(ToneVALUE self, double newval);

/**
 * @brief Returns green value.
 * @param[in] self The receiver of the method.
 * @return The green value of the tone.
 */
double rb_tone_green(ToneVALUE self);

/**
 * @brief Sets green field.
 * @param[out] self The receiver of the method.
 * @param[in] newgreen The new green value to be set.
 */
void rb_tone_set_green(ToneVALUE self, double newval);

/**
 * @brief Returns blue value.
 * @param[in] self The receiver of the method.
 * @return The blue value of the tone.
 */
double rb_tone_blue(ToneVALUE self);

/**
 * @brief Sets blue field.
 * @param[out] self The receiver of the method.
 * @param[in] newblue The new blue value to be set.
 */
void rb_tone_set_blue(ToneVALUE self, double newval);

/**
 * @brief Returns gray value.
 * @param[in] self The receiver of the method.
 * @return The gray value of the tone.
 */
double rb_tone_gray(ToneVALUE self);

/**
 * @brief Sets gray field.
 * @param[out] self The receiver of the method.
 * @param[in] newgray The new gray value to be set.
 */
void rb_tone_set_gray(ToneVALUE self, double newval);

#ifdef __cplusplus
}
#endif

#endif // TONE_H_INCLUDED
