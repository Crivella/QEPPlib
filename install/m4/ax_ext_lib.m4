#Define variables and with-... checks for external library
#Usage AX_EXT_LIB( [PREFIX], [header], [lib], [defaul-use (yes/n)], [default-include-path], [default-lib-path], [other-libs])
AC_DEFUN([AX_EXT_LIB],
  [
  v1=with_$3
  v2=with_$3_inc
  v3=with_$3_lib
  v1_def=def_with_$3
  v2_def=def_with_$3_inc
  v3_def=def_with_$3_lib
  v4=$1_CPPFLAGS
  v5=$1_LDFLAGS
  v6=$1_LIBS
  eval $v1_def=$4
  eval $v2_def=$5
  eval $v3_def=$6

  if test x${!v1} = x; then
    eval $v1=${!v1_def}
  fi
  if test x${!v2} = x; then
    eval $v2=${!v2_def}
  fi
  if test x${!v3} = x; then
    eval $v3=${!v3_def}
  fi


  AC_ARG_WITH( $3, 
    [AS_HELP_STRING([--with-]$3,
      [Use ]$3[ external library @<:@default=]$4[@:>@]
     )
      ],
      [],
      []
  )
  AC_ARG_WITH( $3[-inc], 
    [AS_HELP_STRING([--with-]$3[-inc],
      [Specifiy custom include path for ]$3[ external library. 
      Same as ]$1[_CPPFLAGS=-I/your/include/path @<:@default=]$5[@:>@]
     )
      ],
      [],
      []
  )
  AC_ARG_WITH( $3[-lib], 
    [AS_HELP_STRING([--with-]$3[-lib],
      [Specifiy custom lib path for ]$3[ external library.
      Same as ]$1[_LDFLAGS=-L/your/lib/path @<:@default=]$6[@:>@]
     )
      ],
      [],
      []
  )

  if test x${!v2} != x; then
    if test x${!v4} = x; then
      eval "$v4=\" -I${!v2}\""
    fi
  fi
  if test x${!v3} != x; then
    if test x${!v5} = x; then
      eval "$v5=\" -L${!v3}\""
    fi
  fi

#  echo $v1 = ${!v1}
#  echo $v2 = ${!v2}
#  echo $v3 = ${!v3}
#  echo $v4 = ${!v4}
#  echo $v5 = ${!v5}
#  echo $v6 = ${!v6}
#  echo $7 = ${!v7}
  AS_IF([test "x${!v1}" != xno],
    [
#     if test -d "${!v2}"; then
#       if test "x${!v2}" != x; then eval $v4+=" -I${!v2}"; fi
#       CPPFLAGS=${!v4}
#       AC_CHECK_HEADER($2,
#         [eval $v4+=" -D__LIBXML2"],
#         [AC_MSG_ERROR(Can't find XML2 headers.)]
#       )
#       CPPFLAGS=""
#     else
#       AC_MSG_ERROR(Invalid installation directory for XML libraries.)
#     fi
      AC_ARG_VAR($1[_LIBS],
        [C linker flags for]$1[ library]
      )
      AX_CHECK_LIBRARY($1,
        $2,
        $3,
        [eval "$v4+=\" -D__$1\""
        if test "x${!v6}" = x; then
          eval "$v6+=\" -l$3\""
          if test "x$7" != x; then
            eval "$v6+=\" $7\""
          fi
        else
          echo "!BOHHHHHHHHHHHHHH"
        fi
        ],
        [AC_MSG_ERROR([Can't find ]$2[ header.])]
      )
    ],
    [
      eval $v4=""
      eval $v5=""
      eval $v6=""
    ]
  )

  external_CPPFLAGS+="${!v4}"
  external_LDFLAGS+="${!v5}"
  external_LIBS+="${!v6}"
  ]
)














