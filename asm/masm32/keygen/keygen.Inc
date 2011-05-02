
include windows.inc
include kernel32.inc
include user32.inc
include Comctl32.inc
include shell32.inc
include winmm.inc
include masm32rt.inc

includelib kernel32.lib
includelib user32.lib
includelib Comctl32.lib
includelib shell32.lib
includelib winmm.lib


DlgProc			PROTO	:HWND,:UINT,:WPARAM,:LPARAM
genkey 			PROTO
hex2asc 		PROTO	:DWORD, :DWORD
encrypt 		PROTO	:DWORD, :DWORD
addbytes 		PROTO	:DWORD
keygen 			PROTO	:DWORD, :DWORD, :DWORD, :DWORD
.const

IDD_DIALOG1			equ 101

;#########################################################################

.data

hInstance			dd 0
hWnd				dd 0
selBox				dd 0
butn1				dd 0
edit1				dd 0

part3				dd 0
serialnum			dd 32 DUP (?)
key           		db "8872hd 946JkrERQ",0
charMap				db "0123456789ABCDEF",0
sel1				db "Noiseware v4.2 (build 4205)",0
sel2				db "Portraiture v2.1 (build 2105)",0
sel3				db "RealGrain v1.1 (build 1103)",0
err1				db "Select your version above!",0

;#########################################################################
