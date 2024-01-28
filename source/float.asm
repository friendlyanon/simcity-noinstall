.386
.MODEL FLAT

.CONST

; All the videos are 320 pixels wide and the matching 4:3 height is 240
video_width REAL4 320.0
video_height REAL4 240.0

float_to_unsigned REAL4 8388608.0

.DATA

growth REAL4 0.0

.CODE

OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

@calculate_growth@8 PROC ; ecx = width:PTR, edx = height:PTR
  fild DWORD PTR [ecx]
  fdiv DWORD PTR video_width

  fild DWORD PTR [edx]
  fdiv DWORD PTR video_height

  fcom
  fnstsw ax

  shr ah, 1
  jc @F

  fstp st(0)
  jmp return

@@:
  fstp st(1)

return:
  fstp DWORD PTR growth
  ret
@calculate_growth@8 ENDP

@scale@8 PROC ; ecx = a:PTR, edx = b:PTR
  add esp, -4

  fild DWORD PTR [edx]
  fmul DWORD PTR growth
  fadd DWORD PTR float_to_unsigned

  fild DWORD PTR [ecx]
  fmul DWORD PTR growth
  fadd DWORD PTR float_to_unsigned

  fstp DWORD PTR [esp]
  mov eax, DWORD PTR [esp]
  and eax, 007fffffH

  fstp DWORD PTR [esp]
  mov edx, DWORD PTR [esp]
  and edx, 007fffffH

  add esp, 4
  ret
@scale@8 ENDP ; eax = a, edx = b

END
