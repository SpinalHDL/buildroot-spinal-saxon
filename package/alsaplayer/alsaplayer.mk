################################################################################
#
# wmaker
#
################################################################################

ALSAPLAYER_SITE = https://github.com/alsaplayer/alsaplayer.git
ALSAPLAYER_VERSION = 65967c212961ef13c24a06d28ba57c8a4da11de2
ALSAPLAYER_SITE_METHOD = git
ALSAPLAYER_GIT_SUBMODULES = YES
ALSAPLAYER_AUTORECONF = NO
ALSAPLAYER_CONF_OPTS = --enable-oggvorbis=no -enable-oss=no


ALSAPLAYER_DEPENDENCIES = dbus-glib libmad alsa-lib host-pkgconf

define ALSAPLAYER_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef

ALSAPLAYER_PRE_CONFIGURE_HOOKS += ALSAPLAYER_RUN_AUTOGEN

$(eval $(autotools-package))



