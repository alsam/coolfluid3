import sys
import coolfluid as cf

# This test compares the performance of Navier-Stokes assembly methods

def make_square(x_segs, y_segs):
  blocks = cf.Core.root().create_component('blocks', 'cf3.mesh.BlockMesh.BlockArrays')
  points = blocks.create_points(dimensions = 2, nb_points = 6)
  points[0]  = [0, 0.]
  points[1]  = [1, 0.]
  points[2]  = [0., 1.]
  points[3]  = [1., 1.]

  blocks.create_blocks(1)[0] = [0, 1, 3, 2]
  blocks.create_block_subdivisions()[0] = [x_segs,y_segs]
  blocks.create_block_gradings()[0] = [1., 1., 1., 1.]

  blocks.create_patch_nb_faces(name = 'left', nb_faces = 1)[0] = [2, 0]
  blocks.create_patch_nb_faces(name = 'right', nb_faces = 1)[0] = [1, 3]
  blocks.create_patch_nb_faces(name = 'top', nb_faces = 1)[0] = [3, 2]
  blocks.create_patch_nb_faces(name = 'bottom', nb_faces = 1)[0] = [0, 1]
  
  return blocks

class TestCase:
  def __init__(self, modelname, segments, use_spec):
    self.model = cf.Core.root().create_component(modelname, 'cf3.solver.ModelUnsteady')
    self.domain = self.model.create_domain()
    self.physics = self.model.create_physics('cf3.UFEM.NavierStokesPhysics')
    self.solver = self.model.create_solver('cf3.UFEM.Solver')
    self.segments = segments
    
    self.physics.options().configure_option('density', 1000.)
    self.physics.options().configure_option('dynamic_viscosity', 10.)
    self.physics.options().configure_option('reference_velocity', 1.)
    
    self.ns_solver = self.solver.add_unsteady_solver('cf3.UFEM.NavierStokes')
    self.ns_solver.options().configure_option('use_specializations', use_spec)
    self.ns_solver.options().configure_option('disabled_actions', ['SolveLSS'])
    self.use_spec = use_spec
    
  def square_mesh_quads(self):
    self.mesh = self.domain.create_component('Mesh', 'cf3.mesh.Mesh')
    make_square(self.segments[0], self.segments[1]).create_mesh(self.mesh.uri())
    self.setup_lss()
    
  def square_mesh_triags(self):
    self.mesh = cf.Core.root().create_component('Mesh', 'cf3.mesh.Mesh')
    make_square(self.segments[0], self.segments[1]).create_mesh(self.mesh.uri())
    triangulator = self.domain.create_component('triangulator', 'cf3.mesh.MeshTriangulator')
    triangulator.options().configure_option('mesh', self.mesh)
    triangulator.execute()
    self.mesh.move_component(self.domain.uri())
    self.mesh.raise_mesh_loaded()
    self.setup_lss()
    
  def cube_mesh_hexas(self):
    self.mesh = self.domain.create_component('Mesh', 'cf3.mesh.Mesh')
    blocks = make_square(self.segments[0], self.segments[1])
    blocks.extrude_blocks(positions=[1.], nb_segments=[self.segments[2]], gradings=[1.])
    blocks.create_mesh(self.mesh.uri())
    self.setup_lss()
    
  def cube_mesh_tetras(self):
    self.mesh = cf.Core.root().create_component('Mesh', 'cf3.mesh.Mesh')
    blocks = make_square(self.segments[0], self.segments[1])
    blocks.extrude_blocks(positions=[1.], nb_segments=[self.segments[2]], gradings=[1.])
    blocks.create_mesh(self.mesh.uri())
    triangulator = self.domain.create_component('triangulator', 'cf3.mesh.MeshTriangulator')
    triangulator.options().configure_option('mesh', self.mesh)
    triangulator.execute()
    self.mesh.move_component(self.domain.uri())
    self.mesh.raise_mesh_loaded()
    self.setup_lss()
    
  def setup_lss(self):
    self.ns_solver.create_lss('cf3.math.LSS.TrilinosFEVbrMatrix')
    
  def run(self):
    time = self.model.create_time()
    time.options().configure_option('time_step', 1.)
    time.options().configure_option('end_time', 1.)
    self.model.simulate()
    self.ns_solver.store_timings()
    try:
      assembly_name = 'GenericAssembly'
      if(self.use_spec):
        assembly_name = 'SpecializedAssembly'
      print '<DartMeasurement name=\"{modname} time\" type=\"numeric/double\">{timing}</DartMeasurement>'.format(modname=self.model.name(), timing = self.ns_solver.get_child(assembly_name).properties()['timer_mean'])
    except:
      print 'Could not find timing info'

# Some shortcuts
root = cf.Core.root()
env = cf.Core.environment()

## Global configuration
env.options().configure_option('assertion_throws', False)
env.options().configure_option('assertion_backtrace', False)
env.options().configure_option('exception_backtrace', False)
env.options().configure_option('regist_signal_handlers', False)
env.options().configure_option('log_level', 4)

# Generic assembly over quads
test_case = TestCase('QuadsGeneric', [500,400], False)
test_case.square_mesh_quads()
test_case.run()
test_case.model.delete_component()


# Generic assembly over triags
test_case = TestCase('TriagsGeneric', [500,400], False)
test_case.square_mesh_triags()
test_case.run()
test_case.model.delete_component()

# Generic assembly over hexahedrons
test_case = TestCase('HexasGeneric', [80, 50, 50], False)
test_case.cube_mesh_hexas()
test_case.run()
test_case.model.delete_component()

# Generic assembly over tetrahedrons
test_case = TestCase('TetrasGeneric', [80, 50, 50], False)
test_case.cube_mesh_tetras()
test_case.run()
test_case.model.delete_component()

# Specialized assembly over triangles
test_case = TestCase('TriagsSpecialized', [500,400], True)
test_case.square_mesh_triags()
test_case.run()
test_case.model.delete_component()

# Specialized assembly over tetrahedrons
test_case = TestCase('TetrasSpecialized', [80, 50, 50], True)
test_case.cube_mesh_tetras()
test_case.run()
test_case.model.delete_component()