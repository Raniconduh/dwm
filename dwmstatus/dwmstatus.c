/*
 * Copy me if you can.
 * by 20h
 */

#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <X11/Xlib.h>

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}


char *
mktimes(char *fmt)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL)
		return smprintf("");

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		return smprintf("");
	}

	return smprintf("%s", buf);
}


float
getcputemp() {
	FILE * fp = fopen("/sys/class/hwmon/hwmon2/temp1_input", "r");
	char inp[20];
	fgets(inp, 20, fp);
	fclose(fp);
	int iinp = atoi(inp);
	return (float)iinp / 1000;
}


int
main(void)
{
	char *status;
	char *time;
	float cputemp;

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(1)) {
		time = mktimes("%a %b %d %I:%M %p");
		cputemp = getcputemp();

		status = smprintf(" CPU: %.1fC | %s", cputemp, time);

		// draw status to screen
		XStoreName(dpy, DefaultRootWindow(dpy), status);
		XSync(dpy, False);

		free(time);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}

