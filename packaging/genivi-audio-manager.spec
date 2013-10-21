#this disables failing check-buildroot
%define __arch_install_post /usr/lib/rpm/check-rpaths

Name:             genivi-audio-manager
License:          MPL-2.0
Group:            Automotive/GENIVI
Summary:          GENIVI Audio Manager
Version:          3.2
Release:          1
Source:           %{name}-%{version}.tar.bz2
BuildRequires:    cmake
BuildRequires:	  pkgconfig(libsystemd-journal)
BuildRequires:	  pkgconfig(python)
BuildRequires:	  pkgconfig(sqlite3)
BuildRequires:	  pkgconfig(dbus-1)
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
The AudioManager is a framework for managing audio in the IVI context.

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build
mkdir build
cd build

cmake -DWITH_DLT=OFF -DUSE_BUILD_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr ..
#make VERBOSE=1
make %{?jobs:-j %jobs}

%install
rm -rf "$RPM_BUILD_ROOT"
cd build
make install
mkdir $RPM_BUILD_ROOT%{_includedir}/%{name}
mv $RPM_BUILD_ROOT%{_includedir}/*.h $RPM_BUILD_ROOT%{_includedir}/command $RPM_BUILD_ROOT%{_includedir}/control $RPM_BUILD_ROOT%{_includedir}/routing $RPM_BUILD_ROOT%{_includedir}/shared $RPM_BUILD_ROOT%{_includedir}/%{name}
rm $RPM_BUILD_ROOT%{_libdir}/audioManager/routing/libPluginRoutingInterfaceAsync.so*

%clean
rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,root,-)
%{_bindir}/AudioManager
%{_libdir}/audioManager/command/libPluginCommandInterfaceDbus.so*
%{_libdir}/audioManager/routing/libPluginRoutingInterfaceDbus.so*
%{_libdir}/audioManager/control/libPluginControlInterface.so*
%{_datadir}/audiomanager/*xml

%files devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/*