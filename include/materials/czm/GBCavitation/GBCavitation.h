//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "CZMMaterialBasePK1.h"
#include "GBCavitationBoundaryPropertyUO.h"
#include "ShamNeedlemanEquation.h"
/**
 * Implementation of teh grain boundary cavitation model proposed by Sham
 * and Needlemena 1983 and expanded by Van der Gieseen 1995 **/
class GBCavitation : public CZMMaterialBasePK1 {
public:
  static InputParameters validParams();
  GBCavitation(const InputParameters &parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeTractionIncrementAndDerivatives() override;
  void computeAverageBulkPorperties();
  void initNLSystemParamter(std::vector<std::string> &pname, vecD &pvalue,
                            std::vector<std::string> &rate_pname,
                            vecD &rate_pvalue);

  /// method to kill the traction
  void tractionDecay();

  const bool _use_old_bulk_property;
  /// reference to bulk properties
  ///@{
  const MaterialProperty<RankTwoTensor> &_stress_master;
  const MaterialProperty<RankTwoTensor> &_stress_slave;
  const MaterialProperty<RankTwoTensor> &_inelastic_strain_master;
  const MaterialProperty<RankTwoTensor> &_inelastic_strain_slave;
  const MaterialProperty<RankTwoTensor> &_inelastic_strain_master_old;
  const MaterialProperty<RankTwoTensor> &_inelastic_strain_slave_old;
  ///@}

  /// the computed equivalent stress and strain values on the interface
  ///@{
  MaterialProperty<Real> &_stress_vm;
  const MaterialProperty<Real> &_stress_vm_old;
  MaterialProperty<Real> &_stress_H;
  const MaterialProperty<Real> &_stress_H_old;
  MaterialProperty<Real> &_strain_rate_eq;
  const MaterialProperty<Real> &_strain_rate_eq_old;
  MaterialProperty<Real> &_strain_eq;
  const MaterialProperty<Real> &_strain_eq_old;
  ///@}

  /// sham needleman state variables
  ///@{
  MaterialProperty<Real> &_a;
  const MaterialProperty<Real> &_a_old;
  MaterialProperty<Real> &_b;
  const MaterialProperty<Real> &_b_old;
  MaterialProperty<int> &_nucleation_is_active;
  const MaterialProperty<int> &_nucleation_is_active_old;
  MaterialProperty<Real> &_D;
  MaterialProperty<Real> &_D_rate;
  MaterialProperty<Real> &_VLdot;
  MaterialProperty<Real> &_VL1dot;
  MaterialProperty<Real> &_VL2dot;
  MaterialProperty<Real> &_L;
  ///@}

  /// failure state variables
  ///@{
  MaterialProperty<int> &_element_failed;
  const MaterialProperty<int> &_element_failed_old;
  MaterialProperty<Real> &_time_at_failure;
  const MaterialProperty<Real> &_time_at_failure_old;
  MaterialProperty<RealVectorValue> &_traction_at_failure;
  const MaterialProperty<RealVectorValue> &_traction_at_failure_old;
  MaterialProperty<RealVectorValue> &_jump_at_failure;
  const MaterialProperty<RealVectorValue> &_jump_at_failure_old;
  MaterialProperty<Real> &_residual_life;
  const MaterialProperty<Real> &_residual_life_old;
  ///@}

  /// sham needleman eqauations paramters
  ///@{
  // qp dependent properties
  MaterialProperty<Real> &_n_exponent;
  const MaterialProperty<Real> &_n_exponent_old;
  MaterialProperty<Real> &_beta_exponent;
  const MaterialProperty<Real> &_beta_exponent_old;
  MaterialProperty<Real> &_a0;
  const MaterialProperty<Real> &_a0_old;
  MaterialProperty<Real> &_NI;
  const MaterialProperty<Real> &_NI_old;
  MaterialProperty<Real> &_FN;
  const MaterialProperty<Real> &_FN_old;
  MaterialProperty<Real> &_D_GB;
  const MaterialProperty<Real> &_D_GB_old;
  MaterialProperty<Real> &_eta_sliding;
  const MaterialProperty<Real> &_eta_sliding_old;
  MaterialProperty<Real> &_h;
  const MaterialProperty<Real> &_h_old;
  MaterialProperty<Real> &_b_sat;
  const MaterialProperty<Real> &_b_sat_old;
  MaterialProperty<Real> &_E_GB;
  const MaterialProperty<Real> &_E_GB_old;
  MaterialProperty<Real> &_G_GB;
  const MaterialProperty<Real> &_G_GB_old;
  MaterialProperty<Real> &_thickness;
  const MaterialProperty<Real> &_thickness_old;
  MaterialProperty<Real> &_sigma_0;
  const MaterialProperty<Real> &_sigma_0_old;

  const Real _E_penalty_minus_thickenss;
  const Real _E_penalty_after_failure_minus_thickenss;
  const Real _thickness_after_failure;

  const Real _theta;
  ///@}

  /// switch for activing physics
  ///@{
  const bool _nucleation_on;
  const bool _growth_on;
  const bool _use_triaxial_growth;
  const unsigned int _vdot_method;
  ///@}

  /// failure constants
  ///@{
  const Real _D_failure;
  const Real _minimum_allowed_residual_life;
  const Real _maximum_allowed_opening_traction;
  const Real _minimum_allowed_stiffness;
  ///@}

  /// non linear solver paramters
  ///@{
  const unsigned int _max_time_cut;
  const unsigned int _max_nonlinear_iter;
  const Real _nl_residual_abs_tol;
  const bool _force_substep;
  ///@}

  const GBCavitationBoundaryPropertyUO *_GBCavitationBoundaryPropertyUO;
  /* INTERFACE PARAMETERS */
  // const Real _a0;                  /*initial cavity half radius*/
  // const Real _b0;                  /*initial cavity half spacing*/
  // const Real _b_saturation;        /*saturation cavity half spacing*/
  // const Real _NI;                  /*number of intial cavity*/
  // const Real _FN;                  /*cavitration rate*/
  // const Real _S_thr;               /*thresould value for cavitation to
  // occur*/ const Real _thickness; /*interface Young modulus*/ const
  // Real _E_interface;         /*interface Young modulus*/

  void InitGBCavitationParamsAndProperties();
  void getInitPropertyValuesFromParams(Real &FN_NI, Real &Nmax_NI, Real &a0,
                                       Real &b0, Real &psi, Real &D_gb,
                                       Real &E_GB, Real &G_GB,
                                       Real &eta_sliding, Real &sigma_0,
                                       Real &thickness, Real &beta_exponent,
                                       Real &n_exponent) const;
  void getInitPropertyValuesFromUO(Real &FN_NI, Real &Nmax_NI, Real &a0,
                                   Real &b0, Real &psi, Real &D_gb, Real &E_GB,
                                   Real &G_GB, Real &eta_sliding, Real &sigma_0,
                                   Real &thickness, Real &beta_exponent,
                                   Real &n_exponent) const;
};
