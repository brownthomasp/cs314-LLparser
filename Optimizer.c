/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2016                              *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

void markInstructionsForThisRegister(Instruction *head, int);
void markInstructionsForThisVar(Instruction *, int);
void markCriticalInstructions(Instruction *);
void removeNonCriticalInstructions(Instruction *);

void markInstructionsForThisRegister(Instruction *head, int reg) {
	Instruction *currInstr = head;
	
//	printf("Marking for register %d\n", reg);

	while(currInstr != NULL) {
		if (currInstr->field1 == reg) {
			currInstr->critical = '1';

			switch(currInstr->opcode) {
				case LOAD:
					markInstructionsForThisVar(head, currInstr->field2);
					break;	
				case ADD: 
				case SUB: 
				case MUL:
					markInstructionsForThisRegister(head, currInstr->field2);
					markInstructionsForThisRegister(head, currInstr->field3);
					break;
				default:
					break;
			} 
		}

		currInstr = currInstr->next;
	}
}

void markInstructionsForThisVar(Instruction *head, int var) {
	Instruction *currInstr = head;

//	printf("marking for var %c\n", var);

	while (currInstr != NULL) {
		if (currInstr->opcode == STORE && currInstr->field1 == var) {
			if (currInstr->critical == '1') {
				return;
			}			

			currInstr->critical = '1';
			markInstructionsForThisRegister(head, currInstr->field2);
			break;
		}

		currInstr = currInstr->next;
	}	
}

void markCriticalInstructions(Instruction *head) {
	Instruction *currInstr = head;

	while (currInstr != NULL) {
		if (currInstr->opcode == READ) {
			currInstr->critical = '1';
		} else if (currInstr->opcode == WRITE) {
			currInstr->critical = '1';

			markInstructionsForThisVar(head, currInstr->field1);
		}

		currInstr = currInstr->next;
	}
}

void removeNonCriticalInstructions(Instruction *head) {
	Instruction *currInstr = head, *prevInstr = head->prev;

	while (currInstr != NULL) {
		if (currInstr->critical != '1') {
			if (prevInstr != NULL) {
				prevInstr->next = currInstr->next;
				free(currInstr);
				currInstr = prevInstr->next;
			} else {
				currInstr = currInstr->next;
				prevInstr = currInstr->prev;
				free(prevInstr);
			}
		} else {
			prevInstr = currInstr;
			currInstr = currInstr->next;
		}
	}

}

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	markCriticalInstructions(head);	
	removeNonCriticalInstructions(head);

	if (head) {
		PrintInstructionList(stdout, head);
		DestroyInstructionList(head);
	}
	return EXIT_SUCCESS;
}

