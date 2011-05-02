#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "polarssl/md5.h"
#include "polarssl/sha1.h"
#include "polarssl/xtea.h"


#define WIN32_LEAN_AND_MEAN
#define IDBUTTON1 0x111
#define IDBUTTON2 0x222
#define IDBUTTON3 0x333
#define IDBUTTON4 0x444


char ClsName[] = "test1";
char WndName[] = "test2";
HWND btn1, btn2, btn3, btn4, edit1, edit2, edit3, edit4, edit5, edit6;

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
unsigned char char2hex(char chr);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow) {
	
	MSG        Msg;
	HWND       hWnd;
	WNDCLASSEX WndClsEx;

	WndClsEx.cbSize        = sizeof(WNDCLASSEX);
	WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
	WndClsEx.lpfnWndProc   = WndProcedure;
	WndClsEx.cbClsExtra    = 0;
	WndClsEx.cbWndExtra    = 0;
	WndClsEx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	WndClsEx.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	WndClsEx.lpszMenuName  = NULL;
	WndClsEx.lpszClassName = ClsName;
	WndClsEx.hInstance     = hInstance;
	WndClsEx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&WndClsEx);

	hWnd = CreateWindowEx(0, ClsName, WndName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 540, 370, NULL, NULL, hInstance, NULL);
	
	if( !hWnd ) {
		return FALSE;
	}
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while( GetMessage(&Msg, NULL, 0, 0) ) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	
    switch(Msg) {
    	
    case WM_COMMAND:
    	if (LOWORD(wParam) == IDBUTTON1) { // md5
    		unsigned char digest[32];
    		char buffer[256];
				SendMessage(edit1,WM_GETTEXT,sizeof(buffer)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buffer));
				
				md5((unsigned char *)buffer, strlen(buffer), digest);

				int i;
				char* addy = buffer;
				for (i = 0; i < 16; i++) {
					sprintf(addy,"%02x", digest[i]);
					addy += 2;
				}
				SendMessage(edit2, WM_SETTEXT, 0, (LPARAM)buffer);

    	} else if (LOWORD(wParam) == IDBUTTON2) { // sha1
    		unsigned char digest[32];
    		char buffer[256];
				SendMessage(edit1,WM_GETTEXT,sizeof(buffer)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buffer));
				
				sha1((unsigned char *)buffer, strlen(buffer), digest);

				int i;
				char* addy = buffer;
				for (i = 0; i < 20; i++) {
					sprintf(addy,"%02x", digest[i]);
					addy += 2;
				}
				SendMessage(edit2, WM_SETTEXT, 0, (LPARAM)buffer);
				
    	} else if (LOWORD(wParam) == IDBUTTON3 || (LOWORD(wParam) == IDBUTTON4)) { // xtea encrypt/decrypt
    		
    		unsigned int keys[4];
    		unsigned char key[16];
    		
    		char buffer[9];
				
				key[0] = 0x69;
    		SendMessage(edit3,WM_GETTEXT,sizeof(buffer)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buffer));
    		key[0] = (char2hex(buffer[0]) << 4) | char2hex(buffer[1]);
    		key[1] = (char2hex(buffer[2]) << 4) | char2hex(buffer[3]);
    		key[2] = (char2hex(buffer[4]) << 4) | char2hex(buffer[5]);
    		key[3] = (char2hex(buffer[6]) << 4) | char2hex(buffer[7]);
    		
    		SendMessage(edit4,WM_GETTEXT,sizeof(buffer)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buffer));
    		key[4] = (char2hex(buffer[0]) << 4) | char2hex(buffer[1]);
    		key[5] = (char2hex(buffer[2]) << 4) | char2hex(buffer[3]);
    		key[6] = (char2hex(buffer[4]) << 4) | char2hex(buffer[5]);
    		key[7] = (char2hex(buffer[6]) << 4) | char2hex(buffer[7]);

    		SendMessage(edit5,WM_GETTEXT,sizeof(buffer)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buffer));
    		key[8] = (char2hex(buffer[0]) << 4) | char2hex(buffer[1]);
    		key[9] = (char2hex(buffer[2]) << 4) | char2hex(buffer[3]);
    		key[10] = (char2hex(buffer[4]) << 4) | char2hex(buffer[5]);
    		key[11] = (char2hex(buffer[6]) << 4) | char2hex(buffer[7]);

    		SendMessage(edit6,WM_GETTEXT,sizeof(buffer)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buffer));
    		key[12] = (char2hex(buffer[0]) << 4) | char2hex(buffer[1]);
    		key[13] = (char2hex(buffer[2]) << 4) | char2hex(buffer[3]);
    		key[14] = (char2hex(buffer[4]) << 4) | char2hex(buffer[5]);
    		key[15] = (char2hex(buffer[6]) << 4) | char2hex(buffer[7]);


				xtea_context ctx;
    		unsigned char buf[17];
    		SendMessage(edit1,WM_GETTEXT,sizeof(buf)/sizeof(buffer[0]),reinterpret_cast<LPARAM>(buf));
    		
    		unsigned char input_block[8];
				int i;
				
    		for (i = 0; i < 8; i++) {
    			input_block[i] = (char2hex(buf[i * 2]) << 4) | char2hex(buf[i * 2 + 1]);
    		}
    		
        xtea_setup(&ctx, (unsigned char *) key);
        xtea_crypt_ecb(&ctx, (LOWORD(wParam) == IDBUTTON3)?XTEA_ENCRYPT:XTEA_DECRYPT, input_block, input_block);
        
        char output_block[8];
        char* addy = output_block;
        
        for (i = 0; i < 8; i++) {
        	sprintf(addy, "%02x", input_block[i]);
					addy += 2;
        }
        
        SendMessage(edit2, WM_SETTEXT, 0, (LPARAM)output_block);
				
    	}


    	break;

    	
    case WM_CREATE:
			/* edit controls */
			edit1 = CreateWindow("edit", "", WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,10,  10, 500, 100, hWnd, 0, GetModuleHandle(NULL), NULL);
			edit2 = CreateWindow("edit", "", WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,10, 220, 500, 100, hWnd, 0, GetModuleHandle(NULL), NULL);
			edit3 = CreateWindow("edit", "12345678", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_WANTRETURN|ES_CENTER,10, 195, 125, 25, hWnd, 0, GetModuleHandle(NULL), NULL);
			edit4 = CreateWindow("edit", "12345678", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_WANTRETURN|ES_CENTER,135, 195, 125, 25, hWnd, 0, GetModuleHandle(NULL), NULL);
			edit5 = CreateWindow("edit", "12345678", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_WANTRETURN|ES_CENTER,260, 195, 125, 25, hWnd, 0, GetModuleHandle(NULL), NULL);
			edit6 = CreateWindow("edit", "12345678", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_WANTRETURN|ES_CENTER,385, 195, 125, 25, hWnd, 0, GetModuleHandle(NULL), NULL);
			/* buttons */
			btn1 = CreateWindow("BUTTON", "md5", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,10,120,500,25,hWnd,(HMENU)IDBUTTON1,GetModuleHandle(NULL),NULL);
			btn2 = CreateWindow("BUTTON", "sha1", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,10,145,500,25,hWnd,(HMENU)IDBUTTON2,GetModuleHandle(NULL),NULL);
			btn3 = CreateWindow("BUTTON", "xtea encrypt", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,10,170,250,25,hWnd,(HMENU)IDBUTTON3,GetModuleHandle(NULL),NULL);
			btn4 = CreateWindow("BUTTON", "xtea decrypt", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,260,170,250,25,hWnd,(HMENU)IDBUTTON4,GetModuleHandle(NULL),NULL);

			HFONT hFont;
			hFont = CreateFont (16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "Arial");
			SendMessage (btn1, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (btn2, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (btn3, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (btn4, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (edit1, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (edit2, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (edit3, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (edit4, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (edit5, WM_SETFONT, WPARAM (hFont), TRUE);
			SendMessage (edit6, WM_SETFONT, WPARAM (hFont), TRUE);
			
			break;
    	
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

unsigned char char2hex(char chr) {
	
	char hex[] = "0123456789abcdef";
	int i;	
	
	for (i = 0; i < 16; i++) {
		if (chr == hex[i]) {
			break;
		}
	}

	return i;
	
}