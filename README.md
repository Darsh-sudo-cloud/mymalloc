# mymalloc

My own version of `malloc` and `free` in C. I built it to figure out how memory allocation actually works under the hood.

## What it does

- Gives out memory from a fixed 64 KB heap
- Splits big free chunks so small requests don't waste space
- Merges neighboring free chunks when you free something, so the heap doesn't get chopped up
- Rounds every allocation up to 8 bytes
- Has a stress test that does 10,000 random allocs and frees and checks nothing got overwritten

## How it works

Every block starts with a small header that stores its size and whether it's free. `my_malloc` walks through the blocks from the start and takes the first free one that's big enough. If it's much bigger than needed, it cuts it in two. `my_free` marks the block free and then merges it with any free blocks right next to it.

## Running it

With gcc (I used MSYS2 on Windows):

    gcc main.c mymalloc.c -o malloc_test.exe
    ./malloc_test.exe

    gcc stress.c mymalloc.c -o stress_test.exe
    ./stress_test.exe

The stress test uses a fixed seed, so if it ever fails, you can run it again and get the same failure.

## Limitations

- The heap is fixed at 64 KB
- The search is slow on big heaps since it checks every block
- It doesn't ask the OS for more memory

## What I learned

- How `free()` knows a block's size without a separate list
- Why alignment matters
- How fragmentation builds up, and how merging fixes it
- Tracking down a crash that happened because a block never got split

## Notes

I built this with some AI help while learning. I wrote the tests, worked through the bugs, and went through each part of the code so I could explain it.
