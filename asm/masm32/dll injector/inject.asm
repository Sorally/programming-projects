.386
.model stdcall,flat
 include \masm32\include\kernel32.inc
 includelib \masm32\lib\kernel32.lib
 
.data
    hLib dword ?
    hProc1 dword ?
;    hProc2 dword ?
.data
    lib byte "tstdll.dll", 0
    function1 byte "proc1", 0
;    function2 byte "proc2", 0

.code
start:
    push offset lib
    call LoadLibrary; will load the dll
    mov hLib, eax

    push offset function1
    push hLib
    call GetProcAddress; will get the procedure to execute
    mov hProc1, eax


    call hProc1; will call your function in your DLL
;    call hProc2
    add hProc1, 50; haxor!!
    call hProc1; will call your function in your DLL
    
    push hLib
    call FreeLibrary; free the resource
    ret
end start