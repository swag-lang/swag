
#unittest pass lexer;

.
.1
.1E3
1.e2
1.0E-2
1e2

#unittest error;
1e
#unittest error;
1e+
#unittest error;
1e-
#unittest error;
1eA
#unittest error;
1e_10
#unittest error;
1e1_0_

1e+10
1e-10
0.456E20
0.4_5_6E-2_0
1_1.4_5_6E-2_0
