#ifndef __MAUS_H
#define __MAUS_H

#include "config.h"

#include <glib.h>

typedef struct {
  gchar *shutdown_command;
  gint shutdown_delay;
  gint pin_in;
  gint pin_out;
} MausPrivate;

#define DIRECTION_IN "in"
#define DIRECTION_OUT "out"
#define WHEN_TO_RETURN "both"

typedef enum { VALUE_LOW = 0, VALUE_HIGH = 1 } MausGpioValue;

int maus_gpio_export(gint pin);
int maus_gpio_unexport(gint pin);
int maus_gpio_direction_in(gint pin);
int maus_gpio_direction_out(gint pin);
int maus_gpio_interrupt(gint pin);
int maus_gpio_wait(gint pin);
int maus_gpio_write(gint pin, gint value);
gboolean maus_load_config(MausPrivate *priv);
gboolean maus_setup_gpio(MausPrivate *priv);

#endif
