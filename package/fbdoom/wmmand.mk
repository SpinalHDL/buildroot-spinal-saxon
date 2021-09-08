################################################################################
#
# FBDOOM
#
################################################################################

FBDOOM_VERSION = 1.3.2
FBDOOM_SITE = https://github.com/Dolu1990/fbDOOM.git
FBDOOM_VERSION = 476a0cef4a3068015f85993bc916fca38bc2d970
FBDOOM_SITE_METHOD = git
FBDOOM_DEPENDENCIES = sdl


define FBDOOM_BUILD_CMDS
    $(TARGET_CONFIGURE_OPTS) $(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" LD="$(TARGET_LD)" INC="$(STAGING_DIR)" LIB="$(STAGING_DIR)" TARGET_DIR="$(TARGET_DIR)" -C $(@D)/fbdoom all
endef
define FBDOOM_INSTALL_TARGET_CMDS
    #$(TARGET_CONFIGURE_OPTS) $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" INC="$(STAGING_DIR)" LIB="$(STAGING_DIR)" TARGET_DIR="$(TARGET_DIR)" INSTALL="$(INSTALL)" -C $(@D)/wmMand install
	$(INSTALL) -D -m 0755 $(@D)/fbdoom/fbdoom $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))


