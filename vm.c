/*
    Student Name: Hong Thy Nguyen 
    COP 3402
    HW1: PM/0 Virtual Machine
*/

#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 500 //stack maximum size 

#define LIT  1         // Literal
#define OPR  2         // Operation
#define LOD  3         // Load
#define STO  4         // Store
#define CAL  5         // Call prodecure
#define INC  6         // Increment
#define JMP  7         // Jump
#define JPC  8         // Jump Condition
#define SYS  9         // Start Input Output

#define RTN  0         // Return
#define ADD  1         // Add
#define SUB  2         // Subtract
#define MUL  3         // Multiply
#define DIV  4         // Divide
#define EQL  5         // Equal
#define NEQ  6         // Not Equal
#define LSS  7         // Less Than
#define LEQ  8         // Less Than or Equal 
#define GTR  9         // Greater Than 
#define GEQ 10         // Greater Than or Equal

/*global variables*/
int PAS[ARRAY_SIZE];    
int haltFlag = 1;
int calFlag = 0;
int stackBegin = 0;
int numOfInstruction = 0;
int counter = 0;                        //Counter of the number of  input instructions 
int barLocation[ARRAY_SIZE];
int barCount = 0;
int curBar = 0;

/*instruction registers structure*/
typedef struct Instruction{
    int op;                             //Operation code
    int l;                              // Lexicographical level
    int m;                              //Modifier/Argument (constant, address, or OPR)
}Instruction;


/*Function*/
int base(int *BP, int L);
void fetch(int *PC, Instruction *IR);
void executeORP(int *SP, int *BP, int *PC, Instruction *IR);
void executeSYS(int *SP, int *BP, int *PC, Instruction *IR);
void execute(int *SP, int *BP, int *PC, Instruction *IR);
void typeOpCode(char *stringOP, int OP);
void printOutput(int *SP, int *BP, int *PC, Instruction *IR, char string[]);
void typeOPR(char *stringOPR, int opr);
void printInstrList();


int main(int argc, char *argv[]){

    int SP;         //Stack pointer
    int BP;         //Base pointer
    int PC;         //Program counter
    Instruction IR; //Instruction register

    /*Read the input file*/
    FILE *file; 
    file = fopen(argv[1], "r");
    while(fscanf(file, "%d", &PAS[counter]) == 1){
        counter++;
    }
    fclose(file);

    /*Print out the list of the instruction*/
    printf("******************** List of instruction ********************\n");
    printInstrList();
    
    /*Set default values*/
    BP = counter;
    SP = BP - 1;
    PC = 0;
    stackBegin = BP;

    printf("******************** Execution ******************************\n");
    printf("                     PC    BP     SP    Stack\n");
    printf("Initial values:%7d%7d%7d\t\n", PC, BP, SP);      

    while (haltFlag != 0){
        /*Step 1: Fetch the instruction*/
        fetch(&PC, &IR);

        /*Do not print stack on the first instruction*/
        if(numOfInstruction == 0)
            numOfInstruction = 1;
        else
            numOfInstruction = 2;

        /*Step 2: Execute the code*/
        execute(&SP, &BP, &PC, &IR);

    }

    printf("\nThe program has been executed successfully");

    return 0;
}

int base(int *BP, int L){
    int arb = *BP;   //arb = activation record base
    while(L > 0){
        arb = PAS[arb];
        L--;
    }
    return arb;
}

void fetch(int *PC, Instruction *IR){
    IR->op = PAS[*PC];
    IR->l = PAS[*PC+1];
    IR->m = PAS[*PC+2];

    *PC += 3; 
}

void executeORP(int *SP, int *BP, int *PC,Instruction *IR){
    switch(IR->m){
        case RTN:
            *SP = *BP - 1;
            *BP = PAS[*SP + 2];
            *PC = PAS[*SP + 3];

            //if RTN is called, the lastest AR will be pop-sup from the stack because it is executed already
            barLocation[curBar] = 0;
            curBar--;
            
            break;    
        case ADD:
            PAS[*SP-1] = PAS[*SP-1] + PAS[*SP];
            *SP -= 1;
            break;   
        case SUB:
            PAS[*SP-1] = PAS[*SP-1] - PAS[*SP];
            *SP -= 1;
            break;
        case MUL:
            PAS[*SP-1] = PAS[*SP-1] * PAS[*SP];
            *SP -= 1;
            break;
        case DIV:
            PAS[*SP-1] = PAS[*SP-1] / PAS[*SP];
            *SP -= 1;
            break;
        case EQL:
            PAS[*SP-1] = PAS[*SP-1] == PAS[*SP];
            *SP -= 1;
            break;
        case NEQ:
            PAS[*SP-1] = PAS[*SP-1] != PAS[*SP];
            *SP -= 1;
            break;
        case LSS:
            PAS[*SP-1] = PAS[*SP-1] < PAS[*SP];
            *SP -= 1;
            break;
        case LEQ:
            PAS[*SP-1] = PAS[*SP-1] <= PAS[*SP];
            *SP -= 1;
            break;
        case GTR:
            PAS[*SP-1] = PAS[*SP-1] > PAS[*SP];
            *SP -= 1;
            break;
        case GEQ:
            PAS[*SP-1] = PAS[*SP-1] >= PAS[*SP];
            *SP -= 1;
            break;
        default:
            break;
    }
}

