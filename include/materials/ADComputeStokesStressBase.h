#pragma once

#include "ADMaterial.h"

template <ComputeStage>
class ADComputeStokesStressBase;

template <typename>
class RankTwoTensorTempl;
typedef RankTwoTensorTempl<Real> RankTwoTensor;
typedef RankTwoTensorTempl<DualReal> DualRankTwoTensor;

declareADValidParams(ADComputeStokesStressBase);

template <ComputeStage compute_stage>
class ADComputeStokesStressBase : public ADMaterial<compute_stage>
{
 public:
  ADComputeStokesStressBase(const InputParameters & parameters);

 protected:
  virtual void computeQpProperties() override;
  virtual ADRankTwoTensor computeStress(const ADRankTwoTensor & strain, unsigned int qp) = 0;

 protected:
  const ADVectorVariableGradient & _grad_v;
  ADMaterialProperty(RankTwoTensor) & _stress;

  usingMaterialMembers;
};
