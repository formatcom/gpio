#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

//REF: https://github.com/torvalds/linux/blob/master/include/uapi/linux/gpio.h
//REF: https://github.com/torvalds/linux/blob/master/drivers/gpio/gpiolib.c

int main(void)
{
	struct gpiochip_info cinfo;

	int fd = open("/dev/gpiochip0", O_RDONLY);

	if (fd == -1) return 1;

	ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);

	printf("GPIO chip: %s, \"%s\", %u GPIO lines\n",
						cinfo.name, cinfo.label, cinfo.lines);

	for (int i = 0; i<cinfo.lines; ++i)
	{
		struct gpioline_info linfo;
		linfo.line_offset = i;

		if (i % 2 == 0) printf("\n");

		ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &linfo);
		printf("line %04d: %s %s %s %s         ", linfo.line_offset,
						linfo.flags & GPIOLINE_FLAG_IS_OUT     ? "OUTPUT" : "INPUT ",
						linfo.flags & GPIOLINE_FLAG_ACTIVE_LOW ? "ACTIVE LOW " : "ACTIVE HIGH",
						linfo.name, linfo.consumer);

	}

	printf("\n\n");

	struct gpiohandle_request req;
	struct gpiohandle_data data;

	req.flags |= GPIOHANDLE_REQUEST_OUTPUT;
	req.lines = 0;
	req.lineoffsets[0] = 0;
	req.default_values[0] = 0;

	ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);

	ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);

	printf("line 0000: is %s\n", data.values[0] ? "HIGH" : "LOW");

	close(fd);
	return 0;
}
