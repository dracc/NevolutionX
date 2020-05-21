XBE_TITLE = NevolutionX
INCDIR = $(CURDIR)/Includes
RESOURCEDIR = $(CURDIR)/Resources

SRCS += $(CURDIR)/main.cpp $(INCDIR)/outputLine.cpp $(INCDIR)/xbeMenuItem.cpp \
	$(INCDIR)/menuItem.cpp $(INCDIR)/subsystems.cpp $(INCDIR)/findXBE.cpp \
	$(INCDIR)/renderer.cpp $(INCDIR)/font.cpp $(INCDIR)/networking.cpp \
	$(INCDIR)/ftpServer.cpp $(INCDIR)/ftpConnection.cpp \
	$(INCDIR)/menu.cpp \
	$(INCDIR)/config.cpp \
	$(CURDIR)/3rdparty/SDL_FontCache/SDL_FontCache.c

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -Wall -Wextra -std=gnu++11
CFLAGS   += -std=gnu11

new_all: copy_resources all

include $(NXDK_DIR)/Makefile

copy_resources:
	@mkdir -p $(OUTPUT_DIR)
	@cp $(RESOURCEDIR)/480.bmp $(OUTPUT_DIR)/480.bmp
	@cp $(RESOURCEDIR)/720.bmp $(OUTPUT_DIR)/720.bmp
	@cp $(RESOURCEDIR)/vegur.ttf $(OUTPUT_DIR)/vegur.ttf
	@cp $(CURDIR)/sampleconfig.json $(OUTPUT_DIR)/config.json
