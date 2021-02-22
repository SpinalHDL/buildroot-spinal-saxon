################################################################################
#
# berkeley-float
#
################################################################################

#BERKELEY_FLOAT_VERSION = 1.0
#BERKELEY_FLOAT_SITE = /media/data/open/SaxonSoc/testFloatRiscv
#BERKELEY_FLOAT_SITE_METHOD = local

BERKELEY_FLOAT_SITE = https://github.com/Dolu1990/berkeley-float.git
BERKELEY_FLOAT_VERSION = main
BERKELEY_FLOAT_SITE_METHOD = git
BERKELEY_FLOAT_GIT_SUBMODULES = YES

define BERKELEY_FLOAT_BUILD_CMDS
	SPECIALIZE_TYPE=RISCV $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)/berkeley-softfloat-3/build/Linux-RISCV
	SPECIALIZE_TYPE=RISCV $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)/berkeley-testfloat-3/build/Linux-RISCV
endef

define BERKELEY_FLOAT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/berkeley-testfloat-3/build/Linux-RISCV/testfloat $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))


