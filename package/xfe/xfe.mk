################################################################################
#
# xfe
#
################################################################################

XFE_SITE = https://github.com/tindzk/Xfe.git
XFE_VERSION = e80ac122801778c696e3d2a6625025ae4f7a1e1f
XFE_SITE_METHOD = git
XFE_GIT_SUBMODULES = YES
XFE_AUTORECONF = NO
XFE_CONF_OPTS =


XFE_DEPENDENCIES = host-pkgconf



$(eval $(autotools-package))



