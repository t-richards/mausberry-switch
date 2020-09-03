# mausberry-switch

[![CircleCI](https://img.shields.io/circleci/build/github/t-richards/mausberry-switch?style=flat-square)](https://circleci.com/gh/t-richards/mausberry-switch)

This is a daemon for [Raspberry Pi][rpi] devices that monitors GPIO pins 23 and
24, waiting for a high signal from a [Mausberry Circuits switch][mausberry-circuits]
in order to poweroff the system safely. It is intended to replace the
[official setup script][mausberry-script].

## Installing

This software is only supported on the latest version of Raspberry Pi OS (currently based on Debian Buster).

Fetch and install [the latest release][releases] directly on your Pi:

```bash
# Download the package
wget https://github.com/t-richards/mausberry-switch/releases/download/0.8/mausberry-switch_0.8_armhf.deb

# Install the package
sudo dpkg -i mausberry-switch*.deb
sudo apt-get -f install
```

## Usage

The `mausberry-switch` systemd service will be automatically enabled and started when you install the package.

To stop or disable the service, the appropriate `systemctl` command should be used. For example:

```bash
# Stop the service temporarily
sudo systemctl stop mausberry-switch

# Disable the service from automatically starting at boot
sudo systemctl disable mausberry-switch
```

Configuration options (such as input/output pins, shutdown command/delay) are available in the primary configuration file, `/etc/mausberry-switch.conf`.

After changing this file, you must restart the service to pick up the new configuration.

```bash
sudo systemctl restart mausberry-switch
```

Please see the configuration file for documentation on each supported option.

## Why not just use the official script available from their website?

For reference, we're talking about this code:

```bash
while [ 1 = 1 ]; do
    cat /sys/class/gpio/gpio$GPIOpin1/value
    sleep 1
done
```

In my opinion, if there's a wrong way to monitor GPIO pins, it is a bash script.
I do have to give credit for strong adherence to the KISS principle -  21 lines
of bash is hard to beat.

However, on a Raspberry PI device, we're dealing with a processor that is a
close relative to a piece of tinfoil. Just think about all the
[poor, wasted clock cycles][wasted-clock] that occur while polling the GPIO
state repeatedly.

## Alright, what about one of those Python scripts?

With raw access to GPIO and the ability to wait for GPIO events without wasting
CPU time, Python is definitely a better way to go.

So let's say you have a Model B rev 2 board with 512MB of RAM. After you give
some to the GPU, you only **really** have 400MB or 500MB left. Each instance of
the Python interpreter takes up roughly 4MB, and I don't consider that to be a
small loss.

## Okay then. Prove that yours really is better!

There's some fine [documentation on Linux kernel gpio/sysfs][gpio-sysfs] that
tells us how the kernel can alert us when the value of a GPIO has changed. This
eliminates the need for constant sampling of the GPIO. Here's how it works:

1. Export the pins to userspace with `/sys/class/gpio/export` and set their direction with `/sys/class/gpio/gpioN/direction`.
2. Set the value of `/sys/class/gpio/gpioN/edge` to setup an interrupt-generating pin.
3. `poll(2)` on `/sys/class/gpio/gpioN/value` for the events POLLPRI and POLLERR
4. Wait for poll to return and you've got a winner!

In the context of Linux system calls, *poll* means "wait for events to occur",
as opposed to the traditional definition of polling, which is more along the
lines of "actively sampling".

Here's the output of `top` showing the CPU and RAM usage of this program:

      PID USER      PR  NI  VIRT  RES  SHR S  %CPU %MEM    TIME+  COMMAND
    22682 root      20   0  1504  284  228 S   0.0  0.1   0:00.00 mausberry-switch

# Roadmap

 - [ ] Replace legacy sysfs code with new chardev API / libgpiod (2021)

# License

The software is available as open source under the terms of the [MIT License][LICENSE].

[build-doc]: doc/building.md
[gpio-sysfs]: https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
[LICENSE]: LICENSE
[mausberry-circuits]: http://mausberrycircuits.com/
[mausberry-script]: http://files.mausberrycircuits.com/setup.sh
[releases]: https://github.com/t-richards/mausberry-switch/releases
[rpi]: http://www.raspberrypi.org/
[wasted-clock]: http://www.raspberrypi.org/phpBB3/viewtopic.php?t=63561
