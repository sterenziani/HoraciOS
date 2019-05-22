#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <stdint.h>
#include <speaker.h>
#include <time.h>
#include <SysCallDispatcher.h>
#include <interrupts.h>
#include <keyboard.h>
#include <time.h>
#include <memoryManager.h>
#include <mailbox.h>
#include <mutex.h>
#include <sem.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

//Points to Userland's main function
static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const systemVar = (void *)0x5A00;
static void * const memoryBaseAddress = (void*)0x900000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	print("[x64BareBones]");
	newLine();

	print("CPU Vendor:");
	print(cpuVendor(buffer));
	newLine();

	print("[Loading modules]");
	newLine();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	print("[Done]");
	newLine();
	newLine();

	print("[Initializing kernels binary]");
	newLine();

	clearBSS(&bss, &endOfKernel - &bss);

	print("  text: 0x");
	printHex((uint64_t)&text);
	newLine();
	print("  rodata: 0x");
	printHex((uint64_t)&rodata);
	newLine();
	print("  data: 0x");
	printHex((uint64_t)&data);
	newLine();
	print("  bss: 0x");
	printHex((uint64_t)&bss);

	newLine();
	print("[Done]");
	newLine();
	newLine();
	return getStackBase();
}

int main()
{
	uint32_t mem_amount = 128; 																//Asignamos 128 MB de memoria
	uint64_t mem_amount_bytes = mem_amount * (1 << 20);				// Pasa a bytes
	init_memoryManager(memoryBaseAddress, mem_amount_bytes);	//Inicializamos el memoryManager en la dirección 0x900000 pues en clase vimos que en esa dirección está libre
	init_post_office();
	init_mutex_directory();
	init_sem_directory();

	//Guardo las posiciones del main de Userland tal que se puedan restaurar
	extern uint64_t * RestoreIP;
	RestoreIP = sampleCodeModuleAddress;
	extern void * RestoreSP;
	RestoreSP = (void*)(_getRSP() - 2*8);
	clearScreen();
	load_idt();
	while(1);
	return 0;
}
