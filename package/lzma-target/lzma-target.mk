################################################################################
#
# lzma
#
################################################################################

LZMA_TARGET_VERSION = 4.32.7
LZMA_TARGET_SOURCE = lzma-$(LZMA_VERSION).tar.xz
LZMA_TARGET_SITE = http://tukaani.org/lzma
LZMA_TARGET_LICENSE = LGPL-2.1+ (lzmadec library, lzmainfo, LzmaDecode), GPL-2.0+ (lzma program, lzgrep and lzmore scripts), GPL-3.0+ (tests)
LZMA_TARGET_LICENSE_FILES = COPYING.GPLv2 COPYING.GPLv3 COPYING.LGPLv2.1
LZMA_TARGET_CONF_OPTS =

LZMA_TARGET__DEPENDENCIES = host-pkgconf

$(eval $(autotools-package))
