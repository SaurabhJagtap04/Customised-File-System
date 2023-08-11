#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
//#include<io.h>

// Set maximum number of Inodes that we want in our system
#define MAXINODE 50

// Permissions to create new files
#define READ 1
#define WRITE 2

// Set limit of size for creation of new file (in bytes)
#define MAXFILESIZE 2048

// Types of files to create 
#define REGULAR 1
#define SPECIAL 2

// Set offset in file
#define START 0                                                     
#define CURRENT 1
#define END 2


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//      struct superblock                                                                   //
//      >> int TotalInodes : Total number of Inodes in the system.                          //
//      >> int FreeInode   : Total number of free Inodes in the system.                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
}SUPERBLOCK, *PSUPERBLOCK;



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//      struct inode                                                                        //
//      >> All information as well as data about file is strored in this structure          //
//                                                                                          // 
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int permission;
    struct inode *next;
}INODE, *PINODE, **PPNODE;



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //                                                                                         
//      struct filetable                                                                    //
//      >> Offests, Count, Mode is stored.                                                  //                                                                  
//      >> Pointer to structure 'inode' is created as ptrinode                              //                              
//                                                                                          //                                                                 
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;                           //mode : Read / Write / Read+Write
    PINODE ptrinode;                    //struct inode* ptrinode
}FILETABLE, *PFILETABLE;




///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//      struct ufdt                                                                         //
//      >> Pointer to structure filetable is stored                                         //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//              S T A R T   O F   V I R T U A L   F I L E   S Y S T E M                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


// Array of pointers to strucure 'ufdt'.
// Size of Array is 50 beacuse MAXIONDE is defined as 50.
UFDT UFDTArr[50]; 

//Object of structure 'superblock'
SUPERBLOCK SUPERBLOCKobj;

//Pointer to structure 'inode'
PINODE head = NULL;


