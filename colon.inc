%define DICTIONARY 0

%macro colon 2
	%%add_new_el: dq DICTIONARY
        %define DICTIONARY %%add_new_el
	db %1, 0
	%2:
%endmacro
	