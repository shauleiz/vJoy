_LNG=$(LANGUAGE)


!IF $(KMDF_VERSION_MAJOR) >= 9

# Copy hidkmdf.sys to sub-directory of install 
$(INSTALL_DIR)\$(O)\$(TARGETNAME).sys: $(OBJ_PATH)\$O\$(TARGETNAME).sys
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	copy $(OBJ_PATH)\$O\$(TARGETNAME).sys  $@

!ELSE
$(INSTALL_DIR)\$(O)\$(TARGETNAME).sys: 
!ENDIF