///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
//                   F    U    N    C    T    I    O    N    S                           //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void man(char *name)                                                    //
//                                                                                          //
//       INPUT  : man create                                                                //
//       OUTPUT : Description : Used to create new regular file                             //                      
//                Usage : create File_name Permission                                       //
//                                                                                          //
//       >> Accepts name of command(System call Ex. open, create, write, stat, etc)         //       
//       >> Displays application of that command and Usage                                  //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void man(char *name)
{
    if(name == NULL)return;

    printf("\n-------------------------------------------------------------------------------\n");
    printf("\n   Command Name : %s\n\n",name);

    if(strcmp(name,"create") == 0)
    {
        printf("   Description : Used to create new regular file\n\n");
        printf("   Usage : create File_name Permission\n\n");
    }
    else if(strcmp(name,"read") == 0)
    {
        printf("   Description : Used to read data from regular file\n\n");
        printf("   Usage : read File_name No_Of_BytesTo_Read\n\n");
    }
    else if(strcmp(name,"write") == 0)
    {
        printf("   Description : Used to write in regular file\n\n");
        printf("   Usage : write File_name\n\nAfter this enter the data that we want to write\n\n");
    }
    else if(strcmp(name,"ls") == 0)
    {
        printf("   Description : Used to list all information of files\n\n");
        printf("   Usage : ls\n\n");
    }
    else if(strcmp(name,"stat") == 0)
    {
        printf("   Description : Used to display information of file\n\n");
        printf("   Usage : stat File_name\n\n");
    }
    else if(strcmp(name,"fstat") == 0)
    {
        printf("   Description : Used to display information of file\n\n");
        printf("   Usage : stat File_Descriptor\n\n");
    }
    else if(strcmp(name,"truncate") == 0)
    {
        printf("   Description : Used to remove data from file\n\n");
        printf("   Usage : truncate File_Descriptor\n\n");
    }
    else if(strcmp(name,"open") == 0)
    {
        printf("   Description : Used to open existing file\n\n");
        printf("   Usage : open File_name mode\n\n");
    }
    else if(strcmp(name,"close") == 0)
    {
        printf("   Description : Used to close opened file\n\n");
        printf("   Usage : close File_name\n\n");
    }
    else if(strcmp(name,"closeall") == 0)
    {
        printf("   Description : Used to close all opened file\n\n");
        printf("   Usage : closeall\n\n");
    }
    else if(strcmp(name,"lseek") == 0)
    {
        printf("   Description : Used to change file offset\n\n");
        printf("   Usage : lseek File_Name ChangeInOffset StartPoint\n\n");
    }
    else if(strcmp(name,"rm") == 0)
    {
        printf("   Description : Used to delete the file\n\n");
        printf("   Usage : rm File_Name\n\n");
    }
    else
    {
        printf("ERROR : No manual entry available.\n\n");
    }

    printf("-------------------------------------------------------------------------------\n\n");

}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void DisplayHelp()                                                      //
//                                                                                          //
//       INPUT : clear                                                                      //
//       INPUT : To clear console                                                           //
//                                                                                          //
//       >> Accepts name of command (ls, clear, write)                                      //       
//       >> Displays use of that command                                                    //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void DisplayHelp()
{
    printf("\n----------------- H E L P   W I N D O W ------------------\n\n");

    printf("-> ls       : To List out all the files.\n");
    printf("-> clear    : To clear the console.\n");
    printf("-> create   : To create the file.\n");
    printf("-> open     : To open the file.\n");
    printf("-> close    : To close the file.\n");
    printf("-> closeall : To close all the opened files.\n");
    printf("-> read     : To read all contents from file.\n");
    printf("-> write    : To Write contents into file.\n");
    printf("-> exit     : To Terminate the file system.\n");
    printf("-> stat     : To Display informantion of the file using name.\n");
    printf("-> fstat    : To Display informantion of the file using descriptor.\n");
    printf("-> truncate : To Remove all data from the file.\n");
    printf("-> rm       : To Delete the file.\n\n");

    printf("Enter : 'man command_name' for further information.\n\n");
   
    printf("--------------------------------------------------------\n\n");
   
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int GetFDFromName(char *name)                                           //
//                                                                                          //
//       INPUT : filename                                                                   //
//       OUTPUT : fd                                                                         //
//                                                                                          //
//       >> Used to get FD of a file from its name.                                         //
//                                                                                          //
//       >> Accepts name of file.                                                           //
//       >> Starts traversal of UFDTArr.                                                    //
//       >> Compares accepted file name by file names present in system.                    //       
//       >> Returns fd if file name matches.                                                //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int GetFDFromName(char *name)
{
    int i = 0;

    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
            {
                break;
            }
        }
            
        i++;
    }

    if(i == 50)     
    {
        return -1;
    }
    else
    {
        return i;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int Get_Inode(char*name)                                                //
//                                                                                          //
//       INPUT : filename                                                                   //
//       OUTPUT : Pointer to structure of node of that file.                                //
//                                                                                          //
//       >> This fuction is used in CreateFile() function to check if file already exists.  //
//       >> This fuction is used in Open() function to get data 0f file.                    //
//                                                                                          //
//       >> Accepts name of file.                                                           //
//       >> Starts traversal of UFDTArr.                                                    //
//       >> Compares accepted file name by file names present in system.                    //       
//       >> Returns strucutre Inode assiciated with file name if name matches.              //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

PINODE Get_Inode(char*name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
    {
        return NULL;
    }

    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName)== 0)
        {
            break;
        }
        temp = temp -> next;
    }
    return temp;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void CreateDILB()                                                       //
//                                                                                          //
//       INPUT : NULL                                                                       //
//       OUTPUT : NULL                                                                      //
//                                                                                          //
//       >> Used to create a linked list with each node as structure 'inode'.               //
//       >> All elements of structure 'inode' initialized.                                  //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while(i<= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));
        
        newn -> LinkCount = 0;
        newn -> ReferenceCount = 0;
        newn -> FileType = 0;
        newn -> FileSize = 0;

        newn -> Buffer = NULL;
        newn -> next = NULL; 

        newn -> InodeNumber = i;

            if(temp == NULL)
            {
                head = newn;
                temp = head;
            }
            else
            {
                temp -> next = newn;
                temp = temp -> next;
            }
            i++;
    }
    printf("DILB created successfully\n");
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void InitializeSuperBlock()                                             //
//                                                                                          //
//       INPUT : NULL                                                                       //
//       OUTPUT : NULL                                                                      //
//                                                                                          //
//       >> Used to initialize superblock.                                                  //
//       >> Total number of Inodes and Free Inodes initialized as MAXINODE.                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void InitializeSuperBlock()
{
    int i = 0;
    while(i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int CreateFile(char*name, int permission)                               //
//                                                                                          //
//       INPUT : create demo.txt 1                                                          //
//       OUTPUT : fd (File Descriptor)                                                      //
//                                                                                          //
//       >> Accepts file name and permission (1/2/3).                                       //
//       >> Creates the file                                                                //
//       >> Returns fd.                                                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int CreateFile(char*name, int permission)
{
    int i = 0;
    PINODE temp = head;

    // Permissions should be in limit
    if((name == NULL) || (permission == 0) || (permission > 3))         
    {
        return -1;
    }    

    // Return Error if no free inodes available
    if(SUPERBLOCKobj.FreeInode == 0)                                    
    {
        return -2;
    } 

    // Reduce Free Inode number by 1
    (SUPERBLOCKobj.FreeInode)--;                                        

    // Check if Inode exists of that filename
    if(Get_Inode(name) != NULL)                                         
    {
        return -3;
    } 
 
    // Loop to check empty Inode. Temp points to empty Inode.
    while(temp != NULL)                                                 
    {
        if(temp -> FileType == 0)
        {
            break;
        }
        temp = temp -> next;
    }

    // Set number to the empty Inode (i).
    while(i<50)                                                         
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }
    
    // Memory Allocation
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));    

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    
    UFDTArr[i].ptrfiletable->ptrinode = temp;

    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->permission = permission;

    // Memory allocation for data 
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);        

    return i;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int rm_File(char *name)                                                 //
