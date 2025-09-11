SYSTEMLOCALHEADERS = "${target_prefix}/local/include"

do_configure:append () {
	cat ${B}/gcc/defaults.h | grep -v "\#endif.*GCC_DEFAULTS_H" > ${B}/gcc/defaults.h.new
	cat >>${B}/gcc/defaults.h.new <<_EOF
#define LOCAL_INCLUDE_DIR "${SYSTEMLOCALHEADERS}"
#endif /* ! GCC_DEFAULTS_H */
_EOF
	mv ${B}/gcc/defaults.h.new ${B}/gcc/defaults.h
}
