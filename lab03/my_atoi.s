	.file	"my_atoi.c"
	.text
.globl my_atoi
	.type	my_atoi, @function
my_atoi:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
#####################################################################
# DON'T EDIT ABOVE THIS LINE                                        ######################################################################
# atoi(char *buffer)                                                #
# parse a character string into an integer.                         #
# %rdi contains a pointer to the array of characters (string).      #
# Place your return value in %rax at the end.                       #
#                                                                   #
# Use registers %rax, %rcx, and %r8 - %r11.                         #
#####################################################################
# Write your code here...                                           #
#####################################################################
#pushq %rax

movq	$0, %rax #Move 0 to rax(accumulator)

.L10: #Loop

movzbq (%rdi), %rcx

 cmpq $47, %rcx #compare the array of characters make sure in range
 jle  .L11 #If <= goto done

 
 cmpq $58, %rcx #compare the array of characters make sure in range
 jge .L11 #If > goto done

 imul $10, %rax #Move over to a higher power of 10


 sub $48, %rcx
 add %rcx, %rax #Placeing digit in the proper place

inc %rdi
#add 1, %rdi
 #movq %rdi, %rdi #Increment the array to the next byte
 jg .L10

.L11:



##############################
# DON'T EDIT BELOW THIS LINE #
##############################
	popq	%rbx
	leave
	ret

	.size	my_atoi, .-my_atoi
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
