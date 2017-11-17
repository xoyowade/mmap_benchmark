// Author: Zhiwei Xiao (zwxiao@gmail.com)
//
// Simple benchmark for mmap
// Requirement: Google benchmark https://github.com/google/benchmark

#include <fstream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "benchmark/benchmark.h"

struct MMapInfo {
    int fd;
    void* buf;
};

//const static size_t kLength = 4UL * 1024 * 1024 * 1024;
const static size_t kLength = 256UL * 1024 * 1024;
const static size_t kPageSize = sysconf(_SC_PAGESIZE);
static char data[128];

MMapInfo g_mmap;

int CreateFile(const std::string& file, size_t length) {
    // open
    int fd = open(file.c_str(), 
        O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(-1 != fd); 
  
    // extend
    assert(-1 != ftruncate(fd, length));

    return fd;
}

void WriteBuf(void* b) {
    char* buf = (char*)b;
    for (size_t i = 0; i < kLength/sizeof(data); i++) {
        memcpy(buf, data, sizeof(data));
        buf += sizeof(data);
    }
}

void TouchBuf(void* b, size_t len) {
    char* buf = (char*)b;
    for (size_t i = len/kPageSize; i >= 1; i--) {
      // It's enough to touch only one byte in a page to trigger page fault.
      // Please avoid using a local or global unused variable for destination, 
      // to prevent compiler from optimizing these codes as nop
      memcpy(data, buf, 1);
      buf += kPageSize;
    }
}

static void BM_MMap01(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap", kLength);
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED, g_mmap.fd, 0);

    // timing begins here
    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap02Prealloc(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prealloc", kLength);
    assert(-1 != fallocate(g_mmap.fd, 0, 0, kLength));
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED, g_mmap.fd, 0);

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap03Prefault(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prefault", kLength);
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_POPULATE, g_mmap.fd, 0);

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap04PrefaultNeed(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prefault_need", kLength);
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_POPULATE, g_mmap.fd, 0);
    assert(-1 != madvise(g_mmap.buf, kLength, MADV_WILLNEED));

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap05PrefaultSeq(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prefault_seq", kLength);
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_POPULATE, g_mmap.fd, 0);
    assert(-1 != madvise(g_mmap.buf, kLength, MADV_SEQUENTIAL));

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap06PreallocPrefault(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prealloc_prefault", kLength);
    assert(-1 != fallocate(g_mmap.fd, 0, 0, kLength));
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_POPULATE, g_mmap.fd, 0);

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap07PreallocPrefaultNeed(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prealloc_prefault_need", kLength);
    assert(-1 != fallocate(g_mmap.fd, 0, 0, kLength));
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_POPULATE, g_mmap.fd, 0);
    assert(-1 != madvise(g_mmap.buf, kLength, MADV_WILLNEED));

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap08PrefetchNeed(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prefetch_need", kLength);
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED, g_mmap.fd, 0);
    assert(-1 != madvise(g_mmap.buf, kLength, MADV_WILLNEED));

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap09PrefetchSeq(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_prefetch_seq", kLength);
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED, g_mmap.fd, 0);
    assert(-1 != madvise(g_mmap.buf, kLength, MADV_SEQUENTIAL));

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap10ManualPrefaultNeed(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_manual_prefault_need", kLength);
    assert(-1 != fallocate(g_mmap.fd, 0, 0, kLength));
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED, g_mmap.fd, 0);
    assert(-1 != madvise(g_mmap.buf, kLength, MADV_WILLNEED));
    TouchBuf(g_mmap.buf, kLength);   

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

static void BM_MMap11ManualPrefault(benchmark::State& state) {
    g_mmap.fd = CreateFile("data/mmap_manual_prefault", kLength);
    assert(-1 != fallocate(g_mmap.fd, 0, 0, kLength));
    g_mmap.buf = mmap(NULL, kLength, PROT_WRITE | PROT_READ, MAP_SHARED, g_mmap.fd, 0);
    TouchBuf(g_mmap.buf, kLength);   

    while (state.KeepRunning()) {
        WriteBuf(g_mmap.buf);
    }
}

BENCHMARK(BM_MMap01)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap02Prealloc)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap03Prefault)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap04PrefaultNeed)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap05PrefaultSeq)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap06PreallocPrefault)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap07PreallocPrefaultNeed)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap08PrefetchNeed)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap09PrefetchSeq)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap10ManualPrefaultNeed)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MMap11ManualPrefault)->Unit(benchmark::kMillisecond);

int main(int argc, char** argv) {
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  return 0;
}