void executeSYS(int *SP, int *BP, int *PC,Instruction *IR){

    switch (IR->m)
    {
    case 1:
        printf("Output result is: %d\n", PAS[*SP]);
        *SP -= 1;
        break;
    case 2:
        *SP += 1;
        printf("Please enter an Integer:");
        scanf("%d", &PAS[*SP]);
        break;
    case 3:
        //set Halt flag to zero(End of the program)
        haltFlag = 0;
        break;
    default:
        break;
    }
}

void execute(int *SP, int *BP, int *PC, Instruction *IR){
    //String to print out the instruction type, 4 because OP is only three letters
    char string[4];             

    switch(IR->op){
        case LIT:
            *SP += 1;
            PAS[*SP] = IR->m;
            printOutput(SP, BP, PC, IR, string);
            break;
        case OPR:
            executeORP(SP, BP, PC, IR);
            printOutput(SP, BP, PC, IR, string);
            break;
        case LOD:
            *SP += 1;
            PAS[*SP] = PAS[base(BP,IR->l) + IR->m];
            printOutput(SP, BP, PC, IR, string);
            break;
        case STO:
            PAS[base(BP, IR->l)+ IR->m] = PAS[*SP];
            *SP -= 1;
            printOutput(SP, BP, PC, IR, string);
            break;
        case CAL:
            barLocation[barCount] = *SP+1;

            // printf("\nValue of bar location: %d\n", barLocation[barCount]);

            PAS[*SP + 1] = base(BP,IR->l);
            PAS[*SP + 2] = *BP;
            PAS[*SP + 3] = *PC;
            *BP = *SP + 1;
            *PC = IR->m;
            printOutput(SP, BP, PC, IR, string);

            curBar = barCount;
            barCount++;
            break;
        case INC:
            *SP = *SP + IR->m;            
            printOutput(SP, BP, PC, IR, string);
            break;
        case JMP:                                             
            *PC = IR->m;
            printOutput(SP, BP, PC, IR, string);
            break;
        case JPC:
            if(PAS[*SP] == 0)
                *PC = IR->m;
            *SP -= 1;
            printOutput(SP, BP, PC, IR, string);
            break;
        case SYS:
            executeSYS(SP, BP, PC, IR);
            printOutput(SP, BP, PC, IR, string);
            break;
        default:
            break;
        
    }
}

void typeOpCode(char *stringOP, int OP){
    switch (OP)
    {
        case LIT:
            strcpy(stringOP, "LIT");
            break;
        case OPR:
            strcpy(stringOP, "OPR");
            break;
        case LOD:
            strcpy(stringOP, "LOD");
            break;
        case STO:
            strcpy(stringOP, "STO");
            break;
        case CAL:
            strcpy(stringOP, "CAL");
            break;
        case INC:
            strcpy(stringOP, "INC");
            break;
        case JMP:
            strcpy(stringOP, "JMP");
            break;
        case JPC:
            strcpy(stringOP, "JPC");
            break;
        case SYS:
            strcpy(stringOP, "SYS");
            break;
        default:
            break;
    }  
}

void typeOPR(char *stringOPR, int opr){
    switch (opr)
    {
        case RTN:
            strcpy(stringOPR, "RTN");
            break;
        case ADD:
            strcpy(stringOPR, "ADD");
            break;
        case SUB:
            strcpy(stringOPR, "SUB");
            break;
        case MUL:
            strcpy(stringOPR, "MUL");
            break;
        case DIV:
            strcpy(stringOPR, "DIV");
            break;
        case EQL:
            strcpy(stringOPR, "EQL");
            break;
        case NEQ:
            strcpy(stringOPR, "NEQ");
            break;
        case LSS:
            strcpy(stringOPR, "LSS");
            break;
        case LEQ:
            strcpy(stringOPR, "LEQ");
            break;
        case GTR:
            strcpy(stringOPR, "GTR");
            break;
        case GEQ:
            strcpy(stringOPR, "GEQ");
            break;
        default:
            break;
    }  
}

/*  Execution
    Print the instructions and values of PC, BP, SP, and stack
    1. Check which type of OP code:
        -If it is OPR: get that type of OPR
        -Else: get the type of OP code
    2. Print Operation (string), L, M, PC, BP, SP
    3. Print stack with conditions: add bar if there is a new AR for easy reading
*/
void printOutput(int *SP, int *BP, int *PC, Instruction *IR, char string[]){
   if (IR->op == 2)
        typeOPR(string, IR->m);
    else 
       typeOpCode(string,IR->op);

    printf("\t%s %1d %3d %5d %5d %6d\t",string,IR->l,IR->m, *PC,*BP, *SP);

    int barLocationIndex = 0;
    
    if(numOfInstruction != 1){
        for(int i = counter; i <= *SP; i++){
            if(i == barLocation[barLocationIndex]){
                printf("|");
                barLocationIndex++;
            }
            printf(" %d ",PAS[i]);
        }
    }
    printf("\n");

}

/* Print list of instruction*/
void printInstrList(){
    //get number of instructions
    int num = 0;
    char string[256];
    int op = 0;

    while(num < counter){
        op = PAS[num];
        if (op == 2)
            typeOPR(string, PAS[num+2]);
        else 
            typeOpCode(string,op);
        printf("%s %3d %3d\n", string, PAS[num+1],PAS[num+2]);

        num = num + 3;
    }
}