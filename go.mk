
# go.mk

#ifndef PRJ_OSLB
PRJ_OSLB :=	$(CFG_STDLIBS) $(PRJ_OSLB)
#endif
ifdef PRJ_ADDF
	GO_ADD := $(foreach f,$(PRJ_ADDF),$(CFG_LIBROOT)/$(f))
endif

ifdef PRJ_EXSY
	GO_ADD := $(GO_ADD) -Wl,--exclude-symbols$(foreach s,$(PRJ_EXSY),,$(s))
endif

ifeq ($(BUILD),vs)

GO_LIBS := $(GO_LIBS) $(foreach lib,$(PRJ_LIBS), $(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))
GO_LIBS := $(GO_LIBS) $(PRJ_OSLB)
	
	ifneq ($(PRJ_LIBP),)
		GO_LIBPATHS	:= $(GO_LIBPATHS) /LIBPATH:$(CFG_LIBROOT)
		GO_LIBS	 	:= $(GO_LIBS) $(foreach lib,$(PRJ_LIBP),-l$(lib))
	endif
	GO_LIBPATHS := /LIBPATH:$(CFG_BINROOT)
	ifneq ($(CFG_BINROOT),$(CFG_OUTROOT))
		GO_LIBPATHS := $(GO_LIBPATHS) /LIBPATH:$(CFG_OUTROOT)
	endif
	GO_EXPORTS 	:= $(foreach exp,$(PRJ_EXPORTS), $(CFG_FLAG_EXPORT)$(exp))
else
	GO_LIBS	 	:= $(GO_LIBS) $(foreach lib,$(PRJ_LIBS),-l$(lib)$(CFG_DPOSTFIX))
	GO_LIBS	 	:= $(GO_LIBS) $(foreach lib,$(PRJ_OSLB), -l$(lib))
	ifneq ($(PRJ_LIBP),)
		GO_LIBPATHS	:= $(GO_LIBPATHS) -L$(CFG_LIBROOT)
		GO_LIBS	 	:= $(GO_LIBS) $(foreach lib,$(PRJ_LIBP),-l$(lib))
	endif
	GO_LIBPATHS := $(GO_LIBPATHS) -L$(CFG_BINROOT)
	ifneq ($(CFG_BINROOT),$(CFG_OUTROOT))
		GO_LIBPATHS := $(GO_LIBPATHS) -L$(CFG_OUTROOT)
	endif
endif

ifeq ($(PRJ_TYPE),lib)
ifeq ($(BUILD),vs)	  
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(BLD_DEPENDS_TOTAL)
	- $(CFG_DEL) $(subst /,\,$@)
	$(CFG_AR) $(CFG_AFLAGS) $(CFG_AR_OUT)$@ $(BLD_OBJECTS_TOTAL) $(GO_ADDF)
else
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL)
	- $(CFG_DEL) $@
	$(CFG_AR) $(CFG_AFLAGS) $(CFG_AR_OUT)$@ $(BLD_OBJECTS_TOTAL) $(GO_ADDF)
endif
else
ifeq ($(PRJ_TYPE),dll)

GO_DEPENDS 	:= $(GO_DEPENDS) $(foreach lib,$(PRJ_LIBS),$(CFG_BINROOT)/$(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))

ifeq ($(BUILD),vs)	  
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS) $(BLD_DEPENDS_TOTAL)	
	- $(CFG_DEL) $(subst /,\,$@)
	$(CFG_LD) $(CFG_LFLAGS) $(GO_EXPORTS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(GO_ADD)
else
ifneq ($(PLATFORM),windows)
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(CFG_STDLIB) $(GO_ADD)
else
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(CFG_STDLIB) $(GO_ADD)
endif
endif

else

GO_DEPENDS 	:= $(foreach lib,$(PRJ_LIBS),$(CFG_BINROOT)/$(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))

#$(GO_DEPENDS):
#	$(MAKE) -C $(PRJ_LIBROOT)
	
ifeq ($(BUILD),vs)	
  
$(BLD_PATH_EXE): $(CFG_RES_OBJ) $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS) $(BLD_DEPENDS_TOTAL)
	- $(CFG_DEL) $(subst /,\,$@)
	$(CFG_LD) $(CFG_LFLAGS) $(GO_EXPORTS) $(BLD_OBJECTS_TOTAL) $(CFG_RES_OBJ) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(GO_ADD)
	
else

$(BLD_PATH_EXE): $(CFG_RES_OBJ) $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(PRJ_EXTL) $(BLD_OBJECTS_TOTAL) $(CFG_RES_OBJ) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(CFG_STDLIB) $(GO_ADD)
	
endif	

endif

endif

all: $(BLD_ALL) $(BLD_PATH_EXE) 
rebuild: $(BLD_REBUILD) $(BLD_PATH_EXE)
setup: $(BLD_SETUP)
clean: $(BLD_CLEAN)
