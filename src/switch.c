/*
 * mausberry-switch
 *
 * GPIO monitoring daemon for use with Mausberry switches on the Raspberry Pi.
 */

#include "config.h"

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSZ 64

typedef struct {
    gchar *shutdown_command;
    gint  shutdown_delay;
    gint  pin_in;
    gint  pin_out;
} MausPrivate;

MausPrivate *priv = NULL;

typedef enum {
    DIRECTION_IN  = 0,
    DIRECTION_OUT = 1
} MausGpioDirection;

typedef enum {
    VALUE_LOW  = 0,
    VALUE_HIGH = 1
} MausGpioValue;


int maus_gpio_export(gint pin)
{
    gchar buffer[BUFSZ] = {0};
    gint format_len;
    ssize_t write_len;
    int fd;

    fd = g_open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        g_printf("Failed to open export for writing\n");
        return -1;
    }

    format_len = g_snprintf(buffer, BUFSZ, "%d", pin);
    write_len = write(fd, buffer, format_len);
    close(fd);
    return 0;
}

int maus_gpio_unexport(gint pin)
{
    gchar buffer[BUFSZ] = {0};
    gint format_len;
    ssize_t write_len;
    int fd;

    fd = g_open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        g_printf("Failed to open unexport for writing\n");
        return -1;
    }

    format_len = g_snprintf(buffer, BUFSZ, "%d", pin);
    write_len = write(fd, buffer, format_len);
    close(fd);
    return 0;
}

int maus_gpio_direction(gint pin, gint dir)
{
    const char s_directions_str[]  = "in\0out";

    gchar path[BUFSZ] = {0};
    int fd;

    g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/direction", pin);
    fd = g_open(path, O_WRONLY);
    if (-1 == fd) {
        g_fprintf(stderr, "Failed to open gpio direction for writing\n");
        return -1;
    }

    if (-1 == write(fd, &s_directions_str[DIRECTION_IN == dir ? 0 : 3], DIRECTION_IN == dir ? 2 : 3)) {
        g_fprintf(stderr, "Failed to set gpio direction\n");
        return -1;
    }

    close(fd);
    return 0;
}

int maus_gpio_interrupt(gint pin)
{
    char path[BUFSZ] = {0};
    const char when_to_return[] = "both";
    int fd;

    g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/edge", pin);
    fd = g_open(path, O_WRONLY);
    if (-1 == fd) {
        g_fprintf(stderr, "Failed to open gpio edge for writing\n");
        return -1;
    }

    if (-1 == write(fd, when_to_return, 4)) {
        g_fprintf(stderr, "Failed to configure gpio as interrupt source\n");
        return -1;
    }

    close(fd);
    return 0;
}

int maus_gpio_wait(gint pin)
{
    int value = -1;
    char path[BUFSZ] = {0};
    char value_str[BUFSZ] = {0};
    int fd;

    //open gpio file descriptor
    g_snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/value", pin);
    fd = g_open(path, O_RDONLY);
    if (-1 == fd) {
        g_fprintf(stderr, "Failed to open gpio value for reading\n");
        return -1;
    }

    //wait for kernel to notify us of changes
    struct pollfd pfds[1];
    pfds[0].fd = fd;
    pfds[0].events = POLLPRI | POLLERR;

    for (;;) {

        int rc = poll(pfds, 1, -1);
        if(rc < 0) {
            int errsv = errno;
            if(errsv != EAGAIN && errsv != EINTR && errsv != EINVAL) {
                g_fprintf(stderr, "An error occurred while polling the switch\n");
                return -1;
            }
        }

        if(pfds[0].revents & POLLPRI) {
            lseek(fd, 0, SEEK_SET);
            //read the value
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

int maus_gpio_write(gint pin, gint value)
{
    const char s_values_str[] = "01";

    char path[BUFSZ] = {0};
    int fd;

    snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/value", pin);
    fd = g_open(path, O_WRONLY);
    if (-1 == fd) {
        g_fprintf(stderr, "Failed to open gpio value for writing\n");
        return -1;
    }

    if (1 != write(fd, &s_values_str[VALUE_LOW == value ? 0 : 1], 1)) {
        g_fprintf(stderr, "Failed to write value\n");
        return -1;
    }

    close(fd);
    return 0;
}

gboolean maus_reload_config()
{
    // Initialize config file struct
    GKeyFile *config_file = g_key_file_new();

    // Load configuration from file
    gboolean load_result = g_key_file_load_from_file(
        config_file,
        "/etc/mausberry-switch.conf",
        G_KEY_FILE_NONE,
        NULL
    );

    if (!load_result) {
        g_fprintf(stderr, "Failed to read config file!\n");
        return FALSE;
    }

    // Read keys
    priv->shutdown_command = g_key_file_get_string(config_file, "Config", "ShutdownCommand", NULL);
    priv->shutdown_delay = g_key_file_get_integer(config_file, "Config", "Delay", NULL);
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

gboolean maus_setup_gpio()
{
    // Reset GPIO pins
    if (-1 == maus_gpio_unexport(priv->pin_out) || -1 == maus_gpio_unexport(priv->pin_in))
        g_fprintf(stderr, "GPIO pins not reset.\n");

    // Enable GPIO pins
    if (-1 == maus_gpio_export(priv->pin_out) || -1 == maus_gpio_export(priv->pin_in))
        g_fprintf(stderr, "GPIO pins not exported.\n");

    // Set GPIO directions
    if (-1 == maus_gpio_direction(priv->pin_out, DIRECTION_IN) || -1 == maus_gpio_direction(priv->pin_in, DIRECTION_OUT))
        g_fprintf(stderr, "GPIO directions not set.\n");

    // Initialize switch state
    if (-1 == maus_gpio_write(priv->pin_in, VALUE_HIGH))
        g_fprintf(stderr, "GPIO not initialized.\n");

    // Register 'out' pin as interrupt source
    if (-1 == maus_gpio_interrupt(priv->pin_out))
        g_fprintf(stderr, "GPIO not configured as interrupt.\n");

    return TRUE;
}

void maus_handle_sighup(int sig)
{
    g_printf("Received SIGHUP, reloading configuration.\n");
    maus_reload_config();
    maus_setup_gpio();
}

void maus_handle_termint(int sig)
{
    g_fprintf(stderr, "Received SIGTERM or SIGINT, exiting.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int shutdown_success = 0;
    priv = g_new0(MausPrivate, 1);

    // Set up signal handlers
    signal(SIGHUP, maus_handle_sighup);
    signal(SIGINT, maus_handle_termint);
    signal(SIGTERM, maus_handle_termint);

    // Parse config
    maus_reload_config();

    // Set up pins
    maus_setup_gpio();

    // Wait for switch state to change
    int result = maus_gpio_wait(priv->pin_out);
    g_printf("Power switch pressed! (received a %d)!\n", result);

    // Disable GPIO pins
    if (-1 == maus_gpio_unexport(priv->pin_out) || -1 == maus_gpio_unexport(priv->pin_in))
        g_fprintf(stderr, "Could not unexport gpio pins before shutting down.\n");

    // Delay
    g_printf("Waiting %d seconds before shutting down.\n", priv->shutdown_delay);
    sleep(priv->shutdown_delay);

    // Shutdown
    g_printf("Shutting down.\n");
    shutdown_success = system(priv->shutdown_command);

    return shutdown_success;
}
