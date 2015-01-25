# Crc32: slice-by-16 is the fastest algorithm
```

C:\Testing\Crc32>Crc32_vc2013_x64.exe
Please wait ...
 88 bytes at once: CRC=00EE2DAA, 0.541s, 1893.865 MB/s
  1 byte  at once: CRC=00EE2DAA, 2.038s, 502.436 MB/s
+  4 bytes at once: CRC=17560371, 0.884s, 1158.863 MB/s
+2*4 bytes at once: CRC=17560371, 0.880s, 1164.178 MB/s
+4*4 bytes at once: CRC=17560371, 0.937s, 1092.800 MB/s
+  8 bytes at once: CRC=17560371, 0.526s, 1948.366 MB/s
+2*8 bytes at once: CRC=17560371, 0.492s, 2082.899 MB/s
+4*8 bytes at once: CRC=17560371, 0.454s, 2253.126 MB/s
  4 bytes at once: CRC=00EE2DAA, 0.881s, 1161.926 MB/s
2*4 bytes at once: CRC=00EE2DAA, 0.845s, 1212.291 MB/s
4*4 bytes at once: CRC=00EE2DAA, 0.870s, 1177.049 MB/s
  8 bytes at once: CRC=00EE2DAA, 0.590s, 1736.712 MB/s
 16 bytes at once: CRC=00EE2DAA, 0.419s, 2441.144 MB/s
2*8 bytes at once: CRC=00EE2DAA, 0.600s, 1706.746 MB/s
4*8 bytes at once: CRC=00EE2DAA, 0.583s, 1756.029 MB/s
chunked        : CRC=00EE2DAA, 0.583s, 1757.766 MB/s

C:\Testing\Crc32>Crc32_vc2013_x86.exe
Please wait ...
 88 bytes at once: CRC=00EE2DAA, 0.560s, 1829.252 MB/s
  1 byte  at once: CRC=00EE2DAA, 2.020s, 506.849 MB/s
+  4 bytes at once: CRC=17560371, 1.006s, 1017.667 MB/s
+2*4 bytes at once: CRC=17560371, 1.068s, 959.049 MB/s
+4*4 bytes at once: CRC=17560371, 1.108s, 924.201 MB/s
+  8 bytes at once: CRC=17560371, 0.519s, 1974.139 MB/s
+2*8 bytes at once: CRC=17560371, 0.494s, 2072.375 MB/s
+4*8 bytes at once: CRC=17560371, 0.568s, 1802.472 MB/s
  4 bytes at once: CRC=00EE2DAA, 0.944s, 1085.165 MB/s
2*4 bytes at once: CRC=00EE2DAA, 0.905s, 1131.755 MB/s
4*4 bytes at once: CRC=00EE2DAA, 0.903s, 1134.177 MB/s
  8 bytes at once: CRC=00EE2DAA, 0.512s, 2001.955 MB/s
 16 bytes at once: CRC=00EE2DAA, 0.440s, 2326.377 MB/s
2*8 bytes at once: CRC=00EE2DAA, 0.604s, 1694.467 MB/s
4*8 bytes at once: CRC=00EE2DAA, 0.604s, 1695.022 MB/s
chunked        : CRC=00EE2DAA, 0.507s, 2020.132 MB/s

C:\Testing\Crc32>Crc32_gcc492_x64.exe
Please wait ...
 88 bytes at once: CRC=00EE2DAA, 0.409s, 2503.667 MB/s
  1 byte  at once: CRC=00EE2DAA, 2.309s, 443.482 MB/s
+  4 bytes at once: CRC=17560371, 0.858s, 1193.473 MB/s
+2*4 bytes at once: CRC=17560371, 0.839s, 1220.501 MB/s
+4*4 bytes at once: CRC=17560371, 0.805s, 1272.050 MB/s
+  8 bytes at once: CRC=17560371, 0.533s, 1921.201 MB/s
+2*8 bytes at once: CRC=17560371, 0.543s, 1885.820 MB/s
+4*8 bytes at once: CRC=17560371, 0.433s, 2364.896 MB/s
  4 bytes at once: CRC=00EE2DAA, 0.834s, 1227.818 MB/s
2*4 bytes at once: CRC=00EE2DAA, 0.812s, 1261.084 MB/s
4*4 bytes at once: CRC=00EE2DAA, 0.799s, 1281.602 MB/s
  8 bytes at once: CRC=00EE2DAA, 0.541s, 1892.791 MB/s
 16 bytes at once: CRC=00EE2DAA, 0.303s, 3379.538 MB/s
2*8 bytes at once: CRC=00EE2DAA, 0.407s, 2515.971 MB/s
4*8 bytes at once: CRC=00EE2DAA, 0.394s, 2598.985 MB/s
chunked        : CRC=00EE2DAA, 0.531s, 1928.437 MB/s

C:\Testing\Crc32>Crc32_gcc492_x86.exe
Please wait ...
 88 bytes at once: CRC=00EE2DAA, 0.411s, 2491.484 MB/s
  1 byte  at once: CRC=00EE2DAA, 2.304s, 444.444 MB/s
+  4 bytes at once: CRC=17560371, 0.855s, 1197.661 MB/s
+2*4 bytes at once: CRC=17560371, 0.850s, 1204.706 MB/s
+4*4 bytes at once: CRC=17560371, 0.818s, 1251.834 MB/s
+  8 bytes at once: CRC=17560371, 0.497s, 2060.362 MB/s
+2*8 bytes at once: CRC=17560371, 0.437s, 2343.249 MB/s
+4*8 bytes at once: CRC=17560371, 0.414s, 2473.430 MB/s
  4 bytes at once: CRC=00EE2DAA, 0.870s, 1177.011 MB/s
2*4 bytes at once: CRC=00EE2DAA, 0.830s, 1233.735 MB/s
4*4 bytes at once: CRC=00EE2DAA, 0.788s, 1299.492 MB/s
  8 bytes at once: CRC=00EE2DAA, 0.560s, 1828.571 MB/s
 16 bytes at once: CRC=00EE2DAA, 0.306s, 3346.405 MB/s
2*8 bytes at once: CRC=00EE2DAA, 0.399s, 2566.416 MB/s
4*8 bytes at once: CRC=00EE2DAA, 0.396s, 2585.859 MB/s
chunked        : CRC=00EE2DAA, 0.556s, 1841.727 MB/s
```

