# Create vJoyInterface.dll
$(OBJ_PATH)\$O\$(TARGETNAME).dll $(OBJ_PATH)\$O\$(TARGETNAME).lib:
	bldInterface.bat
	
# Copy vJoyInterface.h to inc 
$(INCLUDES)\$(TARGETNAME).h: $(TARGETNAME).h
	if not exist $(INCLUDES) mkdir $(INCLUDES)
	copy /y $(TARGETNAME).h  $@
	
# Copy vJoyInterface.lib to sub-directory of LIB folder 
$(LIB_DIR)\$(O)\$(TARGETNAME).lib: $(OBJ_PATH)\$O\$(TARGETNAME).lib
	if not exist $(LIB_DIR)\$(O) mkdir $(LIB_DIR)\$(O)
	copy /y $(OBJ_PATH)\$O\$(TARGETNAME).lib  $@
	
# Copy vJoyInterface.dll to sub-directory of LIB folder 
$(LIB_DIR)\$(O)\$(TARGETNAME).dll: $(OBJ_PATH)\$O\$(TARGETNAME).dll
	if not exist $(LIB_DIR)\$(O) mkdir $(LIB_DIR)\$(O)
	copy /y $(OBJ_PATH)\$O\$(TARGETNAME).dll  $@
	
