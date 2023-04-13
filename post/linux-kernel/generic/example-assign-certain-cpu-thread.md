# An Example of assigning lowest usage CPU to thread  

- pthread_setaffinity_np to set CPU affinity
- sched_getaffinity   to get CPU affinity info


``` C
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#define MAX_CPU_COUNT 64

// Structure to hold CPU usage information
typedef struct cpu_usage_s {
    int cpu_id;
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;
    long steal;
    long guest;
    long guest_nice;
} cpu_usage_t;

// Function to read CPU usage information from /proc/stat
int read_cpu_usage(cpu_usage_t *usage, int cpu_count) {
    FILE *fp;
    char buf[512];
    int i;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    for (i = 0; i < cpu_count; i++) {
        if (fgets(buf, sizeof(buf), fp) == NULL) {
            perror("fgets");
            fclose(fp);
            return -1;
        }

        if (strncmp(buf, "cpu", 3) != 0) {
            // Not a CPU line, skip
            continue;
        }

        // Parse CPU usage information
        sscanf(buf, "cpu%d %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
            &usage[i].cpu_id, &usage[i].user, &usage[i].nice, &usage[i].system,
            &usage[i].idle, &usage[i].iowait, &usage[i].irq, &usage[i].softirq,
            &usage[i].steal, &usage[i].guest, &usage[i].guest_nice);
    }

    fclose(fp);
    return 0;
}

// Function to calculate the total CPU usage time
long total_cpu_usage(cpu_usage_t *usage, int cpu_count) {
    long total = 0;
    int i;

    for (i = 0; i < cpu_count; i++) {
        total += usage[i].user + usage[i].nice + usage[i].system +
            usage[i].idle + usage[i].iowait + usage[i].irq +
            usage[i].softirq + usage[i].steal + usage[i].guest +
            usage[i].guest_nice;
    }

    return total;
}

// Function to calculate the CPU usage percentage for a given CPU
float cpu_usage_percentage(cpu_usage_t *prev_usage, cpu_usage_t *curr_usage) {
    long prev_total = prev_usage->user + prev_usage->nice + prev_usage->system +
        prev_usage->idle + prev_usage->iowait + prev_usage->irq +
        prev_usage->softirq + prev_usage->steal + prev_usage->guest +
        prev_usage->guest_nice;
    long curr_total = curr_usage->user + curr_usage->nice + curr_usage->system +
        curr_usage->idle + curr_usage->iowait + curr_usage->irq +
        curr_usage->softirq + curr_usage->steal + curr_usage->guest +
        curr_usage->guest_nice;
    long diff_total = curr_total - prev_total;
    long diff_idle = curr_usage->idle - prev_usage->idle;

    return ((float)(diff_total - diff_idle) / diff_total) * 100.0;
}

// Thread function
void *thread_func(void *arg) {
    int cpu_id = *((int *)arg);
    cpu_set_t cpuset;

    // Set the thread affinity to the specified CPU
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
        pthread_exit(NULL);
    }

    // Do some work on the CPU
    while (1) {
        // Do some work...
    }
    pthread_exit(NULL);
}
int main() {
    int i, cpu_count, min_cpu_id;
    float min_cpu_usage, cpu_usage;
    cpu_usage_t *prev_usage, *curr_usage;
    pthread_t *threads;
    int *cpu_ids;
    // Get the number of CPUs on the system
cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
if (cpu_count <= 0 || cpu_count > MAX_CPU_COUNT) {
    printf("Invalid CPU count: %d\n", cpu_count);
    return -1;
}

// Allocate memory for CPU usage information
prev_usage = (cpu_usage_t *)calloc(cpu_count, sizeof(cpu_usage_t));
curr_usage = (cpu_usage_t *)calloc(cpu_count, sizeof(cpu_usage_t));
if (prev_usage == NULL || curr_usage == NULL) {
    perror("calloc");
    return -1;
}

// Allocate memory for threads and CPU IDs
threads = (pthread_t *)calloc(cpu_count, sizeof(pthread_t));
cpu_ids = (int *)calloc(cpu_count, sizeof(int));
if (threads == NULL || cpu_ids == NULL) {
    perror("calloc");
    return -1;
}

// Start the threads
for (i = 0; i < cpu_count; i++) {
    cpu_ids[i] = i;
    if (pthread_create(&threads[i], NULL, thread_func, &cpu_ids[i]) != 0) {
        perror("pthread_create");
        return -1;
    }
}

// Monitor CPU usage and assign threads to CPUs with the lowest usage
while (1) {
    // Read CPU usage information
    if (read_cpu_usage(curr_usage, cpu_count) != 0) {
        // Error reading CPU usage, exit
        break;
    }

    // Calculate CPU usage percentage for each CPU
    for (i = 0; i < cpu_count; i++) {
        cpu_usage = cpu_usage_percentage(&prev_usage[i], &curr_usage[i]);
        printf("CPU %d usage: %.2f%%\n", i, cpu_usage);
    }

    // Find the CPU with the lowest usage
    min_cpu_id = 0;
    min_cpu_usage = cpu_usage_percentage(&prev_usage[0], &curr_usage[0]);
    for (i = 1; i < cpu_count; i++) {
        cpu_usage = cpu_usage_percentage(&prev_usage[i], &curr_usage[i]);
        if (cpu_usage < min_cpu_usage) {
            min_cpu_id = i;
            min_cpu_usage = cpu_usage;
        }
    }

    // Assign new threads to the CPU with the lowest usage
    for (i = 0; i < cpu_count; i++) {
        if (CPU_ISSET(i, &threads[min_cpu_id].cpuset) == 0) {
            // Thread i is not assigned to the CPU with the lowest usage, reassign it
            CPU_CLR(cpu_ids[i], &threads[i].cpuset);
            CPU_SET(min_cpu_id, &threads[i].cpuset);
            if (pthread_setaffinity_np(threads[i], sizeof(cpuset), &threads[i].cpuset) != 0) {
                perror("pthread_setaffinity_np");
                return -1;
            }
       }
    }
     // Save current CPU usage for next iteration
    memcpy(prev_usage, curr_usage, cpu_count * sizeof(cpu_usage_t));

    // Sleep for a short time to avoid high CPU usage
    usleep(1000);
}

// Join threads
for (i = 0; i < cpu_count; i++) {
    pthread_join(threads[i], NULL);
}

// Free allocated memory
free(prev_usage);
free(curr_usage);
free(threads);
free(cpu_ids);

return 0;
}

```


The main function starts by getting the number of CPUs on the system using the sysconf() function. It then allocates memory for CPU usage information, threads, and CPU IDs. It starts the threads, and in the main loop, it reads the CPU usage information, calculates the CPU usage percentage for each CPU, finds the CPU with the lowest usage, and assigns new threads to that CPU using pthread_setaffinity_np(). Finally, it sleeps for a short time to avoid high CPU usage, and repeats the process in the next iteration.

Note that this is just an example, and the actual implementation may vary depending on the specific requirements and constraints of your application. Also, keep in mind that assigning threads to CPUs with low usage may not always result in optimal performance, as there may be other factors to consider, such as memory access patterns and cache locality.