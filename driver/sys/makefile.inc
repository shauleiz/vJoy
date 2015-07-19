_LNG=$(LANGUAGE)
_INX=.
!ifndef KMDF_VERSION_MINOR
KMDF_VERSION_MINOR=5
!endif

# Timestamp INF file
STAMP=stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR)

# Create Cabinet files
!if "$(_BUILDARCH)" == "AMD64"
CAT=INF2CAT /driver:$(INSTALL_DIR)\$(O)  /os:Vista_X64,7_x64
!else
CAT=INF2CAT /driver:$(INSTALL_DIR)\$(O)  /os:XP_X86,Vista_x86
!endif

# Sign Cabinet files
SIGN=signtool sign /a /v /s PrivateCertStore  $(INSTALL_DIR)\$(O)\*.cat

####################################################################
# Creating a KMDF Version String (e.g. 01009)
!   if $(KMDF_VERSION_MAJOR) < 10
KMDF_MAJOR_VERSION_STRING=0$(KMDF_VERSION_MAJOR)
!   else if $(KMDF_VERSION_MAJOR) < 10
KMDF_MAJOR_VERSION_STRING=$(KMDF_VERSION_MAJOR)
!   else
!       error using incorrect KMDF major version number $(KMDF_VERSION_MAJOR).
!   endif

!   if $(KMDF_VERSION_MINOR) < 10
KMDF_MINOR_VERSION_STRING=00$(KMDF_VERSION_MINOR)
!   else if $(KMDF_VERSION_MINOR) < 100
KMDF_MINOR_VERSION_STRING=0$(KMDF_VERSION_MINOR)
!   else if $(KMDF_VERSION_MINOR) < 1000
KMDF_MINOR_VERSION_STRING=$(KMDF_VERSION_MINOR)
!   else
!       error using incorrect KMDF minor version number $(KMDF_VERSION_MINOR).
!   endif
KMDF_VERSION_STRING=$(KMDF_MAJOR_VERSION_STRING)$(KMDF_MINOR_VERSION_STRING)
####################################################################

####################################################################
# Copy the correct SHIM driver
!if "$(_BUILDARCH)" == "AMD64"
SHIM_SRC=$(INSTALL_DIR)\$(SHIM_X64)
!else
SHIM_SRC=$(INSTALL_DIR)\$(SHIM_X86)
!endif
####################################################################


$(OBJ_PATH)\$O\$(INF_NAME).inf: $(_INX)\$(INF_NAME).inx
    copy $(_INX)\$(@B).inx $@
   $(STAMP)
 
# Copy vJoy.inf to sub-directory of install 
$(INSTALL_DIR)\$(O)\$(TARGETNAME).inf: $(OBJ_PATH)\$O\$(TARGETNAME).inf
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	copy $(OBJ_PATH)\$O\$(TARGETNAME).inf  $@

# Copy vJoy.sys to sub-directory of install 
$(INSTALL_DIR)\$(O)\$(TARGETNAME).sys: $(OBJ_PATH)\$O\$(TARGETNAME).sys
	signtool sign /a /v /s PrivateCertStore $(OBJ_PATH)\$O\$(TARGETNAME).sys
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	copy $(OBJ_PATH)\$O\$(TARGETNAME).sys  $@
	

# Copy correct SHIM (hidkmdf.sys) to sub-directory of install 
$(INSTALL_DIR)\$(O)\$(SHIM_NAME).sys: 
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	signtool sign /a /v /s PrivateCertStore $(SHIM_DIR)\$(O)\$(SHIM_NAME).sys
	copy $(SHIM_DIR)\$(O)\$(SHIM_NAME).sys  $@
	

$(INSTALL_DIR)\$(O)\$(WUDFUPDATE_BASE).dll:
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	copy $(BASEDIR)\redist\wdf\$(_BUILDARCH)\$(WUDFUPDATE_BASE)$(KMDF_VERSION_STRING).dll  \
	$(INSTALL_DIR)\$(O)\$(WUDFUPDATE_BASE)$(KMDF_VERSION_STRING).dll

$(INSTALL_DIR)\$(O)\$(WDFCOINST_BASE).dll:
	if not exist $(INSTALL_DIR)\$(O) mkdir $(INSTALL_DIR)\$(O)
	copy $(BASEDIR)\redist\wdf\$(_BUILDARCH)\$(WDFCOINST_BASE)$(KMDF_VERSION_STRING).dll  \
	$(INSTALL_DIR)\$(O)\$(WDFCOINST_BASE)$(KMDF_VERSION_STRING).dll

all:
	$(CAT)
	$(SIGN)
