/**
 * @file Color.h
 * @brief It defines Color class.
 * @author Masaki Hara
 */

#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <stdbool.h>

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef VALUE ColorVALUE;

/**
 * @brief Corresponding to the Ruby class Color.
 */
extern VALUE rb_cColor;

/**
 * @brief Initializes Color.
 */
extern void Init_Color(void);

/**
 * @brief Returns a new color.
 * @param[in] red The red field of the color.
 * @param[in] green The green field of the color.
 * @param[in] blue The blue field of the color.
 * @param[in] alpha The alpha field of the color.
 * @return The generated color.
 */
ColorVALUE rb_color_new(double red, double green, double blue, double alpha);

/**
 * @brief Returns a new color.
 * @return The generated color, with all fields initialized by 0.0.
 */
ColorVALUE rb_color_new2(void);

/**
 * @brief Compares a color to another color.
 * @return true if they're equal, false if not.
 */
bool rb_color_equal(ColorVALUE self, VALUE other);

/**
 * @brief Sets all fields.
 * @param[out] self The receiver of the method.
 * @param[in] newred The new red value to be set.
 * @param[in] newgreen The new green value to be set.
 * @param[in] newblue The new blue value to be set.
 * @param[in] newalpha The new alpha value to be set.
 */
void rb_color_set(
    ColorVALUE self, double newred, double newgreen, double newblue,
    double newalpha);

/**
 * @brief Copies all fields from <code>other</code>.
 * @param[out] self The receiver of the method.
 * @param[in] other The new color to be set.
 */
void rb_color_set2(ColorVALUE self, ColorVALUE other);

/**
 * @brief Returns red value.
 * @param[in] self The receiver of the method.
 * @return The red value of the color.
 */
double rb_color_red(ColorVALUE self);

/**
 * @brief Sets red field.
 * @param[out] self The receiver of the method.
 * @param[in] newred The new red value to be set.
 */
void rb_color_set_red(ColorVALUE self, double newval);

/**
 * @brief Returns green value.
 * @param[in] self The receiver of the method.
 * @return The green value of the color.
 */
double rb_color_green(ColorVALUE self);

/**
 * @brief Sets green field.
 * @param[out] self The receiver of the method.
 * @param[in] newgreen The new green value to be set.
 */
void rb_color_set_green(ColorVALUE self, double newval);

/**
 * @brief Returns blue value.
 * @param[in] self The receiver of the method.
 * @return The blue value of the color.
 */
double rb_color_blue(ColorVALUE self);

/**
 * @brief Sets blue field.
 * @param[out] self The receiver of the method.
 * @param[in] newblue The new blue value to be set.
 */
void rb_color_set_blue(ColorVALUE self, double newval);

/**
 * @brief Returns alpha value.
 * @param[in] self The receiver of the method.
 * @return The alpha value of the color.
 */
double rb_color_alpha(ColorVALUE self);

/**
 * @brief Sets alpha field.
 * @param[out] self The receiver of the method.
 * @param[in] newalpha The new alpha value to be set.
 */
void rb_color_set_alpha(ColorVALUE self, double newval);

#ifdef __cplusplus
}
#endif

#endif // COLOR_H_INCLUDED
