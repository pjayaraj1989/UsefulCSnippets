#include <stdio.h>  
#include <dlfcn.h>  

/**
 * @brief Prints information about a symbol at a given address.
 *
 * This function uses the `dladdr` function to retrieve and print information
 * about a symbol located at the specified address. The information includes
 * the symbol name, symbol address, shared object name, and shared object base address.
 *
 * @param addr The address of the symbol to inspect.
 */
void printSymbolInfo(void* addr);  

void printSymbolInfo(void* addr) {  
    Dl_info info;  
    if (dladdr(addr, &info)) {  
        printf("Symbol name: %s\n", info.dli_sname);  
        printf("Symbol address: %p\n", info.dli_saddr);  
        printf("Shared object name: %s\n", info.dli_fname);  
        printf("Shared object base address: %p\n", info.dli_fbase);  
    } else {  
        printf("Failed to retrieve symbol information.\n");  
    }  
}  
  
int main() {  
    void* funcPtr = (void*)main; // Replace with the address of the function you want to inspect  
  
    printSymbolInfo(funcPtr);  
  
    return 0;  
}
