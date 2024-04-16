#include "syscall.h"
//#define maxlen 32

int main()
{
    
    OpenFileId file = Open("mota.txt", 0);
    char buffer[1024];
    Read(buffer, 1024, file);
    Close(file);
    PrintString(buffer);

    Halt();
}