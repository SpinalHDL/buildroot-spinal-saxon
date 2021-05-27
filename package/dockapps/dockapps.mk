################################################################################
#
# DOCKAPPS
#
################################################################################

DOCKAPPS_SITE = https://repo.or.cz/dockapps.git
DOCKAPPS_VERSION = wmmixer-1.9
DOCKAPPS_SITE_METHOD = git
DOCKAPPS_GIT_SUBMODULES = YES
DOCKAPPS_DEPENDENCIES = xlib_libX11 xlib_libXpm

define DOCKAPPS_BUILD_CMDS
    $(TARGET_CONFIGURE_OPTS) $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)/cputnik/src all
endef
define DOCKAPPS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/cputnik/src/cputnik $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))


