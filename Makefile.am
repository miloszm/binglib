ACLOCAL_AMFLAGS = -I m4


pkgconfig_DATA = libbinglib.pc


lib_LTLIBRARIES = src/libbinglib.la


src_libbinglib_la_SOURCES = src/utility/address_converter.cpp \
                            src/wallet/bing_wallet.cpp \
                            src/electrumclient/electrum_model.cpp \
                            src/libbitcoinclient/libb_client.cpp \
                            src/ronghuaclient/ronghua_client.cpp \
                            src/ronghuaclient/ronghua_socket_client.cpp \
                            src/ronghuaclient/ronghua_input_queue.cpp \
                            src/libbfunds/purse_accessor.cpp \
                            src/libbfunds/funds_finder.cpp \
                            src/p2pkhtx/online_p2pkh_tx_creator.cpp \
                            src/libbscript/redeem_script.cpp \
                            src/locktx/online_lock_tx_creator.cpp \
                            src/unlocktx/offline_unlock_tx_creator.cpp \
                            src/walletstate/wallet_state.cpp \
                            src/historyinspector/history_inspector.cpp


src_libbinglib_la_LIBADD= @bitcoin_client_LIBS@ @openssl_libcrypto_LIBS@ @openssl_libssl_LIBS@


include_binglibdir = ${includedir}/binglib
include_binglib_HEADERS = \
    src/common/bing_common.hpp \
    src/utility/address_converter.hpp \
    src/utility/blocking_queue.hpp \
    src/wallet/bing_wallet.hpp \
    src/electrumclient/electrum_model.hpp \
    src/electrumclient/electrum_interface.hpp \
    src/libbitcoinclient/libb_client.hpp \
    src/ronghuaclient/ronghua_client.hpp \
    src/ronghuaclient/ronghua_socket_client.hpp \
    src/ronghuaclient/ronghua_input_queue.hpp \
    src/libbfunds/purse_accessor.hpp \
    src/libbfunds/funds_finder.hpp \
    src/p2pkhtx/online_p2pkh_tx_creator.hpp \
    src/libbscript/redeem_script.hpp \
    src/locktx/online_lock_tx_creator.hpp \
    src/unlocktx/offline_unlock_tx_creator.hpp \
    src/walletstate/wallet_state.hpp \
    src/historyinspector/history_inspector.hpp \
    src/common/bing_exception.hpp



check_PROGRAMS = test_utility test_wallet test_electrumclient
test_utility_SOURCES = \
    test/utility/address_converter_test.cpp
test_utility_CPPFLAGS = -I$(top_srcdir)/src
test_utility_LDADD= src/libbinglib.la

test_wallet_SOURCES = \
    test/wallet/bing_wallet_test.cpp
test_wallet_CPPFLAGS = -I$(top_srcdir)/src
test_wallet_LDADD= src/libbinglib.la

test_electrumclient_SOURCES = \
    test/electrumclient/electrum_input_queue_test.cpp
test_electrumclient_CPPFLAGS = -I$(top_srcdir)/src
test_electrumclient_LDADD= src/libbinglib.la

TESTS = $(check_PROGRAMS)
