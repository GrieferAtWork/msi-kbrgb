/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <https://unlicense.org>
 */
#include <hidapi/hidapi.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


static char const *appname = "kbrgb";
#define err(...)                          \
	do {                                  \
		fprintf(stderr, "%s: ", appname); \
		fprintf(stderr, __VA_ARGS__);     \
		fflush(stderr);                   \
		exit(1);                          \
	} while (0)


#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*(arr)))

/* Derived from `https://github.com/bparker06/msi-keyboard/blob/master/keyboard.h' */
/* BEGIN: Magic */
static struct {
	char name[8];
} const kb_modes[] = {
	{ "" },
	{ "normal" },  /* 1 */
	{ "gaming" },  /* 2 */
	{ "breathe" }, /* 3 */
	{ "demo" },    /* 4 */
	{ "wave" },    /* 5 */
};

#define KEYBOARD_REGION_LEFT   1
#define KEYBOARD_REGION_MIDDLE 2
#define KEYBOARD_REGION_RIGHT  3
static struct {
	char name[7];
	uint8_t what;
} const kb_regions[] = {
	{ "left", KEYBOARD_REGION_LEFT },
	{ "middle", KEYBOARD_REGION_MIDDLE },
	{ "right", KEYBOARD_REGION_RIGHT },
	{ "l", KEYBOARD_REGION_LEFT },
	{ "m", KEYBOARD_REGION_MIDDLE },
	{ "r", KEYBOARD_REGION_RIGHT },
};

#define KEYBOARD_COLOR_OFF    0
#define KEYBOARD_COLOR_RED    1
#define KEYBOARD_COLOR_ORANGE 2
#define KEYBOARD_COLOR_YELLOW 3
#define KEYBOARD_COLOR_GREEN  4
#define KEYBOARD_COLOR_SKY    5
#define KEYBOARD_COLOR_BLUE   6
#define KEYBOARD_COLOR_PURPLE 7
#define KEYBOARD_COLOR_WHITE  8
static struct {
	char name[7];
	uint8_t what;
} const kb_colors[] = {
	{ "off", KEYBOARD_COLOR_OFF },
	{ "red", KEYBOARD_COLOR_RED },
	{ "orange", KEYBOARD_COLOR_ORANGE },
	{ "yellow", KEYBOARD_COLOR_YELLOW },
	{ "green", KEYBOARD_COLOR_GREEN },
	{ "sky", KEYBOARD_COLOR_SKY },
	{ "blue", KEYBOARD_COLOR_BLUE },
	{ "purple", KEYBOARD_COLOR_PURPLE },
	{ "white", KEYBOARD_COLOR_WHITE },
};

#define KEYBOARD_INTENSITY_HIGH   0
#define KEYBOARD_INTENSITY_MEDIUM 1
#define KEYBOARD_INTENSITY_LOW    2
#define KEYBOARD_INTENSITY_LIGHT  3
static struct {
	char name[7];
	uint8_t what;
} const kb_intensity[] = {
	{ "high", KEYBOARD_INTENSITY_HIGH },
	{ "medium", KEYBOARD_INTENSITY_MEDIUM },
	{ "low", KEYBOARD_INTENSITY_LOW },
	{ "light", KEYBOARD_INTENSITY_LIGHT },
	{ "h", KEYBOARD_INTENSITY_HIGH },
	{ "m", KEYBOARD_INTENSITY_MEDIUM },
	{ "l", KEYBOARD_INTENSITY_LOW },
};
/* END: Magic */


static void help_mode(void) {
	unsigned int i;
	for (i = 1; i < ARRAY_LENGTH(kb_modes); ++i) {
		if (i != 1)
			printf(", ");
		printf("%s", kb_modes[i].name);
	}
	putchar('\n');
}

static uint8_t kbstr_mode(char const *name) {
	char *endp;
	uint8_t result;
	unsigned int i;
	if (!*name)
		goto badname;
	if (strcmp(name, "?") == 0) {
		help_mode();
		exit(0);
	}
	for (i = 1; i < ARRAY_LENGTH(kb_modes); ++i) {
		if (strcmp(kb_modes[i].name, name) == 0)
			return i;
	}
	result = (uint8_t)strtoul(name, &endp, 0);
	if (!*endp)
		return result;
badname:
	err("Invalid mode name: '%s'", name);
}

static void help_region(void) {
	unsigned int i;
	for (i = 0; i < ARRAY_LENGTH(kb_regions); ++i) {
		if (i != 0)
			printf(", ");
		printf("%s", kb_regions[i].name);
	}
	putchar('\n');
}

static uint8_t kbstr_region(char const *name) {
	char *endp;
	uint8_t result;
	unsigned int i;
	if (strcmp(name, "?") == 0) {
		help_region();
		exit(0);
	}
	for (i = 0; i < ARRAY_LENGTH(kb_regions); ++i) {
		if (strcmp(kb_regions[i].name, name) == 0)
			return kb_regions[i].what;
	}
	if (!*name)
		goto badname;
	result = (uint8_t)strtoul(name, &endp, 0);
	if (!*endp)
		return result;
badname:
	err("Invalid region name: '%s'", name);
}

static void help_color(void) {
	unsigned int i;
	for (i = 0; i < ARRAY_LENGTH(kb_colors); ++i) {
		if (i != 0)
			printf(", ");
		printf("%s", kb_colors[i].name);
	}
	putchar('\n');
}

