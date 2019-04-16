# Simple 2D plane strain test

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Variables]
      [./disp_x]
      [../]
      [./disp_y]
      [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 4
  ny = 4
[]

[Kernels]
  [./sdx]
      type = StressDivergenceNEML
      variable = disp_x
      component = 0
      use_displaced_mesh = true
  [../]
  [./sdy]
      type = StressDivergenceNEML
      variable = disp_y
      component = 1
      use_displaced_mesh = true
  [../]
[]

[Functions]
  [./pullx]
    type = ParsedFunction
    value ='0.5 * t'
  [../]
  [./pully]
    type = ParsedFunction
    value ='-0.3 * t'
  [../]
[]

[BCs]
  [./leftx]
    type = PresetBC
    boundary = left
    variable = disp_x
    value = 0.0
  [../]
  [./lefty]
    type = PresetBC
    boundary = left
    variable = disp_y
    value = 0.0
  [../]
  [./pull_x]
    type = FunctionPresetBC
    boundary = right
    variable = disp_x
    function = pullx
  [../]
  [./pull_y]
    type = FunctionPresetBC
    boundary = top
    variable = disp_y
    function = pully
  [../]
[]

[Materials]
  [./strain]
    type = ComputeNEMLStrain
    use_displaced_mesh = true
  [../]
  [./stress]
    type = ComputeNEMLStressUpdate
    database = "test.xml"
    model = "elastic_model"
    use_displaced_mesh = true
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'newton'
  line_search = none

  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  l_max_its = 2
  l_tol = 1e-14
  nl_max_its = 15
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-12

  start_time = 0.0
  dt = 0.2
  dtmin = 0.2
  end_time = 1.0
[]

[Postprocessors]
  [./nonlin]
    type = NumNonlinearIterations
  [../]
[]

[Outputs]
  exodus = false
  csv = true
[]