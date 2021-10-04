XBE_TITLE = NevolutionX
INCDIR = $(CURDIR)/Includes
RESOURCEDIR = $(CURDIR)/Resources
SRCDIR = $(CURDIR)/Sources

SRCS += \
	$(CURDIR)/main.cpp \
	$(SRCDIR)/audioMenu.cpp \
	$(SRCDIR)/config.cpp \
	$(SRCDIR)/font.cpp \
	$(SRCDIR)/ftpConnection.cpp \
	$(SRCDIR)/ftpServer.cpp \
	$(SRCDIR)/infoLog.cpp \
	$(SRCDIR)/langMenu.cpp \
	$(SRCDIR)/logViewer.cpp \
	$(SRCDIR)/logViewerMenu.cpp \
	$(SRCDIR)/menu.cpp \
	$(SRCDIR)/networkManager.cpp \
	$(SRCDIR)/networking.cpp \
	$(SRCDIR)/renderer.cpp \
	$(SRCDIR)/settingsMenu.cpp \
	$(SRCDIR)/sntpClient.cpp \
	$(SRCDIR)/subAppRouter.cpp \
	$(SRCDIR)/subsystems.cpp \
	$(SRCDIR)/timeMenu.cpp \
	$(SRCDIR)/timing.cpp \
	$(SRCDIR)/videoMenu.cpp \
	$(SRCDIR)/wipeCache.cpp \
	$(SRCDIR)/xbeLauncher.cpp \
	$(SRCDIR)/xbeScanner.cpp \
	$(CURDIR)/nxdk-sdl-gpu/nxdkSDLGPU.cpp \
	$(CURDIR)/3rdparty/SDL_FontCache/SDL_FontCache.c

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
NXDK_DISABLE_AUTOMOUNT_D = y

GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -I$(SDL_GPU_DIR)/include -I$(PBGL_DIR)/include -Wall -Wextra -std=gnu++11 -DFC_USE_SDL_GPU
CFLAGS   += -I$(SDL_GPU_DIR)/include -std=gnu11 -DFC_USE_SDL_GPU

ifneq ($(DEBUG),y)
CFLAGS += -O2
CXXFLAGS += -O2
endif

CLEANRULES = clean-resources clean-gl

include $(NXDK_DIR)/Makefile

override PBGL_DIR := 3rdparty/pbgl
include 3rdparty/pbgl/Makefile

override SDL_GPU_DIR := 3rdparty/sdl-gpu
include nxdk-sdl-gpu/Makefile.inc

RESOURCES = \
	$(OUTPUT_DIR)/config.json \
	$(OUTPUT_DIR)/480.png \
	$(OUTPUT_DIR)/720.png

TARGET += $(RESOURCES)
$(GEN_XISO): $(RESOURCES)

$(OUTPUT_DIR)/config.json: $(CURDIR)/sampleconfig.json
	@mkdir -p $(OUTPUT_DIR)
	cp $(CURDIR)/sampleconfig.json $(OUTPUT_DIR)/config.json
$(OUTPUT_DIR)/%: $(RESOURCEDIR)/%
	$(VE)cp -r '$<' '$@'

.PHONY: clean-resources
clean-resources:
	$(VE)rm -rf $(OUTPUT_DIR)/NeXThemes

.PHONY: clean-gl
clean-gl: clean-sdl-gpu clean-pbgl
