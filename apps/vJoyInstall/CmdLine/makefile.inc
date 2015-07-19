_LNG=$(LANGUAGE)

####################################################################
# Use the correct  manifest file
!if "$(_BUILDARCH)" == "AMD64"
MANIFEST=vJoyInstallx64.exe.manifest
!else
MANIFEST=vJoyInstallx86.exe.manifest
!endif
####################################################################

# Mark file to "run as" administrator
AS_ADMIN=mt.exe -manifest "$(MANIFEST)" -outputresource:$(OBJ_PATH)\$O\$(TARGETNAME).exe


 
# Prepare SVN based header file
MAKE_SVN=SubWCRev.exe .  svn_version.h.tmpl svn_version.h

svn_version.h all:
    DEL svn_version.h
    $(MAKE_SVN)

# Copy installer to sub-directory of install 
$(INSTALL_DIR)\$(O)\$(TARGETNAME).exe: $(OBJ_PATH)\$O\$(TARGETNAME).exe
	$(AS_ADMIN)
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	signtool sign /a /v /s PrivateCertStore $(OBJ_PATH)\$O\$(TARGETNAME).exe
	copy $(OBJ_PATH)\$O\$(TARGETNAME).exe  $@



