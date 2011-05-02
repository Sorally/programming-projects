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



DlgProc proc hWin:HWND,uMsg:UINT,wParam:WPARAM,lParam:LPARAM

	mov	eax,uMsg
	.if eax==WM_INITDIALOG
		mov ecx, hWin
		mov hWnd, ecx
		invoke GetDlgItem,hWin,1001
		mov edit1, eax
		invoke GetDlgItem,hWin,1002
		mov butn1, eax
		invoke GetDlgItem,hWin,1003
		mov selBox, eax
		
		mov part3,0
		invoke EnableWindow, butn1, FALSE
		invoke EnableWindow, edt1, FALSE
		invoke SendMessage,selBox,CB_ADDSTRING,0,offset sel1
		invoke SendMessage,selBox,CB_ADDSTRING,0,offset sel2
		

		
	.elseif eax==WM_COMMAND
		mov		eax,wParam
		mov		edx,eax
		shr		edx,16
		and		eax,0FFFFh
		.if wParam==1002
	        invoke genkey
		.elseif edx==CBN_SELCHANGE
			invoke SendMessage, selBox, CB_GETCURSEL, 0, 0
   			.if eax == CB_ERR
       			jmp err1	
       		.endif
   			.if eax == 0
   				mov part3, 5E2544h ;noiseware
   			.elseif eax == 1
   				mov part3, 5E2573h ;portraiture
   			.else
   				jmp err1  
   			.endif
			invoke EnableWindow, butn1, TRUE
			invoke genkey
		.endif
	.elseif eax==WM_CLOSE
		invoke EndDialog,hWin,0
	.else
		mov eax,FALSE
		ret
	.endif
	err1:
	mov eax,TRUE
	ret

DlgProc endp

genkey proc

invoke timeGetTime
and eax,0FFFFh
mov ecx, eax
and ecx, 0FF00h
shr ecx, 8

.if eax == 0FFFFh
    mov eax,0
.endif

.if ecx == 0FFh
    mov ecx,0
.endif

shl ecx, 24
;or ecx, 5E2544h ;noiseware
;or ecx, 5E2573h  ; portratisfsfsd
or ecx, part3

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

end start
