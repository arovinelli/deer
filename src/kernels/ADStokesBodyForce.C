#include "ADStokesBodyForce.h"

registerADMooseObject("DeerApp", ADStokesBodyForce);

defineADValidParams(
    ADStokesBodyForce,
    ADVectorKernel,
    params.addParam<FunctionName>("function_x", "0", "x-component of body force");
    params.addParam<FunctionName>("function_y", "0", "y-component of body force");
    params.addParam<FunctionName>("function_z", "0", "z-component of body force");
    );

template <ComputeStage compute_stage>
ADStokesBodyForce<compute_stage>::ADStokesBodyForce(const InputParameters & parameters)
  : ADVectorKernel<compute_stage>(parameters),
    _function_x(getFunction("function_x")),
    _function_y(getFunction("function_y")),
    _function_z(getFunction("function_z"))
{
}

template <ComputeStage compute_stage>
ADReal
ADStokesBodyForce<compute_stage>::computeQpResidual()
{
  auto val =  -_test[_i][_qp] * ADRealVectorValue(_function_x.value(_t, _q_point[_qp]),
                                               _function_y.value(_t, _q_point[_qp]),
                                               _function_z.value(_t, _q_point[_qp]));
  return val;
}

template class ADStokesBodyForce<RESIDUAL>;
template class ADStokesBodyForce<JACOBIAN>;
