
Name:       org.tizen.setting-notification
Summary:    notification-setting
Version:    0.1.33
Release:    0
Group:      Applications/Core Applications
License:     Apache-2.0
Source0:    %{name}-%{version}.tar.gz


%if "%{?tizen_profile_name}"=="wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%if "%{?tizen_profile_name}"=="tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif


BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(capi-appfw-package-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-appfw-application)


%description
Description: notification-setting

%define PREFIX           %{TZ_SYS_RO_APP}/%{name}
%define MANIFESTDIR      %{TZ_SYS_RO_PACKAGES}
%define ICONDIR          %{TZ_SYS_RO_ICONS}/default/small

%define RESDIR           %{PREFIX}/res
%define EDJDIR           %{RESDIR}/edje
%define IMGDIR           %{RESDIR}/images
%define BINDIR           %{PREFIX}/bin
%define LIBDIR           %{PREFIX}/lib
%define LOCALEDIR        %{RESDIR}/locale

%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

cmake . \
    -DPREFIX=%{PREFIX}   \
    -DPKGDIR=%{name}     \
    -DIMGDIR=%{IMGDIR}   \
    -DEDJDIR=%{EDJDIR}   \
    -DPKGNAME=%{name}    \
    -DBINDIR=%{BINDIR}   \
    -DMANIFESTDIR=%{MANIFESTDIR}   \
    -DEDJIMGDIR=%{EDJIMGDIR}   \
    -DLIBDIR=%{LIBDIR}   \
    -DICONDIR=%{ICONDIR}   \
    -DLOCALEDIR=%{LOCALEDIR}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/%{LIBDIR}

%post
GOPTION="-g 6514"

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%dir
%{LIBDIR}
%{BINDIR}/*
%{MANIFESTDIR}/*.xml
%{ICONDIR}/*
%{RESDIR}/*
