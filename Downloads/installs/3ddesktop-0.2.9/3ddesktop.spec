Summary:   3D OpenGL virtual desktop switcher
Name:      3ddesktop
Version:   0.2.9
Release:   1
License:   GPL
Group:     User Interface/Desktops
Source:    http://prdownloads.sourceforge.net/desk3d/3ddesktop-%{version}.tar.gz
URL:       http://desk3d.sourceforge.net/
Requires:  imlib2 >= 1.0.2
BuildRoot: /var/tmp/%{name}-%{version}-root

%define _prefix     /usr
%define _sysconfdir /etc
%define _datadir    %{_prefix}/share

%description 
3d Destkop is an OpenGL program for switching virtual desktops in a
seamless 3-dimensional manner. The current desktop is mapped into a 3D
space where you may choose other screens. When activated the current
desktop appears to zoom out into the 3D view.  Several different
visualization modes are available.

%prep
%setup -q

%build
CFLAGS="$MYCFLAGS" ./configure $MYARCH_FLAGS \
	--prefix=%{_prefix} \
	--sysconfdir=%{_sysconfdir} \
	--datadir=%{_datadir}

make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(644, root, root, 755)
%doc README COPYING ChangeLog TODO AUTHORS
%attr(755,root,root) %{_prefix}/bin/*
%{_prefix}/share/3ddesktop/digits.bmp
%{_sysconfdir}/3ddesktop.conf
%{_prefix}/man/man1/3ddesk.1.gz
%{_prefix}/man/man1/3ddeskd.1.gz

%changelog
* Tue Aug 13 2002 wiget@pld-linux.org
- move 3ddesktop.spec to 3ddesktop.spec.in and make VERSION substitute automagicaly
- small changes for easy future development
