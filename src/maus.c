/*
 * mausberry-switch
 *
 * GPIO monitoring daemon for use with Mausberry switches on the Raspberry Pi.
 */
#include "maus.h"

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSZ 64

int maus_gpio_export(gint pin) {
  gchar buffer[BUFSZ] = {0};
  gint format_len;
  int errno_sv;
  int fd;

  fd = g_open("/sys/class/gpio/export", O_WRONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open export for writing: %s\n",
              strerror(errno_sv));
    return -1;
  }

  format_len = g_snprintf(buffer, BUFSZ, "%d", pin);
  if (-1 == write(fd, buffer, format_len)) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to export pin %d: %s\n", pin, strerror(errno_sv));
    return -1;
  }

  fsync(fd);
  close(fd);

  return 0;
}

int maus_gpio_unexport(gint pin) {
  gchar buffer[BUFSZ] = {0};
  gint format_len;
  int errno_sv;
  int fd;

  fd = g_open("/sys/class/gpio/unexport", O_WRONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open unexport for writing: %s\n",
              strerror(errno_sv));
    return -1;
  }

  format_len = g_snprintf(buffer, BUFSZ, "%d", pin);
  if (-1 == write(fd, buffer, format_len)) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to unexport pin %d: %s\n", pin,
              strerror(errno_sv));
    return -1;
  }

  fsync(fd);
  close(fd);

  return 0;
}

int maus_gpio_direction_in(gint pin) {
  gchar path[BUFSZ] = {0};
  int errno_sv;
  int fd;

  g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/direction", pin);
  fd = g_open(path, O_WRONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open direction for writing: %s\n",
              strerror(errno_sv));
    return -1;
  }

  if (-1 == write(fd, DIRECTION_IN, sizeof(DIRECTION_IN))) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to set direction of pin %d to %s: %s\n", pin,
              DIRECTION_IN, strerror(errno_sv));
    return -1;
  }

  fsync(fd);
  close(fd);

  return 0;
}

int maus_gpio_direction_out(gint pin) {
  gchar path[BUFSZ] = {0};
  int errno_sv;
  int fd;

  g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/direction", pin);
  fd = g_open(path, O_WRONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open direction for writing: %s\n",
              strerror(errno_sv));
    return -1;
  }

  if (-1 == write(fd, DIRECTION_OUT, sizeof(DIRECTION_OUT))) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to set direction of pin %d to %s: %s\n", pin,
              DIRECTION_OUT, strerror(errno_sv));
    return -1;
  }

  fsync(fd);
  close(fd);

  return 0;
}

int maus_gpio_interrupt(gint pin) {
  char path[BUFSZ] = {0};
  int errno_sv;
  int fd;

  g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/edge", pin);
  fd = g_open(path, O_WRONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open edge %d for writing: %s\n", pin,
              strerror(errno_sv));
    return -1;
  }

  if (-1 == write(fd, WHEN_TO_RETURN, sizeof(WHEN_TO_RETURN))) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to configure pin %d as interrupt source: %s\n",
              pin, strerror(errno_sv));
    return -1;
  }

  fsync(fd);
  close(fd);

  return 0;
}

int maus_gpio_wait(gint pin) {
  int value = -1;
  char path[BUFSZ] = {0};
  char value_str[BUFSZ] = {0};
  int errno_sv;
  int fd;

  // open gpio file descriptor
  g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/value", pin);
  fd = g_open(path, O_RDONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open pin %d for reading: %s\n", pin,
              strerror(errno_sv));
    return -1;
  }

  // wait for kernel to notify us of changes
  struct pollfd pfds[1];
  pfds[0].fd = fd;
  pfds[0].events = POLLPRI | POLLERR;

  for (;;) {
    int rc = poll(pfds, 1, -1);
    if (rc < 0) {
      errno_sv = errno;
      if (errno_sv != EAGAIN && errno_sv != EINTR && errno_sv != EINVAL) {
        g_fprintf(stderr, "An error occurred while polling the switch: %s\n",
                  strerror(errno_sv));
        return -1;
      }
    }

    if (pfds[0].revents & POLLPRI) {
      lseek(fd, 0, SEEK_SET);
      // read the value
      if (-1 == read(fd, value_str, BUFSZ)) {
        g_fprintf(stderr, "Failed to read switch value\n");
        return -1;
      }
      value = atoi(value_str);
      if (value == VALUE_HIGH) {
        close(fd);
        return value;
      }
    }
  }
}

int maus_gpio_write(gint pin, gint value) {
  const char value_chr = '0' + value;

  char path[BUFSZ] = {0};
  int errno_sv;
  int fd;

  snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/value", pin);
  fd = g_open(path, O_WRONLY);
  if (-1 == fd) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to open pin %d for writing: %s\n", pin,
              strerror(errno_sv));
    return -1;
  }

  if (1 != write(fd, &value_chr, 1)) {
    errno_sv = errno;
    g_fprintf(stderr, "Failed to write value %d to pin %d: %s\n", value, pin,
              strerror(errno_sv));
    return -1;
  }

  fsync(fd);
  close(fd);

  return 0;
}

gboolean maus_load_config(MausPrivate *priv) {
  // Initialize config file struct
  GKeyFile *config_file = g_key_file_new();

  // Parse configuration file
  gboolean load_result = g_key_file_load_from_file(
      config_file, SYSCONFDIR "/mausberry-switch.conf", G_KEY_FILE_NONE, NULL);

  if (!load_result) {
    g_fprintf(stderr, "Failed to load configuration file: '%s'\n",
              SYSCONFDIR "/mausberry-switch.conf");
    return FALSE;
  }

  // Load config options into memory
  priv->shutdown_command =
      g_key_file_get_string(config_file, "Config", "ShutdownCommand", NULL);
  priv->shutdown_delay =
      g_key_file_get_integer(config_file, "Config", "Delay", NULL);
  priv->pin_in = g_key_file_get_integer(config_file, "Pins", "In", NULL);
  priv->pin_out = g_key_file_get_integer(config_file, "Pins", "Out", NULL);

  // Print configuration
  g_printf("== Mausberry Switch Configuration ==\n");
  g_printf("Shutdown command: '%s'\n", priv->shutdown_command);
  g_printf("Shutdown delay: %d\n", priv->shutdown_delay);
  g_printf("Pin IN: %d\n", priv->pin_in);
  g_printf("Pin OUT: %d\n", priv->pin_out);
  g_printf("== End Mausberry Switch Configuration ==\n");

  return TRUE;
}

gboolean maus_setup_gpio(MausPrivate *priv) {
  // Reset GPIO pins
  if (-1 == maus_gpio_unexport(priv->pin_out) ||
      -1 == maus_gpio_unexport(priv->pin_in))
    g_fprintf(stderr, "GPIO pins not reset.\n");

  // Enable GPIO pins
  if (-1 == maus_gpio_export(priv->pin_out) ||
      -1 == maus_gpio_export(priv->pin_in))
    g_fprintf(stderr, "GPIO pins not exported.\n");

  // Set GPIO directions
  if (-1 == maus_gpio_direction_in(priv->pin_out) ||
      -1 == maus_gpio_direction_out(priv->pin_in))
    g_fprintf(stderr, "GPIO directions not set.\n");

  // Initialize switch state
  if (-1 == maus_gpio_write(priv->pin_in, VALUE_HIGH))
    g_fprintf(stderr, "GPIO not initialized.\n");

  // Register 'out' pin as interrupt source
  if (-1 == maus_gpio_interrupt(priv->pin_out))
    g_fprintf(stderr, "GPIO not configured as interrupt.\n");

  return TRUE;
}
