.file "fibs.s"					# Linker stuff (you don't need to worry about this)
.text

##################
# fibs_recursive #
##################

.globl fibs_recursive				# Linker stuff
.type	fibs_recursive, @function

fibs_recursive:					# unsigned int fibs_recursive(unsigned int n) {

	pushq %rbp		#push rbp onto the stack 
	movq %rsp, %rbp		#move rsp to rbp 
	pushq %rbx		#push rbx onto the stack (SAVE)
	subq $24, %rsp   	#make space for teh recusion
	movq %rdi, %rbp		#move n into rbp
	
	movq $0, %rax		#move 0 to the return
	cmpq $0, %rbp		#compare n ==0
	je .L3			#jump to .L3 if true

	movq $1, %rax		#move 1 to the return
	cmpq $1, %rbp		#compare n ==1
	je .L3			#jump to .L3 if true
	
	movq $2, %rax		#move 2 to return (This was for testing) 
	movq %rbp, %rax		#move n to the return
	subq $1, %rax		# make return =return -1
	movq %rax, %rdi		# move n- 1 to arg1 of the next call
	call fibs_recursive

	movq %rax, %rbx		#move return for fibs(n-1) into a save register
	movq %rbp, %rax		#move n to the return
	subq $2, %rax		#make return = return -2
	movq %rax, %rdi		#move n  -2 to arg1 of the next call
	call fibs_recursive

	addq %rbx, %rax		#add fibs(n-1) + fibs(n-2)

.L3:

	addq $24, %rsp		#restore rsp
	popq %rbx		#pop rbx
	popq %rbp		#pop retq			#
	ret							#
								# }

.size	fibs_recursive, .-fibs_recursive  # Linker stuff

.section	.note.GNU-stack,"",@progbits
