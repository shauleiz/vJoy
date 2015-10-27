// This file contains common AssemblyVersion data to be shared across all projects in this solution.
// Based on http://www.zachburlingame.com/2011/03/integrating-the-subversion-revision-into-the-version-automatically-with-net/
///////////////////////////////////////////////////////////////////////////////////////

using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
 
// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyDescription("C# Wrapper for vJoyInterface.dll")]
[assembly: AssemblyCompany("Shaul Eizikovich")]
[assembly: AssemblyProduct("vJoyInterfaceWrap")]
[assembly: AssemblyCopyright("Copyright Shaul Eizikovich ©  2015")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]
 
// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("30750eab-8093-4385-bee2-a438fcebc20e")]

#if DEBUG
[assembly: AssemblyConfiguration("Debug")]
[assembly: AssemblyTitle("vJoyInterfaceWrap  - C# Wrapper [Deb]")] 
#else
[assembly: AssemblyConfiguration("Release")]
[assembly: AssemblyTitle("vJoyInterfaceWrap  - C# Wrapper [Rel]")] 
#endif
 
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Revision
//      Build
[assembly: AssemblyInformationalVersionAttribute("0.2.1.6")]
[assembly: AssemblyVersion("0.2.1.6")]
[assembly: AssemblyFileVersion("2.1.6.999")]
//[assembly: AssemblyTitle("vJoyInterfaceWrap - Built 2015/10/15 12:24:44 from r813M")] // This is visible as the "File Description" on the Details tab of the Explorer pane
