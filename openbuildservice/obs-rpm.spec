# This is a cmake-generated file, do not edit!

Name:          guardfw
Version:       0.0.3
Release:       0
Summary:       Templated C++ wrapper around Linux API functions for proper error handling (shared library)
License:       MIT
URL:           http://guardfw.de
Source:        https://github.com/sgleissner/guardfw/archive/refs/tags/0.0.3.tar.gz
BuildRequires: gcc13
BuildRequires: gcc13-c++
BuildRequires: cmake
BuildRequires: pkg-config

%package devel
Summary: Templated C++ wrapper around Linux API functions for proper error handling (development files)
Provides: guardfw-devel

%package devel-static
Summary: Templated C++ wrapper around Linux API functions for proper error handling (static library)
Provides: guardfw-devel-static
Requires: guardfw-devel

%description
Templated C++ wrapper around Linux API functions for proper error handling. (shared library)

%description devel
Templated C++ wrapper around Linux API functions for proper error handling. (development files)

%description devel-static
Templated C++ wrapper around Linux API functions for proper error handling. (static library)

%prep
%setup -q -n %{name}-%{version}

%build
export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG"
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?jobs:-j%jobs}

%install
make install DESTDIR="$RPM_BUILD_ROOT"

%license LICENSE

%files
%defattr(-,root,root,-)
%doc README.md
%{_libdir}/*.so*

%files devel
%{_includedir}/%{name}/*.hpp
%{_datadir}/pkgconfig/*.pc

%files devel-static
%{_libdir}/%{name}-%{version}/*.a

# TODO: cmake packages config
# https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
# <prefix>/lib/cmake/foo-1.2/FooConfig.cmake

%changelog
