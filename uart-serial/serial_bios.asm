;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; File : serial_bios.asm
;; 
;; --- BIOS ---
;;
;; Tasos Latsas
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
	TITLE	serial_bios
	DOSSEG
	.STACK 100h
	.MODEL SMALL
	
	.DATA
; store variables
ComPort    DW ?
CharAscii  DB ?

; Com selection message
comMsg0    DB "-----------------",13,10
comMsg1    DB " Select Com Port",13,10
comMsg2    DB "-----------------",13,10
comMsg3    DB "[1] -> COM 1",13,10
comMsg4    DB "[2] -> COM 2",13,10,">$"

; com parameter table
parMsg0    DB 13,10,13,10,"---------------------------------------------------------------",13,10
parMsg1    DB " Transfer rate        Parity        End bit        Char length",13,10
parMsg2    DB "---------------------------------------------------------------",13,10
parMsg3    DB "[0] -> 110  bps    [0] -> none    [1] -> 1 bit    [7] -> 7 bits",13,10
parMsg4    DB "[1] -> 150  bps    [1] -> odd     [2] -> 2 bits   [8] -> 8 bits",13,10
parMsg5    DB "[2] -> 300  bps    [2] -> even",13,10,"[3] -> 600  bps",13,10,"[4] -> 1200 bps",13,10
parMsg6    DB "[5] -> 2400 bps",13,10,"[6] -> 4800 bps",13,10,"[7] -> 9600 bps",13,10,"$"

; parameter messages
transfMsg  DB 13,10,"Select Transfer Rate",13,10,">$"
parityMsg  DB 13,10,"Select Parity",13,10,">$"
endbitMsg  DB 13,10,"Select End Bit number",13,10,">$"
charLenMsg DB 13,10,"Select Character length",13,10,">$"

; error messages
comErr     DB 13,10,"Wrong COM selection, Try again!",13,10,"$"
BitErr     DB 13,10,"Error! Unavailable Selection!",13,10,"$"

; (err_hndl)
;###OverrunErr DB 13,10,"Overrun error...",13,10,"$"
ParityErr  DB 13,10,"Parity error...",13,10,"$"
FrameErr   DB 13,10,"Framing error...",13,10,"$"
BreakErr   DB 13,10,"Brake error...",13,10,"$"
TimeErr    DB 13,10,"Time-out error...",13,10,"$"
;###GenErr     DB 13,10,"ERROR!",13,10,"$"

; msgs
TrmStart   DB 13,10,13,10,"*** Terminal session started ***",13,10,">$"
TrmEnd     DB 13,10,"*** Terminal session ended ***$"

;===============================================================================

	.CODE
Start:
	mov    ax, @DATA
	mov    ds, ax
	mov    bp, sp
	

DefineCOM:
	; Display msg
	mov    ah, 09h
	lea    dx, comMsg0
	int    21h
	
	; Read Com
	mov    ah, 01h
	int    21h
	
	sub    al, 31h                       ; sub input - 1
	
	cmp    al, 1
	ja     DispComErrMsg
	
	xor    ah, ah                        ; ah <- 0
	mov    ComPort, ax                   ; store com port in var
	jmp short   DefineParameters
 ;********************************
 ; Error handler - Com Sel.
 DispComErrMsg:
	mov    ah, 09h
	lea    dx, comErr
	int    21h
	jmp    DefineCom
 ;********************************

;===============================================================================

DefineParameters:
	xor    bl, bl                        ; clear bl for parameter input  
    ; Display parameter table
	mov	   ah, 09h
   	lea	   dx, parMsg0
	int    21h

;===============================================================================

