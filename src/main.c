#include "maus.h"

#include <glib-unix.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  MausPrivate *priv = g_new0(MausPrivate, 1);

  // Parse and print config
  maus_load_config(priv);

  // Set up pins
  maus_setup_gpio(priv);

  // Wait for switch state to change
  int result = maus_gpio_wait(priv->pin_out);
  if (result == -1) {
    g_fprintf(stderr, "Failed to wait on power switch. Exiting.\n");
    return EXIT_FAILURE;
  }
  g_printf("Power switch pressed. (received a %d)\n", result);

  // Optional shutdown delay
  if (priv->shutdown_delay > 0) {
    g_printf("Waiting %d seconds before shutting down.\n",
             priv->shutdown_delay);
    sleep(priv->shutdown_delay);
  }

  // Shutdown
  g_printf("Shutting down: %s\n", priv->shutdown_command);
  result = system(priv->shutdown_command);
  if (result == -1) {
    int errno_sv = errno;
    g_fprintf(stderr, "Error executing shutdown command: %s\n",
              strerror(errno_sv));
  }

  return EXIT_SUCCESS;
}
