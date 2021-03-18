################################################################################
#
# mandelbrot
#
################################################################################

MANDELBROT_VERSION = 1.0
MANDELBROT_SITE = $(BR2_EXTERNAL_SPINAL_SAXON_PATH)/package/mandelbrot/src
MANDELBROT_SITE_METHOD = local

define MANDELBROT_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define MANDELBROT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/mandelbrot $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
