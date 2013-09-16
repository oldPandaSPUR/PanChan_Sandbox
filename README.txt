*****Design:
There are 11 General Purpose Registers, indexed from 0 to 10.
GPR0 is the Return Register.
	Return register may not be directly modified (using SET or MOVE, for example)
	Return register is always listed as valid.
	All arithmetic operations store their result ONLY in the Return Register.
GPR1-10 are the Result Registers. These registers are printed (if valid content exists) at the end of execution.
There are 64 Memory Locations, indexed from 0 to 63.
All registers and memory are initialized with value 0 and set to invalid.
Location Validity:
	GPR0 (Return Register) is always valid.
	GPRs1-10 and Memory initialized as invalid.
	Invalid location may not be read.
	Invalid becomes Valid when data is loaded from a valid location.
All Memory Locations and Registers hold signed integers.
	-2147483648 < signed integers < 2147483648
Instructions are stored in a separate part of memory, limited only by client hardware.


*****Exception Handling:
The sandbox will terminate without printing results if any erroneous operation is attempted.
Conditions that may cause termination include:
	Invalid instructions/syntax
	Access of invalid locations.
	Access of out-of-scope locations.
	Attempts to directly modify GPR0.
	Integer overflows.
	General arithmetic calculation errors.
Sandbox will also throw error codes if termination occurs.
See end of this file for Error Codes.


*****Programming Syntax:
Programs are written in a text file.
	UTF-8 Encoded with Unix/Linux format line endings.
Each line of the text file is one instruction, formatted as follows:
	INSTRUCTION ARGUMENT_1 ARGUMENT_2 ARGUMENT_N
All arguments must be signed integers.
	-2147483648 < signed integers < 2147483648
	No leading zeros. (i.e. no 001, 00, etc)
Program ends when the end of the file is reached.


*****Instruction Set:
Arithmetic:
	ADD [GPRa] [GPRb]
		Adds the values within the two GPRs specified.
		Danger of integer overflow, which would cause program termination.
		
	SUB [GPRa] [GPRb]
		Subtracts the value of GPRb from GPRa.
		Danger of integer overflow, which would cause program termination.
		
	MULT [GPRa] [GPRb]
		Multiplies GPRa by GPRb.
		Danger of integer overflow, which would cause program termination.
		
	DIV [GPRa] [GPRb]
		Divides GPRa by GPRb.
		Divide by 0 will cause program termination.

Comparison and Conditional Control:
	EQL [GPRa] [GPRb]
		Compares contents of GPRa to contents of GPRb.
		If contents are equal, 1 is stored in GPR0.
		Else, 0 is stored in GPR0
		
	JMP [GPRa]
		Unconditional jump to instruction location indicated by GPRa.
		Invalid instruction location will cause program termination.
		
	JMPIF [GPRa]
		Jump to instruction location indicated by GPRa if and only if GPR0==1.
		Invalid instruction location will cause program termination.
		
	JMPIFN [GPRa]
		Jump to instruction location indicated by GPRa if and only if GPR0==0.
		Invalid instruction location will cause program termination.

Register Manipulation:
	SET [GPRa] [Value]
		Set GPRa contents to Value specified.
		Value must be an integer.
			If Value is larger than 2147483647 or less than -2147483648, program will terminate.
		Destination register's valid flag is set to 1.
		Destination register is overwritten.

	COPY [GPRa] [GPRb]
		Copy from GPRb to GPRa, if GPRb is a valid GPR and both GPR addresses exist.
		GPRa is set to valid, and overwritten.
		GPRb is left alone.

	CLR [GPRa]
		Set register to be invalid. Does not overwrite register.
		Can be used on registers already marked invalid.
		Can not be used on return register.

Memory Manipulation:
	CLRMEM [MEMa]
		Set MEMa to be invalid. Does not overwrite.
		Can be used on Memory Locations already marked invalid.

	LOAD [GPRa] [MEMa]
		Copy from MEMa to GPRa, if MEMa is valid Memory Location and GPRa address exists.
		GPRa set to valid, and overwritten.
		MEMa is left alone.

	STORE [MEMa] [GPRa]
		Copy from GPRa to MEMa, if GPRa is a valid GPR and MEMa address exists.
		MEMa is set to valid, and overwritten.
		GPRa is left alone.


*****Error Codes:
1 - Instruction with invalid arguments found.
2 - Instruction does not exist.
3 - Invalid access of registers.
4 - Invalid register address.
5 - Invalid instruction address.
6 - Arithmetic error. Possibly integer overflow.
7 - Integer overflow.
