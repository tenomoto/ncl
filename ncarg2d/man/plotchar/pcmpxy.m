.TH PCMPXY 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
PCMPXY - 
This routine is normally not called directly by the user
(though it can be). It is called by each of the routines PCHIQU, PCMEQU,
and PCLOQU when the user has set the mapping flag \'MA\' nonzero to
request mapping of characters from one X/Y coordinate
system to another.
.SH SYNOPSIS
CALL PCMPXY (IMAP,XINP,YINP,XOTP,YOTP)
.SH DESCRIPTION 
.IP IMAP 12
(an input expression of type INTEGER) specifies
which mapping is to be done. Whenever Plotchar routines
call PCMPXY, IMAP = \'MA\'. The default version of PCMPXY
recognizes the following values of IMAP:
.RS
.IP "IMAP = 0"
.RS
.IP "Type of mapping" 3
None. When
IMAP is zero,
the routine
PCMPXY is
being asked
for
information
about its
capabilities
with respect
to a
particular
mapping.
.IP XINP 3
Input: the
real
equivalent of
a possible
value of IMAP
about which
information
is desired.
.IP YINP 3
Output
(real): 0.
says that the
mapping
specified by
XINP is not
available, 1.
that it is
available, 2.
that its
inverse is
available,
and 3. that
both it and
its inverse
are available.
.RE
.IP "IMAP = 1"
.RS
.IP "Type of mapping" 3
Ezmap
projection.
.IP XINP 3
Longitude, in
degrees.
.IP YINP 3
Latitude, in
degrees.
.RE
.IP "IMAP = 2"
.RS
.IP "Type of mapping" 3
Rho/theta
mapping.
.IP XINP 3
Rho, in user
units.
.IP YINP 3
Theta, in
degrees.
.RE
.IP "IMAP = 3"
.RS
.IP "Type of mapping" 3
Projection
from an
arbitrary
parallelogram
in 3-space to
an image
plane. This is
implemented
using a
package which
is currently
under
development
and is not yet
generally
available.
.IP XINP 3
A multiplier
for a unit
vector along
one edge of
the
parallelogram.
.IP YINP 3
A multiplier
for a unit
vector along
an
intersecting
edge of the
parallelogram.
.RE
.IP "IMAP = 100"
.RS
.IP "Type of mapping" 3
The identity
mapping, but
returns 1.E12
outside a
specified
viewport. Used
by the utility
Scrolled_title to do
clipping at
the edges of
the viewport.
.IP XINP 3
X coordinate,
in the
fractional
system.
.IP YINP 3
Y coordinate,
in the
fractional
system.
.RE
.IP "other > 0"
.RS
.IP "Type of mapping" 3
The identity
mapping.
.IP XINP 3
Any real
value.
.IP YINP 3
Any real
value.
.RE
.IP "other < 0"
.RS
.IP "Type of mapping" 3
An inverse
mapping.
.IP XINP 3
A mapped X
coordinate.
.IP YINP 3
A mapped Y
coordinate.
.RE
.RE
.IP "" 12
Note: Values of IMAP less than or equal to zero are not
currently used by Plotchar in calling PCMPXY. The scheme
described is the same as that used for the routine CPMPXY,
in the NCAR Graphics package Conpack. It is possible that,
in the future, values of IMAP less than or equal to zero
will occur in calls to PCMPXY, so it is best to be prepared.
.IP "XINP and YINP" 12 
(input expressions of type REAL)
specify the X and Y coordinates of a point to be mapped.
.sp
Note: When IMAP is less than or equal to zero (which cannot
currently happen, but may someday be possible), XINP and
YINP are used differently, as shown above.
.IP "XOTP and YOTP" 
(output variables of type REAL) 
in which
PCMPXY returns the mapped X and Y coordinates of the point.
If the point is not visible under the mapping selected (as,
for example, when the orthographic projection of Ezmap is
being used and the point to be mapped is on the other side
of the globe), PCMPXY must return, as a signal that this is
the case, whatever special nonzero value has been given to
the internal parameter \'OR\', so that the calling routine
can take appropriate action (to properly clip the character
being drawn, for example).
.sp
Note: When IMAP is less than or equal to zero (which cannot
currently happen, but may someday be possible), XOTP and
YOTP are used differently, as shown in the table above.
.SH USAGE
This routine is normally not called directly by the user
(though it can be). It is called by each of the routines PCHIQU, PCMEQU, and
PCLOQU when the user has set the mapping flag \'MA\' nonzero to
request mapping of characters from one X/Y coordinate
system to another. A call to PCMPXY has the following form:
.sp
.in +5
CALL PCMPXY (IMAP,XINP,YINP,XOTP,YOTP)
.in -5
.sp
The default version of PCMPXY does several useful mappings;
a user version may be supplied to do others.
.SH ACCESS
To use PCMPXY, load the NCAR Graphics libraries @@@ ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order.  
.SH SEE ALSO
Online:
plotchar,
pcdlsc,
pcgetc,
pcgeti,
pcgetr,
pchiqu,
pcloqu,
pcmequ,
pcpnwi,
pcsetc,
pcseti,
pcsetr,
ncarg_cbind.
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved
