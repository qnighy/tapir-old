/**
 * @file Rect.h
 * @brief It defines Rect class.
 * @author Masaki Hara
 */

#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED

#include <stdbool.h>

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Corresponding to the Ruby class Rect.
 */
extern VALUE rb_cRect;

/**
 * @brief Initializes Rect.
 */
extern void Init_Rect(void);

/**
 * @brief Returns a new rectangle.
 * @param[in] x The x field of the color.
 * @param[in] y The y field of the color.
 * @param[in] width The width field of the color.
 * @param[in] height The height field of the color.
 * @return The generated rectangle.
 */
VALUE rb_rect_new(int x, int y, int width, int height);

/**
 * @brief Returns a new rectnagle.
 * @return The generated rectangle, with all fields initialized by 0.
 */
VALUE rb_rect_new2(void);

/**
 * @brief Compares a rectangle to another rectangle.
 * @return true if they're equal, false if not.
 */
bool rb_rect_equal(VALUE self, VALUE other);

/**
 * @brief Sets all fields.
 * @param[out] self The receiver of the method.
 * @param[in] newx The new x value to be set.
 * @param[in] newy The new y value to be set.
 * @param[in] newwidth The new width value to be set.
 * @param[in] newheight The new height value to be set.
 */
void rb_rect_set(VALUE self, int newx, int newy, int newwidth, int newheight);

/**
 * @brief Copies all fields from <code>other</code>.
 * @param[out] self The receiver of the method.
 * @param[in] other The new rectangle to be set.
 */
void rb_rect_set2(VALUE self, VALUE other);

/**
 * @brief Returns x value.
 * @param[in] self The receiver of the method.
 * @return The x value of the rectangle.
 */
int rb_rect_x(VALUE self);

/**
 * @brief Sets x field.
 * @param[out] self The receiver of the method.
 * @param[in] newval The new x value to be set.
 */
void rb_rect_set_x(VALUE self, int newx);

/**
 * @brief Returns y value.
 * @param[in] self The receiver of the method.
 * @return The y value of the rectangle.
 */
int rb_rect_y(VALUE self);

/**
 * @brief Sets y field.
 * @param[out] self The receiver of the method.
 * @param[in] newval The new y value to be set.
 */
void rb_rect_set_y(VALUE self, int newy);

/**
 * @brief Returns width value.
 * @param[in] self The receiver of the method.
 * @return The width value of the rectangle.
 */
int rb_rect_width(VALUE self);

/**
 * @brief Sets width field.
 * @param[out] self The receiver of the method.
 * @param[in] newval The new width value to be set.
 */
void rb_rect_set_width(VALUE self, int newwidth);

/**
 * @brief Returns height value.
 * @param[in] self The receiver of the method.
 * @return The height value of the rectangle.
 */
int rb_rect_height(VALUE self);

/**
 * @brief Sets height field.
 * @param[out] self The receiver of the method.
 * @param[in] newval The new height value to be set.
 */
void rb_rect_set_height(VALUE self, int newheight);

#ifdef __cplusplus
}
#endif

#endif // RECT_H_INCLUDED
