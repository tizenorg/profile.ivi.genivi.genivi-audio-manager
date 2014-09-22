Name:             genivi-audio-manager
License:          MPL-2.0
Group:            Automotive/API
Summary:          GENIVI Audio Manager
Version:          6.0
Release:          0
Source:           %{name}-%{version}.tar.bz2
Source1001:       genivi-audio-manager.manifest
BuildRequires:    cmake
BuildRequires:    pkgconfig(libsystemd-journal)
BuildRequires:    pkgconfig(python)
BuildRequires:    pkgconfig(sqlite3)
BuildRequires:    pkgconfig(dbus-1)
BuildRequires:    pkgconfig(automotive-dlt)
BuildRequires:    pkgconfig(zlib)
BuildRequires:    pkgconfig(CommonAPI)
BuildRequires:    pkgconfig(CommonAPI-DBus)
BuildRequires:    pkgconfig(murphy-common)
BuildRequires:    pkgconfig(murphy-domain-controller)
BuildRequires:    pkgconfig(murphy-dbus-libdbus)
BuildRequires:    bison
BuildRequires:    flex
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
The AudioManager is a framework for managing audio in the IVI context.

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
"%{summary}.
This package provides headers for developing AudioManager plugins 
and applications."

%prep
%setup -q -n %{name}-%{version}
cp %{SOURCE1001} .

%build
mkdir -p build
cd build

%cmake -DUSE_BUILD_LIBS=OFF \
       -DWITH_TESTS=OFF \
       -DWITH_ENABLED_IPC=DBUS \
       -DWITH_DBUS_WRAPPER=ON \
       -DWITH_NSM=OFF \
       -DWITH_DLT=OFF \
       -DCMAKE_INSTALL_PREFIX=%{_prefix} \
       ..
#make VERBOSE=1
%__make %{?_smp_mflags}

%install
rm -rf "%{buildroot}"
cd build
%make_install
mkdir -p %{buildroot}%{_includedir}/%{name}
for cpath in *.h command control routing shared; do
    mv %{buildroot}%{_includedir}/${cpath} %{buildroot}%{_includedir}/%{name}
done
rm %{buildroot}%{_libdir}/audioManager/routing/libPluginRoutingInterfaceAsync.so*

%files
%manifest %{name}.manifest
%doc LICENCE README.html
%{_bindir}/AudioManager
%{_bindir}/domain-manager
%{_libdir}/audioManager/command/libPluginCommandInterfaceDbus.so*
%{_libdir}/audioManager/routing/libPluginRoutingInterfaceDbus.so*
%{_libdir}/audioManager/control/libPluginControlInterface.so*
%{_libdir}/audioManager/control/libPluginControlInterfaceDbus.so*
%{_libdir}/audioManager/control/libPluginControlInterfaceMurphy.so*
%{_datadir}/audiomanager/*xml

%files devel
%manifest %{name}.manifest
%{_includedir}/%{name}/*
