ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = src/libbinglib.la


src_libbinglib_la_SOURCES = src/utility/address_converter.cpp

src_libbinglib_la_LIBADD= @bitcoin_client_LIBS@


include_binglibdir = ${includedir}/binglib
include_binglib_HEADERS = \
    src/utility/address_converter.hpp



check_PROGRAMS = test1
test1_SOURCES = \
    test/utility/address_converter.cpp
test1_CPPFLAGS = -I$(top_srcdir)/src
test1_LDADD= src/libbinglib.la
TESTS = $(check_PROGRAMS)

