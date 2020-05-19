XBE_TITLE = NevolutionX
INCDIR = $(CURDIR)/Includes
RESOURCEDIR = $(CURDIR)/Resources
SRCDIR = $(CURDIR)/Sources

include $(SRCDIR)/Makefile

SRCS += $(CURDIR)/main.cpp

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(INCDIR) -Wall -Wextra -std=gnu++11
CFLAGS   += -std=gnu11

new_all: copy_resources all

include $(NXDK_DIR)/Makefile

copy_resources:
	@mkdir -p $(OUTPUT_DIR)
	@cp $(RESOURCEDIR)/480.bmp $(OUTPUT_DIR)/480.bmp
	@cp $(RESOURCEDIR)/720.bmp $(OUTPUT_DIR)/720.bmp
	@cp $(RESOURCEDIR)/vegur.ttf $(OUTPUT_DIR)/vegur.ttf
