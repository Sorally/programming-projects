
include windows.inc
include kernel32.inc
include user32.inc
include Comctl32.inc
include shell32.inc

includelib kernel32.lib
includelib user32.lib
includelib Comctl32.lib
includelib shell32.lib

DlgProc			PROTO	:HWND,:UINT,:WPARAM,:LPARAM
genkey 			PROTO
hex2asc 		PROTO	:DWORD, :DWORD
encrypt 		PROTO	:DWORD, :DWORD
addbytes 		PROTO	:DWORD
keygen 			PROTO	:DWORD, :DWORD, :DWORD, :DWORD
patcher			PROTO
.const

IDD_DIALOG1			equ 101

;#########################################################################

.data

hInstance		dd 0
hWnd			dd 0
salt          	dd 0
pepper        	dd 0
serialnum		dd 32 DUP (?)
key             db "8872hd 946JkrERQ",0
charMap         db "0123456789ABCDEF",0




;#########################################################################
