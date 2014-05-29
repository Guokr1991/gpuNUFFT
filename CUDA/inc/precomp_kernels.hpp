#ifndef PRECOMP_KERNELS_H
#define PRECOMP_KERNELS_H

#include "cuda_utils.hpp"
#include "gpuNUFFT_operator.hpp"
#include "precomp_utils.hpp"
#include <vector>

// GPU Kernel for Precomputation
// Sector Assignment 
//
void assignSectorsGPU(gpuNUFFT::GpuNUFFTOperator* gpuNUFFTOp, 
  gpuNUFFT::Array<DType>& kSpaceTraj, 
  IndType* assignedSectors);

void sortArrays(gpuNUFFT::GpuNUFFTOperator* gpuNUFFTOp, 
  std::vector<gpuNUFFT::IndPair> assignedSectorsAndIndicesSorted,
  IndType* assignedSectors, 
  IndType* dataIndices,
  gpuNUFFT::Array<DType>& kSpaceTraj,
  DType* trajSorted,
  DType* densCompData,
  DType* densData);

void selectOrderedGPU(DType2* data_d, 
  IndType* data_indices_d, 
  DType2* data_sorted_d,
  int N);

void writeOrderedGPU(DType2* data_sorted_d, 
  IndType* data_indices_d, 
  CufftType* data_d, 
  int N);

#endif