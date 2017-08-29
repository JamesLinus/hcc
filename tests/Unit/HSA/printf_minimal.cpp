// RUN: %hc %s -lhc_am -o %t.out && %t.out | %FileCheck %s

#include <hc.hpp>
#include <hc_printf.hpp>

#include <iostream>

#define TILE (16)
#define GLOBAL (TILE)

#define PRINTF_BUFFER_SIZE (54)

int main() {
  using namespace hc;

  accelerator acc = accelerator();
  PrintfPacket* printf_buf = createPrintfBuffer(acc, PRINTF_BUFFER_SIZE);

  // Testing 16 threads with exact buffer size
  // Each printf here 2 args + 1 counter = 3
  // 3 args * 16 = 48 + 6 overhead = 54
  parallel_for_each(extent<1>(GLOBAL).tile(TILE), [=](tiled_index<1> tidx) [[hc]] {
      const char* str1 = "Thread %03d\n";
      printf(printf_buf, str1, tidx.global[0]);
  }).wait();

  processPrintfBuffer(printf_buf);

  deletePrintfBuffer(printf_buf);

  return 0;
}

// CHECK-DAG: Thread 000
// CHECK-DAG: Thread 007
// CHECK-DAG: Thread 008
// CHECK-DAG: Thread 015
