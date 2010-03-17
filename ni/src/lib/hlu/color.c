/*
 * $Id: color.c,v 1.1.2.1 2010-03-17 20:44:44 brownrig Exp $
 *
 */

#include <stdint.h>
#include <ncarg/hlu/BaseP.h>
#include <ncarg/hlu/WorkstationP.h>
#include <ncarg/gksP.h>


const uint32_t ALPHA_MASK = 0x40000000;
const uint32_t ALPHA_OPAQUE = 0x7f000000;

/*
 * _NhlSetOpacity()
 *
 * Worker function for the _NhlSetXXXXXOpacity() functions below.
 *
 */
static void _NhlSetOpacity(void* layer, float opacity, int attrib)
{
	_NGCAlpha alphaInfo;
	NhlWorkstationLayer wksLayer = (NhlWorkstationLayer) ((NhlLayer)layer)->base.wkptr;

	alphaInfo.type = NGC_SETALPHA;
	alphaInfo.work_id = wksLayer->work.gkswksid;
	alphaInfo.graphicAttrib = attrib;
	alphaInfo.alpha = opacity;

	Gescape_in_data gesc;
	gesc.escape_r1.data = &alphaInfo;
	gesc.escape_r1.size = sizeof(alphaInfo);
	gescape(NGESC_CNATIVE, &gesc, NULL, NULL);
}

/*
 * _NhlGetOpacity()
 *
 * Worker function for the _NhlGetXXXXXOpacity() functions below.
 *
 */
static float _NhlGetOpacity(void* layer, int attrib)
{
	_NGCAlpha alphaInfo;
	NhlWorkstationLayer wksLayer = (NhlWorkstationLayer) ((NhlLayer)layer)->base.wkptr;

	alphaInfo.type = NGC_GETALPHA;
	alphaInfo.work_id = wksLayer->work.gkswksid;
	alphaInfo.graphicAttrib = attrib;
	alphaInfo.alpha = 1.0;    /* initialize, as some drivers don't populate this */

	Gescape_in_data gesc;
	gesc.escape_r1.data = &alphaInfo;
	gesc.escape_r1.size = sizeof(alphaInfo);
	gescape(NGESC_CNATIVE, &gesc, NULL, NULL);
	return alphaInfo.alpha;
}


void _NhlSetLineOpacity(void* layer, float opacity)
{
	_NhlSetOpacity(layer, opacity, NGC_LINEALPHA);
}

float _NhlGetLineOpacity(void* layer) {
	return _NhlGetOpacity(layer, NGC_LINEALPHA);
}


void _NhlSetFillOpacity(void* layer, float opacity)
{
	_NhlSetOpacity(layer, opacity, NGC_FILLALPHA);
}

float _NhlGetFillOpacity(void* layer) {
	return _NhlGetOpacity(layer, NGC_FILLALPHA);
}


void _NhlSetMarkerOpacity(void* layer, float opacity)
{
	_NhlSetOpacity(layer, opacity, NGC_MARKERALPHA);
}

float _NhlGetMarkerOpacity(void* layer) {
	return _NhlGetOpacity(layer, NGC_MARKERALPHA);
}

