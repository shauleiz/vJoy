_LNG=$(LANGUAGE)

# Prepare SVN based header file
MAKE_SVN=SubWCRev.exe .  svn_version.h.tmpl svn_version.h
svn_version.h all:
    DEL svn_version.h
    $(MAKE_SVN)
	
# Copy vJoyInstall.lib to sub-directory of LIB folder 
$(LIB_DIR)\$(O)\$(TARGETNAME).lib: $(OBJ_PATH)\$O\$(TARGETNAME).lib
	if not exist $(LIB_DIR)\$(O) mkdir $(LIB_DIR)\$(O)
	copy /y $(OBJ_PATH)\$O\$(TARGETNAME).lib  $@
	
# Copy vJoyInstall.dll to sub-directory of LIB folder 
$(LIB_DIR)\$(O)\$(TARGETNAME).dll: $(OBJ_PATH)\$O\$(TARGETNAME).dll
	if not exist $(LIB_DIR)\$(O) mkdir $(LIB_DIR)\$(O)
	copy /y $(OBJ_PATH)\$O\$(TARGETNAME).dll  $@
	
# Copy wrapper.h to inc 
$(INCLUDES)\wrapper.h: wrapper.h
	copy /y wrapper.h  $@
	
