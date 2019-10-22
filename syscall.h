/**
 * Gedare Bloom 
 * Kaleshwar Singh
 * Mahia Tasneem
 * Henchhing Limbu
 * Zaykha Kyaw San
 * syscall.h
 *
 * Definitions for the system calls
 */

#ifndef STDINT_H
#define STDINT_H 
#include <stdint.h>
#endif


int syscall(int num);				// int num is the value in $v0
int sys_exit();
int sys_print_int(uint32_t a0);			// Takes $a0 as the argument that it needs to print
int sys_print_string(uint32_t a0);		// Takes $a0 as the address of the string to print
int sys_read_int();						// Stores the read integer into $v0
int sys_read_char();					// Stores the read character into $v0
int sys_print_char(uint32_t a0);		// $a0 contains the character to print
void print_cpu_stats();					// To print the processor stats upon exiting

