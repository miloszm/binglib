ACLOCAL_AMFLAGS = -I m4


pkgconfig_DATA = libbinglib.pc


lib_LTLIBRARIES = src/libbinglib.la


src_libbinglib_la_SOURCES = src/utility/address_converter.cpp src/wallet/bing_wallet.cpp

src_libbinglib_la_LIBADD= @bitcoin_client_LIBS@


include_binglibdir = ${includedir}/binglib
include_binglib_HEADERS = \
    src/utility/address_converter.hpp



check_PROGRAMS = test_utility test_wallet
test_utility_SOURCES = \
    test/utility/address_converter_test.cpp
test_utility_CPPFLAGS = -I$(top_srcdir)/src
test_utility_LDADD= src/libbinglib.la

test_wallet_SOURCES = \
    test/wallet/bing_wallet_test.cpp
test_wallet_CPPFLAGS = -I$(top_srcdir)/src
test_wallet_LDADD= src/libbinglib.la

TESTS = $(check_PROGRAMS)
