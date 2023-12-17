.386
.MODEL FLAT, STDCALL

FUNCPROTO TYPEDEF PROTO
FUNCPTR TYPEDEF PTR FUNCPROTO

MH_CreateHookApiEx PROTO STDCALL, module:PTR WORD, procName:PTR SBYTE, detour:FUNCPTR, original:PTR FUNCPTR, target:PTR FUNCPTR
MH_EnableHook PROTO STDCALL, target:FUNCPTR

.CODE

hook PROC module:PTR WORD, procName:PTR SBYTE, detour:FUNCPTR, original:PTR FUNCPTR
  LOCAL target:FUNCPTR
  and target, 0

  INVOKE MH_CreateHookApiEx, module, procName, detour, original, ADDR target
  .IF eax == 0
    INVOKE MH_EnableHook, target
    .IF eax == 0
      jmp success
    .ENDIF
  .ENDIF

  xor eax, eax
  inc eax

success:
  ret
hook ENDP

END
