#include "ADStokesStressDivergence.h"

registerADMooseObject("DeerApp", ADStokesStressDivergence);

defineADValidParams(
    ADStokesStressDivergence,
    ADVectorKernel,
    params.addRequiredCoupledVar("p", "Pressure variable"));

template <ComputeStage compute_stage>
ADStokesStressDivergence<compute_stage>::ADStokesStressDivergence(const InputParameters & parameters)
  : ADVectorKernel<compute_stage>(parameters),
    _p(adCoupledValue("p")),
    _stress(getADMaterialPropertyByName<RankTwoTensor>("stress"))
{

}

template <ComputeStage compute_stage>
ADReal
ADStokesStressDivergence<compute_stage>::computeQpResidual()
{
  auto div_phi = _grad_test[_i][_qp].tr();
  return _p[_qp] * div_phi - _stress[_qp].contract(_grad_test[_i][_qp]);
}

template class ADStokesStressDivergence<RESIDUAL>;
template class ADStokesStressDivergence<JACOBIAN>;
