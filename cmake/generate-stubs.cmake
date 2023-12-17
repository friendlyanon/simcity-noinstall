cmake_minimum_required(VERSION 3.18)

file(READ "${input}" functions)
string(STRIP "${functions}" functions)
string(REPLACE "\n" \; functions "${functions}")

set(externs "")
set(variables "")
set(thunks "")
set(calls "")
set(def "EXPORTS\n")

foreach(fn IN LISTS functions)
  string(APPEND externs "EXTERN ${fn}_ptr:FUNCPTR\n")
  string(APPEND variables "FARPROC ${fn}_ptr = NULL;\n")
  string(APPEND thunks "\n_${fn} PROC\n  jmp dword ptr [${fn}_ptr]\n_${fn} ENDP\n")
  string(APPEND calls "\n      || get_fn(&${fn}_ptr, \"${fn}\")")
  string(APPEND def "  ${fn} = _${fn}\n")
endforeach()

file(WRITE "${output}/stubs-generated.asm" "\
.386
.MODEL FLAT, STDCALL

FUNCPROTO TYPEDEF PROTO
FUNCPTR TYPEDEF PTR FUNCPROTO

${externs}
.CODE
${thunks}
END
")

file(WRITE "${output}/stubs-generated.c" "\
#include <Windows.h>
#include <stddef.h>

#include \"stubs.h\"

${variables}
static int get_fn(FARPROC* out, LPCSTR name)
{
  *out = GetProcAddress((HMODULE)winmm_handle, name);
  if (*out == NULL) {
    return 1;
  }

  return 0;
}

int stubs_load()
{
  return 0${calls};
}
")

file(WRITE "${output}/stubs-generated.def" "${def}")
