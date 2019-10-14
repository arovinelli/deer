#pragma once

#include "ADKernel.h"

template<ComputeStage>
class ADStokesIncompressibility;

declareADValidParams(ADStokesIncompressibility);

template <ComputeStage compute_stage>
class ADStokesIncompressibility: public ADKernel<compute_stage>
{
 public:
  ADStokesIncompressibility(const InputParameters & parameters);

 protected:
  virtual ADReal computeQpResidual() override;

  usingKernelMembers;

 protected:
  const ADVectorVariableGradient & _grad_v;
};
