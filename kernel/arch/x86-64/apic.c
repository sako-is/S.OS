#include <defines.h>
#ifdef X86_64
#include <kernel/arch/x86-64/apic.h>

static inline void cpuid(int code, uint32* a, uint32* d) {
	asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

bool cpuHasMSR() {
	static uint32 a, d; // eax, edx
	cpuid(1, &a, &d);
	return d & (1 << 5);
}
 
void cpuGetMSR(uint32 msr, uint32 *lo, uint32 *hi) {
	asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
 
void cpuSetMSR(uint32 msr, uint32 lo, uint32 hi) {
	asm volatile("wrmsr" :: "a"(lo), "d"(hi), "c"(msr));
}

void setAPICbase(uintptr apic) {
	uint32 edx = 0;
	uint32 eax = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;
 
	#ifdef __PHYSICAL_MEMORY_EXTENSION__
	edx = (apic >> 32) & 0x0f;
	#endif
 
	cpuSetMSR(IA32_APIC_BASE_MSR, eax, edx);
}

uintptr getAPICbase() {
	uint32 eax, edx;
	cpuGetMSR(IA32_APIC_BASE_MSR, &eax, &edx);
 
	return (eax & 0xfffff000);
}

void initAPIC() {
	uintptr p = getAPICbase();
	setAPICbase(p);

	uint8* ptr = (uint8*)(p + 0xf0);
	(*((uint32*)ptr)) = 39 | 0x100;
}

bool checkAPIC() {
	uint32 eax, edx;
	cpuid(1, &eax, &edx);
	return edx & (1 << 9);
}
#endif
