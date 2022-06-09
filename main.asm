%include "words.inc"
%include "lib.inc"

extern find_word

%define BUF_SIZE 256
%define ERROR_CODE 1
%define OK_CODE 0


section .data:
                error_message_out_of_bounds: db "out of bounds exception", 0
                error_message_word_no_found: db "word no found exception", 0


global _start
section .text
_start:
                sub rsp, BUF_SIZE
                mov rdi, rsp 
                mov rsi, BUF_SIZE 
                call read_word
                test rax, rax
                je .error

                mov rsi, DICTIONARY
                mov rdi, rax
                call find_word
                test rax, rax
                je .word_no_found

                add rax, 8                  ;добавили размер места для адреса памяти 
                mov rdi, rax                
                call string_length
                add rdi, rax                ;добавили размер ключа
                inc rdi                     ;добавили 1, потому что нуль-терминал
                call print_string
                mov rdi, OK_CODE
                jmp .end


.error:             mov rdi, error_message_out_of_bounds
                    jmp .error_exit

.word_no_found:     mov rdi,  error_message_word_no_found   
.error_exit:        call print_error
                    mov rdi, ERROR_CODE
.end:               add rsp, BUF_SIZE
                    call exit






    







    
