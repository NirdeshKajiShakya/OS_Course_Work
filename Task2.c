#include <stdio.h>
#include <stdlib.h>

void print_frames(int frames[], int frame_count, int page, int fault) {
    printf("Page %d: [", page);
    for (int i = 0; i < frame_count; i++) {
        if (frames[i] == -1) printf(" - ");
        else printf("%2d ", frames[i]);
    }
    printf("] %s\n", fault ? "Page Fault" : "Hit");
}

void fifo(int ref[], int size, int frame_count) {
    int *frames = malloc(frame_count * sizeof(int));
    int q_ptr = 0, faults = 0, hits = 0;
    
    for (int i = 0; i < frame_count; i++) frames[i] = -1;
    
    for (int i = 0; i < size; i++) {
        int page = ref[i];
        int hit = 0;
        
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == page) {
                hit = 1;
                break;
            }
        }
        
        if (hit) {
            hits++;
            print_frames(frames, frame_count, page, 0);
        } else {
            faults++;
            frames[q_ptr] = page; // Evict the oldest page blindly
            q_ptr = (q_ptr + 1) % frame_count;
            print_frames(frames, frame_count, page, 1);
        }
    }
    
    printf("\nFIFO Summary - Faults: %d, Hits: %d, Hit Ratio: %.2f%%\n", faults, hits, (float)hits/size*100);
    free(frames);
}

void lru(int ref[], int size, int frame_count) {
    int *frames = malloc(frame_count * sizeof(int));
    int *lru_time = malloc(frame_count * sizeof(int));
    int time_counter = 0, faults = 0, hits = 0;
    
    for (int i = 0; i < frame_count; i++) {
        frames[i] = -1;
        lru_time[i] = 0;
    }
    
    for (int i = 0; i < size; i++) {
        int page = ref[i];
        time_counter++;
        int hit = 0;
        
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == page) {
                hit = 1;
                lru_time[j] = time_counter; // Update access time on hit
                break;
            }
        }
        
        if (hit) {
            hits++;
            print_frames(frames, frame_count, page, 0);
        } else {
            faults++;
            int lru_idx = 0;
            int empty_found = 0;
            
            for (int j = 0; j < frame_count; j++) {
                if (frames[j] == -1) {
                    lru_idx = j; // Prioritise empty frames over eviction
                    empty_found = 1;
                    break;
                }
            }
            
            if (!empty_found) {
                for (int j = 1; j < frame_count; j++) {
                    if (lru_time[j] < lru_time[lru_idx]) {
                        lru_idx = j; // Find the frame with the oldest access time
                    }
                }
            }
            
            frames[lru_idx] = page;
            lru_time[lru_idx] = time_counter;
            print_frames(frames, frame_count, page, 1);
        }
    }
    
    printf("\nLRU Summary  - Faults: %d, Hits: %d, Hit Ratio: %.2f%%\n", faults, hits, (float)hits/size*100);
    free(frames);
    free(lru_time);
}

int main(int argc, char *argv[]) {
    // Allow user to configure memory size via command line, default to 3
    int frame_count = (argc > 1) ? atoi(argv[1]) : 3;
    if (frame_count <= 0) frame_count = 3;

    int ref_string[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int size = sizeof(ref_string) / sizeof(ref_string[0]);
    
    printf("Simulating with %d frames...\n\n", frame_count);
    printf("--- FIFO Algorithm ---\n");
    fifo(ref_string, size, frame_count);
    
    printf("\n--- LRU Algorithm ---\n");
    lru(ref_string, size, frame_count);
    
    return 0;
}