//                                                                                          //
//       INPUT : File Name                                                                  //
//       OUTPUT : 0 (If file removal successful)                                            //
//                                                                                          //
//       >> Function is used to delete the file                                             //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


int rm_File(char *name)
{
    int fd = 0;

    //Check if file is available in the system
    fd = GetFDFromName(name);                           
    if(fd == -1)
    {
        return -1;
    }

    //Reduce link count of that Inode
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    //Check if any other entity is associated with the Inode 
    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        //Set Filetype to 0 making Inode available
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
        
        //Free the filetable structure
        free(UFDTArr[fd].ptrfiletable);
    }

    //Clear File Descriptor entry
    UFDTArr[fd].ptrfiletable = NULL;

    //Increase number of free Inode count
    (SUPERBLOCKobj.FreeInode)++;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int ReadFile(int fd, char *arr, int isize)                              //
//                                                                                          //
//       INPUT : File Name, Empty Array, Number of bytes to read.                           //
//       OUTPUT : Number of bytes successfully read.                                        //
//                                                                                          //
//       >> Function is used to read data from the file.                                    //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


int ReadFile(int fd, char *arr, int isize)
{
    int read_size = 0;

    // Check if Inode has data associated with it
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }           

    //Check for availabe mode to open to open that file
    if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)
    {
        return -2;
    }
    
    //Check for availabe permissions to open to open that file
    if(UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ+WRITE)
    {
        return -2;
    }

    // Check if the read offset is at the end of file
    if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
    {
        return -3;
    }

    // Check if filetype is regular or not
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
    {
        return -4;
    }

    // read_size = End of file data location - read offset location  
    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);

    if(read_size < isize)
    {
        // Add value of readoffset to Buffer
        // Copy data with size 'read_size' if read_size if less than isize
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),read_size);

        //Update readoffset value
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
    }
    else
    {
        // Add value of readoffset to Buffer
        // Copy data with size 'isize' if read_size if greater than isize
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),isize);

        //Update readoffset value
        (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset)+isize;
    }

    return isize;    
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int WriteFile(int fd, char *arr, int isize)                             //
//                                                                                          //
//       INPUT : File Name, Empty Array, Number of bytes to write.                          //
//       OUTPUT : Number of bytes successfully written in the file.                         //
//                                                                                          //
//       >> Function is used to write data into the file.                                   //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int WriteFile(int fd, char *arr, int isize)
{

    // Check available mode to write in file
    if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode)!= READ+WRITE))
    {
        return -1;
    }

    // Check available permissions to write in file
    if(((UFDTArr[fd].ptrfiletable->ptrinode->permission)!= WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission)!= READ+WRITE))
    {
        return -1;
    }

    // Check if file is already written till maximum available size
    if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
    {
        return -2;
    }

    // Check if file is regular
    if((UFDTArr[fd].ptrfiletable->ptrinode-> FileType) != REGULAR)
    {
        return -3;
    }

    // Add value of write offset to Buffer
    // Copy data from given array to file
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

    // Update write offset value
    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    // Update FileActualSize value
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;

    return isize;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int OpenFile(char *name,int mode)                                       //
