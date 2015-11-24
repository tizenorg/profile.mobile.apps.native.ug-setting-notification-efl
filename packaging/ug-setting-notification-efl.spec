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
#BuildRequires: model-build-features
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(capi-appfw-package-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-appfw-application)


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
%define bin_path /usr/apps/%{name}/bin
%define lib_path /usr/apps/%{name}/lib/ug
%define link_path /home/owner/apps_rw/%{name}/lib/ug

export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%cmake .
make -j2

%install
%make_install
mkdir -p %{buildroot}%{bin_path}
mkdir -p %{buildroot}%{link_path}
mkdir -p /etc/smack/accesses.d/
ln -sf /usr/bin/ug-client %{buildroot}%{bin_path}/ug-setting-notification-efl
ln -sf /usr/bin/ug-client %{buildroot}%{bin_path}/ug-setting-notification-do-not-disturb-efl
ln -sf /usr/bin/ug-client %{buildroot}%{bin_path}/ug-setting-notification-app-notifications-efl
ln -sf %{lib_path}/libug-setting-notification-do-not-disturb-efl.so %{buildroot}%{link_path}/libug-setting-notification-do-not-disturb-efl.so


mkdir -p %{buildroot}/usr/apps/%{name}/res

%post -n %{name} -p /sbin/ldconfig

%postun -n %{name} -p /sbin/ldconfig


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_prefix}/apps/%{name}/*
%{link_path}/*
%{_prefix}/share/packages/%{name}.xml
/etc/smack/accesses.d/%{name}.efl
%{_datarootdir}/license/%{name}
%{_datadir}/icons/default/small/%{name}.png

/usr/apps/%{name}/res/locale/*/LC_MESSAGES/*

