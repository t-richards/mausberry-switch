# mausberry-switch

This is a daemon for [Raspberry PI][rpi] devices that monitors GPIO pins 23 and
24, waiting for a low signal from a [Mausberry Circuits switch][mausberry-circuits]
in order to poweroff the system safely. It is intended to replace the
[official setup script][mausberry-script].

## Why not just use the official script available from  from their website?

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
close relative to a piece of tinfoil. Just think about all the [poor, wasted
clock cycles]() that occur while polling the GPIO state repeatedly.

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

## Alright, I'm convinced. How do I install this thing?

TODO(tom): Re-write this section

[rpi]: http://www.raspberrypi.org/
[mausberry-circuits]: http://mausberrycircuits.com/
[mausberry-script]: http://files.mausberrycircuits.com/setup.sh
[wasted-clock]: http://www.raspberrypi.org/phpBB3/viewtopic.php?t=63561
[gpio-sysfs]: https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
