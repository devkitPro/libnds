ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif
 
export TOPDIR	:=	$(CURDIR)
VERSION	:=	$(shell date +%Y%m%d)
.PHONEY: release debug clean all

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
	@tar --exclude=*CVS* -cvjf libnds-src-$(VERSION).tar.bz2 arm7 arm9 common include Makefile libnds_license.txt 

#-------------------------------------------------------------------------------
dist-bin: all
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* -cvjf libnds-$(VERSION).tar.bz2 include lib libnds_license.txt

dist: dist-bin dist-src

#-------------------------------------------------------------------------------
install: dist-bin
#-------------------------------------------------------------------------------
	bzip2 -cd libnds-$(VERSION).tar.bz2 | tar -xv -C $(DEVKITPRO)/libnds

