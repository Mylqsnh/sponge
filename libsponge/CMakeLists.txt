file (GLOB LIB_SOURCES "*.cc" "util/*.cc" "tcp_helpers/*.cc")
add_library (sponge STATIC ${LIB_SOURCES})

#add_sponge_exec (stream_reassembler)

#add_sponge_exec (wrapping_integers)
