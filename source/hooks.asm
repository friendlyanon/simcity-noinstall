.386
.MODEL FLAT, STDCALL

MH_CreateHookApiEx PROTO STDCALL, module:DWORD, procName:DWORD, detour:DWORD, original:PTR DWORD, target:PTR DWORD
MH_EnableHook PROTO STDCALL, target:DWORD

.CODE

hook PROC module:DWORD, procName:DWORD, detour:DWORD, original:PTR DWORD
  LOCAL target:DWORD
  and target, 0

  INVOKE MH_CreateHookApiEx,module,procName,detour,original,ADDR target
  .IF eax == 0
    INVOKE MH_EnableHook,target
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
