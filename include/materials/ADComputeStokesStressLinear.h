#pragma once

#include "ADComputeStokesStressBase.h"

template <ComputeStage>
class ADComputeStokesStressLinear;

declareADValidParams(ADComputeStokesStressLinear);

template <ComputeStage compute_stage>
class ADComputeStokesStressLinear: public ADComputeStokesStressBase<compute_stage>
{
 public:
  ADComputeStokesStressLinear(const InputParameters & parameters);

 protected:
  virtual ADRankTwoTensor computeStress(const ADRankTwoTensor & strain, unsigned int qp) override;

 protected:
  const ADMaterialProperty(Real) & _viscosity;
};