//                                                                                          //
//       INPUT : File Name, Mode to open.                                                   //
//       OUTPUT : File Descriptor (fd).                                                     //
//                                                                                          //
//       >> Function is used to open file in the specified mode                             //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int OpenFile(char *name,int mode)
{
    int i = 0;
    PINODE temp = NULL;

    //Return error if name is empty or mode is unspecified
    if(name == NULL || mode <= 0)
    {
        return -1;
    }
    
    // Point temp to Inode of the specified file
    temp = Get_Inode(name);

    //No free Inode available if temp == NULL
    if(temp == NULL)
    {
        return -2;
    }

    //check for permission
    if(temp ->permission < mode)
    {
        return -3;
    }

    // Get fd
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    // FileTable createtion
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    // Check for memory allocation
    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }

    //Set Count
    UFDTArr[i].ptrfiletable->count = 1;

    // Set mode
    UFDTArr[i].ptrfiletable->mode = mode;

    // Set offsets according to mode
    if(mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }

    // Point PINODE towards pointer inside Filetable
    UFDTArr[i].ptrfiletable->ptrinode = temp;

    // Increase reference count
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    // return fd (i)
    return i;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void CloseFileByName(int fd)                                            //
//                                                                                          //
//       INPUT : fd.                                                                        //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to close previously opened file.                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int CloseFileByName(char *name)                                         //
//                                                                                          //
//       INPUT : File Name.                                                                 //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to close previously opened file.                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if(i == -1)
        return -1;

    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void CloseAllFiles()                                                    //
//                                                                                          //
//       INPUT : Gets called from another function  .                                       //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to close all files that are previously opened.                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


void CloseAllFiles()
{
    int i = 0;
    while(i < 50)
    {
        // Traverse entire array and set contents on ptrfiletable to 0. 
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset= 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
            {
                break;
            }
        }
        i++;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int LseekFile(int fd, int size, int from)                               //
//                                                                                          //
//       INPUT : fd, offset distance, offset starting location (start / current / end)      //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to change the offset in file.                                  //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int LseekFile(int fd, int size, int from)
{
    // Check for valid starting position
    if((fd < 0) || (from > 2))
    {
        return -1;
    }

    // Check if specified fd has file data added to it
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }

    // lseek function for read offset
    if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE))
    {
        if(from == CURRENT)
        {
            // Return error if offset position is greater than actual data in file
            if(((UFDTArr[fd].ptrfiletable->readoffset)+ size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
            {
                return -1;
            }

            // Add mentioned size to current offset
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
        }
        else if(from == START)
        {
            // Return error if offset position is greater than actual data in file
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }

            // Return error if size is negative
            if(size < 0)
            {
                return -1;
            }

            // Change and set offset to mentioned size from start
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if(from == END)
        {
            // Return error if offset position is greater than actual data in file
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+ size > MAXFILESIZE)
            {
                return -1;
            }

            // Return error if hte offest becomes negative after adding current offset 
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size ) < 0)
            {
                return -1;
            }

            // Set offset according to mentioned size
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+ size;
        }
    }
    // lseek function for write offset
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            // Return error if offset position is greater than actual data in file
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+ size) > MAXFILESIZE)
            {
                return -1;
            }

            // Return error if the offest becomes negative after adding current offset 
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+ size) < 0)
            {
                return -1;
            }

            // Update FileActualSize if updated offest will be greater than current FileActualSize
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+ size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
            }

            // Update write offset with mentioned size
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
        }
        else if(from == START)
        {
            // Return error if offset position is greater than actual data in file
            if(size > MAXFILESIZE)
            {
                return -1;
            }

            // Return error if the offest is negative
            if(size < 0)
            {
                return -1;
            }

            // Update FileActualSize if updated offest will be less than current FileActualSize
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            }

            // Update write offset with mentioned size
            (UFDTArr[fd].ptrfiletable->writeoffset) = size;
        }
        else if(from == END)
        {
            // Return error if offset position is greater than size of file
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }

            // Return error if hte offest becomes negative after adding current offset 
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
            {
                return -1;
            }

            // Update the write offset with mentioned size
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : void ls_file()                                                          //
//                                                                                          //
//       INPUT : Gets called from another function.                                         //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to display information of all the files in file system.        //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void ls_file()
{
    int i = 0;
    PINODE temp = head;

    // If number of free Inodes is maximum, return from function as file system is empty.
    if(SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("Error : There are no files\n");
        return;
    }

    printf("\nFile Name\tInode number\tFile size\tLink count\n");
    printf("--------------------------------------------------------\n");
    while(temp != NULL)
    {
        // Check for used Inodes
        if(temp->FileType != 0)
        {
            // print the information on console
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp = temp -> next;
    }
    printf("---------------------------------------------------------\n");
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int fstat_file(int fd)                                                  //
//                                                                                          //
//       INPUT : fd of a file.                                                              //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to display information of opened file                          //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;

    // Check for valid fd
    if(fd < 0)  
    {
        return -1;
    }

    // Check if file system is not empty
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -2;
    }

    // Point temp towards specified Inode
    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    // printf information of that file on console
    printf("\n----------Statistical Information about file-----------\n");

    printf("File name : %s\n",temp->FileName);

    printf("Inode Number : %s",temp->InodeNumber);

    printf("File Size : %d\n",temp -> FileSize);
    
    printf("Actual File Size : %d\n",temp->FileActualSize);

    printf("Link Count : %d\n",temp->LinkCount);

    printf("Reference Count : %d\n",temp -> ReferenceCount);

    if(temp->permission == 1)
    {
        printf("File Permission : Read Only\n");
    }
    else if(temp->permission == 2)
    {
        printf("File permission : Write only\n");
    }
    else if(temp->permission == 3)
    {
        printf("File permission : Read & Write\n");
    }

    printf("---------------------------------------------------------\n\n");

    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int stat_file(char *name)                                               //
//                                                                                          //
//       INPUT : Name of file.                                                              //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used to display information of file.                                //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    // Return error if name is empty
    if(name == NULL) 
    {
        return -1;
    }

    // Travel UFTDArr until temp points to the specified Inode
    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName)== 0)
            break;
        temp = temp -> next;
    }

    // Return error if specified file not found.
    if(temp == NULL) return -2;

    // printf inforamtion about that file on the console

    printf("\n----------Statistical Information about file-----------\n");

    printf("File name : %s\n",temp->FileName);

    printf("Inode Number : %s",temp->InodeNumber);

    printf("File Size : %d\n",temp -> FileSize);

    printf("Actual File Size : %d\n",temp->FileActualSize);

    printf("Link Count : %d\n",temp->LinkCount);

    printf("Reference Count : %d\n",temp -> ReferenceCount);


    if(temp->permission == 1)
    {
        printf("File Permission : Read Only\n");
    }
    else if(temp->permission == 2)
    {
        printf("File permission : Write only\n");
    }
    else if(temp->permission == 3)
    {
        printf("File permission : Read & Write\n");
    }
    
    printf("---------------------------------------------------------\n\n");

    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//       Function : int truncate_File(char *name)                                           //
