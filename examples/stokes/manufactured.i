[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
[]

[Variables]
  [./p]
    family = L2_LAGRANGE
  [../]
  [./v]
    family = LAGRANGE_VEC
  [../]
[]

[Functions]
  [./bx]
    type = ParsedFunction
    value = 'sin(x)'
  [../]

  [./by]
    type = ParsedFunction
    value = 'sin(y)'
  [../]

[]

[Kernels]
  [./stress]
    type = ADStokesStressDivergence
    variable = v
    p = p
  [../]
  [./pressure]
    type = ADStokesIncompressibility
    variable = p
    v = v
  [../]
  [./bf]
    type = ADStokesBodyForce
    variable = v
    function_x = bx
    function_y = by
  [../]
[]

[Materials]
  [./stresscalc]
    type = ADComputeStokesStressLinear
    viscosity = mu
    v = v
  [../]

  [./properties]
    type = GenericConstantMaterial
    prop_names = 'mu'
    prop_values = '1.0'
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
      
  petsc_options = '-snes_converged_reason'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  line_search = none

[]

[Outputs]
  exodus = true
[]
