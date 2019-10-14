#include "ADComputeStokesStressLinear.h"

registerADMooseObject("DeerApp", ADComputeStokesStressLinear);

defineADValidParams(ADComputeStokesStressLinear,
                    ADComputeStokesStressBase,
                    params.addRequiredParam<MaterialPropertyName>("viscosity", "Name of the material defining the viscosity");
                    params.addClassDescription("Linear viscosity stress update"););

template <ComputeStage compute_stage>
ADComputeStokesStressLinear<compute_stage>::ADComputeStokesStressLinear(const InputParameters & parameters)
  : ADComputeStokesStressBase<compute_stage>(parameters),
    _viscosity(getADMaterialProperty<Real>("viscosity"))
{

}

template <ComputeStage compute_stage>
ADRankTwoTensor
ADComputeStokesStressLinear<compute_stage>::computeStress(const ADRankTwoTensor & strain, unsigned int qp)
{
  return _viscosity[qp] * strain;
}