//                                                                                          //
//       INPUT : Name of file.                                                              //
//       OUTPUT : -                                                                         //
//                                                                                          //
//       >> Function is used remove all the data from file.                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

int truncate_File(char *name)
{
    int fd = GetFDFromName(name);

    // Return error if fd not found
    if(fd == -1);
    {
        return -1;
    }

    // Set the data in block to 0
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);

    UFDTArr[fd].ptrfiletable->readoffset = 0;

    UFDTArr[fd].ptrfiletable->writeoffset = 0;

    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;

}

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80],str[80],arr[1024];

    InitializeSuperBlock();
    CreateDILB();

    while(1)
    {
        fflush(stdin);

        strcpy(str,"");

        printf("CUSTOMISED FILE SYSTEM : > ");

        fgets(str,80,stdin);//scanf("%[^'\n']s",str);

        count = sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);

        if(count == 1)
        {
            if(strcmp(command[0],"ls") == 0)
            {
                ls_file();
            }
            else if(strcmp(command[0],"closeall") == 0)
            {
                CloseAllFiles();

                printf("All files closed successfully\n");

                continue;
            }
            else if(strcmp(command[0],"clear") == 0)
            {
                system("cls");

                continue;
            }
            else if(strcmp(command[0],"help") == 0)
            {
                DisplayHelp();

                continue;
            }
            else if(strcmp(command[0],"exit") == 0)
            {
                printf("Terminating the FILE SYSTEM\n");

                break;
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");

                continue;
            }
        }
        else if(count == 2)
        {
            if(strcmp(command[0],"stat") == 0)
            {
                ret = stat_file(command[1]);

                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no such file\n");
                }
                
                continue;
            }
            else if(strcmp(command[0],"fstat") == 0)
            {
                ret = fstat_file(atoi(command[1]));

                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no such file\n");
                }
                
                continue;
            }
            else if(strcmp(command[0],"close") == 0)
            {
                ret = CloseFileByName(command[1]);

                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                }

                continue;
            }
            else if(strcmp(command[0],"rm") == 0)
            {
                ret = rm_File(command[1]);

                if(ret == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                }
                
                continue;
            }
            else if(strcmp(command[0],"man") == 0)
            {
                man(command[1]);
            }
            else if(strcmp(command[0],"write") == 0)
            {
                fd = GetFDFromName(command[1]);

                if(fd == -1)
                {
                    printf("Incorrect parameter\n");

                    continue;
                }

                printf("Enter the data : \n");

                scanf("%[^'\n']",arr);

                ret = strlen(arr);

                if(ret == 0)
                {
                    printf("Error : Incorrect parameter\n");
                    continue;
                }

                ret = WriteFile(fd,arr,ret);

                int c;
                while ((c = getchar()) != '\n' && c != EOF) {} 

                if(ret == -1)
                {
                    printf("ERROR : Permission denied\n");
                }

                if(ret == -2)
                {
                    printf("ERROR : There is no sufficient memory to write\n");
                }

                if(ret == -3)
                {
                    printf("ERROR : It is not a regular file\n");
                }
                
            }
            else if(strcmp(command[0],"truncate")==0)
            {
                ret = truncate_File(command[1]);

                if(ret == -1)
                {
                    printf("Error : Incorrect parameter\n");
                }
            }
            else
            {
                printf("\n ERROR : Command not found !!!\n");

                continue;
            }           
        }
        else if(count == 3)
        {
            if(strcmp(command[0],"create") == 0)
            {
                ret = CreateFile(command[1],atoi(command[2]));

                if(ret >= 0)            
                {    
                    printf("File is successfully created with file descriptor : %d\n",ret);               
                }

                if(ret == -1)     
                {
                    printf("ERROR : Incorrect parameters\n");               
                }

                if(ret == -2)               
                {
                    printf("ERROR : There is no Inodes\n");                
                }

                if(ret == -3)               
                {
                    printf("ERROR : File already exists\n");              
                }

                if(ret == -4)             
                {
                    printf("ERROR : Memory allocation failure\n");
                }
                
                continue;
            }
            else if(strcmp(command[0],"open") == 0)
            {           
                ret = OpenFile(command[1],atoi(command[2]));

                if(ret >= 0)
                {
                    printf("File is successfully created with file descriptor : %d\n",ret);                             
                }

                if(ret == -1)     
                {
                    printf("ERROR : Incorrect parameters\n");               
                }
                
                if(ret == -2)               
                {
                    printf("ERROR : File not present \n");                
                }
                
                if(ret == -3)               
                {
                    printf("ERROR : Permission denied\n");              
                }
                
                continue;              
            }
            else if(strcmp(command[0],"read") == 0)
            {
                fd = GetFDFromName(command[1]);

                if(fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");

                    continue;
                }

                ptr = (char *)malloc(sizeof(atoi(command[2])) + 1);

                if(ptr == NULL)
                {
                    printf("ERROR : Memory allocation failed\n");
                    continue;
                }

                ret = ReadFile(fd,ptr,atoi(command[2]));

                if(ret == -1)
                {
                    printf("ERROR : File not existing\n");
                }
                
                if(ret == -2)
                {
                    printf("ERROR : Permission denied\n");
                }
                
                if(ret == -3)
                {
                    printf("ERROR : Reached at end of file\n");
                }
                
                if(ret == -4)
                {
                    printf("ERROR : It is not a regular file\n");
                }
                
                if(ret == 0)
                {
                    printf("ERROR : File empty\n");
                }
                
                if(ret > 0)
                {
                    write(2,ptr,ret);
                }

                continue;               
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");

                continue;
            }
        }
        else if(count == 4)
        {
            if(strcmp(command[0],"lseek") == 0)
            {
                fd = GetFDFromName(command[1]);

                if(fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");

                    continue;
                }

                ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));

                if(ret == -1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }
            }
            else
            {
                printf("ERROR : Command not found !!!\n");

                continue;
            }
        }
        else
        {
            printf("ERROR : Command not found !!!\n");

            continue;            
        }
    }

    return 0;
}
