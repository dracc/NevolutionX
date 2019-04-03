DEBUG = y
XBE_TITLE = NevolutionX
SRCS += $(wildcard $(CURDIR)/*.c)
NXDK_DIR = $(CURDIR)/../nxdk
NXDK_SDL = y
GEN_XISO = ${XBE_TITLE}.iso
include $(NXDK_DIR)/Makefile
