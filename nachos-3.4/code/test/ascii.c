/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int main()
{
    char ch;
    int i;
    int j = 0;
    OpenFileId file;
    char table[188];
    Create("ascii.txt");
    file = Open("ascii.txt", 0);
    PrintString("\nBANG MA ASCII CAC KY TU DOC DUOC\n");
    for (i = 33; i < 127; i++)
    {
        PrintInt(i);
        PrintChar(':');
        ch = (char)i;
        PrintChar(ch);
        PrintChar('\t');
        table[j] = ch;
        j++;
        table[j] = '\n';
        j++;
    }
    Write(table, 200, file);
    Close(file);
    Halt();
    /* not reached */
}