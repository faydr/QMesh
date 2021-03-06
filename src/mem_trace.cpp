#include "mbed.h"
#include "mem_trace.hpp"
#include "serial_data.hpp"

void print_memory_info() {
    // allocate enough room for every thread's stack statistics
#if 0
    int cnt = osThreadGetCount();
    mbed_stats_stack_t *stats = (mbed_stats_stack_t*) malloc(cnt * sizeof(mbed_stats_stack_t));
 
    cnt = mbed_stats_stack_get_each(stats, cnt);
    for (int i = 0; i < cnt; i++) {
        debug_printf(DBG_INFO, "Thread: 0x%lX, Stack size: %lu / %lu\r\n", (unsigned long) stats[i].thread_id, 
            (unsigned long) stats[i].max_size, (unsigned long) stats[i].reserved_size);
    }
    free(stats);
#endif

    // Grab the heap statistics
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    debug_printf(DBG_INFO, "Heap size: %lu / %lu bytes\r\n", (unsigned long) heap_stats.current_size, 
        (unsigned long) heap_stats.reserved_size);
}