################################################################################
#
# wmaker
#
################################################################################

WMAKER_SITE = https://github.com/window-maker/wmaker.git
WMAKER_VERSION = wmaker-0.95.9
WMAKER_SITE_METHOD = git
WMAKER_GIT_SUBMODULES = YES
WMAKER_INSTALL_STAGING_OPTS	= install
WMAKER_INSTALL_TARGET_OPTS	= install
WMAKER_CONF_OPTS =  --with-incs-from="" \
                    --with-libs-from=""  \
                    --oldincludedir=$(TARGET_DIR)/usr/include \
                    --includedir=$(TARGET_DIR)/usr/include \
	                --x-includes=$(STAGING_DIR)/usr/include \
	                --x-libraries=$(STAGING_DIR)/usr/lib \
	                --libdir=$(TARGET_DIR)/usr/lib \
                    --bindir=$(TARGET_DIR)/bin \
                    --sbindir=$(TARGET_DIR)/sbin \
                    --libexecdir=$(TARGET_DIR)/libexec \
                    --sysconfdir=$(TARGET_DIR)/etc \
                    --sharedstatedir=$(TARGETG_DIR)/com \
                    --localstatedir=$(TARGET_DIR)/var      \
                    --datarootdir=$(TARGET_DIR)/share \
                    --datadir=$(TARGET_DIR) \
                    --infodir=$(TARGET_DIR)/info \
                    --localedir=$(TARGET_DIR)/locale \
                    --mandir=$(TARGET_DIR)/man \
                    --docdir=$(TARGET_DIR)/doc/WindowMaker \
                    --htmldir=$(TARGET_DIR) \
                    --dvidir=$(TARGET_DIR) \
                    --pdfdir=$(TARGET_DIR) \
                    --psdir=$(TARGET_DIR)

WMAKER_DEPENDENCIES = xlib_libX11 xlib_libXft xlib_libXmu host-pkgconf

define WMAKER_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef

WMAKER_PRE_CONFIGURE_HOOKS += WMAKER_RUN_AUTOGEN
HOST_WMAKER_PRE_CONFIGURE_HOOKS += WMAKER_RUN_AUTOGEN


$(eval $(autotools-package))



