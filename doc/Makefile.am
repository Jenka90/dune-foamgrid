
SUBDIRS = doxygen

CURDIR = doc

BASEDIR = ..

# add list of html files to generate from wml
PAGES=

docdir=$(datadir)/doc/dune-foamgrid

EXTRA_DIST = CMakeLists.txt

include $(top_srcdir)/am/webstuff

CLEANFILES = $(PAGES)

if ! BUILD_DOCS
# add tag to notify that dist has been build without documentation
dist-hook:
	echo "# No documentation included in distribution! " > $(distdir)/$(DOCUMENTATION_TAG_FILE)
endif

include $(top_srcdir)/am/webstuff
include $(top_srcdir)/am/global-rules
