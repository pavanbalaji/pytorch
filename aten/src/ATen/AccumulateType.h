#pragma once
#include <ATen/Config.h>
#include <c10/core/DeviceType.h>
#include <c10/core/ScalarType.h>
#include <c10/util/BFloat16.h>
#include <c10/util/Float8_e4m3fn.h>
#include <c10/util/Float8_e5m2.h>
#include <c10/util/Half.h>

// Defines the accumulation type for a scalar type.
// Example:
//   using accscalar_t = acc_type<scalar_t, /*is_cuda*/true>;
//
// Accumulation types are an important concept in numeric computing
// because you frequently want to perform intermediate computations
// at a higher precision than the input and output precision, to avoid
// compounding internal rounding errors.  Accumulation is the most
// well-known intermediate computation (it is of great importance for
// sum reduction and matrix multiply, for example), but in PyTorch
// acc_type ends up getting used for all sorts of other intermediate
// computations, so it perhaps would be more accurately (ahem) called an
// "accurate" type.  acc_type is especially important for reduced
// precision operations like float16 and bfloat16, where relatively
// benign looking inputs can easily end up overflowing/underflowing.
//
// acc_type is parametrized by whether or not you are running on CUDA
// or not, because on CUDA double precision operations are expensive
// and so by default, we don't actually want to use double as an
// acc_type on CUDA.  A lot of things are typed out below, but
// basically, the table is generated by a few rules:
//
//  If bool:
//      Use 'bool' as acc_type.
//  If floating point:
//      If CUDA, use 'float' as acc_type (unless scalar_t is double),
//      otherwise (CPU) use 'double'
//  If integral:
//      Use 'int64_t' as acc_type
//
// You're not forced to use this template; if you happen to know
// something specific about your use case, you can specify your own
// desired behavior.  This template, however, will give you a reasonable
// default that will work for all dtypes supported in PyTorch.

#if defined(__CUDACC__)
#include <cuda.h>
#include <cuda_fp16.h>
#elif defined(__HIPCC__)
#include <hip/hip_fp16.h>
#include <hip/hip_runtime.h>
#endif

namespace at {

template <typename T, c10::DeviceType D>
struct AccumulateTypeDevice {};

template <typename T, bool>
struct AccumulateType {};

template <typename T>
struct AccumulateType<T, false> {
  using type = typename AccumulateTypeDevice<T, c10::DeviceType::CPU>::type;
};

template <typename T>
struct AccumulateType<T, true> {
  using type = typename AccumulateTypeDevice<T, c10::DeviceType::CUDA>::type;
};

template <typename T, c10::DeviceType device>
using acc_type_device = typename AccumulateTypeDevice<T, device>::type;

template <typename T, bool is_cuda>
using acc_type = typename AccumulateType<T, is_cuda>::type;

#define ACC_TYPE(t, acc_t, device_type)         \
  template <>                                   \
  struct AccumulateTypeDevice<t, device_type> { \
    using type = acc_t;                         \
  };
#define MPS_ACC_TYPE(t, acc_t) ACC_TYPE(t, acc_t, c10::DeviceType::MPS)
#define CUDA_ACC_TYPE(t, acc_t) ACC_TYPE(t, acc_t, c10::DeviceType::CUDA)
#define CPU_ACC_TYPE(t, acc_t) ACC_TYPE(t, acc_t, c10::DeviceType::CPU)

MPS_ACC_TYPE(BFloat16, float);
MPS_ACC_TYPE(Half, float);
MPS_ACC_TYPE(Float8_e5m2, float);
MPS_ACC_TYPE(Float8_e5m2fnuz, float);
MPS_ACC_TYPE(Float8_e4m3fn, float);
MPS_ACC_TYPE(Float8_e4m3fnuz, float);
MPS_ACC_TYPE(float, float);
MPS_ACC_TYPE(double, float);
MPS_ACC_TYPE(int8_t, int64_t);
MPS_ACC_TYPE(uint8_t, int64_t);
MPS_ACC_TYPE(char, int64_t);
MPS_ACC_TYPE(int16_t, int64_t);
MPS_ACC_TYPE(int32_t, int64_t);
MPS_ACC_TYPE(int64_t, int64_t);
MPS_ACC_TYPE(bool, bool);
MPS_ACC_TYPE(c10::complex<Half>, c10::complex<float>);
MPS_ACC_TYPE(c10::complex<float>, c10::complex<float>);
MPS_ACC_TYPE(c10::complex<double>, c10::complex<float>);

#if defined(__CUDACC__) || defined(__HIPCC__)
CUDA_ACC_TYPE(half, float);
#endif
CUDA_ACC_TYPE(BFloat16, float);
CUDA_ACC_TYPE(Half, float);
CUDA_ACC_TYPE(Float8_e5m2, float);
CUDA_ACC_TYPE(Float8_e5m2fnuz, float);
CUDA_ACC_TYPE(Float8_e4m3fn, float);
CUDA_ACC_TYPE(Float8_e4m3fnuz, float);
CUDA_ACC_TYPE(float, float);
CUDA_ACC_TYPE(double, double);
CUDA_ACC_TYPE(int8_t, int64_t);
CUDA_ACC_TYPE(uint8_t, int64_t);
CUDA_ACC_TYPE(char, int64_t);
CUDA_ACC_TYPE(int16_t, int64_t);
CUDA_ACC_TYPE(int32_t, int64_t);
CUDA_ACC_TYPE(int64_t, int64_t);
CUDA_ACC_TYPE(bool, bool);
CUDA_ACC_TYPE(c10::complex<Half>, c10::complex<float>);
CUDA_ACC_TYPE(c10::complex<float>, c10::complex<float>);
CUDA_ACC_TYPE(c10::complex<double>, c10::complex<double>);

CPU_ACC_TYPE(BFloat16, float);
CPU_ACC_TYPE(Half, float);
CPU_ACC_TYPE(Float8_e5m2, float);
CPU_ACC_TYPE(Float8_e5m2fnuz, float);
CPU_ACC_TYPE(Float8_e4m3fn, float);
CPU_ACC_TYPE(Float8_e4m3fnuz, float);
CPU_ACC_TYPE(float, double);
CPU_ACC_TYPE(double, double);
CPU_ACC_TYPE(int8_t, int64_t);
CPU_ACC_TYPE(uint8_t, int64_t);
CPU_ACC_TYPE(char, int64_t);
CPU_ACC_TYPE(int16_t, int64_t);
CPU_ACC_TYPE(int32_t, int64_t);
CPU_ACC_TYPE(int64_t, int64_t);
CPU_ACC_TYPE(bool, bool);
CPU_ACC_TYPE(c10::complex<Half>, c10::complex<float>);
CPU_ACC_TYPE(c10::complex<float>, c10::complex<double>);
CPU_ACC_TYPE(c10::complex<double>, c10::complex<double>);

TORCH_API c10::ScalarType toAccumulateType(
    c10::ScalarType type,
    c10::DeviceType device);
TORCH_API c10::ScalarType toAccumulateType(c10::ScalarType type, bool is_cuda);

} // namespace at
