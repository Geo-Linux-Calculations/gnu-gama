#  Tetrahedron - simple demo
#

e = 100;   # edge
d = sqrt(e^2 - (e/2)^2);

A = [  0   0   0];
B = [  e   0   0];
C = [ e/2  d   0];

D = (A + B + C)/3;
p = norm(D - C);
D(3) = sqrt(e^2 - p^2);

# norm (D - A)
# norm (D - B)
# norm (D - C)
# norm (A - C)
# norm (A - B)
# norm (B - C)

# --------------------------------------------------------------------------

printf ("<?xml version='1.0' ?>\n\n")

printf ("<gama-local xmlns='http://www.gnu.org/software/gama/gama-local'>\n")
printf ("<network>\n\n")

printf ("<description>\nTetrahedron\n</description>\n\n")

printf ("<parameters\n   sigma-act = 'aposteriori'\n/>\n\n")
printf ("<points-observations>\n\n")

printf ("<point id='A' x='%.6f' y='%.6f' z='%.6f' fix='xyz' />\n", A)
printf ("<point id='B' x='%.6f' y='%.6f' z='%.6f' fix='xyz' />\n", B)
printf ("<point id='C' x='%.6f' y='%.6f' z='%.6f' fix='xyz' />\n", C)
printf ("<point id='D' x='%.6f' y='%.6f' z='%.6f' adj='xyz' />\n", D)

printf ("\n<vectors>\n")
printf ("<vec from='A' to='D' dx='%.6f' dy='%.6f' dz='%.6f' />\n", D-A)
printf ("<vec from='B' to='D' dx='%.6f' dy='%.6f' dz='%.6f' />\n", D-B)
printf ("<vec from='C' to='D' dx='%.6f' dy='%.6f' dz='%.6f' />\n", D-C)

printf ("\n<cov-mat dim='9' band='0' >\n")
printf ("     1 1 1   1 1 1   1 1 1 \n")
printf ("</cov-mat>\n\n")

printf ("</vectors>\n\n")

printf ("</points-observations>\n\n")

printf ("</network>\n")
printf ("</gama-local>\n")
