$(PROJECT_OUTPUT)/generated/sl_device_init_clocks.c: 
	components/yinja/yinja-$(HOST_OS) -t $(sl_device_DIRECTORY)/sl_device_init_clocks.c.jinja -j $(PROJECT_OUTPUT)/yakka_summary.json > $@
