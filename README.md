# mymalloc

A dynamic memory allocator written in C from scratch. It provides `my_malloc` and `my_free` over a fixed 64 KB heap, with first-fit allocation, block splitting, coalescing of adjacent free blocks, and 8-byte alignment.

## Overview

Each block in the heap begins with a header that records the payload size and a free flag. Blocks are stored back to back, so the allocator can traverse the heap by advancing past each header and its payload.

```
 Heap (64 KB static array)
 ┌────────┬───────────┬────────┬───────────┬────────┬─────────────┐
 │ header │  payload  │ header │  payload  │ header │   payload   │ ...
 └────────┴───────────┴────────┴───────────┴────────┴─────────────┘

 my_malloc(n)                       my_free(ptr)
   1. Scan blocks from the start      1. Locate the header (ptr - header size)
   2. Select the first free fit       2. Mark the block free
   3. Split if the remainder fits     3. Coalesce with adjacent free blocks
   4. Return pointer past header
```

## Features

- **First-fit allocation** from a static 64 KB heap
- **Block splitting** to avoid assigning oversized blocks to small requests
- **Coalescing** of adjacent free blocks to reduce external fragmentation
- **8-byte alignment** for all returned pointers

## Components

| Component | Implementation | Responsibility |
|-----------|----------------|----------------|
| Heap | Static `unsigned char` array of 64 KB | Stores all blocks; no OS interaction |
| Block header | `block_header` struct (`size`, `is_free`) | Describes each block and locates the next one |
| `my_malloc` | First-fit scan with split | Allocates a block and returns a usable pointer |
| `my_free` | Mark free, then `coalesce()` | Releases a block and merges free neighbors |
| `coalesce` | Linear pass over the heap | Combines adjacent free blocks |
| Stress test | `stress.c`, fixed random seed | Validates correctness under 10,000 random operations |

## Build and run

Requires `gcc`. Tested with MSYS2 (UCRT64) on Windows.

```bash
# Demonstration
gcc main.c mymalloc.c -o malloc_test
./malloc_test

# Stress test
gcc stress.c mymalloc.c -o stress_test
./stress_test
```

## Testing

The stress test maintains 50 slots. Each round either allocates a random-sized block and fills it with a slot-specific byte, or verifies that block's contents and frees it. Any mismatch is reported as corruption. After all operations, the test allocates 60 KB to confirm that freed memory was coalesced.

```
OK: 60 KB allocation succeeded after all frees
Total failures: 0
```

## Limitations

- The heap is fixed at 64 KB and does not grow.
- First-fit scans the full heap on every call, so performance degrades with many blocks.
- Coalescing runs a full pass on every free instead of merging only with neighbors.
- Memory is not requested from the operating system.

## Learning outcomes

- How `free()` determines a block's size without a separate bookkeeping structure
- Why alignment is required for correct and efficient memory access
- How fragmentation develops and how coalescing mitigates it
- Diagnosing a defect caused by a block that was never split
