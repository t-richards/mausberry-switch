#include "maus.h"

#include <glib-unix.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int shutdown_success = 0;
  MausPrivate *priv = g_new0(MausPrivate, 1);

  // Parse config
  if (!maus_reload_config(priv)) {
    g_fprintf(stderr, "Critical: exiting.\n");
    exit(EXIT_FAILURE);
  }

  // Set up pins
  maus_setup_gpio(priv);

  // Wait for switch state to change
  int result = maus_gpio_wait(priv->pin_out);
  g_printf("Power switch pressed! (received a %d)!\n", result);

  // Delay
  if (priv->shutdown_delay > 0) {
    g_printf("Waiting %d seconds before shutting down.\n",
             priv->shutdown_delay);
    sleep(priv->shutdown_delay);
  }

  // Shutdown
  g_printf("Shutting down.\n");
  shutdown_success = system(priv->shutdown_command);

  return shutdown_success;
}
