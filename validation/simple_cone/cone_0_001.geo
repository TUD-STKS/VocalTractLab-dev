ms = DefineNumber[0.001, Name "mesh size"];
radius1 =  DefineNumber[0.01, Name "radius1"];
radius2 =  DefineNumber[0.02, Name "radius2"];
length =  DefineNumber[0.085, Name "length"];

Point(1) = {0., 0., 0., ms };
Point(2) = {0., radius1, 0., ms};
Point(3) = {0., 0., radius1, ms};
Point(4) = {0., -radius1, 0., ms};
Point(5) = {0., 0., -radius1, ms};
Point(6) = {length, 0., 0., ms};
Point(7) = {length, radius2, 0., ms};
Point(8) = {length, 0., radius2, ms};
Point(9) = {length, -radius2, 0., ms};
Point(10) = {length, 0., -radius2, ms};

Line(11) = {2, 7};
Line(12) = {3, 8};
Line(13) = {4, 9};
Line(14) = {5, 10};


Circle(100) = {2,1,3};
Circle(101) = {3,1,4};
Circle(102) = {4,1,5};
Circle(103) = {5,1,2};
Curve Loop (104) = {100, 101, 102, 103};
Circle(105) = {7,6,8};
Circle(106) = {8,6,9};
Circle(107) = {9,6,10};
Circle(108) = {10,6,7};
Curve Loop (109) = {105, 106, 107, 108};
Curve Loop (110) = {100, 12, -105, -11};
Curve Loop (111) = {101, 13, -106, -12};
Curve Loop (112) = {102, 14, -107, -13};
Curve Loop (113) = {103, 11, -108, -14};

Plane Surface (1000) = {104};
Plane Surface (1001) = {109};
Surface (1002) = {110};
Surface (1003) = {111};
Surface (1004) = {112};
Surface (1005) = {113};
Surface Loop (1006) = {1000, 1001, 1002, 1003, 1004, 1005};
Physical Surface(1) = {1000}; // entrance
Physical Surface(2) = {1001}; // open end
Physical Surface(3) = {1002, 1003, 1004, 1005}; // walls

Volume (10000) = {1006};
Physical Volume(1) = {10000};
