;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; File : serial_uart.asm
;; 
;; --- UART ---
;;
;; Tasos Latsas
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
	TITLE	serial_uart
	DOSSEG
	.STACK 100h
	.MODEL SMALL
		
	.DATA
; store variables
COM1       DW ?
COM2       DW ?
ComPort    DW ?
LineCtrl   DB ?

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
parMsg3    DB "[0] -> 600    bps   [0] -> none    [1] -> 1 bit    [5] -> 5 bits",13,10
parMsg4    DB "[1] -> 1200   bps   [1] -> odd     [2] -> 2 bits   [6] -> 6 bits",13,10
parMsg5    DB "[2] -> 2400   bps   [2] -> even                    [7] -> 7 bits",13,10
parMsg51   DB "[3] -> 4800   bps                                  [8] -> 8 bits",13,10,"[4] -> 9600   bps",13,10
parMsg6    DB "[5] -> 19,2K  bps",13,10,"[6] -> 38,4K  bps",13,10,"[7] -> 57,6K  bps",13,10,"[8] -> 115,2K bps",13,10,"$"

; parameter messages
transfMsg  DB 13,10,"Select Transfer Rate",13,10,">$"
parityMsg  DB 13,10,"Select Parity",13,10,">$"
endbitMsg  DB 13,10,"Select End Bit number",13,10,">$"
charLenMsg DB 13,10,"Select Character length",13,10,">$"

; error messages
comErr     DB 13,10,"Wrong COM selection, Try again!",13,10,"$"
BitErr     DB 13,10,"Error! Unavailable Selection!",13,10,"$"
; (err_hndl)
ParityErr  DB 13,10,"Parity error...",13,10,"$"
FrameErr   DB 13,10,"Framing error...",13,10,"$"
BreakErr   DB 13,10,"Brake error...",13,10,"$"

; msgs
CrLfPrompt DB 13,10,">$"
EraseChar  DB 8," ",8,"$"
TrmStart   DB 13,10,13,10,"*** Terminal session started ***",13,10,">$"
TrmEnd     DB 13,10,"*** Terminal session ended ***$"

;===============================================================================

	.CODE
Start:
	mov    ax, @DATA
	mov    ds, ax
	mov    bp, sp
	
; get com port addresses
; store addresses in COM1, COM2
    call    GetComAddress

DefineCOM:
	; Display msg
	mov    ah, 09h
	lea    dx, comMsg0
	int    21h
	
	; Read Com
	mov    ah, 01h
	int    21h
	
	sub    al, 31h                       ; sub input - 1
	
	cmp    al, 0
	je     com_one
	cmp    al, 1
	je     com_two

;********************************
 ; Error handler - Com Sel.
 DispComErrMsg:
	mov    ah, 09h
	lea    dx, comErr
	int    21h
	jmp    DefineCom
 ;********************************
	
com_one:
    mov    dx, COM1
    mov    ComPort, dx
    jmp short DefineParameters

com_two:
    mov    dx, COM2
    mov    ComPort, dx

;===============================================================================

DefineParameters:  
    ; Display parameter table
	mov	   ah, 09h
   	lea	   dx, parMsg0
	int    21h

TransferRate:                            
    ; Display msg - transfer sel. prompt            
    mov    ah, 09h
	lea    dx, transfMsg
	int    21h

    mov    ah, 01h                       ; read
	int    21h
    
    sub    al, 30h                       ; ascii -> number
        
    cmp    al, 8                         ; check for wrong input
    ja     TransfErrMsg                  ; sel E [0,8]
    
    xor    ah, ah                        ; clear ah
    push   ax                            ; ax is "Num2Baud" input
    call   Num2Baud

    mov    dx, ComPort			 
    add	   dx, 3
    mov    al, 80h
    out	   dx, al			             ; set DLAB = 1
    sub    dx, 3
    
    pop    ax                            ; get Num2Baud return value (baud rate)
    
    out    dx, al		                 ; set baud rate (LSB)
    add    dx, 1
    xor    al, al
    out    dx, al                        ; (MSB)      

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

;=================================

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
    pop    bx
    
    mov    LineCtrl, bl
    
    jmp short EndBit     
;********************************
; Error handler
ParityErrMsg:
    mov    ah, 09h
	lea    dx, BitErr
	int    21h
	jmp    Parity
;********************************

;================================

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

    jmp short TwoEndBits
    
OneEndBit:
    or     LineCtrl, 00h
    jmp short CharLen
TwoEndBits:
    or     LineCtrl, 04h
    jmp short CharLen
;********************************
; Error handler
EndBitErrMsg:
    mov    ah, 09h
	lea    dx, BitErr
	int    21h
	jmp    EndBit
;********************************

;================================

CharLen:
    ; Display msg
    mov    ah, 09h
	lea    dx, charLenMsg
	int    21h

    ; read
    mov    ah, 01h
	int    21h
	
    sub    al, 35h
        
    cmp    al, 3
    ja     CharLenErrMsg
    
    cmp    al, 0
    je     FiveBits
    cmp    al, 1
    je     SixBits
    cmp    al, 2
    je     SevenBits
    cmp    al, 3
    je     EightBits
    
