// STD
#include <iostream>

// VM
#include "virtual_machine.hpp"
#include "virtual_memory.hpp"

int main()
{
	printf("Testing x86-64 virtual machine\n");

	constexpr std::uint8_t buffer[] = {
		
		// ADD (DISPLACED DESTINATION) VARIANTS

		// MODE 2
		0x49, 0x81, 0x81, 0x20, 0x04, 0x00, 0x00, 0x37, 0x13, 0x00, 0x00,	// ADD [R9+0x420], 1337

		// MODE 3
		0x66, 0x48, 0x81, 0xC3, 0x00, 0x00, 0x05, 0x00,						// ADD RBX, 50000

		0x66, 0x41, 0x83, 0x81, 0x20, 0x04, 0x00, 0x00, 0x13				// ADD word ptr [r9+420], 13

		// ADD (DISPLACED SOURCE) VARIANTS
		// MODE 0
		// 0x4C, 0x03, 0x07,							// ADD R8, [RDI]
		// 
		// // MODE 1
		// 0x67, 0x4C, 0x03, 0x47, 0x0A,				// ADD R8, [EDI+0x0A]
		// 
		// // MODE 2
		// 0x4C, 0x03, 0x87, 0x00, 0x01, 0x00, 0x00,	// ADD R8, [RDI+0x100]
		// 
		// // MODE 3
		// 0x4C, 0x03, 0xC7								// ADD R8, RDI
	};

	auto vm = virtual_machine(buffer);

	// SETUP REGISTERS FOR EMULATOR TEST

	// RBX
	vm.context().get(x86::registr::id::rbx).qword = 0x1337;

	// RDI
	const auto rdi_address = reinterpret_cast<std::uint8_t*>(0xFFFFF);
	auto rdi_value = 0xABCDEF;
	vm.memory().allocate(rdi_address, sizeof(rdi_value));
	vm.memory().write(rdi_value, rdi_address + 0x0A);
	vm.context().get(x86::registr::id::rdi).ptr = rdi_address;

	// R9
	const auto r9_address = reinterpret_cast<std::uint8_t*>(0xFEEEEFEEEE);
	auto r9_value = 0xEEEE;
	vm.memory().allocate(r9_address + 0x420, sizeof(r9_value));
	vm.memory().write(r9_value, r9_address + 0x420);
	vm.context().get(x86::registr::id::r9).ptr = r9_address;

	// PRINT REGISTERS
	printf("Printing registers\n");
	for (size_t i = 0; i < x86::registr::id::END_OF_REGISTER; i++)
	{
		auto id = static_cast<x86::registr::id>(i);
		printf("[%s] %016llX\n", x86::registr::names[id][x86::registr::size::qword].c_str(), vm.context().get(id).qword);
	}
	printf("\n");

	// PRINT MEMORY BUFFER 
	printf("Printing memory buffers\n");
	printf("R9+0x420: %p\n", vm.memory().read<void*>(r9_address + 0x420));

	printf("\n");

	// RUN VM WITH SPECIFIED RELATIVE INSTRUCTION POINTER (BEGINNING)
	vm.run(0x00);

	printf("\n");

	// PRINT REGISTERS
	printf("Printing registers\n");;
	for (size_t i = 0; i < x86::registr::id::END_OF_REGISTER; i++)
	{
		auto id = static_cast<x86::registr::id>(i);
		printf("[%s] %016llX\n", x86::registr::names[id][x86::registr::size::qword].c_str(), vm.context().get(id).qword);
	}

	printf("\n");

	// PRINT MEMORY BUFFER 
	printf("Printing memory buffers\n");
	printf("R9+0x420: %p\n", vm.memory().read<void*>(r9_address + 0x420));

	return 0;
}

