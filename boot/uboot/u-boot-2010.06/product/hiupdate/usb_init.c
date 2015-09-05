static int usb_stor_init(void)
{
	int ret = -1;

try_again:
	if (usb_stop() < 0) {
		debug("usb_stop failed\n");
		return ret;
	}

	if (usb_init_debug() < 0)
		debug("usb_init_debug failed\n");

	wait_ms(1000);
	ret = usb_init();
	if (ret == -3)
		goto try_again;

	if (ret < 0) {
		debug("usb_init failed!\n");
		return ret;
	}

	/*
	 * check whether a storage device is attached (assume that it's
	 * a USB memory stick, since nothing else should be attached).
	 */
	ret = usb_stor_scan(0);
	if (-1 == ret)
		debug("No USB device found. Not initialized!\n");

	return ret;
}

static void usb_stor_exit(void)
{
	usb_stop();
}

