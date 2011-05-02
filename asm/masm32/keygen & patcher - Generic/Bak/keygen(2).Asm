.386
.model flat, stdcall  ;32 bit memory model
option casemap :none  ;case sensitive

include keygen.inc

.code

start:

	invoke GetModuleHandle,NULL
	mov hInstance,eax

    invoke InitCommonControls
	invoke DialogBoxParam,hInstance,IDD_DIALOG1,NULL,addr DlgProc,NULL
	invoke ExitProcess,0

;########################################################################

DlgProc proc hWin:HWND,uMsg:UINT,wParam:WPARAM,lParam:LPARAM

	mov	eax,uMsg
	.if eax==WM_INITDIALOG
		mov ecx, hWin
		mov hWnd, ecx
		invoke genkey
	.elseif eax==WM_COMMAND
		.if wParam==1002
            invoke genkey
		.endif
		.if wParam==1004
			invoke patcher
		.endif
	.elseif eax==WM_CLOSE
		invoke EndDialog,hWin,0
	.else
		mov eax,FALSE
		ret
	.endif
	mov eax,TRUE
	ret

DlgProc endp

genkey proc

mov eax, salt     ; 0xFFFF
mov ecx, pepper   ; 0xFF
inc eax
inc ecx
.if eax == 0FFFFh
    mov eax,0
.endif
mov salt, eax

.if ecx == 0FFh
    mov ecx,0
.endif
mov pepper, ecx
shl ecx, 24
or ecx, 5E2544h


invoke keygen, eax, 90000304h, ecx, 278B19B0h

ret

genkey endp

keygen proc s1:DWORD, s2:DWORD, s3:DWORD, s4:DWORD

mov eax,0
invoke addbytes, s1
invoke addbytes, s2
invoke addbytes, s3
invoke addbytes, s4
mov ecx, s1
shl ecx, 16
add eax,ecx

invoke encrypt, eax, s2
invoke hex2asc, esi, 0
invoke hex2asc, edx, 8
invoke encrypt, s3, s4
invoke hex2asc, esi, 16
invoke hex2asc, edx, 24
invoke SetDlgItemText,hWnd,1001,offset serialnum

ret

keygen endp

hex2asc proc hex:DWORD, star:DWORD


push edx

mov edi, 4

mov eax, 0
mov ebx, 0
mov edx, offset serialnum
add edx, star

loop1:
mov al, byte ptr hex
mov bl, al
and al,0Fh
shr bl,4
and bl,0Fh
mov ecx, offset charMap
mov cl, byte ptr [ecx + ebx]
mov byte ptr [edx], cl
inc edx
mov ecx, offset charMap
mov cl, byte ptr [ecx + eax]
mov byte ptr [edx], cl
inc edx
dec edi
inc ebp
test edi,edi
jne loop1
mov byte ptr [edx], 0

pop edx
pop ebp
retn


hex2asc endp

encrypt proc v0:DWORD, v1:DWORD



MOV ECX, [v0]
MOV EAX, ECX

MOV ECX,v1
MOV ESI,EAX
mov eax, ecx
MOV EDX,EAX
MOV ECX,0
MOV EDI,20h
LEA EBX,DWORD PTR DS:[EBX]

loop1:

MOV EBX, offset key          ; loop {
MOV EAX,ECX
AND EAX,3
MOV EAX,DWORD PTR DS:[EBX+EAX*4]
MOV EBX,EDX
SHR EBX,5
MOV EBP,EDX
SHL EBP,4
XOR EBX,EBP
ADD EAX,ECX
ADD EBX,EDX
XOR EAX,EBX
ADD ESI,EAX                             

SUB ECX,61C88647h                         ; pivot

MOV EBX, offset key            
MOV EAX,ECX
SHR EAX,0Bh
AND EAX,3
MOV EAX,DWORD PTR DS:[EBX+EAX*4]
MOV EBX,ESI
SHR EBX,5
MOV EBP,ESI
ADD EAX,ECX
SHL EBP,4
XOR EBX,EBP
ADD EBX,ESI
XOR EAX,EBX
ADD EDX,EAX
SUB EDI,1
TEST EDI,EDI
JA SHORT loop1                           ; } end loop

pop ebp
retn

encrypt endp

addbytes proc var1:DWORD

mov ebx, var1
loop1:
mov ecx,0
mov cl, bl
add eax, ecx
shr ebx,8
cmp ebx,0
jne loop1
ret

addbytes endp

patcher proc

		push 0
		push FILE_ATTRIBUTE_NORMAL
		push OPEN_EXISTING
		push 0
		push FILE_SHARE_READ
		push GENERIC_WRITE
		push offset progname
		call CreateFile			; Open our (existing) target
		mov hFile,eax			; get the handle
		call GetLastError		                ; retrieve last error

		cmp eax,2			; error 2 - File not found
		jz FileNotFound			
		cmp eax,20h			; Sharing violation - target is running or in use
		jz SharingViolation


; Push the file handle, retreive the size and check if it matches:

		push 0

		push hFile			
		call GetFileSize
		mov nFileSize,eax
		cmp eax,FILESIZE
		jnz FilesizeNotOK
	
; Start patching :))

		push FILE_BEGIN
		push 0
		push SEEK_SIZE
		push hFile
		call SetFilePointer		; Move file pointer to where we're going to write ( +SEEK_SIZE )
		push 0
		push offset hFileHighSize
		push 2			; We need to write 2 bytes to the target
		push offset patch		; push the pointer offset of 'patch'
		push hFile		; push the file handle
		call WriteFile		; Write our bytes :D
		push 0
		push offset szPatchOKCaption
		push offset szPatchOK
		push 0
		call MessageBox

		xor eax,eax
		jmp das_ende

FilesizeNotOK:
		push 0
		push offset szPatchNOTOK
		push offset szFilesizeNotOK
		push 0
		jmp errormsg
				
SharingViolation:
		push 0
		push offset szPatchNOTOK
		push offset szSharingViolation
		push 0
		jmp errormsg
		
FileNotFound:
		push 0
		push offset szPatchNOTOK
		push offset FileNotFound
		push 0

errormsg:
		call MessageBox

das_ende:
		push hFile		; push the file handle one more time.
		call CloseHandle		; close it
		invoke ExitProcess,eax
		invoke PostQuitMessage,0


patcher endp

end start
