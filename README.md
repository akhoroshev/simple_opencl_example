### How to run

- install OpenCL 
- clone repo
- `mkdir build && cd build && cmake .. && make && ./bitwise_matrix_and`

### Results

On my laptop without dedicated graphics card:
```
Problem size: 8589934592 bits
-------------------------------------------------
Device: Intel(R) Core(TM) i5-7200U CPU @ 2.50GHz (OpenCL CPU)

Time for executing: 204.28 ms
Bandwidth: 14.6857 GB/s
-------------------------------------------------
Device: Intel(R) HD Graphics Kabylake ULT GT2    (OpenCL GPU)

Time for executing: 196.71 ms
Bandwidth: 15.2509 GB/s
-------------------------------------------------
Device: CPU without OpenCL

Time for executing: 215.69 ms
Bandwidth: 13.9089 GB/s
```