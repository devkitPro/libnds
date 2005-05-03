#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
-include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
BUILD		:= build

ARM9SOURCES :=	source/arm9 source/common
ARM7SOURCES :=	source/arm7 source/common

DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)

export PATH		:=	$(DEVKITARM)/bin:$(PATH)

export BASEDIR	:= $(CURDIR)
export LIBDIR	:= $(BASEDIR)/lib
export DEPENDS	:= $(BASEDIR)/deps
export INCDIR	:= $(BASEDIR)/include

ARM9CFILES		:=	$(foreach dir,$(ARM9SOURCES),$(notdir $(wildcard $(dir)/*.c)))
ARM9SFILES		:=	$(foreach dir,$(ARM9SOURCES),$(notdir $(wildcard $(dir)/*.s)))
ARM9BINFILES	:=	$(foreach dir,$(ARM9SOURCES),$(notdir $(wildcard $(dir)/*.bin)))

ARM7CFILES		:=	$(foreach dir,$(ARM7SOURCES),$(notdir $(wildcard $(dir)/*.c)))
ARM7SFILES		:=	$(foreach dir,$(ARM7SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export ARM9_VPATH	:=	$(foreach dir,$(ARM9SOURCES),$(BASEDIR)/$(dir))
export ARM7_VPATH	:=	$(foreach dir,$(ARM7SOURCES),$(BASEDIR)/$(dir))

export ARM9OBJS 	:= $(ARM9BINFILES:.bin=.o) $(ARM9CFILES:.c=.o) $(ARM9SFILES:.s=.o) 
export ARM7OBJS 	:= $(ARM7CFILES:.c=.o) $(ARM7SFILES:.s=.o)

export ARCH	:=	-mthumb -mthumb-interwork

export ARM9FLAGS	:=	$(ARCH) -mcpu=arm9 -mtune=arm9 -DARM9
export ARM7FLAGS	:=	$(ARCH) -mcpu=arm7tdmi -mtune=arm7tdmi -DARM7 

export BASEFLAGS	:=	-g -Wall -O2\
			 			-fomit-frame-pointer\
						-ffast-math \
						-I$(INCDIR)

.PHONEY:	all libs dist

#---------------------------------------------------------------------------------
all:	lib $(BUILD)/arm9 $(BUILD)/arm7 $(DEPENDS)
#---------------------------------------------------------------------------------
	@make -C $(BUILD)/arm9 -f $(BASEDIR)/Makefile.arm9
	@make -C $(BUILD)/arm7 -f $(BASEDIR)/Makefile.arm7
	
#---------------------------------------------------------------------------------
$(BUILD)/arm7: 
#---------------------------------------------------------------------------------
	mkdir -p $@
#---------------------------------------------------------------------------------
$(BUILD)/arm9: 
#---------------------------------------------------------------------------------
	mkdir -p $@
#---------------------------------------------------------------------------------
$(DEPENDS):
#---------------------------------------------------------------------------------
	mkdir -p $@/arm7
	mkdir -p $@/arm9
	
#---------------------------------------------------------------------------------
lib:
#---------------------------------------------------------------------------------
	mkdir -p lib

#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	rm -fr $(DEPENDS) $(BUILD) *.bz2
	
#---------------------------------------------------------------------------------
dist: all
#---------------------------------------------------------------------------------
	@tar --exclude=*CVS* -cvjf libnds-src-$(DATESTRING).tar.bz2 source include license.txt Makefile Makefile.arm9 Makefile.arm7 rules
	@tar --exclude=*CVS* -cvjf libnds-$(DATESTRING).tar.bz2 include lib license.txt

