#include <kernel/util/util.h>
#include <kernel/print/print.h>

#include <defines.h>

size_t strlen(const char* str) {
	size_t len = 0;
	while(str[len]) 
		len++;

	return len;
}

void reverse(char* str, int len) {
	int i, temp;  

	for(i = 0; i < len/2; i++) {  
		temp = str[i];  
		str[i] = str[len - i - 1];  
		str[len - i - 1] = temp;  
	}  
}

char* itoa(int num, char* buffer, int base) {
	int i = 0;
	bool is_negative = false;
 
	/* Handle 0 explicitly, otherwise empty string is printed for 0 */
	if(num == 0) {
		buffer[i] = '0';
		buffer[i++] = '\0';
		return buffer;
	}

	if(num < 0 && base == 10) {
		is_negative = true;
		num = -num;
	}
 
	while(num != 0) {
		int rem = num % base;
		buffer[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}

	if(is_negative) 
		buffer[i++] = '-';
	buffer[i] = '\0'; // Append string terminator

	reverse(buffer, i);

	return buffer;
}

char* uitoa(uint32 num, char* buffer, int base) {
	int i = 0;
 
	/* Handle 0 explicitly, otherwise empty string is printed for 0 */
	if(num == 0) {
		buffer[i] = '0';
		buffer[i++] = '\0';
		return buffer;
	}
 
	while(num != 0) {
		int rem = num % base;
		buffer[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}

	buffer[i] = '\0'; // Append string terminator

	reverse(buffer, i);

	return buffer;
}

uint64 uRound(float64 x) {
	return (uint64)(x + 0.5);
}
