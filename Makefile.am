# $Id: duneproject 5502 2009-04-03 15:05:58Z sander $

# we need the module file to be able to build via dunecontrol
EXTRA_DIST = dune.module \
  CMakeLists.txt \
  config.h.cmake

SUBDIRS = dune doc m4 src cmake

# don't follow the full GNU-standard
# we need automake 1.9 or newer
AUTOMAKE_OPTIONS = foreign 1.9

# pass most important options when "make distcheck" is used
DISTCHECK_CONFIGURE_FLAGS = --with-dune-common=$(DUNE_COMMON_ROOT) --with-dune-geometry=$(DUNE_GEOMETRY_ROOT) --with-dune-grid=$(DUNE_GRID_ROOT)  CXX="$(CXX)" CC="$(CC)"

include $(top_srcdir)/am/top-rules
include $(top_srcdir)/am/global-rules

# Generate package configuration files for finding
# installed modules with CMake
include $(top_srcdir)/am/cmake-pkg-config

