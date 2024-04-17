// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MaxFileLength 32
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}

// Chuc nang: Sao chep vung nho User sang vung nho System
char* User2System(int virtAddr, int limit)
{
    int i; //chi so index
    int oneChar;
    char* kernelBuffer = NULL;
    kernelBuffer = new char[limit + 1];
    if (kernelBuffer == NULL)
        return kernelBuffer;
        
    memset(kernelBuffer, 0, limit + 1);
    
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);//doc tung byte cua virtAddr va luu vao kernelbuf
        kernelBuffer[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuffer;
}

// Sao chep vung nho System sang vung nho User
int System2User(int virtAddr, int len, char* buffer)
{
    if (len < 0) return -1;
    if (len == 0)return len;
    int i = 0;
    int oneChar = 0;
    do{
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);//doc tung byte cua kernel buffer va luu vao dia chi user virt addr
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

char* float_to_buffer(float value, int& max_len) {
    max_len = snprintf(NULL, 0, "%f", value); // Determine necessary buffer size
    char *str = new char[max_len + 1];  // Allocate space
    snprintf(str, max_len + 1, "%f", value); // Format float into string
    return str;
}

void
ExceptionHandler(ExceptionType which)
{
    int op1, op2, result;
    int type = machine->ReadRegister(2);
    switch(which)
    {
        case NoException:
            return;

        case PageFaultException:
            DEBUG('a', "PageFault.\n");
            interrupt->Halt();
            break;

        case BusErrorException:
            DEBUG('a', "BusError.\n");
            interrupt->Halt();
            break;

        case AddressErrorException:
            DEBUG('a', "AddressError.\n");
            interrupt->Halt();
            break;

        case OverflowException:
            DEBUG('a', "Overflow.\n");
            interrupt->Halt();
            break;

        case IllegalInstrException:
            DEBUG('a', "IllegalInstr.\n");
            interrupt->Halt();
            break;

        case NumExceptionTypes:
            DEBUG('a', "IllegalInstr.\n");
            interrupt->Halt();
            break;

        case SyscallException:
                switch(type)
                {
                    case SC_Halt:
                        DEBUG('a', "Shutdown, initiated by user program.\n");
                        interrupt->Halt();
                        break;

                    case SC_Sub:
                        op1 = machine->ReadRegister (4); 
                        op2 = machine->ReadRegister (5); 
                        result = op1 - op2; 
                        machine->WriteRegister (2, result); 
                        interrupt->Halt();
                        break; 
                    case SC_FloatToBuffer:
                    {
                        int virtAddr;
                        int number;
                        char* buffer;
                        int i =0;
                        virtAddr = machine->ReadRegister(4); 
                        number =  machine->ReadRegister(5);

                        buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space
                        float* val = (float*)&number;
                        int len = 255;
                        buffer = float_to_buffer(*val, len); 
                        while(buffer[i] != '\0')
                        {
                            i++;
                        }
                        System2User(virtAddr, 255, buffer); // Copy chuoi tu vung nho System Space sang vung nho User Space
                        machine->WriteRegister(2, i);
                        delete buffer; 
                        IncreasePC(); // Tang Program Counter 
                        return;
                        break;
                    }
                    case SC_PrintInt:  
                    {
                        int num = machine->ReadRegister(4);
                        int temp = num; 
                        int lenghtOfNum = 0; 
                        int firstNumIndex = 0; 
                        bool isNegative = false; 
                        if(num == 0)
                            {
                                gSynchConsole->Write("0", 1); 
                                IncreasePC();
                                return;    
                            }
                            
                            if(num < 0)
                            {
                                isNegative = true;
                                num = num * -1; 
                                firstNumIndex = 1; 
                            }   
                            
                            while(temp)
                            {
                                lenghtOfNum++;
                                temp /= 10;
                            }
                            //tao buffer tu so nguyen de ghi ra man hinh
                            char* buffer;
                            int MAX_LEN = 255;
                            buffer = new char[MAX_LEN + 1];
                            for(int i = firstNumIndex + lenghtOfNum - 1; i >= firstNumIndex; i--)
                            {
                                buffer[i] = (char)((num % 10) + 48);
                                num /= 10;
                            }
                            if(isNegative)
                            {
                                buffer[0] = '-';
                                buffer[lenghtOfNum + 1] = 0;
                                gSynchConsole->Write(buffer, lenghtOfNum + 1);
                                delete buffer;
                                IncreasePC();
                                return;
                            }
                            buffer[lenghtOfNum] = 0;    
                            gSynchConsole->Write(buffer, lenghtOfNum);
                            delete buffer;
                            IncreasePC();
                            return; 
                    }
                    case SC_PrintChar:
                        {
                            // Input: Ki tu(char)
                            // Output: Ki tu(char)
                            // Cong dung: Xuat mot ki tu la tham so arg ra man hinh
                            char c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
                            gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
                            IncreasePC();
                            return;
                            break;
                        }
                    case SC_PrintString:
                        {
                            int virtAddr;
                            char* buffer;
                            virtAddr = machine->ReadRegister(4); 
                            buffer = User2System(virtAddr, 255); 
                            int length = 0;
                            while (buffer[length] != 0) length++; 
                            gSynchConsole->Write(buffer, length + 1); 
                            delete buffer; 
                            IncreasePC(); // Tang Program Counter 
                            return;
                            break;
                        }
                    case SC_PrintFloat:
                    {
                        int number =  machine->ReadRegister(4);
                        float* val = (float*)&number;
                        int len = 255;
                        char* buffer = float_to_buffer(*val, len);
                        gSynchConsole->Write(buffer, len);
                        IncreasePC(); // Tang Program Counter 
                        return;
                        break;
                    }
                    case SC_ReadInt:
		            {
                    char* buffer;
                    int MAX_LEN = 255;
                    buffer = new char[MAX_LEN + 1];
                    int numbytes = gSynchConsole->Read(buffer, MAX_LEN);
                    int number = 0; 
						                    
                    bool isNegative = false; 
                    int firstNumIndex = 0;
                    int lastNumIndex = 0;
                    if(buffer[0] == '-')
                    {
                        isNegative = true;
                        firstNumIndex = 1;
                        lastNumIndex = 1;                        			   		
                    }
                    
                    // Kiem tra tinh hop le cua so nguyen buffer
                    for(int i = firstNumIndex; i < numbytes; i++)					
                    {
                        if(buffer[i] == '.')
                        {
                            int j = i + 1;
                            for(; j < numbytes; j++)
                            {
				// So khong hop le
                                if(buffer[j] != '0')
                                {
                                    printf("\n\n The number is not valid");
                                    DEBUG('a', "\n The number is not valid");
                                    machine->WriteRegister(2, 0);
                                    IncreasePC();
                                    delete buffer;
                                    return;
                                }
                            }
                            lastNumIndex = i - 1;				
                            break;                           
                        }
                        else if(buffer[i] < '0' && buffer[i] > '9')
                        {
                            printf("\n\n The number is not valid");
                            DEBUG('a', "\n The number is not valid");
                            machine->WriteRegister(2, 0);
                            IncreasePC();
                            delete buffer;
                            return;
                        }
                        lastNumIndex = i;    
                    }			
                    
                    // La so nguyen hop le, tien hanh chuyen chuoi ve so nguyen
                    for(int i = firstNumIndex; i<= lastNumIndex; i++)
                    {
                        number = number * 10 + (int)(buffer[i] - 48); 
                    }
                    
                    // neu la so am thi * -1;
                    if(isNegative)
                    {
                        number = number * -1;
                    }
                    machine->WriteRegister(2, number);
                    IncreasePC();
                    delete buffer;
                    return;		
		            }                  
                    case SC_ReadChar:
                    {

                    int maxBytes = 255;
                    char* buffer = new char[255];
                    int numBytes = gSynchConsole->Read(buffer, maxBytes);

                    if(numBytes > 1) //Neu nhap nhieu hon 1 ky tu thi khong hop le
                    {
                        printf("Chi duoc nhap duy nhat 1 ky tu!");
                        DEBUG('a', "\nERROR: Chi duoc nhap duy nhat 1 ky tu!");
                        machine->WriteRegister(2, 0);

                    }
                    else if(numBytes == 0) //Ky tu rong
                    {
                        printf("Ky tu rong!");
                        DEBUG('a', "\nERROR: Ky tu rong!");
                        machine->WriteRegister(2, 0);

                    }
                    else
                    {
                        //Chuoi vua lay co dung 1 ky tu, lay ky tu o index = 0, return vao thanh ghi R2
                        char c = buffer[0];
                        machine->WriteRegister(2, c);
                    }

                    delete buffer;
                    IncreasePC(); // error system
                    return;
                    break;
                    }
                    case SC_ReadString:
                    {
                        int virtualAddr, length;
                        char* kernelbuffer;
                        virtualAddr = machine->ReadRegister(4);
                        length = machine->ReadRegister(5); 
                        kernelbuffer = User2System(virtualAddr, length); //lay dia chi cua vung kernel luu vao buffer
                        gSynchConsole->Read(kernelbuffer, length); 
                        System2User(virtualAddr, length, kernelbuffer); 
                        delete kernelbuffer; 
                        IncreasePC(); 
                        return;
                        break;
                    }
                    case SC_ReadFloat:
                    {
                        int sign = 1;
                        int intPart = 0;
                        int decPart = 0;
                        int decimalCount = 0; 
                        int state = 0; // 0: before decimal, 1: after decimal
                        float* result = new float;
                        char* string = new char[1024+1];
                        gSynchConsole->Read(string, 1024);
                        if (*string == '-') {
                            sign = -1;
                            string++;
                        } else if (*string == '+') {
                            string++;
                        }

                        // Process integer part
                        while (*string >= '0' && *string <= '9') { 
                            intPart = intPart * 10 + (*string - '0');
                            string++;
                        }

                        // Check for decimal point
                        if (*string == '.') {
                            string++;
                            state = 1; 
                        }

                        // Process decimal part
                        while (*string >= '0' && *string <= '9') { 
                            decPart = decPart * 10 + (*string - '0');
                            decimalCount++;
                            string++;
                        }

                        // Check for extra characters
                        if (*string != '\0') {
                            return; // Error: extra characters
                        }

                        // Calculate final value 
                        float divisor = 1.0;
                        for (int i = 0; i < decimalCount; i++) {
                            divisor *= 10.0;
                        }
                        *result = sign * (intPart + decPart / divisor);
                        int* val;
                        val = (int*)(result);                
                        machine->WriteRegister(2, *val);
                        IncreasePC(); // Tang Program Counter 
                        return;
                        break;
                    }
                    case SC_Create: 
                    { 
                        int virtAddr; 
                        char* filename; 
                        // Lấy tham số tên tập tin từ thanh ghi r4 
                        virtAddr = machine->ReadRegister(4); 
                        filename = User2System(virtAddr,MaxFileLength+1); 
                        if (filename == NULL) 
                        { 
 
                            machine->WriteRegister(2,-1); // trả về lỗi cho chương 
                            // trình người dùng 
                            delete filename; 
                            return; 
                        } 

                        if (!fileSystem->Create(filename,0)) 
                        { 
                            //printf("\n Error create file '%s'",filename); 
                            machine->WriteRegister(2,-1); 
                            delete filename; 
                            return; 
                        } 
                        machine->WriteRegister(2,0); // trả về cho chương trình 
                            // người dùng thành công 
                        delete filename;
                        IncreasePC();
                        break; 
                        }
                    case SC_Open:
                    {

                        //OpenFileID Open(char *name, int type)
                        int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
                        int type = machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
                        char* filename;
                        filename = User2System(virtAddr, MaxFileLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
                        //Kiem tra xem OS con mo dc file khong
                        
                        // update 4/1/2018
                        int freeSlot = fileSystem->FindFreeSlot();
                        if (freeSlot != -1) //Chi xu li khi con slot trong
                        {
                            if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
                            {
                                
                                if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
                                {
                                    machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
                                }
                            }
                            else if (type == 2) // xu li stdin voi type quy uoc la 2
                            {
                                machine->WriteRegister(2, 0); //tra ve OpenFileID
                            }
                            else // xu li stdout voi type quy uoc la 3
                            {
                                machine->WriteRegister(2, 1); //tra ve OpenFileID
                            }
                            delete[] filename;
                            IncreasePC();
                            break;
                        }
                        machine->WriteRegister(2, -1); //Khong mo duoc file return -1
                        
                        delete[] filename;
                        IncreasePC();
                        break;
                    }
                    case SC_Close:
                    {

                        int fid = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
                        if (fid >= 0 && fid <= 14) //Chi xu li khi fid nam trong [0, 14]
                        {
                            if (fileSystem->openf[fid]) //neu mo file thanh cong
                            {
                                delete fileSystem->openf[fid]; //Xoa vung nho luu tru file
                                fileSystem->openf[fid] = NULL; //Gan vung nho NULL
                                machine->WriteRegister(2, 0);
                                IncreasePC();
                                break;
                            }
                        }
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        break;
                    }
                    case SC_Read:
                    {
                        int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
                        int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
                        int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6 
                        int OldPos;
                        int NewPos;
                        char *buf;
                        // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                        if (id < 0 || id > 14)
                        {
                            printf("\nKhong the read vi id nam ngoai bang mo ta file.");
                            machine->WriteRegister(2, -1);
                            IncreasePC();
                            return;
                        }
                        // Kiem tra file co ton tai khong
                        if (fileSystem->openf[id] == NULL)
                        {
                            printf("\nKhong the read vi file nay khong ton tai.");
                            machine->WriteRegister(2, -1);
                            IncreasePC();
                            return;
                        }
                        if (fileSystem->openf[id]->type == 3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra ve -1
                        {
                            printf("\nKhong the read file stdout.");
                            machine->WriteRegister(2, -1);
                            IncreasePC();
                            return;
                        }
                        OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
                        buf = User2System(virtAddr, charcount); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
                        // Xet truong hop doc file stdin (type quy uoc la 2)
                        if (fileSystem->openf[id]->type == 2)
                        {
                            // Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
                            int size = gSynchConsole->Read(buf, charcount); 
                            System2User(virtAddr, size, buf); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
                            machine->WriteRegister(2, size); // Tra ve so byte thuc su doc duoc
                            delete buf;
                            IncreasePC();
                            return;
                        }
                        // Xet truong hop doc file binh thuong thi tra ve so byte thuc su
                        if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
                        {
                            // So byte thuc su = NewPos - OldPos
                            NewPos = fileSystem->openf[id]->GetCurrentPos();
                            // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su 
                            System2User(virtAddr, NewPos - OldPos, buf); 
                            machine->WriteRegister(2, NewPos - OldPos);
                        }
                        else
                        {
                            // Truong hop con lai la doc file co noi dung la NULL tra ve -2
                            //printf("\nDoc file rong.");
                            machine->WriteRegister(2, -2);
                        }
                        delete buf;
                        IncreasePC();
                        return;
                    }
                    case SC_Write:
                    {
                            int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
                            int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
                            int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6
                            int OldPos;
                            int NewPos;
                            char *buf;
                            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                            if (id < 0 || id > 14)
                            {
                                printf("\nKhong the write vi id nam ngoai bang mo ta file.");
                                machine->WriteRegister(2, -1);
                                IncreasePC();
                                return;
                            }
                            // Kiem tra file co ton tai khong
                            if (fileSystem->openf[id] == NULL)
                            {
                                printf("\nKhong the write vi file nay khong ton tai.");
                                machine->WriteRegister(2, -1);
                                IncreasePC();
                                return;
                            }
                            // Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
                            if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
                            {
                                printf("\nKhong the write file stdin hoac file only read.");
                                machine->WriteRegister(2, -1);
                                IncreasePC();
                                return;
                            }
                            OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
                            buf = User2System(virtAddr, charcount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
                            // Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
                            if (fileSystem->openf[id]->type == 0)
                            {
                                if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
                                {
                                    // So byte thuc su = NewPos - OldPos
                                    NewPos = fileSystem->openf[id]->GetCurrentPos();
                                    machine->WriteRegister(2, NewPos - OldPos);
                                    delete buf;
                                    IncreasePC();
                                    return;
                                }
                            }
                            if (fileSystem->openf[id]->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
                            {
                                int i = 0;
                                while (buf[i] != 0 && buf[i] != '\n' && buf[i] != NULL) // Vong lap de write den khi gap ky tu '\n'
                                {
                                    gSynchConsole->Write(buf + i, 1); // Su dung ham Write cua lop SynchConsole 
                                    i++;
                                }
                                buf[i] = '\n';
                                gSynchConsole->Write(buf + i, 1); // Write ky tu '\n'
                                machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
                                delete buf;
                                IncreasePC();
                                return;
                            }
                    }
                    case SC_Exec:
		            {
			            // Input: vi tri int
			            // Output: Fail return -1, Success: return id cua thread dang chay
			            // SpaceId Exec(char *name);
			            int virtAddr;
			            virtAddr = machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
			            char* name;
			            name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel
	
			            if(name == NULL)
			            {
				            DEBUG('a', "\n Not enough memory in System");
				            printf("\n Not enough memory in System");
				            machine->WriteRegister(2, -1);
				            //IncreasePC();
				            return;
			            }
			            OpenFile *oFile = fileSystem->Open(name);
			            if (oFile == NULL)
			            {
				            printf("\nExec:: Can't open this file.");
				            machine->WriteRegister(2,-1);
				            IncreasePC();
				            return;
			            }

			            delete oFile;

			            // Return child process id
			            int id = pTab->ExecUpdate(name); 
			            machine->WriteRegister(2,id);

			            delete[] name;	
			            IncreasePC();
			            return;
		            }
                        
                    case SC_Join:
		            {       
			            // int Join(SpaceId id)
			            // Input: id dia chi cua thread
			            // Output: 
			            int id = machine->ReadRegister(4);
			
			            int res = pTab->JoinUpdate(id);
			
			            machine->WriteRegister(2, res);
			            IncreasePC();
			            return;
		            }
		            case SC_Exit:
		            {
			            //void Exit(int status);
			            // Input: status code
			            int exitStatus = machine->ReadRegister(4);

			            if(exitStatus != 0)
			            {
				            IncreasePC();
				            return;
				
			            }			
			
			            int res = pTab->ExitUpdate(exitStatus);
			            //machine->WriteRegister(2, res);

			            currentThread->FreeSpace();
			            currentThread->Finish();
			            IncreasePC();
			            return; 
		            }
                    case SC_CreateSemaphore:
		            {
			            // int CreateSemaphore(char* name, int semval).
			            int virtAddr = machine->ReadRegister(4);
			            int semval = machine->ReadRegister(5);

			            char *name = User2System(virtAddr, MaxFileLength + 1);
			            if(name == NULL)
			            {
				            DEBUG('a', "\n Not enough memory in System");
				            printf("\n Not enough memory in System");
				            machine->WriteRegister(2, -1);
				            delete[] name;
				            IncreasePC();
				            return;
			            }
			
			            int res = semTab->Create(name, semval);

			            if(res == -1)
			            {
				            DEBUG('a', "\n Khong the khoi tao semaphore");
				            printf("\n Khong the khoi tao semaphore");
				            machine->WriteRegister(2, -1);
				            delete[] name;
				            IncreasePC();
				            return;				
			            }
			
			            delete[] name;
			            machine->WriteRegister(2, res);
			            IncreasePC();
			            return;
                    }
		            case SC_Wait:			
		            {
			            // int Wait(char* name)
			            int virtAddr = machine->ReadRegister(4);

			            char *name = User2System(virtAddr, MaxFileLength + 1);
			            if(name == NULL)
			            {
				            DEBUG('a', "\n Not enough memory in System");
				            printf("\n Not enough memory in System");
				            machine->WriteRegister(2, -1);
				            delete[] name;
				            IncreasePC();
				            return;
			            }
			
			            int res = semTab->Wait(name);

			            if(res == -1)
			            {
				            DEBUG('a', "\n Khong ton tai ten semaphore nay!");
				            printf("\n Khong ton tai ten semaphore nay!");
				            machine->WriteRegister(2, -1);
				            delete[] name;
				            IncreasePC();
				            return;				
			            }
			
			            delete[] name;
			            machine->WriteRegister(2, res);
			            IncreasePC();
			            return;
		            }
		            case SC_Signal:		
		            {
			            // int Signal(char* name)
			            int virtAddr = machine->ReadRegister(4);

			            char *name = User2System(virtAddr, MaxFileLength + 1);
			            if(name == NULL)
			            {
				            DEBUG('a', "\n Not enough memory in System");
				            printf("\n Not enough memory in System");
				            machine->WriteRegister(2, -1);
				            delete[] name;
				            IncreasePC();
				            return;
			            }
			
			            int res = semTab->Signal(name);

			            if(res == -1)
			            {
				            DEBUG('a', "\n Khong ton tai ten semaphore nay!");
				            printf("\n Khong ton tai ten semaphore nay!");
				            machine->WriteRegister(2, -1);
				            delete[] name;
				            IncreasePC();
				            return;				
			            }
			
			            delete[] name;
			            machine->WriteRegister(2, res);
			            IncreasePC();
			            return;
	            	}
           }              
    }
}