FiveBits:
    or     LineCtrl, 0
    jmp short Initialize
SixBits:
    or     LineCtrl, 1
SevenBits:
    or     LineCtrl, 2 
    jmp short Initialize
EightBits:
    or     LineCtrl, 3
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
    mov    dx, ComPort
    add    dx, 3
    xor    al, al
    out    dx, al                                ; set DLAB = 0
    mov    al, LineCtrl
    out    dx, al
    
; disp start msg
    mov	   ah, 09h
    lea	   dx, TrmStart
    int    21h
    
MainLoop:
;check
    mov    ah, 01h                       ; check if there is a char in buffer
    int    16h
    jnz    Send                          ; if key pressed then goto send

;===============================================================================

;receive
    mov    dx, ComPort                   ; get line status
    add    dx, 5                         
    in     al, dx
    test   al, 01h                       ; if no char then start over
    jz     MainLoop

    sub    dx, 5
    in     al, dx
    
    push   ax                            ; save char
    add    dx, 5                         ; goto line status address
    in     al, dx                        ; get line status
    push   ax                            ; push line status for error handler
    call   RC_ERROR_HANDLER              ; call error handler
                                         ; proc uses ret 2, no pop needed
    pop    ax                            ; restore char

    cmp    al, 1Bh                       ; if key = ESC then terminate
    je     ExitMainLoop
    
    jmp short Display

;===============================================================================

Send:
    mov    cl, al    
    mov    ah, 00h                       ; read char
    int    16h 

Transfer:    
    mov    dx, ComPort                   ; get line status
    add    dx, 5                         
    in     al, dx
        
    test   al, 20h                       ; check if send register is empty
    jz     Transfer                      ; wait until is empty
    
    mov    al, cl
    
    sub    dx, 5
    out    dx, al
    
    cmp    al, 1Bh                       ; if key = ESC then terminate
    je     ExitMainLoop                  ; else display char
        
;===============================================================================
Display:
; this code part is used from both receive+transfer
; it displays characters + escape chars on screen 
    cmp    al, 0Dh                       ; key = enter, change line
    je     NewLine
    cmp    al, 08h                       ; key = backspace, erase
    je     Erase
    
    mov    dl, al                        ; disp pressed key
    mov    ah, 02h
    int    21h
         
    jmp    MainLoop                      ; start over
    
NewLine:                                 ; change line
    mov    ah, 09h
    lea    dx, CrLfPrompt
    int    21h
    
    jmp    MainLoop
    
Erase:
    mov    ah, 09h
    lea    dx, EraseChar
    int    21h

    jmp    MainLoop

;===============================================================================

ExitMainLoop:
; disp end msg
    mov	   ah, 09h
    lea	   dx, TrmEnd
    int    21h
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
    jz     exit_proc
    lea    dx, BreakErr
    int    21h

exit_proc:
    pop    dx
    pop    bx
    pop    ax
    pop    bp
    
    ret    2
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
    je     Baud600
    
    cmp    bl, 1
    je     Baud1200
    
    cmp    bl, 2
    je     Baud2400
    
    cmp    bl, 3
    je     Baud4800
    
    cmp    bl, 4
    je     Baud9600
    
    cmp    bl, 5
    je     Baud19_2k
    
    cmp    bl, 6
    je     Baud38_4k
    
    cmp    bl, 7
    je     Baud57_6k

    cmp    bl, 8
    je     Baud115_2k
    
Baud600:
    mov    bl, 0C0h
    jmp short ExitNum2Baud
Baud1200:
    mov    bl, 60h
    jmp short ExitNum2Baud
Baud2400:
    mov    bl, 30h
    jmp short ExitNum2Baud
Baud4800:
    mov    bl, 18h
    jmp short ExitNum2Baud
Baud9600:
    mov    bl, 0Ch
    jmp short ExitNum2Baud
Baud19_2k:
    mov    bl, 0Ch
    jmp short ExitNum2Baud
Baud38_4k:
    mov    bl, 03h
    jmp short ExitNum2Baud
Baud57_6k:
    mov    bl, 02h
    jmp short ExitNum2Baud
Baud115_2k:
    mov    bl, 01h

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
    mov    bl, 30h
    jmp short ExitNum2Parity
ParityODD:
    mov    bl, 28h
    jmp short ExitNum2Parity
ParityEVEN:
    mov    bl, 38h

ExitNum2Parity:
    mov    [bp+4], bx
    pop    bx
    pop    bp
    
    ret
Num2Parity      ENDP

;-----------------------------------------------------

GetComAddress   PROC Near
; uses bios to get com1+2 addresses
    push   bp
    mov    bp, sp
    push   ax
    push   bx
    push   si
    push   es
    
    mov    bx, 40h
    mov    es, bx
    xor    si, si
    mov    ax, es:[si]
    mov    COM1, ax
    
    mov    si, 02h
    mov    ax, es:[si]
    mov    COM2, ax
        
    pop    es
    pop    si
    pop    bx
    pop    ax
    pop    bp
    
    ret
GetComAddress    ENDP
    
;-----------------------------------------------------

END Start
