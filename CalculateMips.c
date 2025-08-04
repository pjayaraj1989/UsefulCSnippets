#include <stdio.h>
#include <time.h>

// Function to simulate instruction execution
void execute_instructions(int num_instructions)
{
    volatile int dummy = 0;
    for (int i = 0; i < num_instructions; i++)
    {
        dummy += i; // Simple operation to simulate instruction execution
    }
}

int main()
{
    int num_instructions = 1000000; // Number of instructions to execute
    clock_t start_time, end_time;
    double cpu_time_used;
    double mips;

    // Start measuring time
    start_time = clock();

    // Execute instructions
    execute_instructions(num_instructions);

    // Stop measuring time
    end_time = clock();

    // Calculate the time taken in seconds
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Calculate MIPS
    mips = (num_instructions / cpu_time_used) / 1000000.0;

    printf("Number of instructions: %d\n", num_instructions);
    printf("Time taken: %f seconds\n", cpu_time_used);
    printf("MIPS: %f\n", mips);

    return 0;
}
