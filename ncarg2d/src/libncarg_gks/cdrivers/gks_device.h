/*
 *	$Id: gks_device.h,v 1.7 2003-01-06 23:30:13 fred Exp $
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
* along with this software; if not, write to the Free Software          *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
* USA.                                                                  *
*                                                                       *
************************************************************************/

/*
 *      File:		gks_device.h
 *
 *      Author:		John Clyne
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *      Date:		Wed May  1 17:49:30 MDT 1991
 *
 *      Description:	This file contains definition of a gks device.
 */
/*
 *	opcodes for gks instructions as defined by me. We don't use GKS
 *	definition because it uses negative numbers which are a pain to 
 *	use as indexes into arrays.
 */
#include <stddef.h>
#define	OPEN_WORKSTATION			0
#define	ACTIVATE_WORKSTATION			1
#define	CLOSE_WORKSTATION			2
#define	CLEAR_WORKSTATION			3
#define	POLYLINE				4
#define	POLYMARKER				5
#define	TEXT					6
#define	FILL_AREA				7
#define	CELL_ARRAY				8
#define	SET_LINETYPE				9
#define	SET_LINEWIDTH_SCALE_FACTOR		10
#define	SET_POLYLINE_COLOR_INDEX		11
#define	SET_MARKER_TYPE				12
#define	SET_MARKER_SIZE_SCALE_FACTOR		13
#define	SET_POLYMARKER_COLOR_INDEX		14
#define	SET_TEXT_FONT_AND_PRECISION		15
#define	SET_CHARACTER_EXPANSION_FACTOR		16
#define	SET_CHARACTER_SPACING			17
#define	SET_TEXT_COLOR_INDEX			18
#define	SET_CHARACTER_HEIGHT_AND_UP_VECTOR	19
#define	SET_TEXT_PATH				20
#define	SET_TEXT_ALIGNMENT			21
#define	SET_FILL_AREA_INTERIOR_STYLE		22
#define	SET_FILL_AREA_STYLE_INDEX		23
#define	SET_FILL_AREA_COLOR_INDEX		24
#define	SET_COLOR_REPRESENTATION		25
#define	SET_CLIP_INDICATOR			26
#define	SET_WINDOW				27
#define	GET_COLOR_REPRESENTATION		28
#define	ESCAPE					29
#define	UPDATE_WORKSTATION			30
#define	DEACTIVATE_WORKSTATION			31
#define	SET_VIEWPORT				32

/*
 *	total number of gks functions defined
 */
#define	MAX_GKS_FUNCTIONS			33


/*
 *	The GKSdev structure. This structure contains all the functions 
 *	necessary to implement a gks output device. 
 */ 
typedef	struct	GKSdev_	{
	char		*name;
	int		(*exec_gksc)();
	void		(*conv_points)();
	unsigned	sizeof_point;
	void		(*conv_string)();
	unsigned	sizeof_string;
	void		(*conv_ints)();
	unsigned	sizeof_int;
	void		(*conv_floats)();
	unsigned	sizeof_float;
	void		(*conv_indexes)();
	unsigned	sizeof_index;
	void		(*conv_rgbs)();
	unsigned	sizeof_rgb;
	int		(*operations[MAX_GKS_FUNCTIONS])();
	} GKSdev;

extern	GKSdev  *GKS_GetDevByName(
#ifdef	NeedFuncProto
        char    *name
#endif
);

extern GKSdev	*GKS_GetCTXTdev(
#ifdef	NeedFuncProto
	void
#endif
);

extern GKSdev	*GKS_GetX11dev(
#ifdef	NeedFuncProto
	void
#endif
);

extern GKSdev	*GKS_GetPSdev(
#ifdef	NeedFuncProto
	void
#endif
);

extern GKSdev	*GKS_GetPDFdev(
#ifdef	NeedFuncProto
	void
#endif
);
