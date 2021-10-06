ms = DefineNumber[0.002, Name "mesh size"];
radius1 =  DefineNumber[0.01, Name "radius1"];
radius2 =  DefineNumber[0.02, Name "radius2"];
length =  DefineNumber[0.085, Name "length"];

///////////////////////////
// Define points
///////////////////////////

// for first circle
Point(1) = {0., 0., 0., ms };
Point(2) = {0., radius2, 0., ms};
Point(3) = {0., 0., radius2, ms};
Point(4) = {0., -radius2, 0., ms};
Point(5) = {0., 0., -radius2, ms};
// for second circle 
Point(6) = {length, 0., 0., ms};
Point(7) = {length, radius1, 0., ms};
Point(8) = {length, 0., radius1, ms};
Point(9) = {length, -radius1, 0., ms};
Point(10) = {length, 0., -radius1, ms};
// for third circle 
// center is the same as previous circle
Point(12) = {length, radius2, 0., ms};
Point(13) = {length, 0., radius2, ms};
Point(14) = {length, -radius2, 0., ms};
Point(15) = {length, 0., -radius2, ms};
// for fourth circle 
Point(16) = {2.*length, 0., 0., ms};
Point(17) = {2.*length, radius1, 0., ms};
Point(18) = {2.*length, 0., radius1, ms};
Point(19) = {2.*length, -radius1, 0., ms};
Point(20) = {2.*length, 0., -radius1, ms};

///////////////////////////
// Define curves
///////////////////////////

// for first cone
Line(11) = {2, 7};
Line(12) = {3, 8};
Line(13) = {4, 9};
Line(14) = {5, 10};

// for second cone
Line(15) = {12, 17};
Line(16) = {13, 18};
Line(17) = {14, 19};
Line(18) = {15, 20};

// first circle
Circle(100) = {2,1,3};
Circle(101) = {3,1,4};
Circle(102) = {4,1,5};
Circle(103) = {5,1,2};
Curve Loop (104) = {100, 101, 102, 103};

// second circle
Circle(105) = {7,6,8};
Circle(106) = {8,6,9};
Circle(107) = {9,6,10};
Circle(108) = {10,6,7};
Curve Loop (109) = {105, 106, 107, 108};

// side of the first cone
Curve Loop (110) = {100, 12, -105, -11};// side 1 of first cone
Curve Loop (111) = {101, 13, -106, -12};// side 2 of first cone
Curve Loop (112) = {102, 14, -107, -13};// side 3 of first cone
Curve Loop (113) = {103, 11, -108, -14};// side 4 of first cone

// third circle
Circle(114) = {12, 6, 13};
Circle(115) = {13, 6, 14};
Circle(116) = {14, 6, 15};
Circle(117) = {15, 6, 12};
Curve Loop (118) = {114, 115, 116, 117};

// fourth circle
Circle(119) = {17, 16, 18};
Circle(120) = {18, 16, 19};
Circle(121) = {19, 16, 20};
Circle(122) = {20, 16, 17};
Curve Loop (123) = {119, 120, 121, 122};

// side of the second cone
Curve Loop (124) = {114, 16, -119, -15};
Curve Loop (125) = {115, 17, -120, -16};
Curve Loop (126) = {116, 18, -121, -17};
Curve Loop (127) = {117, 15, -122, -18};

///////////////////////////
// Define surfaces
///////////////////////////

Plane Surface (1000) = {104}; // entrance disk
Plane Surface (1001) = {109, 118}; // junction ring
Plane Surface (1002) = {123}; // end disk

// sides of the first cone
Surface (1003) = {110};
Surface (1004) = {111};
Surface (1005) = {112};
Surface (1006) = {113};

// sides of the second cone
Surface (1007) = {124};
Surface (1008) = {125};
Surface (1009) = {126};
Surface (1010) = {127};


Surface Loop (1011) = {1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010};
Physical Surface(1) = {1000}; // entrance
Physical Surface(2) = {1002}; // open end
Physical Surface(3) = {1001, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010}; // walls

Volume (10000) = {1011};
Physical Volume(1) = {10000};
