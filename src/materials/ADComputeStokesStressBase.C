#include "ADComputeStokesStressBase.h"
#include "RankTwoTensor.h"

defineADValidParams(
    ADComputeStokesStressBase,
    ADMaterial,
    params.addRequiredCoupledVar("v", "Velocity vector variable"));

template <ComputeStage compute_stage>
ADComputeStokesStressBase<compute_stage>::ADComputeStokesStressBase(const InputParameters & parameters)
  : ADMaterial<compute_stage>(parameters),
    _grad_v(adCoupledVectorGradient("v")),
    _stress(declareADProperty<RankTwoTensor>("stress"))
{

}

template <ComputeStage compute_stage>
void
ADComputeStokesStressBase<compute_stage>::computeQpProperties()
{
  ADRankTwoTensor strain = (_grad_v[_qp] + _grad_v[_qp].transpose()) / 2.0;

  _stress[_qp] = computeStress(strain, _qp);
}

adBaseClass(ADComputeStokesStressBase);
