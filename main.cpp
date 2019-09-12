#define __CL_ENABLE_EXCEPTIONS
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "utils.h"
#include <iostream>

using data_type = uint32_t;

void performCalculation(const std::vector<data_type> &a,
                        const std::vector<data_type> &b,
                        std::vector<data_type> &c) {
  std::cout << "-------------------------------------------------" << std::endl
            << "Device: CPU without OpenCL" << std::endl
            << std::endl;

  // Run computation N times and measure average time
  size_t N = 100;
  timer t;
  for (size_t _ = 0; _ < N; _++) {
    for (size_t i = 0; i < a.size(); i++) {
      c[i] = a[i] & b[i];
    }
  }
  float avg = t.stop<timer::ms>() / (float)N;
  std::cout << "Time for executing: " << avg << " ms" << std::endl;
  // bandwidth = ( size(a) + size(b) + size(c) ) / time
  std::cout << "Bandwidth: "
            << calc_bandwidth_gb_s(
                   (a.size() + b.size() + c.size()) * sizeof(data_type), avg)
            << " GB/s" << std::endl;
}

void performOpenCLCalculation(const cl::Device &device,
                              const std::vector<data_type> &a,
                              const std::vector<data_type> &b,
                              std::vector<data_type> &c) {
  std::cout << "-------------------------------------------------" << std::endl
            << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl
            << std::endl;

  cl::Context context(device);
  cl::Program program(context, loadProgram("kernel.cl"));

  cl::CommandQueue queue(context, device);
  // "-cl-std=CL2.0" for
  // https://software.intel.com/ru-ru/articles/opencl-20-non-uniform-work-groups
  program.build({device}, "-cl-std=CL2.0");

  // Prepare buffers
  // Copy content of `a` to `inputA`
  cl::Buffer inputA = cl::Buffer(
      context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      a.size() * sizeof(data_type), const_cast<data_type *>(a.data()));
  // Copy content of `b` to `inputB`
  cl::Buffer inputB = cl::Buffer(
      context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      b.size() * sizeof(data_type), const_cast<data_type *>(b.data()));
  // Allocate buffer for result
  cl::Buffer outputC =
      cl::Buffer(context, CL_MEM_READ_WRITE, c.size() * sizeof(data_type));

  // Set kernel args
  cl::Kernel kernelBFS(program, "bitwise_and");
  {
    int iArg = 0;
    kernelBFS.setArg(iArg++, inputA);
    kernelBFS.setArg(iArg++, inputB);
    kernelBFS.setArg(iArg, outputC);
  }

  // Run computation N times and measure average time
  {
    timer t;
    size_t N = 100;
    for (size_t i = 0; i < N; i++) {
      queue.enqueueNDRangeKernel(kernelBFS, cl::NullRange, a.size());
      queue.finish();
    }
    float avg = t.stop<timer::ms>() / (float)N;
    std::cout << "Time for executing: " << avg << " ms" << std::endl;
    // bandwidth = ( size(a) + size(b) + size(c) ) / time
    std::cout << "Bandwidth: "
              << calc_bandwidth_gb_s(
                     (a.size() + b.size() + c.size()) * sizeof(data_type), avg)
              << " GB/s" << std::endl;
  }

  // Copy result from `outputC` to `c`
  queue.enqueueReadBuffer(outputC, CL_TRUE, 0, c.size() * sizeof(uint32_t),
                          c.data());
}

int main() {
  try {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
      std::cerr << "OpenCL platform not found" << std::endl;
      return -1;
    }

    std::vector<cl::Device> devices;
    for (const auto &platform : platforms) {
      std::vector<cl::Device> devs;
      platform.getDevices(CL_DEVICE_TYPE_ALL, &devs);
      devices.insert(devices.begin(), devs.begin(), devs.end());
    }

    // Set problem size
    const size_t size = 1 << 28;
    std::cout << "Problem size: " << size * sizeof(data_type) * 8 << " bits"
              << std::endl;
    auto [a, b, c] = make_data<data_type>(size);

    for (const auto &device : devices) {
      performOpenCLCalculation(device, a, b, c);
    }

    performCalculation(a, b, c);

  } catch (const cl::Error &err) {
    std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")"
              << std::endl;
  }

  return 0;
}