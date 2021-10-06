To run the multimodal simulation, 
  - click on the button "Run test junction" in thumbnail 3D acoustic simulation
  - select the file param_junction_simple_cones.csv

To run the FEM simulation, 
  - open the file junction_simple_cones_0_002.geo with gmsh
  - generate the 3D mesh
  - save the mesh as junction_simple_cones_0_002.msh being careful to select ASCII 2 as format
  - run the shel script run_FEM.sh
