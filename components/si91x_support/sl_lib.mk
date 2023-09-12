ifndef HOST_OS
$(error Must define HOST_OS to "windows", "linux", or "macos")
endif

$(PROJECT_OUTPUT)/generated/sl_platform.c: $(sl_lib_DIRECTORY)/sl_platform.c.template
	components/yinja/yinja-$(HOST_OS) -t $(sl_lib_DIRECTORY)/sl_platform.c.template -j $(PROJECT_OUTPUT)/yakka_summary.json > $@

$(PROJECT_OUTPUT)/generated/sl_component_catalog.h: $(sl_lib_DIRECTORY)/component_catalog/sl_component_catalog.h.jinja
	components/yinja/yinja-$(HOST_OS) -t $(sl_lib_DIRECTORY)/component_catalog/sl_component_catalog.h.jinja -j $(PROJECT_OUTPUT)/yakka_summary.json > $@
