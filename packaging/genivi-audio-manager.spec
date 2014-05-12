Name:             genivi-audio-manager
License:          MPL-2.0
Group:            Automotive/API
Summary:          GENIVI Audio Manager
Version:          6.0
Release:          0
Source:           %{name}-%{version}.tar.bz2
BuildRequires:    cmake
BuildRequires:	  pkgconfig(libsystemd-journal)
BuildRequires:	  pkgconfig(python)
BuildRequires:	  pkgconfig(sqlite3)
BuildRequires:	  pkgconfig(dbus-1)
BuildRequires:	  pkgconfig(automotive-dlt)
BuildRequires:	  pkgconfig(zlib)
BuildRequires:    pkgconfig(CommonAPI)
BuildRequires:    pkgconfig(CommonAPI-DBus)
BuildRequires:	  pkgconfig(murphy-common)
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
The AudioManager is a framework for managing audio in the IVI context.

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
%{summary}. This package provides headers for developing AudioManager plugins and applications.

%prep
%setup -q -n %{name}-%{version}

%build
mkdir -p build
cd build

cmake -DUSE_BUILD_LIBS=OFF -DWITH_TESTS=OFF -DWITH_ENABLED_IPC=DBUS -DWITH_DBUS_WRAPPER=ON -DWITH_NSM=OFF -DWITH_DLT=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
#make VERBOSE=1
%__make %{?_smp_mflags}

%install
rm -rf "$RPM_BUILD_ROOT"
cd build
%make_install DESTDIR=$RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT%{_includedir}/%{name}
mv $RPM_BUILD_ROOT%{_includedir}/*.h $RPM_BUILD_ROOT%{_includedir}/command $RPM_BUILD_ROOT%{_includedir}/control $RPM_BUILD_ROOT%{_includedir}/routing $RPM_BUILD_ROOT%{_includedir}/shared $RPM_BUILD_ROOT%{_includedir}/%{name}
rm $RPM_BUILD_ROOT%{_libdir}/audioManager/routing/libPluginRoutingInterfaceAsync.so*

%files
%doc LICENCE README.html
%{_bindir}/AudioManager
%{_libdir}/audioManager/command/libPluginCommandInterfaceDbus.so*
%{_libdir}/audioManager/routing/libPluginRoutingInterfaceDbus.so*
%{_libdir}/audioManager/control/libPluginControlInterface.so*
%{_libdir}/audioManager/control/libPluginControlInterfaceDbus.so*
%{_libdir}/audioManager/control/libPluginControlInterfaceMurphy.so*
%{_datadir}/audiomanager/*xml

%files devel
%{_includedir}/%{name}/*
