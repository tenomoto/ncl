'\" t
.TH ISGETI 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh	
.SH NAME
ISGETI - Gets the current values of internal parameters
of type INTEGER. 
.SH SYNOPSIS
CALL ISGETI (CNP,IVP)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_isgeti (char *cnp, int *ivp)
.SH DESCRIPTION 
.IP CNP 12
(an input expression of type CHARACTER) is the name of
an internal parameter. Only the first two characters of the
name are meaningful, but a longer character string may be
used to more clearly document what internal parameter is
being retrieved.
.IP IVP 
(an output variable of type INTEGER) 
is the name of a variable into which
the value of the parameter specified by CNP is to be
retrieved.
If the internal parameter is inherently a real
value "r", "INT(r)" will be returned as the value of IVP.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH USAGE
This routine allows you to retrieve the current value of
Isosurface parameters.  For a complete list of parameters available
in this utility, see the isosurface_params man page.
.SH ACCESS
To use ISGETI, load the NCAR Graphics libraries ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order.  To use c_isgeti, load the 
NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order.
.SH MESSAGES
See the isosurface man page for a description of all Isosurface error
messages and/or informational messages.
.SH SEE ALSO
Online:
isosurface, isosurface_params, ezisos, 
isgetr, isosrf, isseti, issetr, pwrzi, 
ncarg_cbind
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
