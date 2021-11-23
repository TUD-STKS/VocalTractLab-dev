from dolfin import *
import math
import cmath
import numpy as np
import matplotlib.pyplot as plt
from scipy import spatial
# import modules

# load the mesh
#mesh, boundary, ds, dx = modules.getMesh('cone_0_001')
meshFile = 'junction_simple_cones_0_002'
mesh = Mesh(meshFile+".xml")
subdomains = MeshFunction("size_t", mesh, meshFile+ "_physical_region.xml")
boundary = MeshFunction('size_t', mesh, meshFile+ "_facet_region.xml")
ds = Measure("ds", domain=mesh, subdomain_data=boundary)
dx = Measure("dx", domain=mesh, subdomain_data=subdomains)


# define frequency limits and increment
freqMin = 0.1 # in kHz
freqIncr = 5. # in kHz
freqMax = 10000.
frequencies = (np.arange(1+np.int(freqMax/freqIncr))+1)*freqIncr

# calculate and print element characteristics
elemSize = [c.h() for c in cells(mesh)]
print("Minimum element size {} mm".format(np.amin(elemSize)))
print("Maximum element size {} mm".format(np.amax(elemSize)))
print("Median element size {} mm".format(np.median(elemSize)))
print("Mean element size {} mm".format(np.mean(elemSize)))

# define Function space 
poly = 1 # polynomial degree
el = FiniteElement("CG", mesh.ufl_cell(), poly)
V = FunctionSpace(mesh, MixedElement([el,el]))
U = VectorFunctionSpace(mesh, el, 3)
p_r, p_i = TrialFunctions(V)
q_r, q_i = TestFunctions(V)
pres = Function(V)
print("DOF = {}".format(V.dim()))

# get the coordinates of the point in the center of the exit
point = [0.08, 0., 0.]
V_dofs = V.tabulate_dof_coordinates().reshape(-1, mesh.geometry().dim())
V_len = len(V.split()) # number of function spaces
tol = 2.3
dofs = []
idxPoint = []
startIdxes = []
print("V_len: {}".format(V_len))
for n in range(V_len):
    if V.sub(n).num_sub_spaces() == 0:
        tmp_dofs = V.sub(n).dofmap().dofs() # all DOFs
        startIdxes.append(np.min(tmp_dofs)) # smallest DOF
        dofs.append(tmp_dofs) # DOFs for all sub spaces in ascending order

startIdx = np.min(startIdxes) # smallest index
for nn in range(np.shape(dofs)[0]):
    distance, idx = spatial.KDTree(V_dofs[dofs[nn], :]).query(point) # find closest node
    if distance <= tol:
        idxPoint.append([idx, V_dofs[dofs[nn], :][idx]]) # write coordinates
if idxPoint != []:
    idxC = idxPoint[0][0]

print("Node idx: {}".format(idx))
print("Coordinates {}".format(idxPoint[0]))

# Air properties
c = Constant(344.0)
rho = Constant(1.115771) 

# boundary condition values
vreal_in = Constant(-1.0) # inward normal vector
vimag_in = Constant(0.0)
zreal_w = Constant(c*rho/0.01) # taken from Fleischer et al. (2018), PLOS ONE
zimag_w = Constant(0.0)
zreal_lips = 1./100.;
zimag_lips = 0.0;

# geometry values
alips = assemble(1.0*ds(2))
aglottis = assemble(1.0*ds(1))
print("alips = {}; aglottis = {}".format(alips, aglottis))

# initialisation
absp = []
phasep = []
absu = []
phaseu = []
ii = 0

# solve wave problem
for freq in frequencies:
    ii = ii +1;
    print("Process {} / {}".format(freq, np.amax(frequencies)))

    k = Constant(((2*math.pi*freq) / c))
    k2 = Constant(k**2)
    zr_lips = Constant(((2*math.pi*freq*rho*zimag_lips)))
    zi_lips = Constant(((-2*math.pi*freq*rho*zreal_lips)))

    a = (-k2 * (inner(q_r, p_r) + inner(q_i, p_i)) + \
            inner(nabla_grad(q_r), nabla_grad(p_r)) + \
            inner(nabla_grad(q_i), nabla_grad(p_i))) * dx(1) \
            + ((k*c*rho/(zreal_w**2 + zimag_w**2)) * \
            (zreal_w * (inner(q_i, p_r) - inner(q_r, p_i)) + \
            zimag_w * (inner(q_r, p_r) + inner(q_i, p_i)))) * ds(3) \
            + ((k*c*rho/(zr_lips**2 + zi_lips**2)) * \
            (zr_lips * (inner(q_i, p_r) - inner(q_r, p_i)) + \
            zi_lips * (inner(q_r, p_r) + inner(q_i, p_i)))) * ds(2)
    F = a

    f_r = Expression('2*p*f*r*vr', p=math.pi, f=freq, r=rho, vr=vreal_in, degree=2) # Neumann (real part)
    f_i = Expression('2*p*f*r*vi', p=math.pi, f=freq, r=rho, vi=vimag_in, degree=2) # Neumann (imaginary part)
    L = Constant(0.0)*q_r*dx(1) + f_i*q_r*ds(1) - f_r*q_i*ds(1)
    F = F-L

    solve(lhs(F) == rhs(F), pres)
    #        solver_parameters=dict(linear_solver='numps', preconditioner='amg'))

    pres_real, pres_imag = pres.split(deepcopy=True)
    tmp_pr = pres_real.vector().get_local()[idxC]
    tmp_pi = pres_imag.vector().get_local()[idxC]
    p_lips = complex(tmp_pr, tmp_pi)
    Z_lips = complex(float(zr_lips), float(zi_lips))
    abs_u_lips = abs(p_lips/Z_lips)
    phase_u_lips = cmath.phase(p_lips/Z_lips)

    absp.append(abs(p_lips))
    phasep.append(cmath.phase(p_lips))
    absu.append(abs_u_lips)
    phaseu.append(phase_u_lips)

    np.savetxt('tfFEM.txt', np.transpose(np.vstack([frequencies[:ii], np.array(absu), np.array(phaseu), np.array(absp), np.array(phasep)])))

# save solution to vtk file
vtkFile = File('simple_tube.pvd')
vtkFile << pres_real

plot(sqrt(pres_real**2 + pres_imag**2))
plt.show()


