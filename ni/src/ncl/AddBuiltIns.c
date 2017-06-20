/*
 *      $Id$
 */
/************************************************************************
*                                                                       *
*                Copyright (C)  1993                                    *
*           University Corporation for Atmospheric Research             *
*               All Rights Reserved                                     *
*                                                                       *
************************************************************************/
/*
 *	File:		AddBuiltIns.c
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Oct 15 10:43:07 MDT 1993
 *
 *	Description:	
 */
#ifdef __cpluplus
extern "C" {
#endif
#include <stdio.h>
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#include "defs.h"
#include "NclDataDefs.h"
#include "NclBuiltIns.h"
#include "MathFuncs.h"
#include "HLUFunctions.h"
extern NhlErrorTypes _NclILoadScript(void);
extern NhlErrorTypes _NclICreateFile(void);
extern NhlErrorTypes _NclISetFileOption(void);
extern NhlErrorTypes _NclIshorttoint(void);
extern NhlErrorTypes _NclIushorttoint(void);

extern NhlErrorTypes _NclIAddFiles(void);

extern NhlErrorTypes _NclIGetFileGroups(void);

extern NhlErrorTypes _NclIGetGroupVars(void);

extern NhlErrorTypes    _NclIFileIsPresent(void);

extern NhlErrorTypes _NclIFileExists(void);

extern int _MachineIsBigEndian();

extern NhlErrorTypes _NclIGetFileChunkSizes(void);
extern NhlErrorTypes _NclIGetFileCompressionLevel(void);

extern NhlErrorTypes _NclIGetFileVersion(void);

extern NhlErrorTypes    _NclIGetFileDimsizes(void);

extern NhlErrorTypes    _NclIGetFileVarTypes(void);

extern NhlErrorTypes _NclIprintFileVarSummary(void);
extern NhlErrorTypes _NclIprintVarSummary(void);
extern NhlErrorTypes _NclINhlGetParentId(void);
extern NhlErrorTypes _NclINhlGetParentWorkstation(void);
extern NhlErrorTypes sprintf_W(void);
extern NhlErrorTypes sprinti_W(void);

extern NhlErrorTypes _NclIFileVlenDef(void);
extern NhlErrorTypes _NclIFileEnumDef(void);
extern NhlErrorTypes _NclIFileCompoundDef(void);
extern NhlErrorTypes _NclIFileWriteCompound(void);
extern NhlErrorTypes _NclIFileOpaqueDef(void);
extern NhlErrorTypes _NclIFileGrpDef(void);

extern NhlErrorTypes _NclIFileVarDef(void);

extern NhlErrorTypes _NclIFileVarChunkDef(void);

extern NhlErrorTypes _NclIFileVarCompressLevelDef(void);

extern NhlErrorTypes _NclIFileVarChunkCacheDef(void);

extern NhlErrorTypes _NclIFileDimDef(void);

extern NhlErrorTypes _NclIFileChunkDimDef(void);

extern NhlErrorTypes _NclIFileAttDef(void);
extern NhlErrorTypes _NclIFileVarAttDef(void);
extern NhlErrorTypes _NclIdim_stddev(void);
extern NhlErrorTypes _NclIdim_stddev_n(void);
extern NhlErrorTypes _NclIstddev(void);


extern NhlErrorTypes _NclIdim_variance(void);

extern NhlErrorTypes _NclIdim_variance_n(void);

extern NhlErrorTypes _NclIvariance(void);

extern NhlErrorTypes _NclIgaus(void);
extern NhlErrorTypes _Nclmax(void);
extern NhlErrorTypes _Nclmin(void);
extern NhlErrorTypes _Nclmaxind(void);
extern NhlErrorTypes _Nclminind(void);
extern NhlErrorTypes _Ncldim_max(void);
extern NhlErrorTypes _Ncldim_max_n(void);
extern NhlErrorTypes _Ncldim_min(void);
extern NhlErrorTypes _Ncldim_min_n(void);
extern NhlErrorTypes _Nclmask(void);
extern NhlErrorTypes _Nclwhere(void);
extern NhlErrorTypes _Nclispan(void);
extern NhlErrorTypes _Nclfspan(void);
extern NhlErrorTypes _Nclind(void);
extern NhlErrorTypes _Nclnum(void);
extern NhlErrorTypes _Ncl1dtond(void);
extern NhlErrorTypes _Nclndto1d(void);

extern NhlErrorTypes _Ncldim_product(void);
extern NhlErrorTypes _Ncldim_product_n(void);
extern NhlErrorTypes _Nclproduct(void);
extern NhlErrorTypes _Ncldim_sum(void);
extern NhlErrorTypes _Ncldim_sum_n(void);
extern NhlErrorTypes _Nclsum(void);

extern NhlErrorTypes _Ncldim_cumsum(void);

extern NhlErrorTypes _Ncldim_cumsum_n(void);

extern NhlErrorTypes _Nclcumsum(void);

extern NhlErrorTypes _Ncldim_avg(void);

extern NhlErrorTypes _Ncldim_avg_n(void);

extern NhlErrorTypes _Nclavg(void);

extern NhlErrorTypes _NclIinttobyte(void);
extern NhlErrorTypes _NclIinttochar(void);
extern NhlErrorTypes _NclIinttoshort(void);

extern NhlErrorTypes _NclIshorttobyte(void);
extern NhlErrorTypes _NclIshorttochar(void);

extern NhlErrorTypes _NclIlongtobyte(void);
extern NhlErrorTypes _NclIlongtochar(void);
extern NhlErrorTypes _NclIlongtoshort(void);
extern NhlErrorTypes _NclIlongtoint(void);

extern NhlErrorTypes _NclItochar(void);
extern NhlErrorTypes _NclItobyte(void);
extern NhlErrorTypes _NclItoubyte(void);

extern NhlErrorTypes _NclItoshort(void);
extern NhlErrorTypes _NclItoushort(void);

extern NhlErrorTypes _NclItoint(void);
extern NhlErrorTypes _NclItouint(void);

extern NhlErrorTypes _NclItolong(void);
extern NhlErrorTypes _NclItoulong(void);

extern NhlErrorTypes _NclItoint64(void);
extern NhlErrorTypes _NclItouint64(void);

extern NhlErrorTypes _NclItofloat(void);
extern NhlErrorTypes _NclItodouble(void);

extern NhlErrorTypes _NclItostring(void);

extern NhlErrorTypes _NclItostring_with_format(void);

extern NhlErrorTypes _NclItosigned(void);

extern NhlErrorTypes _NclItounsigned(void);


extern NhlErrorTypes _NclIfloattobyte(void);
extern NhlErrorTypes _NclIfloattochar(void);
extern NhlErrorTypes _NclIfloattoshort(void);
extern NhlErrorTypes _NclIfloattoint(void);
extern NhlErrorTypes _NclIfloattolong(void);

extern NhlErrorTypes _NclIdoubletobyte(void);
extern NhlErrorTypes _NclIdoubletochar(void);
extern NhlErrorTypes _NclIdoubletoshort(void);
extern NhlErrorTypes _NclIdoubletoint(void);
extern NhlErrorTypes _NclIdoubletolong(void);
extern NhlErrorTypes _NclIdoubletofloat(void);

extern NhlErrorTypes _NclIchartodouble(void);
extern NhlErrorTypes _NclIchartofloat(void);
extern NhlErrorTypes _NclIchartolong(void);
extern NhlErrorTypes _NclIchartoshort(void);
extern NhlErrorTypes _NclIchartoint(void);
extern NhlErrorTypes _NclIstringtointeger(void);
extern NhlErrorTypes _NclIstringtoshort(void);
extern NhlErrorTypes _NclIstringtolong(void);
extern NhlErrorTypes _NclIstringtoint64(void);
extern NhlErrorTypes _NclIstringtoushort(void);
extern NhlErrorTypes _NclIstringtouint(void);
extern NhlErrorTypes _NclIstringtoulong(void);
extern NhlErrorTypes _NclIstringtouint64(void);
extern NhlErrorTypes _NclIstringtodouble(void);
extern NhlErrorTypes _NclIstringtofloat(void);
extern NhlErrorTypes _NclIstringtochar(void);
extern NhlErrorTypes _NclIchartostring(void);
extern NhlErrorTypes _Nclsystem(void);

extern NhlErrorTypes _Nclsystemfunc(void);

extern NhlErrorTypes _Nclstrlen(void);

extern NhlErrorTypes _Nclidsfft(void);

extern NhlErrorTypes _NclIIsVar(void);

extern NhlErrorTypes _NclIIsFileVar(void);

extern NhlErrorTypes _NclIIsCoord(void);
extern NhlErrorTypes _NclIIsAtt(void);

extern NhlErrorTypes _NclIIsFileVarAtt(void);

extern NhlErrorTypes _NclIIsDim(void);

extern NhlErrorTypes _NclIIsDimNamed(void);

extern NhlErrorTypes    _NclIVarIsUnlimited(void);

extern NhlErrorTypes _NclIIsFileVarDim(void);

extern NhlErrorTypes _NclIIsFileVarCoord(void);

extern NhlErrorTypes _NclIIsDefined(void);
extern NhlErrorTypes _NclIIsProc(void);
extern NhlErrorTypes _NclIIsFunc(void);
extern NhlErrorTypes _NclIExit(void);
extern NhlErrorTypes _NclIStatusExit(void);

extern NhlErrorTypes _NclIIsNumeric(void);
extern NhlErrorTypes _NclIIsENumeric(void);
extern NhlErrorTypes _NclIIsSNumeric(void);
extern NhlErrorTypes _NclIIsDouble(void);
extern NhlErrorTypes _NclIIsFloat(void);
extern NhlErrorTypes _NclIIsGraphic(void);
extern NhlErrorTypes _NclIIsFile(void);
extern NhlErrorTypes _NclIIsString(void);
extern NhlErrorTypes _NclIIsChar(void);
extern NhlErrorTypes _NclIIsByte(void);
extern NhlErrorTypes _NclIIsLong(void);
extern NhlErrorTypes _NclIIsUlong(void);
extern NhlErrorTypes _NclIIsInt64(void);
extern NhlErrorTypes _NclIIsUint64(void);
extern NhlErrorTypes _NclIIsUbyte(void);
extern NhlErrorTypes _NclIIsShort(void);
extern NhlErrorTypes _NclIIsUshort(void);
extern NhlErrorTypes _NclIIsInteger(void);
extern NhlErrorTypes _NclIIsUint(void);
extern NhlErrorTypes _NclIIsLogical(void);
extern NhlErrorTypes _NclIIsList(void);
extern NhlErrorTypes _NclIIsUnsigned(void);
extern NhlErrorTypes _NclIisbigendian(void);
extern NhlErrorTypes _NclIFileVarTypeOf(void);
extern NhlErrorTypes _NclITypeOf(void);
extern NhlErrorTypes _NclIUnDef(void);
extern NhlErrorTypes _NclINhlIsApp(void);
extern NhlErrorTypes _NclINhlIsDataComm(void);
extern NhlErrorTypes _NclINhlIsDataItem(void);
extern NhlErrorTypes _NclINhlIsDataSpec(void);
extern NhlErrorTypes _NclINhlIsTransform(void);
extern NhlErrorTypes _NclINhlIsView(void);
extern NhlErrorTypes _NclINhlIsWorkstation(void);

extern NhlErrorTypes _NclINhlGetWorkspaceObjectId(void);

extern NhlErrorTypes _NclINhlGetClassResources(void);

extern NhlErrorTypes _NclINhlAppGetDefaultParentId(void);

extern NhlErrorTypes _NclIGetFileVarChunkDimsizes(void);

extern NhlErrorTypes _NclIFileVarDimsizes(void);
extern NhlErrorTypes _NclIGetFileVarDims(void);

extern NhlErrorTypes _NclIGetFileVarAtts(void);

extern NhlErrorTypes _NclIGetFileCompoundVarComponentNames(void);

extern NhlErrorTypes _NclIGetVarDims(void);
extern NhlErrorTypes _NclIGetVarAtts(void);

extern NhlErrorTypes _NclIAttSetValues(void);

extern NhlErrorTypes _NclIPop(void);
extern NhlErrorTypes _NclIPush(void);
extern NhlErrorTypes _NclIAppend(void);
extern NhlErrorTypes _NclINewList(void);
extern NhlErrorTypes _NclIListGetType(void);
extern NhlErrorTypes _NclIListSetType(void);
extern NhlErrorTypes _NclIListCount(void);
extern NhlErrorTypes _NclIListIndex(void);
extern NhlErrorTypes _NclIListIndexFromName(void);
extern NhlErrorTypes _NclIListVarNameFromIndex(void);

extern NhlErrorTypes _NclINhlGetErrorObjectId(void);

extern NhlErrorTypes _NclINhlGetIsoLines(void);

#ifdef BuildVDC
extern NhlErrorTypes _VDC_IFileCoordDef(void);
#endif

NhlErrorTypes _NclIGetScriptName(void);

NhlErrorTypes _NclIGetScriptPrefixName(void);

NhlErrorTypes _Ncldefault_fillvalue(void);

NhlErrorTypes _Nclset_default_fillvalue(void);

NhlErrorTypes _Nclget_cpu_time(void);

NhlErrorTypes _Nclget_wall_time(void);

NhlErrorTypes _NclCreateGraphic(void);

NhlErrorTypes _NclIgetfilepath(void);

void _NclAddBuiltIns(void)
{
	void *args;
	ng_size_t dimsizes[NCL_MAX_DIMENSIONS];
	int nargs = 0;

	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
	NclRegisterProc(_NclILoadScript,args,"loadscript",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclsinh,args,"sinh",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclcosh,args,"cosh",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Ncltanh,args,"tanh",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclsin,args,"sin",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclcos,args,"cos",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Ncltan,args,"tan",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclasin,args,"asin",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclacos,args,"acos",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclatan,args,"atan",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	SetArgTemplate(args,1,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclatan2,args,"atan2",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclceil,args,"ceil",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclfloor,args,"floor",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclfabs,args,"fabs",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Ncllog,args,"log",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclexp,args,"exp",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Ncllog10,args,"log10",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"snumeric",0,NclANY);nargs++;
	NclRegisterFunc(_Nclsqrt,args,"sqrt",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"string",1,dimsizes);nargs++;
	NclRegisterProc(_Nclsystem,args,"system",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_Nclsystemfunc,args,"systemfunc",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args, nargs, "string", 0, NclANY); nargs++;
	NclRegisterFunc(_Nclstrlen, args, "strlen", nargs);
	nargs = 0;

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"float",1,NclANY);nargs++;
	SetArgTemplate(args,1,"float",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	dimsizes[0] = 2;
	SetArgTemplate(args,3,"numeric",1,dimsizes);nargs++;
	NclRegisterFunc(_Nclidsfft,args,"idsfft",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,dimsizes);nargs++;
	NclRegisterProc(_NclIChangeWorkstation,args,"NhlChangeWorkstation",nargs);
	nargs = 0;
	args = NewArgs(5);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	SetArgTemplate(args,4,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclISetColor,args,"NhlSetColor",nargs);
	
	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"float",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterFunc(_NclINewColor,args,"NhlNewColor",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	NclRegisterProc(_NclIFreeColor,args,"NhlFreeColor",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	NclRegisterFunc(_NclIIsAllocatedColor,args,"NhlIsAllocatedColor",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	NclRegisterFunc(_NclIGetBB,args,"NhlGetBB",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	SetArgTemplate(args,2,"graphic",1,NclANY);nargs++;
	NclRegisterFunc(_NclIAddData,args,"NhlAddData",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	SetArgTemplate(args,2,"graphic",1,NclANY);nargs++;
	NclRegisterProc(_NclIRemoveData,args,"NhlRemoveData",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"logical",1,dimsizes);nargs++;
	NclRegisterProc(_NclIRemoveOverlay,args,"NhlRemoveOverlay",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	NclRegisterFunc(_NclIAddAnnotation,args,"NhlAddAnnotation",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	NclRegisterProc(_NclIRemoveAnnotation,args,"NhlRemoveAnnotation",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	NclRegisterProc(_NclIUpdateData,args,"NhlUpdateData",nargs);

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclIDataPolyline,args,"NhlDataPolyline",nargs);

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclIDataPolygon,args,"NhlDataPolygon",nargs);

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclIDataPolymarker,args,"NhlDataPolymarker",nargs);

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclINDCPolyline,args,"NhlNDCPolyline",nargs);

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclINDCPolygon,args,"NhlNDCPolygon",nargs);
	
	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"float",1,NclANY);nargs++;
	SetArgTemplate(args,3,"float",1,NclANY);nargs++;
	NclRegisterProc(_NclINDCPolymarker,args,"NhlNDCPolymarker",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	NclRegisterFunc(_NclIClassName,args,"NhlClassName",nargs);
	
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	NclRegisterFunc(_NclIName,args,"NhlName",nargs);

	nargs = 0;
	NclRegisterFunc(_NclINhlPalGetDefined,NULL,"NhlPalGetDefined",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,2,"graphic",1,dimsizes);nargs++;
	NclRegisterProc(_NclIAddPrimitive,args,"NhlAddPrimitive",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"graphic",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"graphic",1,NclANY);nargs++;
	NclRegisterProc(_NclIRemovePrimitive,args,"NhlRemovePrimitive",nargs);

	nargs = 0;
	args = NewArgs(3);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	NclRegisterProc(_NclISetDashPattern,args,"NhlSetDashPattern",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	NclRegisterFunc(_NclINewDashPattern,args,"NhlNewDashPattern",nargs);

	nargs = 0;
	args = NewArgs(9);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"integer",1,NclANY);nargs++;
	SetArgTemplate(args,4,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,5,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,6,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,7,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,8,"numeric",1,NclANY);nargs++;
	NclRegisterProc(_NclISetMarker,args,"NhlSetMarker",nargs);

	nargs = 0;
	args = NewArgs(8);
	SetArgTemplate(args,0,"graphic",1,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"integer",1,NclANY);nargs++;
	SetArgTemplate(args,3,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,4,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,5,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,6,"numeric",1,NclANY);nargs++;
	SetArgTemplate(args,7,"numeric",1,NclANY);nargs++;
	NclRegisterFunc(_NclINewMarker,args,"NhlNewMarker",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttobyte,args,"integertobyte",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttobyte,args,"inttobyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttochar,args,"integertocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttochar,args,"inttocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttochar,args,"integertochar",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttochar,args,"inttochar",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttoshort,args,"integertoshort",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"integer",0,NclANY);nargs++;
	NclRegisterFunc( _NclIinttoshort,args,"inttoshort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"short",0,NclANY);nargs++;
	NclRegisterFunc( _NclIshorttobyte,args,"shorttobyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"short",0,NclANY);nargs++;
	NclRegisterFunc( _NclIshorttochar,args,"shorttocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"short",0,NclANY);nargs++;
	NclRegisterFunc( _NclIshorttochar,args,"shorttochar",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"long",0,NclANY);nargs++;
	NclRegisterFunc( _NclIlongtobyte,args,"longtobyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"long",0,NclANY);nargs++;
	NclRegisterFunc( _NclIlongtochar,args,"longtocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"long",0,NclANY);nargs++;
	NclRegisterFunc( _NclIlongtochar,args,"longtochar",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"long",0,NclANY);nargs++;
	NclRegisterFunc(_NclIlongtoshort,args,"longtoshort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"long",0,NclANY);nargs++;
	NclRegisterFunc(_NclIlongtoint,args,"longtointeger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"long",0,NclANY);nargs++;
	NclRegisterFunc(_NclIlongtoint,args,"longtoint",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItochar,args,"tochar",nargs);
        NclRegisterFunc(_NclItochar,args,"tocharacter",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItobyte,args,"tobyte",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItoshort,args,"toshort",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItoushort,args,"toushort",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItoint,args,"tointeger",nargs);
        NclRegisterFunc(_NclItoint,args,"toint",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
	NclRegisterFunc(_NclItosigned,args,"tosigned",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
	NclRegisterFunc(_NclItounsigned,args,"tounsigned",nargs);



        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItouint,args,"touinteger",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItouint,args,"touint",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItolong,args,"tolong",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItoulong,args,"toulong",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItoubyte,args,"toubyte",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItoint64,args,"toint64",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItouint64,args,"touint64",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItofloat,args,"tofloat",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItodouble,args,"todouble",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
        NclRegisterFunc(_NclItostring,args,"tostring",nargs);

        nargs = 0;
        args = NewArgs(2);
        SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs, "string", 1, dimsizes);  nargs++;
        NclRegisterFunc(_NclItostring_with_format,args,"tostring_with_format",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattobyte,args,"floattobyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattochar,args,"floattocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattochar,args,"floattochar",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattoshort,args,"floattoshort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattoint,args,"floattointeger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattoint,args,"floattoint",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"float",0,NclANY);nargs++;
	NclRegisterFunc( _NclIfloattolong,args,"floattolong",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletobyte,args,"doubletobyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletochar,args,"doubletocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletochar,args,"doubletochar",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletoshort,args,"doubletoshort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletoint,args,"doubletointeger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletoint,args,"doubletoint",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletolong,args,"doubletolong",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"double",0,NclANY);nargs++;
	NclRegisterFunc( _NclIdoubletofloat,args,"doubletofloat",nargs);


	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartodouble,args,"charactertodouble",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartodouble,args,"chartodouble",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartofloat,args,"charactertofloat",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartofloat,args,"chartofloat",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartolong,args,"charactertolong",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartolong,args,"chartolong",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartoshort,args,"charactertoshort",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartoshort,args,"chartoshort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartoint,args,"charactertointeger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartoint,args,"charactertoint",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartoint,args,"chartointeger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartoint,args,"chartoint",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartostring,args,"charactertostring",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"character",0,NclANY);nargs++;
	NclRegisterFunc( _NclIchartostring,args,"chartostring",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtoshort,args,"stringtoshort",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
        NclRegisterFunc( _NclIstringtoushort,args,"stringtoushort",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtodouble,args,"stringtodouble",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtolong,args,"stringtolong",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
        NclRegisterFunc( _NclIstringtoulong,args,"stringtoulong",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
        NclRegisterFunc( _NclIstringtoint64,args,"stringtoint64",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
        NclRegisterFunc( _NclIstringtouint64,args,"stringtouint64",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtointeger,args,"stringtoint",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
        NclRegisterFunc( _NclIstringtouint,args,"stringtouint",nargs);
        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
        NclRegisterFunc( _NclIstringtouint,args,"stringtouinteger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtointeger,args,"stringtointeger",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtofloat,args,"stringtofloat",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtochar,args,"stringtocharacter",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY);nargs++;
	NclRegisterFunc( _NclIstringtochar,args,"stringtochar",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsVar,args,"isvar",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"file",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFileVar,args,"isfilevar",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsCoord,args,"iscoord",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsAtt,args,"isatt",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args, nargs, "file", 1, dimsizes);  nargs++;
    SetArgTemplate(args, nargs, "string", 1, dimsizes);  nargs++;
    NclRegisterFunc( _NclIVarIsUnlimited, args, "isunlimited", nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"file",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFileVarDim,args,"isfilevardim",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"file",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFileVarCoord,args,"isfilevarcoord",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"file",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFileVarAtt,args,"isfilevaratt",nargs);
	
	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsDim,args,"isdim",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,"integer",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsDimNamed,args,"isdimnamed",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	NclRegisterFunc( _Nclproduct,args,"product",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_product,args,"dim_product",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"integer",1,NclANY); nargs++;
	NclRegisterFunc(_Ncldim_product_n,args,"dim_product_n",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_sum,args,"dim_sum",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"integer",1,NclANY); nargs++;
	NclRegisterFunc(_Ncldim_sum_n,args,"dim_sum_n",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	NclRegisterFunc( _Nclsum,args,"sum",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	SetArgTemplate(args,nargs, "integer", 1, dimsizes);  nargs++;
	SetArgTemplate(args,nargs, "integer", 1, dimsizes);  nargs++;
	NclRegisterFunc( _Ncldim_cumsum_n,args,"dim_cumsum_n",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	SetArgTemplate(args,nargs, "integer", 1, dimsizes);  nargs++;
	NclRegisterFunc( _Ncldim_cumsum,args,"dim_cumsum",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	SetArgTemplate(args,nargs, "integer", 1, dimsizes);  nargs++;
	NclRegisterFunc( _Nclcumsum,args,"cumsum",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	NclRegisterFunc( _Nclavg,args,"avg",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_avg,args,"dim_avg",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,"numeric",0,NclANY); nargs++;
	SetArgTemplate(args,nargs,NclANY,1,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_avg_n,args,"dim_avg_n",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _Nclndto1d,args,"ndtooned",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"logical",0,NclANY); nargs++;
	NclRegisterFunc( _Nclnum,args,"num",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"logical",1,NclANY); nargs++;
	NclRegisterFunc( _Nclind,args,"ind",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,"numeric",1,NclANY); nargs++;
	NclRegisterFunc( _Ncl1dtond,args,"onedtond",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs, "snumeric", 1, dimsizes);  nargs++;
	SetArgTemplate(args,nargs, "snumeric", 1, dimsizes);  nargs++;
/*	SetArgTemplate(args,nargs, "integer", 1, dimsizes);  nargs++;*/
	SetArgTemplate(args,nargs, "snumeric", 1, dimsizes);  nargs++;
	NclRegisterFunc( _Nclfspan, args, "fspan", nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
/*
	SetArgTemplate(args,nargs,"integer",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"integer",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"integer",1,dimsizes); nargs++;
*/
	SetArgTemplate(args,nargs,"snumeric",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"snumeric",1,dimsizes); nargs++;
	SetArgTemplate(args,nargs,"snumeric",1,dimsizes); nargs++;
	NclRegisterFunc( _Nclispan,args,"ispan",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,NclANY,1,dimsizes); nargs++;
	NclRegisterFunc( _Nclmask,args,"mask",nargs);

	nargs = 0;
	args = NewArgs(3);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"logical",0,NclANY); nargs++;
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _Nclwhere,args,"where",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",1,NclANY); nargs++;
	NclRegisterFunc( _Nclmaxind,args,"maxind",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",1,NclANY); nargs++;
	NclRegisterFunc( _Nclminind,args,"minind",nargs);
	nargs = 0;

	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY); nargs++;
	NclRegisterFunc( _Nclmax,args,"max",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY); nargs++;
	NclRegisterFunc( _Nclmin,args,"min",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_max,args,"dim_max",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY); nargs++;
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"integer",1,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_max_n,args,"dim_max_n",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_min,args,"dim_min",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,nargs,"snumeric",0,NclANY); nargs++;
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"integer",1,NclANY); nargs++;
	NclRegisterFunc( _Ncldim_min_n,args,"dim_min_n",nargs);

	nargs = 0;
	NclRegisterProc( _NclIExit,args,"exit",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"integer",1,dimsizes); nargs++;
	NclRegisterProc( _NclIStatusExit,args,"status_exit",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFunc,args,"isfunc",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsProc,args,"isproc",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsDefined,args,"isdefined",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsUnsigned,args,"isunsigned",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsInteger,args,"isinteger",nargs);
	NclRegisterFunc( _NclIIsInteger,args,"isint",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
        NclRegisterFunc( _NclIIsUint,args,"isuint",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
        NclRegisterFunc( _NclIIsUint,args,"isuinteger",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsByte,args,"isbyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsShort,args,"isshort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsUshort,args,"isushort",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsLong,args,"islong",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsUlong,args,"isulong",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
        NclRegisterFunc( _NclIIsInt64,args,"isint64",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
        NclRegisterFunc( _NclIIsUint64,args,"isuint64",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
        NclRegisterFunc( _NclIIsList,args,"islist",nargs);

        nargs = 0;
        args = NewArgs(1);
        SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
        NclRegisterFunc( _NclIIsUbyte,args,"isubyte",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFloat,args,"isfloat",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsDouble,args,"isdouble",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsString,args,"isstring",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsChar,args,"ischar",nargs);
	NclRegisterFunc( _NclIIsChar,args,"ischaracter",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsNumeric,args,"isnumeric",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsENumeric,args,"isenumeric",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsSNumeric,args,"issnumeric",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsFile,args,"isfile",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsGraphic,args,"isgraphic",nargs);
	
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclIIsLogical,args,"islogical",nargs);

	NclRegisterFunc( _NclIisbigendian,NULL,"isbigendian",0);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
	NclRegisterFunc( _NclITypeOf,args,"typeof",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,nargs,"file",0,NclANY); nargs++;
	SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
	NclRegisterFunc( _NclIFileVarTypeOf,args,"filevartypeof",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
	NclRegisterProc( _NclIUnDef,args,"undef",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsApp,args,"NhlIsApp",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsDataComm,args,"NhlIsDataComm",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsDataItem,args,"NhlIsDataItem",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsDataSpec,args,"NhlIsDataSpec",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsTransform,args,"NhlIsTransform",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsView,args,"NhlIsView",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"graphic",0,NclANY);nargs++;
	NclRegisterFunc(_NclINhlIsWorkstation,args,"NhlIsWorkstation",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"numeric",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIgaus,args,"gaus",nargs);
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"numeric",0,NclANY);nargs++;
	NclRegisterFunc(_NclIvariance,args,"variance",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"numeric",0,NclANY);nargs++;
	NclRegisterFunc(_NclIdim_variance,args,"dim_variance",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"numeric",0,NclANY);nargs++;
        dimsizes[0] = 1; 
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	NclRegisterFunc(_NclIdim_variance_n,args,"dim_variance_n",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"numeric",0,NclANY);nargs++;
	NclRegisterFunc(_NclIstddev,args,"stddev",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"numeric",0,NclANY);nargs++;
	NclRegisterFunc(_NclIdim_stddev,args,"dim_stddev",nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"numeric",0,NclANY);nargs++;
        dimsizes[0] = 1;
	SetArgTemplate(args,1,"integer",1,NclANY);nargs++;
	NclRegisterFunc(_NclIdim_stddev_n,args,"dim_stddev_n",nargs);

	nargs = 0;
	NclRegisterFunc(_NclINhlGetWorkspaceObjectId,args,"NhlGetWorkspaceObjectId",nargs);
	nargs = 0;
	NclRegisterFunc(_NclINhlAppGetDefaultParentId,args,"NhlAppGetDefaultParentId",nargs);

    nargs = 0;
    NclRegisterFunc(_NclINhlGetErrorObjectId, args, "NhlGetErrorObjectId", nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"string",0,NclANY);nargs++;
        dimsizes[0] = 1;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	NclRegisterFunc(_NclINhlGetClassResources,args,"NhlGetClassResources",nargs);

	nargs = 0;
	args = NewArgs(4);
	SetArgTemplate(args,0,"string",0,NclANY);nargs++;
        dimsizes[0] = 1;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	SetArgTemplate(args,2,NclANY,1,dimsizes);nargs++;
	SetArgTemplate(args,3,"logical",1,dimsizes);nargs++;
	NclRegisterFunc(_NclCreateGraphic,args,"create_graphic",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,NclANY,0,NclANY);nargs++;
	NclRegisterFunc(_NclIGetVarDims,args,"getvardims",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,NclANY,0,NclANY);nargs++;
	NclRegisterFunc(_NclIGetVarAtts,args,"getvaratts",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIGetFileVarAtts,args,"getfilevaratts",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIGetFileCompoundVarComponentNames,args,
			"getfilecompoundvarcomponentnames",nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args, nargs, "file", 0, NclANY);  nargs++;
	NclRegisterFunc(_NclIGetFileChunkSizes, args, "getfilechunksizes", nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args, nargs, "file", 0, NclANY);  nargs++;
	NclRegisterFunc(_NclIGetFileCompressionLevel, args, "getfilecompressionlevel", nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args, nargs, "file", 0, NclANY);  nargs++;
	NclRegisterFunc(_NclIGetFileVersion, args, "get_file_version", nargs);

	nargs = 0;
	args = NewArgs(1);
	dimsizes[0] = 1;
	SetArgTemplate(args, nargs, "file", 0, NclANY);  nargs++;
	NclRegisterFunc(_NclIGetFileDimsizes, args, "getfiledimsizes", nargs);

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args, nargs, "file", 0, NclANY);nargs++;
	SetArgTemplate(args, nargs, "string", 0, NclANY);nargs++;
	NclRegisterFunc(_NclIGetFileVarTypes, args, "getfilevartypes", nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIGetFileVarDims,args,"getfilevardims",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIGetFileVarChunkDimsizes,args,"getfilevarchunkdimsizes",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIFileVarDimsizes,args,"filevardimsizes",nargs);
	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",1,dimsizes);nargs++;
	SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
	NclRegisterFunc(_NclIFileVarDimsizes,args,"getfilevardimsizes",nargs);

	nargs = 0;
	args = NewArgs(4);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"snumeric",1,NclANY);nargs++;
	SetArgTemplate(args,3,"logical",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileDimDef,args,"filedimdef",nargs);

	nargs = 0;
	args = NewArgs(2);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileGrpDef,args,"filegrpdef",nargs);

	nargs = 0;
	args = NewArgs(5);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"string",1,NclANY);nargs++;
	SetArgTemplate(args,4,"string",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileVlenDef,args,"filevlendef",nargs);

	nargs = 0;
	args = NewArgs(6);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"string",1,NclANY);nargs++;
	SetArgTemplate(args,4,"string",1,NclANY);nargs++;
	SetArgTemplate(args,5,"snumeric",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileEnumDef,args,"fileenumdef",nargs);

	nargs = 0;
	args = NewArgs(7);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"string",1,NclANY);nargs++;
	SetArgTemplate(args,4,"string",1,NclANY);nargs++;
	SetArgTemplate(args,5,"string",1,NclANY);nargs++;
	SetArgTemplate(args,6,"integer",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileCompoundDef,args,"filecompounddef",nargs);

	nargs = 0;
	args = NewArgs(5);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"string",1,NclANY);nargs++;
	SetArgTemplate(args,4,"list",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileWriteCompound,args,"filewritecompound",nargs);

	nargs = 0;
	args = NewArgs(5);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"snumeric",1,NclANY);nargs++;
	SetArgTemplate(args,4,"string",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileOpaqueDef,args,"fileopaquedef",nargs);

	nargs = 0;
	args = NewArgs(4);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"string",1,NclANY);nargs++;
	SetArgTemplate(args,3,"string",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileVarDef,args,"filevardef",nargs);

#ifdef BuildVDC
extern NhlErrorTypes _VDC_FileCoordDef(void);
	nargs = 0;
	args = NewArgs(7);
	dimsizes[0] = 1;
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;   // file pointer
	SetArgTemplate(args,1,"string",1,NclANY);nargs++; // names
	SetArgTemplate(args,2,"string",1,NclANY);nargs++; // dimensions
	SetArgTemplate(args,3,"string",1,NclANY);nargs++; // time dim name
	SetArgTemplate(args,4,"string",1,NclANY);nargs++; // units
	SetArgTemplate(args,5,"string",1,NclANY);nargs++; // axis
	SetArgTemplate(args,6,"string",1,NclANY);nargs++; // type
	NclRegisterProc(_VDC_IFileCoordDef,args,"vdc_filecoorddef",nargs);
#endif

/*Begin: Wei added for Variable Chunking, April 8, 2010*/
	nargs = 0;
	args = NewArgs(3);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"integer",0,NclANY);nargs++;
	NclRegisterProc(_NclIFileVarChunkDef,args,"filevarchunkdef",nargs);

      /*Wei added for Variable Compression, April 12, 2010*/
	nargs = 0;
	args = NewArgs(3);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,"integer",1,NclANY);nargs++;
	NclRegisterProc(_NclIFileVarCompressLevelDef,args,"filevarcompressleveldef",nargs);
	NclRegisterProc(_NclIFileVarCompressLevelDef,args,"filevardeflateleveldef",nargs);

      /*Wei added for File Compression, April 14, 2010*/
        nargs = 0;
        args = NewArgs(4);
        dimsizes[0] = 1;
        SetArgTemplate(args,0,"file",0,NclANY);nargs++;
        SetArgTemplate(args,1,"string",1,NclANY);nargs++;
        SetArgTemplate(args,2,"numeric",1,NclANY);nargs++;
        SetArgTemplate(args,3,"logical",1,NclANY);nargs++;
        NclRegisterProc(_NclIFileChunkDimDef,args,"filechunkdimdef",nargs);

      /*Wei added for File Compression, April 15, 2010*/
        nargs = 0;
        args = NewArgs(5);
        dimsizes[0] = 1;
        SetArgTemplate(args,0,"file",0,NclANY);nargs++;
        SetArgTemplate(args,1,"string",1,dimsizes);nargs++;
        SetArgTemplate(args,2,"integer",1,dimsizes);nargs++;
        SetArgTemplate(args,3,"integer",1,dimsizes);nargs++;
        SetArgTemplate(args,4,"float",1,dimsizes);nargs++;
        NclRegisterProc(_NclIFileVarChunkCacheDef,args,"filevarchunkcachedef",nargs);
/*End: Wei added*/

	nargs = 0;
	args = NewArgs(2);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,NclANY,0,NclANY);nargs++;
	NclRegisterProc(_NclIFileAttDef,args,"fileattdef",nargs);

	nargs = 0;
	args = NewArgs(3);
	SetArgTemplate(args,0,"file",0,NclANY);nargs++;
	SetArgTemplate(args,1,"string",1,NclANY);nargs++;
	SetArgTemplate(args,2,NclANY,0,NclANY);nargs++;
	NclRegisterProc(_NclIFileVarAttDef,args,"filevarattdef",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args, nargs, "string", 1, dimsizes);  nargs++;
    SetArgTemplate(args, nargs, "snumeric", 0, NclANY);  nargs++;
    NclRegisterFunc(sprintf_W, args, "sprintf", nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"string",1,dimsizes);nargs++;
    SetArgTemplate(args,nargs,"snumeric",0,NclANY);nargs++;
    NclRegisterFunc(sprinti_W,args,"sprinti",nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"graphic",0,NclANY);nargs++;
    NclRegisterFunc(_NclINhlGetParentWorkstation,args,"NhlGetParentWorkstation",nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"graphic",0,NclANY);nargs++;
    NclRegisterFunc(_NclINhlGetParentId,args,"NhlGetParentId",nargs);

    nargs = 0;
    args = NewArgs(2);
    SetArgTemplate(args,nargs,"graphic",0,NclANY);nargs++;
    SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
    NclRegisterFunc(_NclINhlGetIsoLines,args,"NhlGetIsoLines",nargs);
    NclRegisterFunc(_NclINhlGetIsoLines,args,"get_isolines",nargs);

    nargs = 0;
    args = NewArgs(2);
    SetArgTemplate(args,nargs,"graphic",0,NclANY);nargs++;
    SetArgTemplate(args,nargs,"logical",1,dimsizes);nargs++;
    NclRegisterProc(_NclIAttSetValues,args,"attsetvalues",nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"list",0,NclANY);nargs++;
    NclRegisterFunc(_NclIPop,args,"ListPop",nargs);

    nargs = 0;
    args = NewArgs(2);
    SetArgTemplate(args,nargs,"list",0,NclANY);nargs++;
    SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
    NclRegisterProc(_NclIPush,args,"ListPush",nargs);

    nargs = 0;
    args = NewArgs(2);
    SetArgTemplate(args,nargs,"list",0,NclANY);nargs++;
    SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
    NclRegisterProc(_NclIAppend,args,"ListAppend",nargs);

    nargs = 0;
    args = NewArgs(1);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"list",1,dimsizes);nargs++;
    NclRegisterFunc(_NclIListGetType,args,"ListGetType",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"list",1,dimsizes);nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes);nargs++;
    NclRegisterProc(_NclIListSetType,args,"ListSetType",nargs);

    nargs = 0;
    args = NewArgs(1);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"list",1,dimsizes);nargs++;
    NclRegisterFunc(_NclIListCount,args,"ListCount",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"list",1,dimsizes);nargs++;
    SetArgTemplate(args,nargs,NclANY,1,NclANY);nargs++;
    NclRegisterFunc(_NclIListIndex,args,"ListIndex",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"list",1,dimsizes);nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes);nargs++;
    NclRegisterFunc(_NclIListIndexFromName,args,"ListIndexFromName",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"list",1,dimsizes);nargs++;
    SetArgTemplate(args,nargs,"integer",1,dimsizes);nargs++;
    NclRegisterFunc(_NclIListVarNameFromIndex,args,"ListVarNameFromIndex",nargs);

    nargs = 0;
    args = NewArgs(1);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"string",1,dimsizes);nargs++;
    NclRegisterFunc(_NclINewList,args,"NewList",nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,NclANY,0,NclANY);nargs++;
    NclRegisterProc(_NclIprintVarSummary,args,"printVarSummary",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"file",0,NclANY); nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    NclRegisterProc(_NclIprintFileVarSummary,args,"printFileVarSummary",nargs);

    nargs = 0;
    args = NewArgs(3);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"file",0,NclANY); nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,"integer",1,dimsizes); nargs++;
    NclRegisterFunc(_NclIGetFileGroups,args,"getfilegroups",nargs);


    nargs = 0;
    args = NewArgs(3);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"file",0,NclANY); nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,"integer",1,dimsizes); nargs++;
    NclRegisterFunc(_NclIGetGroupVars,args,"getgroupvars",nargs);


    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    NclRegisterFunc(_NclIAddFiles,args,"addfiles",nargs);



    nargs = 0;
    args = NewArgs(5);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,"string",1,NclANY); nargs++;
    SetArgTemplate(args,nargs,"numeric",1,NclANY); nargs++;
    SetArgTemplate(args,nargs,"list",1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,NclANY,0,NclANY); nargs++;
    NclRegisterFunc(_NclICreateFile,args,"createfile",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"string",0,NclANY); nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    NclRegisterFunc(_NclIAddFiles,args,"addfilelist",nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args, nargs, "string", 0, NclANY);  nargs++;
    NclRegisterFunc( _NclIFileIsPresent, args, "isfilepresent", nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args, nargs, "string", 0, NclANY);  nargs++;
    NclRegisterFunc( _NclIFileExists, args, "fileexists", nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"short",0,NclANY); nargs++;
    NclRegisterFunc(_NclIshorttoint,args,"shorttoint",nargs);
    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"short",0,NclANY); nargs++;
    NclRegisterFunc(_NclIshorttoint,args,"shorttointeger",nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"ushort",0,NclANY); nargs++;
    NclRegisterFunc(_NclIushorttoint,args,"ushorttoint",nargs);

    nargs = 0;
    args = NewArgs(3);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,NclANY,1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,NclANY,1,NclANY); nargs++;
    NclRegisterProc(_NclISetFileOption,args,"setfileoption",nargs);
    
    nargs = 0;
    args = NewArgs(0);
    dimsizes[0] = 1;
    NclRegisterFunc(_NclIGetScriptName, args, "get_script_name", nargs);

    nargs = 0;
    args = NewArgs(0);
    dimsizes[0] = 1;
    NclRegisterFunc(_NclIGetScriptPrefixName, args, "get_script_prefix_name", nargs);

    nargs = 0;
    args = NewArgs(1);
    dimsizes[0] = 1;
    SetArgTemplate(args, nargs, "string", 1, dimsizes);  nargs++;
    NclRegisterFunc(_Ncldefault_fillvalue, args, "default_fillvalue", nargs);

    nargs = 0;
    args = NewArgs(1);
    SetArgTemplate(args,nargs,"file",0,NclANY); nargs++;
    NclRegisterFunc(_NclIgetfilepath,args,"getfilepath",nargs);

    nargs = 0;
    args = NewArgs(2);
    dimsizes[0] = 1;
    SetArgTemplate(args,nargs,"string",1,dimsizes); nargs++;
    SetArgTemplate(args,nargs,NclANY,1,dimsizes); nargs++;
    NclRegisterProc(_Nclset_default_fillvalue,args,"set_default_fillvalue",nargs);

	NclRegisterFunc(_Nclget_cpu_time,NULL,"get_cpu_time",0);
	NclRegisterFunc(_Nclget_wall_time,NULL,"get_wall_time",0);
/*
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"float",0,NclANY);nargs++;
	NclRegisterFunc(_Nclrint,args,"rint",nargs);
	
	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"float",0,NclANY);nargs++;
	NclRegisterFunc(_Nclasinh,args,"asinh",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"float",0,NclANY);nargs++;
	NclRegisterFunc(_Nclacosh,args,"acosh",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"float",0,NclANY);nargs++;
	NclRegisterFunc(_Nclatanh,args,"atanh",nargs);

	nargs = 0;
	args = NewArgs(1);
	SetArgTemplate(args,0,"float",0,NclANY);nargs++;
	NclRegisterFunc(_Ncltrunc,args,"trunc",nargs);

*/
	
	return;
}


#ifdef __cpluplus
}
#endif
