Name:		ug-setting-notification-efl
Summary:	UG for Settings of Notification Center
Version:	0.0.1
Release:	1
Group:		Core/Applications
License:	Apache-2.0
Source:		%{name}-%{version}.tar.gz
Source1001: %{name}.manifest


%if "%{?tizen_profile_name}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%if "%{?tizen_profile_name}"=="tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif


BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(dlog)
BuildRequires: edje-bin
BuildRequires: cmake
BuildRequires: gettext-tools
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(capi-appfw-package-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(libtzplatform-config)


%description
UI Gadget module for Settings App.
This package will be loaded into a Settings Application.
User can control the behaviour of notification of each application individually.


%description
ug-setting-notification-do-not-disturb-efl.
%package -n ug-setting-notification-do-not-disturb-efl
Summary: do-not-disturb
Group:      TO_BE/FILLED_IN
%description -n ug-setting-notification-do-not-disturb-efl
Description: do not disturb efl


%prep
%setup -q
cp %{SOURCE1001} .


%build
%define _pkg_dir %{TZ_SYS_RO_APP}/%{name}
%define _pkg_lib_dir %{_pkg_dir}/lib

%define _sys_icons_dir %{TZ_SYS_RO_ICONS}
%define _sys_packages_dir %{TZ_SYS_RO_PACKAGES}
%define _sys_license_dir %{TZ_SYS_SHARE}/license
%define _sys_smack_dir %{TZ_SYS_SMACK}/accesses.d

%define _bin_path %{TZ_SYS_RO_APP}/%{name}/bin
%define _link_path /home/owner/apps_rw/%{name}/lib/ug

export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
export LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--rpath=%{_pkg_lib_dir} -Wl,--as-needed -Wl,-zdefs"


%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
#export LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--rpath=%{_pkg_lib_dir} -Wl,--as-needed -Wl,-zdefs"
%endif

%cmake . -DCMAKE_INSTALL_PREFIX=%{_pkg_dir} \
	-DENABLE_ATTACH_PANEL=%{_enable_attach_panel} \
	-DSYS_ICONS_DIR=%{_sys_icons_dir} \
	-DSYS_PACKAGES_DIR=%{_sys_packages_dir} \
	-DSYS_LICENSE_DIR=%{_sys_license_dir} \
	-DSYS_SMACK_DIR=%{_sys_smack_dir}
	
make -j2

%install
%make_install
mkdir -p %{buildroot}%{_bin_path}
mkdir -p %{buildroot}%{_link_path}
ln -sf %{TZ_SYS_BIN}/ug-client %{buildroot}%{_bin_path}/ug-setting-notification-efl
ln -sf %{TZ_SYS_BIN}/ug-client %{buildroot}%{_bin_path}/ug-setting-notification-do-not-disturb-efl
ln -sf %{TZ_SYS_BIN}/ug-client %{buildroot}%{_bin_path}/ug-setting-notification-app-notifications-efl
ln -sf %{lib_path}/libug-setting-notification-do-not-disturb-efl.so %{buildroot}%{_link_path}/libug-setting-notification-do-not-disturb-efl.so

%post -n %{name} -p /sbin/ldconfig

%postun -n %{name} -p /sbin/ldconfig


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_pkg_dir}/*
%{_link_path}/*
%{_prefix}/share/packages/%{name}.xml
%{_sys_smack_dir}/%{name}.efl
%{_datarootdir}/license/%{name}
%{_datadir}/icons/default/small/%{name}.png

#/usr/apps/%{name}/res/locale/*/LC_MESSAGES/*
%{_pkg_dir}/res/locale/*/LC_MESSAGES/*
