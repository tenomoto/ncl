C
C $Id: intrvldp.f,v 1.2 2003-08-11 22:44:01 haley Exp $
C
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C This file is free software; you can redistribute it and/or modify
C it under the terms of the GNU General Public License as published
C by the Free Software Foundation; either version 2 of the License, or
C (at your option) any later version.
C
C This software is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C General Public License for more details.
C
C You should have received a copy of the GNU General Public License
C along with this software; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
C USA.
C
C NOTE: If you make any changes to this software, please remember to
C make the same changes to the corresponding single precision routine.
C
      FUNCTION INTRVLDP(T,X,N)
      DOUBLE PRECISION TT
c
      INTEGER N
      DOUBLE PRECISION T,X(N)
c
c                                 coded by alan kaylor cline
c                           from fitpack -- january 26, 1987
c                        a curve and surface fitting package
c                      a product of pleasant valley software
c                  8603 altus cove, austin, texas 78759, usa
c
c this function determines the index of the interval
c (determined by a given increasing sequence) in which
c a given value lies.
c
c on input--
c
c   t is the given value.
c
c   x is a vector of strictly increasing values.
c
c and
c
c   n is the length of x (n .ge. 2).
c
c on output--
c
c   intrvldp returns an integer i such that
c
c          i =  1       if             t .le. x(2)  ,
c          i =  n-1     if x(n-1) .le. t            ,
c          otherwise       x(i)  .le. t .le. x(i+1),
c
c none of the input parameters are altered.
c
c-----------------------------------------------------------
c
      SAVE I
      DATA I/1/
c
      TT = T
c
c check for illegal i
c
      IF (I.GE.N) I = N/2
c
c check old interval and extremes
c
      IF (TT.LT.X(I)) THEN
          IF (TT.LE.X(2)) THEN
              I = 1
              INTRVLDP = 1
              RETURN
          ELSE
              IL = 2
              IH = I
          END IF
      ELSE IF (TT.LE.X(I+1)) THEN
          INTRVLDP = I
          RETURN
      ELSE IF (TT.GE.X(N-1)) THEN
          I = N - 1
          INTRVLDP = N - 1
          RETURN
      ELSE
          IL = I + 1
          IH = N - 1
      END IF
c
c binary search loop
c
    1 I = (IL+IH)/2
      IF (TT.LT.X(I)) THEN
          IH = I
      ELSE IF (TT.GT.X(I+1)) THEN
          IL = I + 1
      ELSE
          INTRVLDP = I
          RETURN
      END IF
      GO TO 1
      END
