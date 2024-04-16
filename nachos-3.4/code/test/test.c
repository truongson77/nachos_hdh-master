#include "syscall.h"


void TestInt()
{
    int input;
    PrintChar('\n');
    PrintString("Nhap so nguyen:");
    input = ReadInt();

    PrintString("so nguyen vua nhap:");
    PrintInt(input);
    PrintChar('\n');
}

void TestChar()
{
    char input;
    PrintChar('\n');
    PrintString("Nhap ky tu:");
    input = ReadChar();

    PrintString("Ky tu vua nhap:");
    PrintChar(input);
    PrintChar('\n');
}

void TestString()
{
    char* input;
    PrintChar('\n');
    PrintString("Nhap chuoi:");
    ReadString(input, 255);

    PrintString("chuoi vua nhap:");
    PrintString(input);
    PrintChar('\n');
}

void TestFloat()
{
    int n;
    float* temp;
    PrintChar('\n');
    PrintString("Nhap so thuc:");
    n = ReadFloat();
    temp = (float*)&n ;
    PrintString("so thuc vua nhap:");
    PrintFloat(*(int*)temp);
    PrintChar('\n');
}
int main()
{
    TestFloat();
    Halt();
}