TransferRate:                            
    ; Display msg - transfer sel. prompt            
    mov    ah, 09h
	lea    dx, transfMsg
	int    21h

    mov    ah, 01h                       ; read
	int    21h
    
    sub    al, 30h                       ; ascii -> number
        
    cmp    al, 7                         ; check for wrong input
    ja     TransfErrMsg                  ; sel E [1,8]
    
    xor    ah, ah                        ; clear ah
    push   ax                            ; ax is "Num2Baud" input
    call   Num2Baud
    pop    ax                            ; get return value (baud rate)
    mov    bl, al                        ; place value in bl
       
    jmp short Parity                     ; continue with parity setup
 ;********************************
 ; Error handler	
 TransfErrMsg:
    mov    ah, 09h
	lea    dx, BitErr
	int    21h
	xor    bl, bl
	jmp    TransferRate                  ; start over
 ;********************************

;===============================================================================

Parity:
    ; Display msg
    mov    ah, 09h
	lea    dx, parityMsg
	int    21h

    ; read
    mov    ah, 01h
	int    21h
	
    sub    al, 30h                       ; ascii -> number

    cmp    al, 2
    ja     ParityErrMsg
    
    xor    ah, ah
    push   ax
    call   Num2Parity
    pop    ax
    or     bl, al
    
    jmp short EndBit     
;********************************
; Error handler
ParityErrMsg:
    mov    ah, 09h
	lea    dx, BitErr
	int    21h
	jmp    Parity
;********************************

;===============================================================================

EndBit:
    ; Display msg
    mov    ah, 09h
	lea    dx, endbitMsg
	int    21h

    ; read
    mov    ah, 01h
	int    21h
	
    sub    al, 31h

    cmp    al, 1
    ja     EndBitErrMsg
    
    cmp    al, 0
    je     OneEndBit
    cmp    al, 1
    je     TwoEndBits
    
OneEndBit:
    or     bl, 00h
    jmp short CharLen
TwoEndBits:
    or     bl, 04h
    jmp short CharLen
;********************************
; Error handler
EndBitErrMsg:
    mov    ah, 09h
	lea    dx, BitErr
	int    21h
	jmp    EndBit
;********************************

;===============================================================================

CharLen:
    ; Display msg
    mov    ah, 09h
	lea    dx, charLenMsg
	int    21h

    ; read
    mov    ah, 01h
	int    21h
	
    sub    al, 37h
        
    cmp    al, 1
    ja     CharLenErrMsg
    
    cmp    al, 0
    je     SevenBits
    cmp    al, 1
    je     EightBits
    
SevenBits:
    or     bl, 02h 
    jmp short Initialize
EightBits:
    or     bl, 03h
    jmp short Initialize
;********************************
; Error handler    
CharLenErrMsg:
    mov    ah, 09h
	lea    dx, BitErr
	int    21h
	jmp    CharLen
;********************************

;===============================================================================

Initialize:
    mov    ah, 00h                       ; serial port initialization
    mov    al, bl                        ; al <- com settings
    mov    dx, ComPort                   ; com selection
    int    14h

; disp start msg
    mov	ah, 09h
    lea	dx, TrmStart
    int 	21h
    
MainLoop:
;check
    mov    ah, 01h                       ; check if there is a char in buffer
    int    16h
    jnz    Send                          ; if key pressed then goto send

;receive                                 ; else    
    mov    ah, 03h                       ; check if a char is received
    mov    dx, ComPort
    int    14h
    
    test   ah, 01h
    jz     MainLoop                      ; if not start over
    
    mov    ah, 02h                       ; else place it in al (ASCII) -read
    mov    dx, ComPort
    int    14h
    
    push   ax
    call   RC_ERROR_HANDLER
    pop    ax
    
    cmp    al, 1Bh                       ; if key = ESC then terminate
    je     ExitMainLoop
                                         ; else
    mov    dl, al                        ; disp char
    mov    ah, 02h
    int    21h
        
    jmp    MainLoop                      ; start over

Send:
    mov    cl, al
    
    mov    ah, 00h                       ; read char
    int    16h 
    
