# - Define GNU standard installation directories
# Provides install directory variables as defined for GNU software:
#  http://www.gnu.org/prep/standards/html_node/Directory-Variables.html
# Inclusion of this module defines the following variables:
#  CMAKE_INSTALL_<dir>      - destination for files of a given type
#  CMAKE_INSTALL_FULL_<dir> - corresponding absolute path
# where <dir> is one of:
#  BINDIR           - user executables (bin)
#  SBINDIR          - system admin executables (sbin)
#  LIBEXECDIR       - program executables (libexec)
#  SYSCONFDIR       - read-only single-machine data (etc)
#  SHAREDSTATEDIR   - modifiable architecture-independent data (com)
#  LOCALSTATEDIR    - modifiable single-machine data (var)
#  LIBDIR           - object code libraries (lib or lib64)
#  INCLUDEDIR       - C header files (include)
#  OLDINCLUDEDIR    - C header files for non-gcc (/usr/include)
#  DATAROOTDIR      - read-only architecture-independent data root (share)
#  DATADIR          - read-only architecture-independent data (DATAROOTDIR)
#  INFODIR          - info documentation (DATAROOTDIR/info)
#  LOCALEDIR        - locale-dependent data (DATAROOTDIR/locale)
#  MANDIR           - man documentation (DATAROOTDIR/man)
#  DOCDIR           - documentation root (DATAROOTDIR/doc/PROJECT_NAME)
# Each CMAKE_INSTALL_<dir> value may be passed to the DESTINATION options of
# install() commands for the corresponding file type.  If the includer does
# not define a value the above-shown default will be used and the value will
# appear in the cache for editing by the user.
# Each CMAKE_INSTALL_FULL_<dir> value contains an absolute path constructed
# from the corresponding destination by prepending (if necessary) the value
# of CMAKE_INSTALL_PREFIX.

#=============================================================================
# Copyright 2011 Nikita Krupen'ko <krnekit@gmail.com>
# Copyright 2011 Kitware, Inc.
#
# Copyright (c) 2011, Nikita Krupen'ko
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

#  - Redistributions of source code must retain the above copyright notice,
#  this list of conditions and the following disclaimer.  

#  - Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation
#  and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# Installation directories
#
if(NOT DEFINED CMAKE_INSTALL_BINDIR)
  set(CMAKE_INSTALL_BINDIR "bin" CACHE PATH "user executables (bin)")
endif()

if(NOT DEFINED CMAKE_INSTALL_SBINDIR)
  set(CMAKE_INSTALL_SBINDIR "sbin" CACHE PATH "system admin executables (sbin)")
endif()

if(NOT DEFINED CMAKE_INSTALL_LIBEXECDIR)
  set(CMAKE_INSTALL_LIBEXECDIR "libexec" CACHE PATH "program executables (libexec)")
endif()

if(NOT DEFINED CMAKE_INSTALL_SYSCONFDIR)
  set(CMAKE_INSTALL_SYSCONFDIR "etc" CACHE PATH "read-only single-machine data (etc)")
endif()

if(NOT DEFINED CMAKE_INSTALL_SHAREDSTATEDIR)
  set(CMAKE_INSTALL_SHAREDSTATEDIR "com" CACHE PATH "modifiable architecture-independent data (com)")
endif()

if(NOT DEFINED CMAKE_INSTALL_LOCALSTATEDIR)
  set(CMAKE_INSTALL_LOCALSTATEDIR "var" CACHE PATH "modifiable single-machine data (var)")
endif()

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(_LIBDIR_DEFAULT "lib")
  # Override this default 'lib' with 'lib64' iff:
  #  - we are on Linux system but NOT cross-compiling
  #  - we are NOT on debian
  #  - we are on a 64 bits system
  # reason is: amd64 ABI: http://www.x86-64.org/documentation/abi.pdf
  # Note that the future of multi-arch handling may be even
  # more complicated than that: http://wiki.debian.org/Multiarch
  if(CMAKE_SYSTEM_NAME MATCHES "Linux"
      AND NOT CMAKE_CROSSCOMPILING
      AND NOT EXISTS "/etc/debian_version")
    if(NOT DEFINED CMAKE_SIZEOF_VOID_P)
      message(AUTHOR_WARNING
        "Unable to determine default CMAKE_INSTALL_LIBDIR directory because no target architecture is known. "
        "Please enable at least one language before including GNUInstallDirs.")
    else()
      if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        set(_LIBDIR_DEFAULT "lib64")
      endif()
    endif()
  endif()
  set(CMAKE_INSTALL_LIBDIR "${_LIBDIR_DEFAULT}" CACHE PATH "object code libraries (${_LIBDIR_DEFAULT})")
