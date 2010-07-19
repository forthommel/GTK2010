#
# set up dirs
#
TOP      := $(shell pwd)
bin      := bin
obj      := obj
lib      := lib
CONTROLd := control
VMEd     := VME
%GUI     := GUI


#
# set up dependencies
#
vpath %.h      $(CONTROLd):$(VMEd)
##:$(VMEd)/CAENVMELib:$(VMEd)/CAENVMETool
vpath %.code   $(CONTROLd)
vpath %.cpp    $(CONTROLd):$(VMEd)
vpath %.o      $(obj)

#------------------------------------------------------------------------------

VMELIBS       = -lCAENVME

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --new --libs)
ROOTGLIBS     = $(shell root-config --new --glibs)

# Linux with egcs
CXX           = g++
CXXFLAGS      = -O2 -Wall -fPIC -DLINUX -w
#-ggdb
LD            = g++
LDFLAGS       = -g 
SOFLAGS       = -shared

#CXXFLAGS     += $(ROOTCFLAGS) 
CXXFLAGS     += -I$(TOP)/$(CONTROLd) 
CXXFLAGS     += -I$(TOP)/$(VMEd)
CXXFLAGS     += -I$(TOP)/$(VMEd)/CAENVMELib
CXXFLAGS     += -I$(TOP)/$(VMEd)/CAENVMETool 
CXXFLAGS     += -I/usr/include/freetype2
CXXFLAGS     += -I$(TOP)
#LIBS          = $(ROOTLIBS) $ 
#GLIBS         = $(ROOTGLIBS)

CC	          = g++
CCFLAGS       = -O3 -Wall -fPIC -DLINUX -w
AR            = ar
RANLIB        = ranlib

                                       
#------------------------------------------------------------------------------

CONTROL		= $(bin)/Control
CONTROLo	= $(obj)/Control.o \
		        $(obj)/bridgeV1718.o \
		        $(obj)/scaler1151N.o \
		        $(obj)/tdcV1x90.o
#\
#                  $(obj)/cfdV812.o \
#                  $(obj)/scaler1151N.o \
#		  $(obj)/tdcV1290.o


all :  $(CONTROL) 

$(CONTROL) : $(CONTROLo)
	$(LD) $(LDFLAGS) $^ $(VMELIBS) $(GLIBS) -o $(CONTROL)
	@echo "$(CONTROL) done"
	@echo 	

clean:
	@rm -f $(obj)/*
	@rm -f $(bin)/*
	@rm -f $(ROOTWd)/rootwindowDict.*
	@rm -f core
	@rm -f *~

obj/%.o : %.cpp 
	@echo "Compiling" $< 
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Done"  
	@echo  

obj/%.o : %.c
	@echo "Compiling" $<
	$(CC) $(CCFLAGS) -c $< -o $@
	@echo "Done"  
	@echo  
