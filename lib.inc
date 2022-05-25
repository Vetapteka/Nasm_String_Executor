section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
                mov rax, 60
                xor rdi, rdi
                syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length: 
                mov rax, rdi
.loop:          mov bl, [rax]
                cmp bl, 0
                je .end
                inc rax
                jmp .loop      
.end:           sub rax, rdi
                ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
                call string_length
                mov rdx, rax
                mov rax, 1
                mov rsi, rdi
                mov rdi, 1
                syscall
                ret

; Принимает код символа и выводит его в stdout

print_char:
                push rdi
                mov rdi, rsp
                call print_string
                pop rdi
                ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
                mov rdi, 0xA


; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.

print_uint:
                mov rax, rdi
                push rcx
                mov r10, 10
                mov rcx, rsp
                sub rsp, 22
                dec rcx
                mov [rcx], byte 0

.loop:          xor rdx, rdx
                div r10
                add rdx, '0'
                dec rcx
                mov [rcx], dl
                test rax, rax
                jne .loop

                mov rdi, rcx
                call print_string
                add rsp, 22
                
                pop rcx
                ret



; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
                test rdi, rdi
                jl .change_sign
.end:           call print_uint
                ret
.change_sign:   push rdi
                mov rdi, 0x2D           ;minus
                call print_char
                pop rdi
                neg rdi
                jmp .end

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
                xor rcx, rcx
.loop:          
                mov r10b, byte[rdi+rcx]
                mov r11b, byte[rsi+rcx]
                cmp r10, r11
                jne .fail_end

                test r10b, r10b
                je .end
                inc rcx
                jmp .loop

.end:           xor rax, rax
                inc rax
                ret

.fail_end:      xor rax, rax
                ret




; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
                xor rax, rax
                push rax
                mov rdi, 0
                mov rsi, rsp
                mov rdx, 1
                syscall
                pop rax
                ret
    

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

%assign PEREVOD 0xA
%assign TAB 0x9
%assign SPACE 0x20
%assign READ 0


read_word:
                ;rdi - buffer
                ;rsi - размер буфера
                push r10
                push r12
                push r13
                dec rsi
                dec rsi                 ;for extra 0 terminal
                mov r12, rsi
                xor r10, r10
                xor r13, r13
                mov r13, rdi
                xor rax, rax


.prob_sym:      call read_char
                cmp al, PEREVOD
                je .prob_sym
                cmp al, TAB
                je .prob_sym
                cmp al, SPACE
                je .prob_sym
                cmp al, READ
                je .fail_end

.read:          mov [r13 + r10], al 

                call read_char
                cmp al, PEREVOD
                je .end
                cmp al, TAB
                je .end
                cmp al, SPACE
                je .end
                test al, al 
                je .end

                inc r10
                cmp r10, r12
                jle .read
                jmp .fail_end

         
.fail_end:      xor rax, rax
                xor rdx, rdx
                jmp .ret

.end:           inc r10
                xor rax, rax
                mov [r13 + r10], al 
               ; inc r10
                mov rax, r13
                mov rdx, r10

.ret:           pop r13
                pop r12
                pop r10
                ret



; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
                push rsi
                push r10
                mov rsi, rdi
                xor rdx, rdx
                xor rax, rax
                xor r10, r10
                cld
.loop:          lodsb
                cmp al, 0x30
                jl .end
                cmp al, 0x39
                jg .end
                imul r10, 10
                sub rax, 0x30
                add r10, rax
                inc rdx
                jmp .loop
                
.end:           mov rax, r10
                pop r10
                pop rsi
                ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
                push rsi
                mov rsi, rdi
                xor rax, rax
                cld
                lodsb
                cmp al, 0x2d
                je .neg
                jmp .read

.neg:           inc rdi
                call parse_uint
                neg rax
                inc rdx
                jmp .end
.read:          call parse_uint

.end:           pop rsi
                ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
                call string_length
                cmp rdx, rax
                push rcx
                mov rcx, rax
                mov rax, 0             ;becase mov doesn't change flags
                jl .end                ;buffer lower than str
                inc rcx

.loop:          mov r10b, [rdi + rax]
                mov [rsi + rax], r10b
                inc rax
                loop .loop  
                dec rax         

.end:           
                pop rcx
                ret
