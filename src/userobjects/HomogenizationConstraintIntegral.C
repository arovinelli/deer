//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "HomogenizationConstraintIntegral.h"

#include "Function.h"

registerMooseObject("DeerApp", HomogenizationConstraintIntegral);

InputParameters
HomogenizationConstraintIntegral::validParams()
{
  InputParameters params = ElementUserObject::validParams();
  params.addRequiredCoupledVar("displacements", "The problem displacements");
  params.addRequiredParam<std::vector<std::string>>("constraint_types",
    "Type of each constraint: strain or stress"); 
  params.addRequiredParam<std::vector<FunctionName>>("targets",
                                        "Functions giving the targets to hit");
  params.addParam<bool>("large_kinematics", false,
                        "Using large displacements?");

  return params;
}

HomogenizationConstraintIntegral::HomogenizationConstraintIntegral(const
                                                                   InputParameters
                                                                   & parameters)
  : ElementUserObject(parameters),
    _ld(getParam<bool>("large_kinematics")),
    _ndisp(coupledComponents("displacements")),
    _ncomps(HomogenizationConstants::required.at(_ld)[_ndisp-1]),
    _stress(getMaterialPropertyByName<RankTwoTensor>("stress")),
    _material_jacobian(
        getMaterialPropertyByName<RankFourTensor>("material_jacobian")),
    _F(getMaterialPropertyByName<RankTwoTensor>("def_grad")),
    _PK1(getMaterialPropertyByName<RankTwoTensor>("PK1")),
    _J(getMaterialPropertyByName<Real>("detJ")),
    _invF(getMaterialPropertyByName<RankTwoTensor>("inv_def_grad")),
    _df(getMaterialPropertyByName<RankTwoTensor>("df")),
    _indices(HomogenizationConstants::indices.at(_ld)[_ndisp-1])
{
  const std::vector<FunctionName> & names =
      getParam<std::vector<FunctionName>>("targets");
  
  unsigned int nfns = names.size();
  if (nfns != _ncomps) {
    mooseError("Homogenization constraint user object needs ",
               _ncomps, " functions");
  }
  for (unsigned int i = 0; i < nfns; i++) {
    const Function * const f = &getFunctionByName(names[i]);
    if (!f) mooseError("Function ", names[i], " not found.");
    _targets.push_back(f);
  }

  const std::vector<std::string> & types = 
      getParam<std::vector<std::string>>("constraint_types");
  if (types.size() != _ncomps) {
    mooseError("Number of constraint types must match the number of "
               "functions");
  }
  for (unsigned int i = 0; i < _ncomps; i++) {
    _ctypes.push_back(HomogenizationConstants::map_string(types[i]));
  }
}

void
HomogenizationConstraintIntegral::initialize()
{
  _residual.zero();
  _jacobian.zero();
}

void
HomogenizationConstraintIntegral::execute()
{
  for (_qp = 0; _qp < _qrule->n_points(); _qp++) {
    Real dV = _JxW[_qp] * _coord[_qp];    
    _residual += computeResidual() * dV;
    _jacobian += computeJacobian() * dV;
  }
}

void
HomogenizationConstraintIntegral::threadJoin(const UserObject & y)
{
  const HomogenizationConstraintIntegral & other = 
      static_cast<const HomogenizationConstraintIntegral &>(y);
  _residual += other._residual;
  _jacobian += other._jacobian;
}

void
HomogenizationConstraintIntegral::finalize()
{
  std::vector<Real> residual(&_residual(0,0), &_residual(0,0) + 9);
  std::vector<Real> jacobian(&_jacobian(0,0,0,0), &_jacobian(0,0,0,0) + 81);

  gatherSum(residual);
  gatherSum(jacobian);

  std::copy(residual.begin(), residual.end(), &_residual(0,0));
  std::copy(jacobian.begin(), jacobian.end(), &_jacobian(0,0,0,0));
}

const RankTwoTensor &
HomogenizationConstraintIntegral::getResidual() const
{
  return _residual;
}

const RankFourTensor &
HomogenizationConstraintIntegral::getJacobian() const
{
  return _jacobian;
}

