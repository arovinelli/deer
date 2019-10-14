#pragma once

#include "ADVectorKernel.h"

template <ComputeStage>
class ADStokesStressDivergence;

declareADValidParams(ADStokesStressDivergence);

template <ComputeStage compute_stage>
class ADStokesStressDivergence: public ADVectorKernel<compute_stage>
{
 public:
  ADStokesStressDivergence(const InputParameters & parameters);

 protected:
  virtual ADReal computeQpResidual() override;

  usingVectorKernelMembers;

 protected:
  const ADVariableValue & _p;
  const ADMaterialProperty(RankTwoTensor) & _stress;
};
