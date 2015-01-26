Slice-by-16*2 and gcc 4.9.2 x64 allows to reach speed of 1 byte/cycle. A few most interestong results:
```
C:\Testing\Crc32>Crc32_vc2013_x64.exe
  8 bytes at once: CRC=00EE2DAA, 0.590s, 1736.712 MB/s
 16 bytes at once: CRC=00EE2DAA, 0.419s, 2441.144 MB/s

C:\Testing\Crc32>Crc32_gcc492_x64.exe
 8 bytes at once: CRC=00EE2DAA, 0.541s, 1892.791 MB/s
16 bytes at once: CRC=00EE2DAA, 0.303s, 3379.538 MB/s
2*16 bytes at once: CRC=00EE2DAA, 0.273s, 3750.916 MB/s
```
See full tests in the benchmark.txt
