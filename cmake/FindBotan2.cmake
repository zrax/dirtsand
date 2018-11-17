# This file is part of dirtsand.
#
# dirtsand is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# dirtsand is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with dirtsand.  If not, see <http://www.gnu.org/licenses/>.

find_package(PkgConfig QUIET)
pkg_check_modules(PC_BOTAN2 QUIET botan-2)

find_path(Botan2_INCLUDE_DIR
    NAMES botan/version.h
    HINTS ${PC_BOTAN2_INCLUDEDIR} ${PC_BOTAN2_INCLUDE_DIRS}
    PATH_SUFFIXES botan-2
)

find_library(Botan2_LIBRARY
    NAMES botan-2 libbotan-2
    HINTS ${PC_BOTAN2_LIBDIR} ${PC_BOTAN2_LIBRARY_DIRS}
)

mark_as_advanced(Botan2_INCLUDE_DIR Botan2_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Botan2
    REQUIRED_VARS Botan2_LIBRARY Botan2_INCLUDE_DIR
)

if(Botan2_FOUND AND NOT TARGET Botan2::Botan2)
    add_library(Botan2::Botan2 UNKNOWN IMPORTED)
    set_target_properties(Botan2::Botan2 PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${Botan2_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Botan2_INCLUDE_DIR}"
    )
endif()
