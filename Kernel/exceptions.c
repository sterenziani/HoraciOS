#include <naiveConsole.h>
#include <lib.h>
#include <stdlib.h>
#include <font.h>

static void print_register_values();

void exceptionDispatcher(int exception_id, uint64_t* RIP, uint64_t* RSP)
{
	extern uint64_t * RestoreIP;
	extern void * RestoreSP;
	uint64_t RIP_Back = (uint64_t)&RIP;
	uint64_t RSP_Back = (uint64_t)&RSP;
	*RIP = (uint64_t)RestoreIP;
	*RSP = (uint64_t)RestoreSP;
	switch(exception_id)
	{
		case 0: printFormat("ZERO DIVISION EXCEPTION", ORANGE_COLOR, BACKGROUND_COLOR);
						break;
		case 6: printFormat("INVALID OPERATION EXCEPTION", ORANGE_COLOR, BACKGROUND_COLOR);
						break;
	}
	print_register_values(RIP_Back, RSP_Back);
}

static void print_register_values(uint64_t * RIP, uint64_t * RSP)
{
	//Apunta al RAX pusheado por el pushState
	//Cada 8 bytes a su derecha estara alguno de los registros pusheados
	uint64_t * oldRSP = RIP + 8;
	newLine();
	print("RIP: 0x");
	printHex((uint64_t)RIP);
	print("              ");
	print("RSP: 0x");
	printHex((uint64_t)RSP);
	print("              ");
	print("RBP: 0x");
	printHex(oldRSP[8*4]);
	newLine();
	print("RAX: 0x");
	printHex(oldRSP[8*0]);
	print("          ");
	print("RBX: 0x");
	printHex(oldRSP[8*1]);
	print("          ");
	print("RCX: 0x");
	printHex(oldRSP[8*2]);
	print("          ");
	print("RDX: 0x");
	printHex(oldRSP[8*3]);
	print("          ");
	newLine();
	print("RDI: 0x");
	printHex(oldRSP[8*5]);
	print("          ");
	print("RSI: 0x");
	printHex(oldRSP[8*6]);
	newLine();
	print("R8: 0x");
	printHex(oldRSP[8*7]);
	print("          ");
	print("R9: 0x");
	printHex(oldRSP[8*8]);
	print("          ");
	print("R10: 0x");
	printHex(oldRSP[8*9]);
	print("          ");
	print("R11: 0x");
	printHex(oldRSP[8*10]);
	newLine();
	print("R12: 0x");
	printHex(oldRSP[8*11]);
	print("          ");
	print("R13: 0x");
	printHex(oldRSP[8*12]);
	print("          ");
	print("R14: 0x");
	printHex(oldRSP[8*13]);
	print("          ");
	print("R15: 0x");
	printHex(oldRSP[8*14]);
	newLine();
	return;
}
