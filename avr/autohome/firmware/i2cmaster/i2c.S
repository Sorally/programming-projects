;The device address constant (ADDR24LC64 in this case) needs to match that of the 
;EEPROM being used. The 24LC64 and 24LC256 use address A0 when their address pins are tied low. 

;There are two main subroutines of interest. 

;WriteI2Mem writes the byte contained in register "I2WData" into the EEPROM location 
;pointed to by ZH,ZL. 

;ReadI2Mem reads the byte at the EEPROM location pointed to by ZH,ZL and returns the byte in 
;register "temp". 

;**************Start I2C Memory Driver Constants and Registers 
;temp and I2WDtata must be high registers 
; 
.def   DCounter   = r2 
.def   temp      = r16   ; 
.def   I2WData   = r20   ; Write data for EEPROM routines 

; I2C Device addressing 
.equ   ARDUINO   = 0xA0   ; 'LC64, 'LC128, and 'LC256 address byte 
.equ   I2CREAD   = 0x01   ; read/write bit = read 
.equ   I2CWRITE   = 0x00   ; read/write bit = write 
.equ   I2CDelayConstant=$04    ; Constant for delay loop- change as function of uC clock frequency 

; IO Port Bits 
.equ   bSDA      = 1   ;* SET THIS The bit on the I/O Port used for SDA 
.equ   bSCL      = 0   ;* SET THIS The bit on the I/O Port used for SCL 
.equ   mSDA      = (1<<bSDA);Calculate binary value corresponding to I/O port bit for masking 
.equ   mSCL      = (1<<bSCL);Calculate binary value corresponding to I/O port bit for masking 

;Port B 
.equ   I2CPORT    = PORTB   ;*Set to correspond to I/O Port used for I2C 
.equ   I2CDDR     = DDRB   ;*Set to correspond to I/O Port used for I2C 
.equ   I2CPIN     = PINB   ;*Set to correspond to I/O Port used for I2C 

;----------------------------------------------------------------- 
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 
;      START I2C ROUTINES 
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 
;----------------------------------------------------------------- 
I2CDelay: 
   push   temp 
   ldi   temp,I2CDelayConstant 
delaymore: 
   dec   temp 
   brne   delaymore 
   pop   temp 
   ret 
;-----------------------------------------------------------------    
; Initialize I2C interface, ports, etc. 
I2CInit: 
   ; Set up port assignments 
   ldi   temp,mSCL      ; SCL only output by default 
   out   I2CDDR,temp       
   out   I2CPORT,temp   ; All outputs high, input pullups disabled 
    
   cbi   I2CPORT,bSDA   ; Data line always drives low if driven 
   cbi   I2CDDR,bSDA      ; SDA as input to start with - floats high 
   cbi   I2CPORT,bSCL   ; Set SCL low to try and avoid... 
   sbi   I2CDDR,bSCL      ; ...false start transitions 
   rcall   I2CDelay 
   rjmp   I2CStop      ; Stop any erroneous transfer 
;----------------------------------------------------------------- 
; I2CStart 
; Send I2C start condition 
; Assumes SCL and SDA are high to start with. 
; Leaves SCL and SDA low 
I2CStart: 
   rcall   I2CDelay    
   sbi   I2CDDR,bSDA      ; Drive data line low (start bit) 
   rcall   I2CDelay 
   cbi   I2CPORT,bSCL   ; Clock line low (ready to start) 
   ret    
;----------------------------------------------------------------- 
; I2CStop 
; Send I2C stop condition 
; Assumes SCL is low to start with, SDA may be in either state 
; Leaves SCL and SDA high 
I2CStop: 
   sbi   I2CDDR,bSDA      ; SDA driven low 
   rcall   I2CDelay    
   sbi   I2CPORT,bSCL   ; Clock line high (ready to stop) 
   rcall   I2CDelay    
   cbi   I2CDDR,bSDA      ; Data line floats high (stop bit) 
   rcall   I2CDelay 
   ret 
;----------------------------------------------------------------- 
ReadI2Mem:   ; Reads external memory at Z to temp 
   rcall   I2CStart 
   ldi   I2WData,(ADDR24LC64|I2CWRITE) 
   rcall   I2CSendAddress 
   brne   ReadI2MemError   ; No ack! 
   mov   I2WData,ZH 
   rcall   I2CSendByte 
   mov   I2WData,ZL 
   rcall   I2CSendByte 
   rcall   I2CStop 
   rcall   I2CDelay 
   rcall   I2CStart 
   ldi   I2WData,(ADDR24LC64|I2CREAD) 
   rcall   I2CSendAddress 
   brne   ReadI2MemError   ; No ack! 
   ldi   I2WData,1      ; Don't send ack 
   rcall   I2CReadByte 
   ret          
