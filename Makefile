ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif
 
export TOPDIR	:=	$(CURDIR)
VERSION	:=	$(shell date +%Y%m%d)
.PHONY: release debug clean all docs

all: release debug 

#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=release
	$(MAKE) -C arm7 BUILD=release
 
#-------------------------------------------------------------------------------
debug: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=debug
	$(MAKE) -C arm7 BUILD=debug

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib
 
#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C arm9 clean
	@$(MAKE) -C arm7 clean

#-------------------------------------------------------------------------------
dist-src: clean
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* -cjf libnds-src-$(VERSION).tar.bz2 arm7 arm9 source include icon.bmp Makefile libnds_license.txt 

#-------------------------------------------------------------------------------
dist-bin: all
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* -cjf libnds-$(VERSION).tar.bz2 include lib icon.bmp libnds_license.txt

dist: dist-bin dist-src

#-------------------------------------------------------------------------------
install: dist-bin
#-------------------------------------------------------------------------------
	mkdir -p $(DEVKITPRO)/libnds
	bzip2 -cd libnds-$(VERSION).tar.bz2 | tar -x -C $(DEVKITPRO)/libnds

#---------------------------------------------------------------------------------
docs:
#---------------------------------------------------------------------------------
	doxygen libnds.dox
	cat warn.log


