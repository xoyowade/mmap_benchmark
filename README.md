# MMap tuning on tmpfs
Benchmark for different mmap prefault/prefetch methods, which writes a 256MB mmaped buffer on tmpfs, with 128B blocks. Explaination could be found on my Chinese [blog post](http://xoyo.space/2017/11/mmap-performance-analyzing-and-tuning/).

# Run
```
make && ./runbm.sh
```

# Result
## Environment
```
$ sudo lscpu
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                48
On-line CPU(s) list:   0-47
Thread(s) per core:    2
Core(s) per socket:    12
Socket(s):             2
NUMA node(s):          2
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 85
Model name:            Intel(R) Xeon(R) Gold 5118 CPU @ 2.30GHz
Stepping:              4
CPU MHz:               2294.717
BogoMIPS:              4590.54
Virtualization:        VT-x
L1d cache:             32K
L1i cache:             32K
L2 cache:              1024K
L3 cache:              16896K
NUMA node0 CPU(s):     0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46
NUMA node1 CPU(s):     1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41,43,45,47
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc aperfmperf eagerfpu pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch epb intel_pt tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 hle avx2 smep bmi2 erms invpcid rtm cqm mpx avx512f rdseed adx smap clflushopt clwb avx512cd xsaveopt xsavec xgetbv1 cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local dtherm ida arat pln pts

$ uname -a
Linux hftdev 4.4.0-87-generic #110-Ubuntu SMP Tue Jul 18 12:55:35 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux
```
## Data
Unit: millisecond

same_node: backed tmpfs on the same processor node as running process

diff_node: backed tmpfs on the different processor node with running process
```
+-------------------------------+-----------+-----------+
|             name              | same_node | diff_node |
+-------------------------------+-----------+-----------+
| BM_MMap11ManualPrefault       |    48.127 |    63.045 |
| BM_MMap10ManualPrefaultNeed   |    48.294 |    62.709 |
| BM_MMap06PreallocPrefault     |    48.316 |    62.539 |
| BM_MMap03Prefault             |    48.421 |    62.744 |
| BM_MMap07PreallocPrefaultNeed |    48.527 |    62.970 |
| BM_MMap04PrefaultNeed         |    48.543 |    63.133 |
| BM_MMap05PrefaultSeq          |    48.570 |    62.954 |
| BM_MMap02Prealloc             |   106.558 |   152.471 |
| BM_MMap08PrefetchNeed         |   127.054 |   174.662 |
| BM_MMap09PrefetchSeq          |   128.844 |   173.948 |
| BM_MMap01                     |   129.806 |   174.084 |
+-------------------------------+-----------+-----------+
```
Terms in benchmark names:
- ManualPrefault: touch every byte with loop
- Prefault: set mmap MAP_POPULATE flag on
- Prealloc: call fallocate to actually allocate file space in advance
- Need: call madvise to use MADV_WILLNEED strategy to prefetch
- Seq: call madvise to use MADV_SEQUENTIAL strategy to prefetch
