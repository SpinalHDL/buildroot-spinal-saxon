################################################################################
#
# WMMAND
#
################################################################################

WMMAND_VERSION = 1.3.2
WMMAND_SITE = https://sourceforge.net/projects/wmmand/files/wmMand/wmMand-$(WMMAND_VERSION)
WMMAND_SOURCE = wmMand-$(WMMAND_VERSION).tar.bz2
WMMAND_DEPENDENCIES = xlib_libX11 xlib_libXpm


define WMMAND_BUILD_CMDS
    $(TARGET_CONFIGURE_OPTS) $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" INC="$(STAGING_DIR)" LIB="$(STAGING_DIR)" TARGET_DIR="$(TARGET_DIR)" -C $(@D)/wmMand all
endef
define WMMAND_INSTALL_TARGET_CMDS
    $(TARGET_CONFIGURE_OPTS) $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" INC="$(STAGING_DIR)" LIB="$(STAGING_DIR)" TARGET_DIR="$(TARGET_DIR)" INSTALL="$(INSTALL)" -C $(@D)/wmMand install
	#$(INSTALL) -D -m 0755 $(@D)/cputnik/src/cputnik $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))


