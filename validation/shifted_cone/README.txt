To run the multimodal simulation, 
  - click on the button "Run test junction" in thumbnail 3D acoustic simulation
  - select the file param_shifted_cone.csv

To run the FEM simulation, 
  - open the file shifted_cone.geo with gmsh
  - generate the 3D mesh
  - save the mesh as shifted_cone.msh being careful to select ASCII 2 as format
  - run the shel script run_FEM.sh
