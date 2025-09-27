/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018 Intel Corporation
 * Copyright (c) 2024 TOKITA Hiroshi
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>
#include <zephyr/random/random.h>

#define STRIP_NODE DT_ALIAS(led_strip)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define DELAY_TIME K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY)

#define RGB(_r, _g, _b) {.r = (_r), .g = (_g), .b = (_b)}

static struct led_rgb hsb_to_rgb(uint32_t hsb)
{
	float r = 0, g = 0, b = 0;
	int hue = (hsb >> 16) & 0xFF;
	int sat = (hsb >> 8) & 0xFF;
	int bri = hsb & 0xFF;
	uint8_t i = hue / 60;
	float v = bri / ((float)100);
	float s = sat / ((float)100);
	float f = hue / ((float)360) * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (i % 6) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	}

	struct led_rgb rgb = {r : r * 255, g : g * 255, b : b * 255};

	return rgb;
}

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

int main(void)
{
	int rc;

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return 0;
	}

	LOG_INF("Displaying pattern on strip");
	while (1) {
		int hue;
		int sat;
		int bri;
		uint32_t hsb;
		for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
			int rando = sys_rand16_get() % 11;
			if (rando != 0) {
				hue = 270;
				sat = 100;
				bri = 4;
			} else {
				hue = 350;
				sat = 100;
				bri = 36;
			}
			hsb = (hue << 16) + (sat << 8) + bri;
			pixels[i] = hsb_to_rgb(hsb);
		};
		rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
		if (rc) {
			LOG_ERR("couldn't update strip: %d", rc);
		}

		k_msleep(342);
	};
	return 0;
};
