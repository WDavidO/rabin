# rabin cryptosystem for testing 8 bit AVR key exchange library
#Rabin Cryptosystem with BigNumber library slightly inspired by rabin repo by Sriharsha Gangam
#compiling

As of 22 Nov 2016 all code in this repository was writtn personally by myself and I release it all under the GPLv3


Note I have removed all code from the repos I originally forked and rewritten everything from scratch. This was originally mostly out of convenience as the original
repos used a blinding factor that was unnecessary for me but I have since not been able to contact the author of the original repos to request permission to release
my repos under the GPLv3 so I'm cutting out the last of his code (extended GCD function) and replacing it with code I've written myself. 

g++ ./main.cpp ./rabin1024.cpp ./buffer1024.cpp ./rabintools.cpp -lcrypto
the above works for me. The original version recommended -lssl which did not work for me for the original version switching to -lcrypto did though this may vary by linux distribution.

#Introduction

#micro controller key exchange with RSA

I'm currently working on a stripped down key exchange library for use on AVR 8 bit microcontrollers. The expectation is that all Public key operations will involve one machine with very high processing capabilities (compared to a microcontroller, so something like a raspberry pi or larger,) and one very low power microcontroller. Unauthenticated key exchange only requires a public key data flow in one direction and by limiting the potential values of e (the public key exponent for RSA) to only 3, encryption can be done by computing the square of the plaintext mod p followed by a big number multiply of the result with the plaintext again also mod p. This amounts to two big number multiplies (with one that can involve optimizations based on the two multiplicands being identical) and two bug number modulo operations. The larger machine on the other end of the data flow can then decrypt in the standard RSA way and any full featured RSA library can be used. This exchange can be used to create a shared secret and authentication can be done through some other means or alternatively if there is some method outside of the key exchange to show the public key used for encryption is owned by the proper communicating large machine one side of the communication is already authenticated.

#moving to Rabin

From the perspective of encryption Rabin works the same as RSA but it uses an e of 2 and creates 4 possible plaintexts for each ciphertext. In general this has made Rabin unpopular however when dealing with a communication link between a very low power system and a more powerful one it could be meaningfully beneficial. Given the same scenario above Rabin only requires the low power system to square the plaintext mod p which is conceptually about half the work of a minimal e=3 RSA encryption but in practice actually even better as the single multiply can be optimized due to it only involving one multiplicand. Using Rabin for key exchange (which is really the only thing anyone should be using Rabin or RSA for anyway...) also means that dealing with multiple possible values can be quite simple. In order for either scheme to be secure large primes must be used (the particular library I am developing will use a hardcoded size of 1024 bits for the modulous,) and large random looking values must be encrypted ie the key to be exchanged needs to be properly padded. This padding scheme can be designed to help identify the correct value of the 4 possibilities generated or alternatively the large system can send a plaintext challenge message to the microcontroller which can encrypt it with the proper key using a suitable symmetric encryption algorithm and send that value back to the larger system. The large system can then simply decrypt the new ciphertext with each of the possible keys one at a time until the challenge value is obtained indicating the correct key has been identified. The two systems then have a secure symmetrically encrypted communications path. 

#What does any of this have to do with this repo where is this 8-bit AVR code?

I forked the rabin repository and have made significant changes to it to create a reusable rabin encryption/decryption library hard coded to 1024 bit encryption. This code base is hopefully optimized enough to run in a reasonable amount of time on my target system the AVR ATmega328P microcontroller used in the Arduino UNO. A carful reading of the above will also clearly indicate that this library for the most part amounts to a few big number functions hard coded for 1024bit encryption in a stripped down fashion more suitable for a small microcontroller than modern big int libraries that have many functions and usually support arbitrary precision. In order to make use of carry based optimizations the code is a mixture of C++ (very close to C in style) and assembly code however for testing and development purposes assembly sections are currently in x86 making use of only instructions that also exist in one form or another in the 8-bit AVR instruction set. All of this is to say I need to port the assembly portions to AVR and do some testing and profiling before the library will be suitable for its purpose (if it ever is :) ) Before doing so however I plan to adapt and use the code in this repository to further test the x86 version. 
