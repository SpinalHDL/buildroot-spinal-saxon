################################################################################
#
# GPIO_INTERRUPT
#
################################################################################

GPIO_INTERRUPT_VERSION = 1.0
GPIO_INTERRUPT_SITE = $(BR2_EXTERNAL_SPINAL_SAXON_PATH)/package/gpio-interrupt/src
GPIO_INTERRUPT_SITE_METHOD = local

define GPIO_INTERRUPT_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define GPIO_INTERRUPT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/gpio-interrupt $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
