.\"
.\"	$Id: cssgrid.m,v 1.1 1999-06-29 00:23:03 fred Exp $
.\"
.TH CSSGRID 3NCARG "JUNE 1999" UNIX "NCAR GRAPHICS"
.SH NAME
CSSGRID - cubic spline interpolation on a sphere.
.SH SYNOPSIS
CALL CSSGRID (N, X, Y, Z, F, NI, NJ, PLAT, PLON, FF, IWK, WK, IER)
.SH DESCRIPTION
.IP N 12
(integer,input) The number of input data points (N > 2). 
.IP X 12
(real, input) An array containing the X component of the Cartesian 
coordinates of the input data. X(I)**2 + Y(I)**2 + Z(I)**2 = 1. 
for I = 1 to N. The first three points must not be collinear 
(lie on a common great circle). 
.IP Y 12
(real, input) An array containing the Y component of the Cartesian 
coordinates of the input data. X(I)**2 + Y(I)**2 + Z(I)**2 = 1. 
for I = 1 to N. The first three points must not be collinear 
(lie on a common great circle). 
.IP Z 12
(real, input) An array containing the Z component of the Cartesian 
coordinates of the input data. X(I)**2 + Y(I)**2 + Z(I)**2 = 1. 
for I = 1 to N. The first three points must not be collinear 
(lie on a common great circle). 
.IP F 12
(real, input) An array containing data values. F(I) is a functional 
value at (X(I),Y(I),Z(I)) for I = 1 to N. 
.IP NI 12
(integer, input) The number of rows in the uniform output grid. NI can be 1. 
.IP NJ 12
(integer, input) The number of columns in the uniform output grid. NJ can be 1. 
.IP PLAT 12
(real, output) An array of length NI 
containing the latitudes the output grid lines. 
The values in PLAT should be in radians.
.IP PLON 12
(real, output) An array of length NJ 
containing the longitudes the output grid lines. 
The value in PLON should be in radians.
.IP FF 12
(real, output) An NI by NJ array containing the desired 
interpolated values. FF(I,J) is the interpolated value at 
the coordinate specified by PLAT(I) and PLON(J) for I = 1 
to NI and J = 1 to NJ. 
.IP IWK 12 
(integer, input) An integer workspace of length 15*N. 
.IP WK 12
(real, input) A work array dimensioned for N.
.IP IER 12
(integer, output) An error return value.  If IER is returned as 0, then
no errors were detected. If IER is non-zero, then refer to the man
page for cssgrid_errors for details.
.SH USAGE
CSSGRID is called to find an interpolating cubic spline for random data
on a sphere.
.SH ACCESS
To use CSSGRID, load the NCAR Graphics library ngmath.
.SH SEE ALSO
css_overview,
csstri,
csscoord,
cstrans,
csvoro,
c_cssgrid,
c_csstri,
c_csscoord,
c_cstrans,
c_csvoro,
cssgrid_errors
.sp
Complete documentation for Cssgrid is available at URL
.br
http://ngwww.ucar.edu/ngdoc/ng/ngmath/cssgrid/csshome.html
.SH COPYRIGHT
Copyright (C) 1997-1999
.br
University Corporation for Atmospheric Research
.br
The use of this Software is governed by a License Agreement.
