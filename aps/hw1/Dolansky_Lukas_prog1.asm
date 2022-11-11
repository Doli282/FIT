#.data
#.align 2
#.word 1
#.word 0xf
#.word 0xc
#.word 1 2 3 4 5 11 35 37 49 99 100 101 0x66 0 0xffffffff 0x01010101


.text
jal x0, main # jump to main()

prime: # int prime (unsigned int number) - find prime numbers
  addi t1, x0, 1
  slt t0, t1, a0 # numbers less than 2 are not prime numbers
  beq x0, t0, is_not_prime
  prime_loop:
    addi t1, t1, 1 # increment divisor
    beq t1, a0, is_prime # number reached -> number is prime
    rem t0, a0, t1 # calculate reminder of input/t1
    beq x0, t0, is_not_prime # reminder is 0 -> number is not prime
    jal x0, prime_loop # repeat loop

is_prime:
  addi a0, x0, 1 # number is prime
  jal x0, prime_end
is_not_prime:
  add a0, x0, x0 # number is NOT prime
prime_end:
  #ret
  jalr x0, ra, 0


main: # main()
lw s1, 0x00000004(x0) # load size of array
lw s2, 0x8(x0) # offset initialization
add s3, x0, x0 # counter
beq s1, s3, end_loop
for_loop:
  lw a0, 0x0(s2) # load number from the array
  jal ra, prime # run the prime rutine to find prime bumbers
  sw a0, 0x0(s2) # save the result of the prime rutine
  addi s2, s2, 4 # increment offset
  addi s3, s3, 1 # increment counter
  bne s1, s3, for_loop # untill all numbrers were processed - continue in loop


end_loop: # infinite end loop
jal x0, end_loop # repeat the end loop