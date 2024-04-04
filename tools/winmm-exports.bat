@echo off

:: This will create the list of function exports for generate-stubs.cmake

dumpbin /exports C:\Windows\System32\winmm.dll 2>&1 | ^
awk -va=0 ^" ^
a == 0 ^&^& /RVA/ { a = 1; getline; next } ^
a == 1 ^&^& $0 == ^"^"^"^" { exit } ^
a == 1 ^&^& $4 { print $4 } ^
 ^"