RankTwoTensor
HomogenizationConstraintIntegral::computeResidual()
{
  RankTwoTensor res;
  res.zero();

  for (_h = 0; _h < _ncomps; _h++) {
    if (_ld) {
      if (_ctypes[_h] == HomogenizationConstants::ConstraintType::Stress) 
        res(_indices[_h].first,_indices[_h].second) = 
            _PK1[_qp](_indices[_h].first,_indices[_h].second) - 
            _targets[_h]->value(_t, _q_point[_qp]);
      else if (_ctypes[_h] == HomogenizationConstants::ConstraintType::Strain) {
        Real f = (_indices[_h].first == _indices[_h].second) ? 1.0 : 0.0;
        res(_indices[_h].first,_indices[_h].second) = 
            _F[_qp](_indices[_h].first,_indices[_h].second) - 
            (f + _targets[_h]->value(_t, _q_point[_qp]));
      }
      else 
        mooseError("Unknown constraint type in the integral!");
    }
    else {
      if (_ctypes[_h] == HomogenizationConstants::ConstraintType::Stress) 
        res(_indices[_h].first,_indices[_h].second) = 
            _stress[_qp](_indices[_h].first,_indices[_h].second) - 
            _targets[_h]->value(_t, _q_point[_qp]);
      else if (_ctypes[_h] == HomogenizationConstants::ConstraintType::Strain) {
        Real f = (_indices[_h].first == _indices[_h].second) ? 1.0 : 0.0;
        res(_indices[_h].first,_indices[_h].second) = 
            0.5*(_F[_qp](_indices[_h].first,_indices[_h].second) +
                 _F[_qp](_indices[_h].second,_indices[_h].first)) -
            (f + _targets[_h]->value(_t, _q_point[_qp]));
      }
      else 
        mooseError("Unknown constraint type in the integral!");
    }
  }
  return res;
}

RankFourTensor
HomogenizationConstraintIntegral::computeJacobian()
{
  RankFourTensor res;
  res.zero();
 
    for (_h = 0; _h < _ncomps; _h++) {
      unsigned int i = _indices[_h].first;
      unsigned int j = _indices[_h].second;
      for (_hh = 0; _hh < _ncomps; _hh++) {
        unsigned int a = _indices[_hh].first;
        unsigned int b = _indices[_hh].second;
        if (_ctypes[_h] == HomogenizationConstants::ConstraintType::Stress) {
          if (_ld) 
            res(i,j,a,b) = ldStressJacobian(i,j,a,b);
          else 
            res(i,j,a,b) = sdStressJacobian(i,j,a,b);
        }
        else if (_ctypes[_h] == HomogenizationConstants::ConstraintType::Strain) {
          if (_ld) 
            res(i,j,a,b) = ldStrainJacobian(i,j,a,b);
          else 
            res(i,j,a,b) = sdStrainJacobian(i,j,a,b);
        }
        else {
          mooseError("Unknown constraint type in Jacobian calculator!");
        }
      }
    }

  return res;
}

Real 
HomogenizationConstraintIntegral::sdStressJacobian(unsigned int i, unsigned int j, 
                                                   unsigned int a, unsigned int b)
{
  return _material_jacobian[_qp](i,j,a,b);
}

Real
HomogenizationConstraintIntegral::sdStrainJacobian(unsigned int i, unsigned int j,
                                                   unsigned int a, unsigned int b)
{
  Real res = 0.0;
  if ((i==a) && (j==b))
    res += 0.5;
  if ((i==b) && (j==a))
    res += 0.5;
  return res;
}

Real 
HomogenizationConstraintIntegral::ldStressJacobian(unsigned int i, unsigned int j, 
                                                   unsigned int a, unsigned int b)
{
  Real res = 0.0;
  for (unsigned int k = 0; k < _ndisp; k++) {
    res += _J[_qp] * _stress[_qp](i,k) * (_invF[_qp](b,a) * _invF[_qp](j,k) - 
                                          _invF[_qp](j,a) * _invF[_qp](b,k));
    for (unsigned int s = 0; s < _ndisp; s++) {
      for (unsigned int t = 0; t < _ndisp; t++) {
        res += _J[_qp] * _material_jacobian[_qp](i,k,s,t) * _df[_qp](s,a) *
            _invF[_qp](b,t) * _invF[_qp](j,k);
      }
    }
  }

  return res;
}

Real 
HomogenizationConstraintIntegral::ldStrainJacobian(unsigned int i, unsigned int j, 
                                                   unsigned int a, unsigned int b)
{
  if ((i==a) && (j==b))
    return 1.0;
  else
    return 0.0;
}
