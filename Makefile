XBE_TITLE = NevolutionX
INCDIR = $(CURDIR)/Includes
RESOURCEDIR = $(CURDIR)/Resources

SRCS += $(CURDIR)/main.cpp $(INCDIR)/outputLine.cpp \
	$(INCDIR)/subsystems.cpp $(INCDIR)/findXBE.cpp $(INCDIR)/findThemes.cpp \
	$(INCDIR)/renderer.cpp $(INCDIR)/font.cpp $(INCDIR)/networking.cpp \
	$(INCDIR)/networkManager.cpp \
	$(INCDIR)/ftpServer.cpp $(INCDIR)/ftpConnection.cpp \
	$(INCDIR)/menu.cpp $(INCDIR)/langMenu.cpp $(INCDIR)/timeMenu.cpp \
	$(INCDIR)/settingsMenu.cpp $(INCDIR)/audioMenu.cpp $(INCDIR)/videoMenu.cpp \
	$(INCDIR)/config.cpp \
	$(INCDIR)/wipeCache.cpp \
	$(CURDIR)/3rdparty/SDL_FontCache/SDL_FontCache.c

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
NXDK_DISABLE_AUTOMOUNT_D = y

GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -Wall -Wextra -std=gnu++11
CFLAGS   += -std=gnu11

new_all: copy_resources all

include $(NXDK_DIR)/Makefile

copy_resources: $(OUTPUT_DIR)/config.json
	@cp $(RESOURCEDIR)/480.png $(RESOURCEDIR)/720.png $(RESOURCEDIR)/vegur.ttf $(OUTPUT_DIR)

$(OUTPUT_DIR)/config.json: $(CURDIR)/sampleconfig.json
	@mkdir -p $(OUTPUT_DIR)
	cp $(CURDIR)/sampleconfig.json $(OUTPUT_DIR)/config.json
