#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(QEPPlib, 0.5.0, [BUG-REPORT-ADDRESS])
#AC_CONFIG_SRCDIR([src/qepp_read.c])
AC_CONFIG_MACRO_DIR([m4/])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Check compile options
AX_CHECK_COMPILE_FLAG([-std=c11], 
  [CFLAGS+=" -std=c11"], 
  [AC_MSG_ERROR(C compiler cannot compile C11 code)]
)
AX_CHECK_COMPILE_FLAG([-Wall], 
  [CFLAGS+=" -Wall"], 
  [AC_MSG_ERROR(C compiler does not accept Wall flag)]
)
AX_CHECK_COMPILE_FLAG([-fpic], 
  [CFLAGS+=" -fpic"], 
  [AC_MSG_ERROR(C compiler does not accept fpic flag)]
)


# External default
def_with_mpi="no"

if test x$with_mpi = x; then
  with_mpi=$def_with_mpi
fi

def_prefix=$HOME/.local
if test x$prefix = xNONE; then
  prefix=$def_prefix
fi
if test x$prefix = x; then
  prefix=$def_prefix
fi

# Check if parallel execution is enabled
AC_ARG_WITH( mpi, 
  [AS_HELP_STRING([--with-mpi],
    [Enable parallel execution @<:@default = no@:>@]
   )
  ],
  [],
  []
)
AS_IF([test "x$with_mpi" != xno],
  [AX_MPI(
    [CC=$MPICC
      external_CPPFLAGS+=" -D__MPI"
    ],
    [AC_MSG_ERROR(Can't find MPI compiler.)]
   )
  ],
  []
)


# Checks for libraries.
##Check if XML2 is enabled
AX_EXT_LIB( [LIBXML2], [libxml/parser.h], [xml2], [yes], [/usr/include/libxml2])

##Check if GSL is enabled
AX_EXT_LIB( [GSL], [gsl/gsl_integration.h], [gsl], [no], [], [], [-lgslblacs -lm])


CPPFLAGS+=" $external_CPPFLAGS"
CPPFLAGS+=" -I./inc"
CPPFLAGS+=" -I$prefix/inc"
CPPFLAGS=" $CPPFLAGS"

LDFLAGS+=" $external_LDFLAGS"
LDFLAGS=" $LDFLAGS"

LIBS+=" $external_LIBS"
LIBS=" $LIBS"

CPPFLAGS="$(echo -e "${CPPFLAGS}" | tr -s " " | cut -d " " -f 2-200)"
LDFLAGS="$(echo -e "${LDFLAGS}" | tr -s " " | cut -d " " -f 2-200)"
LIBS="$(echo -e "${LIBS}" | tr -s " " | cut -d " " -f 2-200)"



# Checks for header files.
#AC_CHECK_HEADERS([stdlib.h string.h unistd.h math.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor getcwd pow sqrt strchr strstr strtol])

AC_CONFIG_FILES([make.inc])
AC_CONFIG_FILES([configure.msg])
AC_OUTPUT

sed '/@delete@/d' configure.msg
echo configure: success



#  [AC_CHECK_LIB([LIBXML2], 
#    [libxml/parser.h], 
#    [xml2], 
#    [CPPFLAGS+=" -D__LIBXML2"], 
#    [AC_MSG_ERROR(libxml2 library not found.)]
#   )


#AX_CHECK_LIBRARY([LIBGSL], 
#    [gsl/gsl_integration.h], 
#    [gsl], 
#    [CPPFLAGS+=" -D__GSL"], 
#    [AC_MSG_ERROR(libgsl library not found.)],
#    [-lgslcblas]
#   )

#echo -e "\nCFLAGS: $CFLAGS\nLDFLAGS: $LDFLAGS\nLIBS: $LIBS\nCPPFLAGS: $CPPFLAGS\n"
#echo -e "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\nwith-mpi=$with_mpi\nwith-xml2=$with_xml2\nwith-gsl=$with_gsl\n\n"




























