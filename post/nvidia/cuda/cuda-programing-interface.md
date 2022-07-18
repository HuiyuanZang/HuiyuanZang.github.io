# Cuda Programming Interface

## Table of Contents

## About
This article is only used for a quick recap of what [1][NVIDIA CUDA Progrmmming Interface](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#programming-interface) describes. And it highlights CUDA knowledge bulletin for memorizing.

## Compilation

### Compilation with NVCC
#### Offline Compilation with NVCC 

All details about NVCC is looking at [2][NVIDIA CUDA Compiler Driver NVCC](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html)

All details about ptx is looking at [3][Parallel Thread Execution ISA](https://docs.nvidia.com/cuda/parallel-thread-execution/index.html)

| NVCC's Input  | NVCC's Output |
|:---:|:---:|
| *.cpp *.c *.cu *.ptx  | *.ptx *.cubin *.o   |

During linking stage, the application can then
- Either link to the compiled host code (this is the most common case),
- Or ignore the modified host code (if any) and use the CUDA driver API to load and execute the PTX code or cubin object. 

#### Just-In-Time Compilation  

NVRTC can be used to compile CUDA C++ device code to PTX at runtime. NVRTC is a runtime compilation library for CUDA C++; more information can be found in the NVRTC User guide. 

All details about NVRTC is looking at [4][The User guide to NVRTC](https://docs.nvidia.com/cuda/nvrtc/index.html)


#### Cross Compilation with NVCC and gnu cross toolchain




### CUDA RUNTIME 

#### Initialization and Reset
- no explicit init function
- cudaDeviceReset()

#### Device Memory
- cudaMalloc() and  cudaFree()
- cudaMallocPitch() and 
- cudaMalloc3D()

#### Device Memory L2 Access Management
Starting with CUDA 11.0, devices of compute capability 8.0 and above have the capability to influence persistence of data in the L2 cache, potentially providing higher bandwidth and lower latency accesses to global memory. Therefore, this feature isn't avaliable on Jetson Nano.

 



## Reference
- [1][NVIDIA CUDA Progrmmming Interface](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#programming-interface)
- [2][NVIDIA CUDA Compiler Driver NVCC](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html)
- [3][Parallel Thread Execution ISA](https://docs.nvidia.com/cuda/parallel-thread-execution/index.html)
- [4][The User guide to NVRTC](https://docs.nvidia.com/cuda/nvrtc/index.html)


