#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "elf.h"

#define MODULE_NAME "elf_module.elf"

// Functions exposed to the ELF module
int console_output(const char* text){
	iprintf("%s", text);
	return 0;
}


void panic(const char* error){
	iprintf("%s\n", error);
	iprintf("\nPANIC: FAULT!\n");
	while(1) swiWaitForVBlank();
}

int main(int argc, char **argv) {
	consoleDemoInit();
	iprintf("BraisSG ELF Loader test starting...\n");
	iprintf("INIT FAT... "); fflush(stdout); swiWaitForVBlank();
	if (fatInitDefault()) {
		iprintf("[OK]\n"); fflush(stdout);
		swiWaitForVBlank();
	} else {
		iprintf("[ERR]\n");
		panic("fatInitDefault failure: terminating\n");
	}


	// Load the module
	FILE* elf_module = fopen(MODULE_NAME, "rb");
	if(!elf_module){
		panic("ELF Module " MODULE_NAME " not found!");
	}

	fseek(elf_module, 0, SEEK_END);
	int modsize = ftell(elf_module);
	fseek(elf_module, 0, SEEK_SET);

	iprintf("ELF file is %d bytes, loading to RAM...\n", modsize);
	void* elf_ram = (void*) malloc(modsize);
	if(!elf_ram){
		panic("Out of memory error!");
	}

	int bytes_loaded = (int) fread(elf_ram, modsize, 1, elf_module);
	fclose(elf_module);
	iprintf("Loaded %d bytes\n", bytes_loaded);

	void* handle = elf_dlmemopen(elf_ram, ELF_RTLD_DEFAULT);
	char error = elf_dlerror(handle);

	if(error){
		elf_dlclose(handle);
		panic("ELF error!\n");
	} else {
		iprintf("handle opened!\n");
		swiWaitForVBlank();
	}

	void * memory = malloc( elf_lbounds( handle ) );
	if(!memory){
		elf_dlclose(handle);
		panic("Out of memory for relocation!\n");
	} else {
		iprintf("alloc for relocation...\n");
		swiWaitForVBlank();
	}

	iprintf("Start dynamic linking...\n");
	elf_link(handle, memory);
	error = elf_dlerror( handle );
	if (error) {
		elf_dlclose(handle);
		panic("Link error\n");
	} else {
		iprintf("elf dynamic link done!\n");
	}

	typedef int (*puts_t)(const char *s);
	typedef void ( * func_t )( puts_t );
	
	func_t start_sym = ( func_t ) elf_dlsym( handle, "_start");

	if (!start_sym) {
 		error = elf_dlerror( handle );
		elf_dlclose(handle);
		panic("Cannot find _start symbol!\n");
	} else {

		iprintf("Calling _start!\n"); swiWaitForVBlank(); swiWaitForVBlank();
  		( *start_sym )(&console_output);
		swiWaitForVBlank();
	}


	elf_dlclose(handle);
	free(memory);
	free(elf_ram);

	iprintf("\nELF memory freed, done!\n");

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		if(keysDown() & KEY_START) break;
	}

	return 0;
}
