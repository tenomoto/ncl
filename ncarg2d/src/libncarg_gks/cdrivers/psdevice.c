/*
 *      $Id: psdevice.c,v 1.5 2003-01-06 23:30:16 fred Exp $
 */
/************************************************************************
*                                                                       *
*                Copyright (C)  2000                                    *
*        University Corporation for Atmospheric Research                *
*                All Rights Reserved                                    *
*                                                                       *
* This file is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published     *
* by the Free Software Foundation; either version 2 of the License, or  *
* (at your option) any later version.                                   *
*                                                                       *
* This software is distributed in the hope that it will be useful, but  *
* WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
* General Public License for more details.                              *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this software; if not, write to the Free Software         *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
* USA.                                                                  *
*                                                                       *
************************************************************************/

/************************************************************************
*                                                                       *
*                            Copyright (C)  1996                        *
*            University Corporation for Atmospheric Research            *
*                            All Rights Reserved                        *
*                                                                       *
************************************************************************/
/*
 *      File:           psdevice.c
 *
 *      Author:         Fred Clare
 *                      National Center for Atmospheric Research
 *                      PO 3000, Boulder, Colorado
 *
 *      Date:           Fri Oct 18 09:17:29 MDT 1996
 *
 *      Description:    Breaking up gks_device.c so each driver can be a
 *                      delay-loaded dynamically shared object.
 *                      (The old dev_tab caused the text segments of all
 *                      the cdrivers to be included in the data segment -
 *                      and therefore caused resolution of those symbols
 *                      from the dso's even though the text symbols were
 *                      not actually called.  This way forces a symbol
 *                      call to actually force the dso to load.
 */
#include "gks_device.h"
#include "ps.h"
#include "ps_device.h"

static GKSdev psdev =
{
        "ps", 

        NULL,

        ps_ConvPoints, sizeof(PSPoint), 
        ps_ConvString, sizeof(char), ps_ConvInts, sizeof(int), 
        ps_ConvFloats, sizeof(int), ps_ConvIndexes, sizeof(int),
        ps_ConvRGBs, sizeof(PSColor),

        ps_OpenWorkstation, ps_ActivateWorkstation, 
        ps_CloseWorkstation, ps_ClearWorkstation, 
        ps_Polyline, ps_Polymarker, ps_Text, ps_FillArea, 
        ps_Cellarray, ps_SetLinetype, ps_SetLineWidthScaleFactor, 
        ps_SetPolylineColorIndex, ps_SetMarkerType, 
        ps_SetMarkerSizeScaleFactor, ps_SetPolymarkerColorIndex, 
        ps_SetTextFontAndPrecision, ps_SetCharacterExpansionFactor, 
        ps_SetCharacterSpacing, ps_SetTextColorIndex, 
        ps_SetCharacterHeightAndUpVector, ps_SetTextPath, 
        ps_SetTextAlignment, ps_SetFillAreaInteriorStyle, 
        ps_SetFillAreaStyleIndex, ps_SetFillAreaColorIndex, 
        ps_SetColorRepresentation, ps_SetClipIndicator, 
        ps_SetWindow, ps_GetColorRepresentation,
        ps_Esc, ps_UpdateWorkstation, ps_DeactivateWorkstation,
        ps_SetViewport
};

GKSdev
*GKS_GetPSdev
#ifdef  NeedFuncProto
(
        void
)
#else
()
#endif
{
        return &psdev;
}