Transfer:    
    mov    ah, 03                        ; get Port status
    mov    dx, ComPort
    int    14h
    
    mov    al, cl
    
    test   ah, 20h                       ; check if send register is empty
    jz     Transfer                      ; wait until is empty

    mov    ah, 01h                       ; send the char
    mov    dx, ComPort
    int    14h
    
    cmp    al, 1Bh                       ; if key = ESC then terminate
    je     ExitMainLoop
    
    mov    dl, al                        ; disp pressed key (local echo)
    mov    ah, 02h
    int    21h
         
    jmp    MainLoop                      ; start over
    
ExitMainLoop:
; disp end msg
    mov	ah, 09h
    lea	dx, TrmEnd
    int 	21h
; quit program
	mov	   ax, 4C00h					; exit program
	int	   21h


;===============================================================================
; procedures

RC_ERROR_HANDLER   PROC Near
    push   bp
    mov    bp, sp

    push   ax
    push   bx
    push   dx
;    jmp short exit_proc

    mov    bx, [bp+4]                   ; bx <- com status
    mov    ah, 09h                      ; for msg output

;check parity
    test   bh, 04h
    jz     stopbit_check
    lea    dx, ParityErr
    int    21h
    
stopbit_check:
    test   bh, 08h
    jz     break_check
    lea    dx, FrameErr
    int    21h

break_check:
    test   bh, 10h
    jz     timedout_check
    lea    dx, BreakErr
    int    21h

timedout_check:
    test   bh, 80h
    jz     exit_proc
    lea    dx, TimeErr
    int    21h
    
exit_proc:
    pop    dx
    pop    bx
    pop    ax
    pop    bp
    
    ret
RC_ERROR_HANDLER   ENDP

;-----------------------------------------------------

Num2Baud        PROC Near
; input: ax, al contains an integer (range 1-8)
; output: a 3bit number representing the baud rate
    push   bp
    mov    bp, sp
    push   bx                           ; save bx
    mov    bx, [bp+4]                   ; bx <- ax
    
    cmp    bl, 0
    je     Baud110
    
    cmp    bl, 1
    je     Baud150
    
    cmp    bl, 2
    je     Baud300
    
    cmp    bl, 3
    je     Baud600
    
    cmp    bl, 4
    je     Baud1200
    
    cmp    bl, 5
    je     Baud2400
    
    cmp    bl, 6
    je     Baud4800
    
    cmp    bl, 7
    je     Baud9600
    
Baud110:
    mov    bl, 00h
    jmp short ExitNum2Baud
Baud150:
    mov    bl, 20h
    jmp short ExitNum2Baud
Baud300:
    mov    bl, 40h
    jmp short ExitNum2Baud
Baud600:
    mov    bl, 60h
    jmp short ExitNum2Baud
Baud1200:
    mov    bl, 80h
    jmp short ExitNum2Baud
Baud2400:
    mov    bl, 0A0h
    jmp short ExitNum2Baud
Baud4800:
    mov    bl, 0C0h
    jmp short ExitNum2Baud
Baud9600:
    mov    bl, 0E0h
    jmp short ExitNum2Baud

ExitNum2Baud:
    mov    [bp+4], bx                   ; return value
    pop    bx
    pop    bp

    ret
Num2Baud        ENDP

;-----------------------------------------------------

Num2Parity      PROC Near
; input: ax, al contains an integer (range 1-3)
; output: a 3bit number representing the parity
    push   bp
    mov    bp, sp
    push   bx
    mov    bx, [bp+4]
    
    cmp    bl, 0
    je     ParityNONE
    
    cmp    bl, 1
    je     ParityODD
    
    cmp    bl, 2
    je     ParityEVEN
    
ParityNONE:
    mov    bl, 00h
    jmp short ExitNum2Parity
ParityODD:
    mov    bl, 08h
    jmp short ExitNum2Parity
ParityEVEN:
    mov    bl, 18h

ExitNum2Parity:
    mov    [bp+4], bx
    pop    bx
    pop    bp
    
    ret
Num2Parity      ENDP

;-----------------------------------------------------

END Start
