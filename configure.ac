AC_INIT([bingx], [0.0.2], [mm@example.com])

AC_CONFIG_MACRO_DIR([m4])

# Check for pkg-config.
PKG_PROG_PKG_CONFIG
AS_IF([test -n "$PKG_CONFIG"], [],
    [AC_MSG_ERROR([pkg-config is required but was not found.])])


AM_INIT_AUTOMAKE([subdir-objects])

LT_PREREQ(2.4.2)
LT_INIT

AC_PROG_CXX

AC_CONFIG_FILES([Makefile])

PKG_CHECK_MODULES([bitcoin_client], [libbitcoin-client >= 3.0.0])
AC_SUBST([bitcoin_client_PKG], ['libbitcoin-client >= 3.0.0'])
AC_SUBST([bitcoin_client_CPPFLAGS], [${bitcoin_client_CFLAGS}])
AC_MSG_NOTICE([bitcoin_client_CPPFLAGS : ${bitcoin_client_CPPFLAGS}])
AC_MSG_NOTICE([bitcoin_client_LIBS : ${bitcoin_client_LIBS}])

PKG_CHECK_MODULES([openssl_libssl], [libssl])
AC_SUBST([openssl_libssl_PKG], ['OpenSSL-libssl'])
AC_SUBST([openssl_libssl_CPPFLAGS], [${openssl_libssl_CFLAGS}])
AC_MSG_NOTICE([openssl_libssl_CPPFLAGS : ${openssl_libssl_CPPFLAGS}])
AC_MSG_NOTICE([openssl_libssl_LIBS : ${openssl_libssl_LIBS}])

PKG_CHECK_MODULES([openssl_libcrypto], [libcrypto])
AC_SUBST([openssl_libcrypto_PKG], ['OpenSSL-libcrypto'])
AC_SUBST([openssl_libcrypto_CPPFLAGS], [${openssl_libcrypto_CFLAGS}])
AC_MSG_NOTICE([openssl_libcrypto_CPPFLAGS : ${openssl_libcrypto_CPPFLAGS}])
AC_MSG_NOTICE([openssl_libcrypto_LIBS : ${openssl_libcrypto_LIBS}])

PKG_CHECK_MODULES([nlohmann_json], [nlohmann_json])
AC_SUBST([nlohmann_json_PKG], ['nlohmann_json'])
AC_SUBST([nlohmann_json_CPPFLAGS], [${nlohmann_json_CFLAGS}])
AC_MSG_NOTICE([nlohmann_json_CPPFLAGS : ${nlohmann_json_CPPFLAGS}])
AC_MSG_NOTICE([nlohmann_json_LIBS : ${nlohmann_json_LIBS}])


CXXFLAGS="-ggdb -I../include/ $CXXFLAGS -std=c++14 $bitcoin_client_CPPFLAGS $openssl_libcrypto_CPPFLAGS $openssl_libssl_CPPFLAGS $nlohmann_json_CPPFLAGS $DEBUG"


PKG_PROG_PKG_CONFIG
AS_IF([test -n "$PKG_CONFIG"], [],
    [AC_MSG_ERROR([pkg-config is required but was not found.])])

# Process options.
#==============================================================================
# Implement --with-pkgconfigdir and output ${pkgconfigdir}.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--with-pkgconfigdir option])
AC_ARG_WITH([pkgconfigdir],
    AS_HELP_STRING([--with-pkgconfigdir=DIR],
        [Path to pkgconfig directory. @<:@default=${libdir}/pkgconfig@:>@]),
    [pkgconfigdir=$withval],
    [pkgconfigdir=${libdir}/pkgconfig])
AC_MSG_RESULT([$pkgconfigdir])
AC_SUBST([pkgconfigdir])

AC_CONFIG_FILES([libbinglib.pc])

AC_OUTPUT
