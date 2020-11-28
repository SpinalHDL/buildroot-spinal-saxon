include $(sort $(wildcard $(BR2_EXTERNAL_SPINAL_SAXON_PATH)/package/*/*.mk))

ifndef SAXON_CPU_COUNT
$(error SAXON_CPU_COUNT is not set)
endif

HOST_CFLAGS+=-DSAXON_CPU_COUNT=$(SAXON_CPU_COUNT)
HOST_CFLAGS+=-I$(BR2_EXTERNAL_SPINAL_SAXON_PATH)

MPG123_CPU=generic_nofpu

#XLIB_LIBXSHMFENCE_CFLAGS+="-DSYS_futex=SYS_futex_time64"
TARGET_CFLAGS+="-DSYS_futex=SYS_futex_time64"
