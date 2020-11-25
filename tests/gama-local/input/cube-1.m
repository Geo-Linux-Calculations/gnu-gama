#  Cube - simple demo 1
#
#

# vertices

A = [  0   0   0];
B = [100   0   0] + randn(1,3)/100;
C = [100 100   0] + randn(1,3)/100;
D = [  0 100   0] + randn(1,3)/100;

E = [  0   0 100] + randn(1,3)/100;
F = [100   0 100] + randn(1,3)/100;
G = [100 100 100] + randn(1,3)/100;
H = [  0 100 100] + randn(1,3)/100;

S = (A + B + C + D + E + F + G + H)/8 + [10 -15  5] + randn(1,3)/300;

# vectors

AB = B - A + randn(1,3)/300;
BC = C - B + randn(1,3)/300;
CD = D - C + randn(1,3)/300;
DA = A - D + randn(1,3)/300;

AE = E - A + randn(1,3)/300;
BF = F - B + randn(1,3)/300;
CG = G - C + randn(1,3)/300;
DH = H - D + randn(1,3)/300;

EG = G - E + randn(1,3)/300;
FH = H - F + randn(1,3)/300;

AS = S - A + randn(1,3)/300;
BS = S - B + randn(1,3)/300;
CS = S - C + randn(1,3)/300;
DS = S - D + randn(1,3)/300;
ES = S - E + randn(1,3)/300;
FS = S - F + randn(1,3)/300;
GS = S - G + randn(1,3)/300;
HS = S - H + randn(1,3)/300;

AG = G - A + randn(1,3)/300;
DH = H - D + randn(1,3)/300;
EC = C - E + randn(1,3)/300;
FD = D - F + randn(1,3)/300;


# --------------------------------------------------------------------------

printf ("<?xml version='1.0' ?>\n\n")

printf ("<gama-local xmlns='http://www.gnu.org/software/gama/gama-local'>\n")
printf ("<network>\n\n")

printf ("<description>\nCube-1\n</description>\n\n")

printf ("<parameters\n   sigma-act = 'aposteriori'\n/>\n\n")
printf ("<points-observations>\n\n")

printf ("<point id='A' x='%.3f' y='%.3f' z='%.3f' fix='xyz' />\n", A)
printf ("<point id='B' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", B)
printf ("<point id='C' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", C)
printf ("<point id='D' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", D)
printf ("<point id='E' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", E)
printf ("<point id='F' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", F)
printf ("<point id='G' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", G)
printf ("<point id='H' x='%.2f' y='%.2f' z='%.2f' adj='xyz' />\n", H)
printf ("<point id='S' x='%.2f' y='%.2f' z='%.2f' adj='XYZ' />\n", S)


printf ("\n<vectors>\n")
printf ("<vec from='A' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", AS)
printf ("<vec from='B' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", BS)
printf ("<vec from='C' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", CS)
printf ("<vec from='D' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", DS)
printf ("<vec from='E' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", ES)
printf ("<vec from='F' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", FS)
printf ("<vec from='G' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", GS)
printf ("<vec from='H' to='S' dx='%.4f' dy='%.4f' dz='%.4f' />\n", HS)

printf ("<vec from='A' to='G' dx='%.4f' dy='%.4f' dz='%.4f' />\n", AG)
printf ("<vec from='D' to='H' dx='%.4f' dy='%.4f' dz='%.4f' />\n", DH)
printf ("<vec from='A' to='B' dx='%.4f' dy='%.4f' dz='%.4f' />\n", AB)
printf ("<vec from='C' to='G' dx='%.4f' dy='%.4f' dz='%.4f' />\n", CG)
printf ("<vec from='E' to='C' dx='%.4f' dy='%.4f' dz='%.4f' />\n", EC)
printf ("<vec from='F' to='D' dx='%.4f' dy='%.4f' dz='%.4f' />\n", FD)


printf ("\n<cov-mat dim='42' band='0' >\n")
printf ("     1 1 1 1 1  1 1 1 1 1 \n")
printf ("     1 1 1 1 1  1 1 1 1 1 \n")
printf ("     1 1 1 1 1  1 1 1 1 1 \n")
printf ("     1 1 1 1 1  1 1 1 1 1 \n")
printf ("     1 1                  \n")
printf ("</cov-mat>\n\n")

printf ("</vectors>\n\n")

printf ("</points-observations>\n\n")

printf ("</network>\n")
printf ("</gama-local>\n")
