
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

patch db 32,32       ;  What we're going to write to target.exe (32 = 20h, ie. ASCII space)
progname       db "noiseware.8bf.exe",0      ; The file we're going to patch
SEEK_SIZE    DWORD 2068         ;  How far we need to seek through the file (ie. the offset)
FILESIZE        DWORD 2560         ;  The size of our target

hFile 		DWORD	0
hFileHighSize 	DWORD 0
nFileSize 		DWORD 0
szPatchOK	db "Patch was successful",0
szPatchOKCaption	db "File patched!",0
szPatchNOTOK	db "Patching failed!",0
szFileNotFound	db "File not found!",0
szSharingViolation	db "Sharing Violation! Target is probably running or being used..",0
szFilesizeNotOK	db "Filesize does not match.",0



;#########################################################################
