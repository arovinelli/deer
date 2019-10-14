#include "ADStokesIncompressibility.h"

registerADMooseObject("DeerApp", ADStokesIncompressibility);

defineADValidParams(
    ADStokesIncompressibility,
    ADKernel,
    params.addRequiredCoupledVar("v", "Velocity vector variable"));

template <ComputeStage compute_stage>
ADStokesIncompressibility<compute_stage>::ADStokesIncompressibility(const InputParameters & parameters)
  : ADKernel<compute_stage>(parameters),
    _grad_v(adCoupledVectorGradient("v"))
{

}

template <ComputeStage compute_stage>
ADReal
ADStokesIncompressibility<compute_stage>::computeQpResidual()
{
  auto val = _grad_v[_qp].tr() * _test[_i][_qp];

  return val;
}

template class ADStokesIncompressibility<RESIDUAL>;
template class ADStokesIncompressibility<JACOBIAN>;