endif()

if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "C header files (include)")
endif()

if(NOT DEFINED CMAKE_INSTALL_OLDINCLUDEDIR)
  set(CMAKE_INSTALL_OLDINCLUDEDIR "/usr/include" CACHE PATH "C header files for non-gcc (/usr/include)")
endif()

if(NOT DEFINED CMAKE_INSTALL_DATAROOTDIR)
  set(CMAKE_INSTALL_DATAROOTDIR "share" CACHE PATH "read-only architecture-independent data root (share)")
endif()

#-----------------------------------------------------------------------------
# Values whose defaults are relative to DATAROOTDIR.  Store empty values in
# the cache and store the defaults in local variables if the cache values are
# not set explicitly.  This auto-updates the defaults as DATAROOTDIR changes.

if(NOT CMAKE_INSTALL_DATADIR)
  set(CMAKE_INSTALL_DATADIR "" CACHE PATH "read-only architecture-independent data (DATAROOTDIR)")
  set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}")
endif()

if(NOT CMAKE_INSTALL_INFODIR)
  set(CMAKE_INSTALL_INFODIR "" CACHE PATH "info documentation (DATAROOTDIR/info)")
  set(CMAKE_INSTALL_INFODIR "${CMAKE_INSTALL_DATAROOTDIR}/info")
endif()

if(NOT CMAKE_INSTALL_LOCALEDIR)
  set(CMAKE_INSTALL_LOCALEDIR "" CACHE PATH "locale-dependent data (DATAROOTDIR/locale)")
  set(CMAKE_INSTALL_LOCALEDIR "${CMAKE_INSTALL_DATAROOTDIR}/locale")
endif()

if(NOT CMAKE_INSTALL_MANDIR)
  set(CMAKE_INSTALL_MANDIR "" CACHE PATH "man documentation (DATAROOTDIR/man)")
  set(CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_DATAROOTDIR}/man")
endif()

if(NOT CMAKE_INSTALL_DOCDIR)
  set(CMAKE_INSTALL_DOCDIR "" CACHE PATH "documentation root (DATAROOTDIR/doc/PROJECT_NAME)")
  set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME}")
endif()

#-----------------------------------------------------------------------------

mark_as_advanced(
  CMAKE_INSTALL_BINDIR
  CMAKE_INSTALL_SBINDIR
  CMAKE_INSTALL_LIBEXECDIR
  CMAKE_INSTALL_SYSCONFDIR
  CMAKE_INSTALL_SHAREDSTATEDIR
  CMAKE_INSTALL_LOCALSTATEDIR
  CMAKE_INSTALL_LIBDIR
  CMAKE_INSTALL_INCLUDEDIR
  CMAKE_INSTALL_OLDINCLUDEDIR
  CMAKE_INSTALL_DATAROOTDIR
  CMAKE_INSTALL_DATADIR
  CMAKE_INSTALL_INFODIR
  CMAKE_INSTALL_LOCALEDIR
  CMAKE_INSTALL_MANDIR
  CMAKE_INSTALL_DOCDIR
  )

# Result directories
#
foreach(dir
    BINDIR
    SBINDIR
    LIBEXECDIR
    SYSCONFDIR
    SHAREDSTATEDIR
    LOCALSTATEDIR
    LIBDIR
    INCLUDEDIR
    OLDINCLUDEDIR
    DATAROOTDIR
    DATADIR
    INFODIR
    LOCALEDIR
    MANDIR
    DOCDIR
    )
  if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_${dir}})
    set(CMAKE_INSTALL_FULL_${dir} "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_${dir}}")
  else()
    set(CMAKE_INSTALL_FULL_${dir} "${CMAKE_INSTALL_${dir}}")
  endif()
endforeach()