static uint8_t kbstr_color(char const *name) {
	char *endp;
	uint8_t result;
	unsigned int i;
	if (strcmp(name, "?") == 0) {
		help_color();
		exit(0);
	}
	for (i = 0; i < ARRAY_LENGTH(kb_colors); ++i) {
		if (strcmp(kb_colors[i].name, name) == 0)
			return kb_colors[i].what;
	}
	if (!*name)
		goto badname;
	result = (uint8_t)strtoul(name, &endp, 0);
	if (!*endp)
		return result;
badname:
	err("Invalid color name: '%s'", name);
}

static void help_intensity(void) {
	unsigned int i;
	for (i = 0; i < ARRAY_LENGTH(kb_intensity); ++i) {
		if (i != 0)
			printf(", ");
		printf("%s", kb_intensity[i].name);
	}
	putchar('\n');
}

static uint8_t kbstr_intensity(char const *name) {
	char *endp;
	uint8_t result;
	unsigned int i;
	if (strcmp(name, "?") == 0) {
		help_intensity();
		exit(0);
	}
	for (i = 1; i < ARRAY_LENGTH(kb_intensity); ++i) {
		if (strcmp(kb_intensity[i].name, name) == 0)
			return kb_intensity[i].what;
	}
	if (!*name)
		goto badname;
	result = (uint8_t)strtoul(name, &endp, 0);
	if (!*endp)
		return result;
badname:
	err("Invalid intensity name: '%s'", name);
}

static uint8_t kbstr_rgb(char const *name) {
	char *endp;
	uint8_t result;
	if (!*name)
		goto badname;
	result = (uint8_t)strtoul(name, &endp, 0);
	if (!*endp)
		return result;
badname:
	err("Invalid rgb name: '%s'", name);
}






/* Derived from: https://github.com/bparker06/msi-keyboard/blob/master/keyboard.cpp */
static hid_device *keyboard;

/* BEGIN: Magic */
static void kb_setmode(uint8_t mode) {
	uint8_t buf[8] = { 1, 2, 65, mode, 0, 0, 0, 236 };
	hid_send_feature_report(keyboard, buf, sizeof(buf));
}

static void kb_setcolorpreset(uint8_t region, uint8_t color, uint8_t intensity) {
	uint8_t buf[8] = { 1, 2, 66, region, color, intensity, 0, 236 };
	hid_send_feature_report(keyboard, buf, sizeof(buf));
}

static void kb_setcolor(uint8_t region, uint8_t r, uint8_t g, uint8_t b) {
	unsigned char buf[8] = { 1, 2, 64, region, r, g, b, 236 };
	hid_send_feature_report(keyboard, buf, sizeof(buf));
}
/* END: Magic */


#define FOREACH_SPLIT_STRING(iter, str, splitOn) \
	for (iter       = strtok(str, splitOn);      \
	     iter; iter = strtok(NULL, splitOn))


int main(int argc, char *argv[]) {
	int error;
	error = hid_init();
	if (error != 0)
		err("hid_init() failed");
	keyboard = hid_open(0x1770, 0xff00, NULL);
	if (keyboard == NULL)
		err("Failed to open keyboard");
	if (argc) {
		--argc;
		appname = *argv++;
	}
	while (argc) {
		char cmd;
		char *arg;
		char *params[4];
		char lmr_str[6];
		char *region;
		unsigned int paramc;
		arg = *argv++;
		if (strcmp(arg, "--help") == 0) {
			printf("usage: %s COMMAND [COMMAND] [...]\n"
			       "COMMAND:\n"
			       "\tm:MODE\n"
			       "\tp:REGIONS:COLOR:INTENSITY\n"
			       "\tc:REGIONS:R:G:B\n"
			       "MODE:      ",
			       appname);
			help_mode();
			printf("REGIONS:   all | REGION[,...]");
			printf("REGION:    ");
			help_region();
			printf("COLOR:     ");
			help_color();
			printf("INTENSITY: ");
			help_intensity();
			printf("R/G/B:     <Integer value>\n"
			       "example: '%s m:normal p:all:green:h' (all-green keyboard)",
			       appname);
			exit(0);
		}
		--argc;
		cmd = *arg++;
		if (!cmd || *arg++ != ':') {
badarg:
			err("Invalid argument: '%s'", argv[-1]);
		}
		/* Split parameters */
		for (paramc = 0;;) {
			if (paramc >= ARRAY_LENGTH(params))
				goto badarg;
			params[paramc] = arg;
			++paramc;
			arg = strchr(arg, ':');
			if (!arg)
				break;
			*arg++ = '\0';
		}
		switch (cmd) {

		case 'm': /* Mode */
			if (paramc != 1)
				goto badarg;
			kb_setmode(kbstr_mode(params[0]));
			break;

		case 'p': /* colorPreset */
			if (paramc != 3)
				goto badarg;
			if (strcmp(params[0], "all") == 0)
				params[0] = strcpy(lmr_str, "l,m,r");
			FOREACH_SPLIT_STRING(region, params[0], ",") {
				kb_setcolorpreset(kbstr_region(region),
				                  kbstr_color(params[1]),
				                  kbstr_intensity(params[2]));
			}
			break;

		case 'c': /* Color */
			if (paramc != 4)
				goto badarg;
			if (strcmp(params[0], "all") == 0)
				params[0] = strcpy(lmr_str, "l,m,r");
			FOREACH_SPLIT_STRING(region, params[0], ",") {
				kb_setcolor(kbstr_region(region),
				            kbstr_rgb(params[1]),
				            kbstr_rgb(params[2]),
				            kbstr_rgb(params[3]));
			}
			break;

		default:
			goto badarg;
		}
	}
	hid_close(keyboard);
	hid_exit();
	return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