ReadI2MemError: 
   rcall   I2CStop 
   clr   DCounter 
ReadI2MemErrorDelay: 
   dec   DCounter 
   brne   ReadI2MemErrorDelay 
   rjmp   ReadI2Mem   ; and try again 
;----------------------------------------------------------------- 
WriteI2Mem:            ; Writes I2WData to external memory at Z 
   mov   temp,I2WData   ; save value 
   rcall   I2CStart 
   ldi   I2WData,(ADDR24LC64|I2CWRITE) 
   rcall   I2CSendAddress 
   brne   WriteI2MemError   ; No ack! 
   mov   I2WData,ZH 
   rcall   I2CSendByte 
   mov   I2WData,ZL 
   rcall   I2CSendByte 
   mov   I2WData,temp 
   rcall   I2CSendByte 
   rcall   I2CStop 
   ret 

WriteI2MemError: 
   rcall   I2CStop 
   clr   DCounter 
WriteI2MemErrorDelay: 
   dec   DCounter 
   brne   WriteI2MemErrorDelay 
   mov   I2WData,temp    ; restore data value 
   rjmp   WriteI2Mem      ; and try again 
;------------------------------------------------------------------ 
; I2CReadByte Read a byte from the I2C bus and acknowledge 
; If I2WData is zero, acknowledge is sent, otherwise not. 
; Byte is returned in temp 
; Leaves SCL low, I2C bus not driven. 
I2CReadByte: 
   ldi   temp,8 
   mov   DCounter,temp 
   clr   temp 
   cbi   I2CDDR,bSDA      ; release bus (floats high)    
I2CReadBits: 
   sbi   I2CPORT,bSCL   ; clock high 
   rcall   I2CDelay    
   add   temp,temp    
   sbic   I2CPIN,bSDA      ; if SDA clear, skip increment 
   inc   temp    
   cbi   I2CPORT,bSCL   ; clock low 
   rcall   I2CDelay 
   dec   DCounter 
   brne   I2CReadBits 
   tst   I2WData      ; Send an ACK...? 
   brne   I2CReadBitsNack    
   sbi   I2CDDR,bSDA      ; yes... drive data bus low 
I2CReadBitsNack: 
   sbi   I2CPORT,bSCL   ; clock high 
   rcall   I2CDelay 
   cbi   I2CPORT,bSCL   ; clock low 
   rcall   I2CDelay 
   cbi   I2CDDR,bSDA      ; release bus (floats high) 
   ret 
;------------------------------------------------------------------ 
; I2CSendByte 
; Sends 8 bits of data from I2WData and listens for an ACK at the 
; end. Returns Z if ack received, else NZ. 
; Assumes a start condition has already been sent, and SDA and SCL 
; are low Leaves SCL low, SDA not driven. Uses DCounter and I2WData. 
I2CSendAddress: 
I2CSendByte: 
   push   temp 
   ldi   temp,8 
   mov   DCounter,temp 
   pop   temp 
I2CSendBits: 
   add   I2WData,I2WData 
   brcc   I2CSendBits0 
   cbi   I2CDDR,bSDA      ; send 1 (floats high) 
   rjmp   I2CSendBits1 
I2CSendBits0: 
   sbi   I2CDDR,bSDA      ; send 0 (drives low) 
I2CSendBits1:    
   rcall   I2CDelay 
   sbi   I2CPORT,bSCL   ; clock high 
   rcall   I2CDelay 
   cbi   I2CPORT,bSCL   ; clock low 
   dec   DCounter 
   brne   I2CSendBits 
   cbi   I2CDDR,bSDA      ; stop driving data bus 
   rcall   I2CDelay 
   sbi   I2CPORT,bSCL   ; clock high 
   rcall   I2CDelay 
   in   I2WData,I2CPIN   ; sample data bus 
   cbi   I2CPORT,bSCL   ; clock low 
   rcall   I2CDelay 
   andi   I2WData,mSDA   ; check ack bit (z set if OK) 
   ret 
;------------------------------------------------------------------