global find_word

extern string_equals

section .text

; Указатель на нуль-терминированную строку в rdi
; Указатель на начало словаря в rsi
; результат в rax

find_word:
                add rsi, 8
                call string_equals
                sub rsi, 8
                cmp rax, 1
                je .found
                mov rsi, [rsi]
                test rsi, rsi
                je .not_found
                jmp find_word

.found:         mov rax, rsi
                ret

.not_found:     xor rax, rax
                ret


