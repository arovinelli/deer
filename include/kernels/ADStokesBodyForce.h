#pragma once

#include "ADVectorKernel.h"
#include "Function.h"

template <ComputeStage>
class ADStokesBodyForce;

declareADValidParams(ADStokesBodyForce);

template <ComputeStage compute_stage>
class ADStokesBodyForce: public ADVectorKernel<compute_stage>
{
  public:
   ADStokesBodyForce(const InputParameters & parameters);

  protected:
   virtual ADReal computeQpResidual() override;

   usingVectorKernelMembers;
   using KernelBase::_q_point;

  protected:
   const Function & _function_x;
   const Function & _function_y;
   const Function & _function_z;

};
