Summary: gcc compiled for the AVR platform with TinyOS patches
Name: avr-gcc
Version: PKG_VERSION
Release: PKG_RELEASE
Packager: Janos Sallai, Vanderbilt University <janos.sallai@vanderbilt.edu>
URL: ftp://ftp.gnu.org/pub/gnu/gcc/gcc-4.1.2/gcc-core-4.1.2.tar.bz2

License: GNU GPL
Group: Development/Tools

%description
gcc compiled for the AVR platform. 

%install
rm -rf $RPM_BUILD_ROOT
cp -a PKG_DIRECTORY $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr