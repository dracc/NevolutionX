DEBUG = y
XBE_TITLE = NevolutionX
SRCS += $(wildcard $(CURDIR)/*.c)
NXDK_DIR = $(CURDIR)/../nxdk
NXDK_SDL = y
include $(NXDK_DIR)/Makefile
