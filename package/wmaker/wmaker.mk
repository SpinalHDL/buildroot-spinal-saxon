################################################################################
#
# wmaker
#
################################################################################

WMAKER_SITE = https://github.com/window-maker/wmaker.git
WMAKER_VERSION = wmaker-0.95.9
WMAKER_SITE_METHOD = git
WMAKER_GIT_SUBMODULES = YES
WMAKER_AUTORECONF = YES
WMAKER_CONF_OPTS =


WMAKER_DEPENDENCIES = xlib_libX11 xlib_libXft xlib_libXmu host-pkgconf

define WMAKER_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef

WMAKER_PRE_CONFIGURE_HOOKS += WMAKER_RUN_AUTOGEN
HOST_WMAKER_PRE_CONFIGURE_HOOKS += WMAKER_RUN_AUTOGEN


$(eval $(autotools-package))



