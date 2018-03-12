#ifdef NIO_LIB_ONLY
#include "niohlu.h"
#include "nioNresDB.h"
#else
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#endif
#include "defs.h"
#include "NclDataDefs.h"
#include "NclFileInterfaces.h"
#include "NclData.h"
#include "NclFile.h"
#include "DataSupport.h"
#include "NclBuiltInSupport.h"
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include <vapor/VDC_c.h>

#define VDC_MAX_DIMS 4

#define AC_BLACK     "\x1b[30m"
#define AC_RED       "\x1b[31m"
#define AC_GREEN     "\x1b[32m"
#define AC_YELLOW    "\x1b[33m"
#define AC_BLUE      "\x1b[34m"
#define AC_MAGENTA   "\x1b[35m"
#define AC_CYAN      "\x1b[36m"
#define AC_WHITE     "\x1b[37m"
#define AC_BLACK_B   "\x1b[90m"
#define AC_RED_B     "\x1b[91m"
#define AC_GREEN_B   "\x1b[92m"
#define AC_YELLOW_B  "\x1b[93m"
#define AC_BLUE_B    "\x1b[94m"
#define AC_MAGENTA_B "\x1b[95m"
#define AC_CYAN_B    "\x1b[96m"
#define AC_WHITE_B   "\x1b[97m"
#define AC_RESET     "\x1b[0m"

#define VDC_DEBUG
#ifdef VDC_DEBUG
#define VDC_DEBUG_printf(...) fprintf (stderr, __VA_ARGS__)
#define VDC_DEBUG_printff(...) { int isTty = isatty(fileno(stderr)); fprintf (stderr, "%s[%s:%i]%s%s", isTty?AC_BLACK_B:"", __FILE__, __LINE__, __func__, isTty?AC_RESET:""); fprintf (stderr, __VA_ARGS__); }
#include <signal.h>
#define VDC_DEBUG_raise(...) raise(__VA_ARGS__)
const char *qts(long q) { return NrmQuarkToString(q); }
long stq(const char *s) { return NrmStringToQuark(s); }
#else
#define VDC_DEBUG_printf(...)
#define VDC_DEBUG_printff(...)
#define VDC_DEBUG_raise(...)
#endif

#define VDC_DEBUG2
#ifdef VDC_DEBUG2
#define VDC_DEBUG2_printf(...) fprintf (stderr, __VA_ARGS__)
#define VDC_DEBUG2_printff(...) { fprintf (stderr, "[%s:%i]%s", __FILE__, __LINE__, __func__); fprintf (stderr, __VA_ARGS__); }
#include <signal.h>
#define VDC_DEBUG2_raise(...) raise(__VA_ARGS__)
#else
#define VDC_DEBUG2_printf(...)
#define VDC_DEBUG2_printff(...)
#define VDC_DEBUG2_raise(...)
#endif

static NhlErrorTypes VDCAddCoordVarCustom(void* therec, NclQuark thevar, NclBasicDataTypes data_type, int n_dims, NclQuark *dim_names, NclQuark timeDimName, NclQuark units, NclQuark axisStr);

typedef struct _VDCRecord VDCRecord;

typedef struct _AList { struct _AList *next; } AList;
int AList_size(void *list) { AList *l = (AList*)list; if (l->next) return AList_size(l->next) + 1; else return 1; }

typedef struct _DimList { struct _DimList *next; NclFDimRec rec; } DimList;
DimList *DimList_new() { DimList *p = (DimList*)NclMalloc(sizeof(DimList)); p->next = NULL; return p; }
void DimList_delete(DimList *p) { NclFree(p); }
void DimList_deleteList(DimList *p) { if (p->next) DimList_deleteList(p->next); DimList_delete(p); }
DimList *DimList_last(DimList *p) { if (p->next) return DimList_last(p->next); return p; }
void DimList_push(DimList **l, DimList *p) { if (*l) DimList_push(&(*l)->next, p); else *l = p; }
void DimList_pushFront(DimList **l, DimList *p) { if (*l) { DimList_push(&p, *l); *l = p; } else *l = p; }
DimList *DimList_index(DimList *p, int i) { if (i<0 || !p) { raise(SIGABRT); fprintf(stderr, "Error: Index %i out of bounds", i); exit(1); } if (i) return DimList_index(p->next, i-1); return p; }

typedef struct _VarList { struct _VarList *next; NclFVarRec rec; } VarList;
VarList *VarList_new() { VarList *p = (VarList*)NclMalloc(sizeof(VarList)); p->next = NULL; return p; }
void VarList_delete(VarList *p) { NclFree(p); }
void VarList_deleteList(VarList *p) { if (p->next) VarList_deleteList(p->next); VarList_delete(p); }

typedef struct _AttList { struct _AttList *next; NclFAttRec rec; } AttList;
AttList *AttList_new() { AttList *p = (AttList*)NclMalloc(sizeof(AttList)); p->next = NULL; return p; }
void AttList_delete(AttList *p) { NclFree(p); }
void AttList_deleteList(AttList *p) { if (p->next) AttList_deleteList(p->next); AttList_delete(p); }

struct _VDCRecord { NclQuark		file_path_q;
	int				wr_status;
	VDC*			dataSource;

	NclFVarRec      *variables;
	int				numVariables;
	int				numVariablesAndCoords;

	NclFAttRec		*globalAtts;
	long			*globalAttsValues;
	int				numAtts;

	DimList			*dims;
	int				dimsCount;

	int levelOfDetail;
	int compressionEnabled;

	int defineMode;
	int hasScalarDim;
	//long magic = 0xDB6C77A47778D171; // Temporary solution to check if VDC pointer
};

static NclBasicDataTypes _VDCXTypeToNCLDataType(VDC_XType xtype)
{
	switch (xtype) {
		case VDC_XType_TEXT:    return NCL_string;
		case VDC_XType_INT32:   return NCL_int;
		case VDC_XType_INT64:   return NCL_int64;
		case VDC_XType_FLOAT:   return NCL_float;
		case VDC_XType_DOUBLE:  return NCL_double;
		case VDC_XType_INVALID: return NCL_none;
		default:                return NCL_none;
	}
}

static VDC_XType _NCLDataTypeToVDCXType(NclBasicDataTypes NCLtype)
{
	switch (NCLtype) {
		case NCL_string: return VDC_XType_TEXT;
		case NCL_int:    return VDC_XType_INT32;
		case NCL_int64:  return VDC_XType_INT64;
		case NCL_long:   return VDC_XType_INT64;
		case NCL_float:  return VDC_XType_FLOAT;
		case NCL_double: return VDC_XType_DOUBLE;
		case NCL_none:   return VDC_XType_INVALID;
		default:         return VDC_XType_INVALID;
	}
}

static const char *_NCLDataTypeToString(NclBasicDataTypes NCLtype)
{
	switch (NCLtype) {
		case NCL_none:      return "none";
		case NCL_byte:      return "byte";
		case NCL_ubyte:     return "ubyte";
		case NCL_char:      return "char";
		case NCL_short:     return "short";
		case NCL_ushort:    return "ushort";
		case NCL_int:       return "int";
		case NCL_uint:      return "uint";
		case NCL_float:     return "float";
		case NCL_long:      return "long";
		case NCL_ulong:     return "ulong";
		case NCL_int64:     return "int64";
		case NCL_uint64:    return "uint64";
		case NCL_double:    return "double";
		case NCL_string:    return "string";
		case NCL_numeric:   return "numeric";
		case NCL_enumeric:  return "enumeric";
		case NCL_snumeric:  return "snumeric";
		case NCL_logical:   return "logical";
		case NCL_obj:       return "obj";
		case NCL_list:      return "list";
		case NCL_group:     return "group";
		case NCL_compound:  return "compound";
		case NCL_opaque:    return "opaque";
		case NCL_enum:      return "enum";
		case NCL_vlen:      return "vlen";
		case NCL_reference: return "reference";
		case NCL_virtual:   return "virtual";
		case NCL_listarray: return "listarray";
		default:            return "UNKNOWN DATA TYPE";
	}
}

static int _dimensionNameToId(VDCRecord *rec, const char *name)
{
	NclQuark name_q = NrmStringToQuark(name);
	NclQuark ncl_scalar_name = NrmStringToQuark("ncl_scalar");

	DimList *l = rec->dims;
	int i = 0;
	while (l) {
		if (l->rec.dim_name_quark == name_q)
			return i;
		if (l->rec.dim_name_quark != ncl_scalar_name)
			i++;
		l = l->next;
	}
#ifdef VDC_DEBUG
	printf("%s: dim \"%s\" not found!\n", __func__, name);
	exit(1);
#endif
	return -1;
}

#define AssertDefineMode(rec) if (_assertDefineMode(rec)) return NhlFATAL
static int _assertDefineMode(VDCRecord *rec)
{
	if (rec->defineMode == 0) {
		// VDC_ReDefine(rec->dataSource);
		// rec->defineMode = 1;
		VDC_DEBUG_printf("Error: must be in define mode\n");
		NhlPError(NhlFATAL, NhlEUNKNOWN, "must be in define mode");
		return -1;
	}
	return 0;
}

#define AssertWriteMode(rec) if (_assertWriteMode(rec)) return NhlFATAL
static int _assertWriteMode(VDCRecord *rec)
{
	if (rec->defineMode == 1) {
		printf("Calling end define\n");
		VDC_EndDefine(rec->dataSource);
		rec->defineMode = 0;
	}
	return 0;
}

static void *VDCInitializeFileRec (NclFileFormat *format)
{
    VDC_DEBUG_printff("()\n");
    VDCRecord*  rec = (VDCRecord*) NULL;
    
	rec = (VDCRecord*)NclCalloc(1, sizeof(VDCRecord));
	if (!rec) {
		NhlPError(NhlFATAL,ENOMEM,NULL);
		return NULL;
	}

	rec->variables = NULL;
	rec->numVariables = 0;
	rec->numVariablesAndCoords = 0;
	rec->globalAtts = NULL;
	rec->globalAttsValues = NULL;
	rec->numAtts = 0;

	rec->dims = NULL;
	rec->dimsCount = 0;

	rec->levelOfDetail = -1;
	rec->compressionEnabled = 1;

	rec->defineMode = 1;
	rec->hasScalarDim = 0;

	*format = _NclVDC;
	return (void *) rec;
}


static NclQuark _VDCFixExt (NclQuark path)
{
	const char *path_cstr = NrmQuarkToString(path);
	if (!strcmp(".vdc", path_cstr + strlen(path_cstr) - 4)) {
		char newPath[PATH_MAX];
		char *newPathExt = &newPath[strlen(path_cstr) - 4];
		strcpy(newPath, path_cstr);
		strcpy(newPathExt, ".nc");
		VDC_DEBUG_printff(": changed path from \"%s\" to \"%s\"\n", path_cstr, newPath);
		return NrmStringToQuark(newPath);
	}
	return path;
}


static void *VDCCreateFile (void *rec_, NclQuark path)
{
    VDC_DEBUG_printff("(\"%s\")\n", NrmQuarkToString(path));
	VDCRecord *rec = (VDCRecord*)rec_;
	path = _VDCFixExt(path);

	VDC *p = VDC_new();
    VDC_DEBUG_printff(": Initializing \"%s\"...\n", NrmQuarkToString(path));
	int success = VDC_InitializeDefaultBS(p, NrmQuarkToString(path), VDC_AccessMode_W);
    VDC_DEBUG_printf(" done\n");
    if (success < 0) {
		VDC_delete(p);
        NhlPError(NhlFATAL, NhlEUNKNOWN, "Failed to create VDC file \"%s\"", NrmQuarkToString(path));
        return NULL;
    }       
	rec->file_path_q = path;
	// rec->wr_status = // TODO VDC

	rec->dataSource = p;
    VDC_DEBUG_printff(" Done\n");
    return((void*) rec);
}

static void _defineGlobalAttributes (VDCRecord *rec)
{
    VDC_DEBUG_printff("()\n");
	VDC *p = rec->dataSource;

	char **names;
	int count;
	VDC_GetAttNames(p, "", &names, &count);

	// TODO VDC num_elements > 1

    rec->numAtts = count;
    rec->globalAtts = (NclFAttRec*)NclMalloc(sizeof(NclFAttRec) * rec->numAtts);
    rec->globalAttsValues = NclMalloc(sizeof(long) * rec->numAtts);
	for (int i = 0; i < count; i++) {
		rec->globalAtts[i].att_name_quark = NrmStringToQuark(names[i]);

		int type = VDC_GetAttType(p, "", names[i]);
		if (type == VDC_XType_TEXT) {
			rec->globalAtts[i].data_type = NCL_string;
			rec->globalAtts[i].num_elements = 1;

			char *val;
			VDC_GetAtt_text(p, "", names[i], &val);
			rec->globalAttsValues[i] =  NrmStringToQuark(val);
			VDC_FreeString(&val);
		}
		else if (type == VDC_XType_INT32 || type == VDC_XType_INT64) {
			rec->globalAtts[i].data_type = type == VDC_XType_INT32 ? NCL_int : NCL_int64;
			rec->globalAtts[i].num_elements = 1;

			long *vals;
			int count;
			VDC_GetAtt_long(p, "", names[i], &vals, &count);
			if (count != 1) fprintf(stderr, "Unsupported attribute length of %i\n", count); // TODO VDC
			rec->globalAttsValues[i] =  vals[0];
			VDC_FreeLongArray(&vals);
		}
		else if (type == VDC_XType_FLOAT || type == VDC_XType_DOUBLE) {
			rec->globalAtts[i].data_type = type == VDC_XType_FLOAT ? NCL_float : NCL_double;
			rec->globalAtts[i].num_elements = 1;

			double *vals;
			int count;
			VDC_GetAtt_double(p, "", names[i], &vals, &count);
			if (count != 1) fprintf(stderr, "Unsupported attribute length of %i\n", count); // TODO VDC
			((double*)rec->globalAttsValues)[i] = vals[0];
			VDC_FreeDoubleArray(&vals);
		} else {
			rec->globalAtts[i].data_type = NCL_string;
			rec->globalAtts[i].num_elements = 1;
			rec->globalAttsValues[i] =  NrmStringToQuark( "-------" );
		}
	}
	VDC_FreeStringArray(&names, &count);
}

static void _defineDimensions (VDCRecord *rec)
{
    VDC_DEBUG_printff("()\n");

	VDC *p = rec->dataSource;

	char **names;
	int count;
	VDC_GetDimensionNames(p, &names, &count);
	VDCDimension *dim = VDCDimension_new();

	DimList **d = &rec->dims;
	rec->dimsCount = count;

	for (int i = 0; i < count; i++) {
		VDC_GetDimension(p, names[i], dim);

		*d = DimList_new();
        (*d)->rec.dim_name_quark = NrmStringToQuark(names[i]);
        (*d)->rec.dim_size = VDCDimension_GetLength(dim);
        (*d)->rec.is_unlimited = 0;
		d = &(*d)->next;
	}

	VDCDimension_delete(dim);
	VDC_FreeStringArray(&names, &count);
}

static void _defineVariables (VDCRecord *rec)
{
    VDC_DEBUG_printff("()\n");

	VDC *p = rec->dataSource;
	assert(rec->dims); // Dimensions need to be defined first

	char **dataNames, **coordNames;
	int dataCount, coordCount;
	VDC_GetDataVarNames(p, &dataNames, &dataCount);
	VDC_GetCoordVarNames(p, &coordNames, &coordCount);
	VDCBaseVar *v = VDCBaseVar_new();

	rec->numVariables = dataCount;
	rec->numVariablesAndCoords = dataCount + coordCount;
    rec->variables = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec) * (rec->numVariablesAndCoords));

	for (int i = 0; i < rec->numVariablesAndCoords; i++) {
		const char *name = i < dataCount ? dataNames[i] : coordNames[i - dataCount];
		VDC_GetBaseVarInfo(p, name, v);

        rec->variables[i].var_name_quark = NrmStringToQuark(name);
        rec->variables[i].data_type = _VDCXTypeToNCLDataType(VDCBaseVar_GetXType(v));

		char **dims;
		int dimCount;
		VDC_GetVarDimNames(p, name, 0, &dims, &dimCount);

        rec->variables[i].num_dimensions = dimCount;
		assert(dimCount <= NCL_MAX_DIMENSIONS);

		for (int j = 0; j < dimCount; j++)
			rec->variables[i].file_dim_num[j] = _dimensionNameToId(rec, dims[dimCount - j - 1]);  /* MUST ALIGN WITH DIMENSION DEFS */

		VDC_FreeStringArray(&dims, &dimCount);
	}

	VDCBaseVar_delete(v);
	VDC_FreeStringArray(&dataNames, &dataCount);
	VDC_FreeStringArray(&coordNames, &coordCount);
}

static void *VDCOpenFile (void *therec, NclQuark path, int wr_status)
{
    VDC_DEBUG_printff("(\"%s\", %i)\n", NrmQuarkToString(path), wr_status);
	path = _VDCFixExt(path);

	VDCRecord *rec = (VDCRecord*) therec;
    //ng_size_t numGeometry = 0;

	if(rec == NULL) {
		VDC_DEBUG_printff(": rec is NULL\n");
		return(NULL);
	}

    rec->dataSource = VDC_new();
	int success = VDC_InitializeDefaultBS(rec->dataSource, NrmQuarkToString(path), wr_status == 1 ? VDC_AccessMode_R : VDC_AccessMode_A);

    if (success < 0) {
		VDC_delete(rec->dataSource);
        NhlPError(NhlFATAL, NhlEUNKNOWN, "Failed to open VDC file \"%s\"", NrmQuarkToString(path));
        return NULL;
    }       
	rec->file_path_q = path;
	rec->wr_status = wr_status;

	_defineGlobalAttributes(rec);
	_defineDimensions(rec);
    _defineVariables(rec);

	return((void*)rec);
}


static void VDCFreeFileRec (void* therec)
{
    VDC_DEBUG_printff("()\n");

	VDCRecord *rec = (VDCRecord*)therec;

    if (rec == NULL)
            return;

	// TODO VDC free sub variables and any file pointers
    if (rec->variables)  NclFree(rec->variables);
    if (rec->globalAtts) NclFree(rec->globalAtts);
    if (rec->globalAttsValues) NclFree(rec->globalAttsValues);
    if (rec->dims) DimList_deleteList(rec->dims);     

	if (rec->defineMode)
		VDC_EndDefine(rec->dataSource);
    VDC_DEBUG_printff(": deleting VDC pointer.\n");
    VDC_delete(rec->dataSource);

    NclFree(rec);
	rec = NULL;

    VDC_DEBUG_printff(": done.\n");
	return;
}


static NclQuark* VDCGetVarNames (void* therec, int *num_vars)
{
	VDC_DEBUG_printff("()\n");
	
	VDCRecord *rec = (VDCRecord*)therec;
	int i;
	
	NclQuark* names = (NclQuark*) NclMalloc(sizeof(NclQuark) * rec->numVariablesAndCoords);
	if (rec->numVariables > 0 && names == NULL) {
	        NhlPError(NhlFATAL,ENOMEM,NULL);
	        return NULL;
	}
	
	for (i=0; i<rec->numVariablesAndCoords; i++) {
	        names[i] = rec->variables[i].var_name_quark;
	}
	
	*num_vars = rec->numVariablesAndCoords;
    VDC_DEBUG_printff(": returning %i variables\n", *num_vars);
	return names;
}


static NclFVarRec *VDCGetVarInfo (void *therec, NclQuark var_name)
{
	VDC_DEBUG_printff("(%s)\n", NrmQuarkToString(var_name));	

	VDCRecord *rec = (VDCRecord*)therec;

	for (int i = 0; i < rec->numVariablesAndCoords; i++) {
		if (var_name == rec->variables[i].var_name_quark) {
			NclFVarRec *ret = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec));
			ret->var_name_quark = rec->variables[i].var_name_quark;
			ret->var_full_name_quark = rec->variables[i].var_name_quark;
			ret->var_real_name_quark = rec->variables[i].var_name_quark;
			ret->data_type = rec->variables[i].data_type;
			ret->num_dimensions = rec->variables[i].num_dimensions;

			if (ret->num_dimensions == 0) {
				ret->num_dimensions = 1;
				// ncl_scalar file_dim_num is required to be 0
				ret->file_dim_num[0] = 0;
				// On a side note, the NclNetCdf code implies that a variable can
				// be both a scalar and have multiple dimensions. This is not accounted for here.

				// Yes, this is required because of legacy code:
			} else if (rec->hasScalarDim) {
				for (int j = 0; j < ret->num_dimensions; j++) {
					printf("---- Adding 1\n");
					ret->file_dim_num[j] = rec->variables[i].file_dim_num[j] + 1;
				}
			} else {
				for (int j = 0; j < ret->num_dimensions; j++) {
					ret->file_dim_num[j] = rec->variables[i].file_dim_num[j];
				}
			}

			for (int j = 0; j < ret->num_dimensions; j++) {
				printf("[1] %s[%i] = dims[%i] = '%s'\n",
						NrmQuarkToString(ret->var_name_quark), j, ret->file_dim_num[j],
						NrmQuarkToString(
							DimList_index(rec->dims, ret->file_dim_num[j])->rec.dim_name_quark 
						)
					);
			}

			return ret;
		}
	}

	VDC_DEBUG_printff(": Variable \"%s\" not found.", NrmQuarkToString(var_name));
	return NULL;
}


static NclQuark *VDCGetDimNames (void* therec, int* num_dims)
{
    VDC_DEBUG_printff("()\n");

	VDCRecord *rec = (VDCRecord*)therec;

    NclQuark* names = (NclQuark*) NclMalloc(sizeof(NclQuark) * rec->dimsCount);
	if (rec->dimsCount > 0 && names == NULL) {
            NhlPError(NhlFATAL,ENOMEM,NULL);
            return NULL;
    }

    int i = 0;
	DimList *d = rec->dims;
	while (d) {
		names[i++] = d->rec.dim_name_quark;
		d = d->next;
	}

    *num_dims = rec->dimsCount;
    VDC_DEBUG_printff(": returning %i dimensions\n", *num_dims);
    return names;        
}


static NclFDimRec *VDCGetDimInfo (void* therec, NclQuark dim_name_q)
{
    VDC_DEBUG_printff("(%s)\n", NrmQuarkToString(dim_name_q));

	VDCRecord* rec = (VDCRecord*)therec;

	DimList *d = rec->dims;
	while (d) {
		if (dim_name_q == d->rec.dim_name_quark) {
            NclFDimRec *ret = (NclFDimRec*) NclMalloc(sizeof(NclFDimRec));
			*ret = d->rec;
			return ret;
		}
		d = d->next;
	}

    VDC_DEBUG_printff(": ERROR not found\n", NrmQuarkToString(dim_name_q));
	return NULL;
}


static NclQuark *VDCGetAttNames (void* therec, int *num_atts)
{	
    VDC_DEBUG_printff("()\n");

	VDCRecord* rec = (VDCRecord*)therec;
    int i;

    NclQuark* names = (NclQuark*) NclMalloc(sizeof(NclQuark) * rec->numAtts);
    if (rec->numAtts > 0 && names == NULL) {
        NhlPError(NhlFATAL,ENOMEM,NULL);
        return NULL;
    }

    for (i=0; i<rec->numAtts; i++) {
        names[i] = rec->globalAtts[i].att_name_quark;
    }

    *num_atts = rec->numAtts;

    VDC_DEBUG_printff(": returning %i attributes\n", *num_atts);
    return names;
}


static NclFAttRec* VDCGetAttInfo (void* therec, NclQuark att_name_q)
{
    VDC_DEBUG_printff("(\"%s\")\n", NrmQuarkToString(att_name_q));

	VDCRecord* rec = (VDCRecord*)therec;
    int i;

    for (i=0; i<rec->numAtts; i++) {
        if (att_name_q == rec->globalAtts[i].att_name_quark) {
            NclFAttRec* ret = (NclFAttRec*) NclMalloc(sizeof(NclFAttRec));
            *ret = rec->globalAtts[i];

            return ret;
        }
    }

    #ifdef VDC_DEBUG
    fprintf(stderr, "not found\n");
    #endif
	return NULL;
}


static NclQuark *VDCGetVarAttNames (void *therec , NclQuark thevar, int* num_atts)
{
    VDC_DEBUG_printff("(%s)\n", NrmQuarkToString(thevar));

	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
    *num_atts = 0;
	NclQuark *names_q = NULL;

	VDCBaseVar *v = VDCBaseVar_new();
	int success = VDC_GetBaseVarInfo(p, NrmQuarkToString(thevar), v);

	if (success) {
		char **names;
		int count, countWithCRatios;
		VDCBaseVar_GetAttributeNames(v, &names, &count);
		countWithCRatios = count + 1; // adding 'computed' compression ratios attribute

		names_q = (NclQuark*) NclMalloc(sizeof(NclQuark) * countWithCRatios);
		*num_atts = countWithCRatios;

		for (int i = 0; i < count; i++)
			names_q[i] = NrmStringToQuark(names[i]);

		names_q[count] = NrmStringToQuark("C_Ratios");

		VDC_FreeStringArray(&names, &count);
	}

	VDCBaseVar_delete(v);
	return names_q;
}


static NclFAttRec *VDCGetVarAttInfo (void *therec, NclQuark thevar, NclQuark theatt)
{
    // VDC_DEBUG_printff("(%s, %s)\n", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	NclFAttRec *att = NULL; // (NclFAttRec*)NclMalloc((unsigned) sizeof(NclFAttRec));

	if (theatt == NrmStringToQuark("C_Ratios")) {
		att = (NclFAttRec*)NclMalloc(sizeof(NclFAttRec));
		att->att_name_quark = theatt;
		att->data_type = NCL_int;
		att->num_elements = VDC_GetCRatiosCount(p, NrmQuarkToString(thevar));
		return att;
	}

	int xt = VDC_GetAttType(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt));
	if (xt >= 0) {
		att = (NclFAttRec*)NclMalloc(sizeof(NclFAttRec));
		att->att_name_quark = theatt;
		att->data_type = _VDCXTypeToNCLDataType(xt);
		int count;
		VDC_GetAtt_Count(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &count);
		att->num_elements = count;
	}
	#ifdef VDC_DEBUG
	else fprintf(stderr, "[%s:%i]%s Warning: '%s'->'%s' not found\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thevar), NrmQuarkToString(theatt));
	#endif

	return att;
}


static NclFVarRec *VDCGetCoordInfo (void* therec, NclQuark thevar)
{
    VDC_DEBUG_printff("(\"%s\")\n", NrmQuarkToString(thevar));
	return(VDCGetVarInfo(therec, thevar));
}


static void *VDCReadVar (void* therec, NclQuark thevar, long* _start, long* _finish,long* _stride,void* storage)
{
	VDC_DEBUG_printff("('%s')\n", NrmQuarkToString(thevar));

	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	float *out = (float*)storage;
	long start [VDC_MAX_DIMS];
	long finish[VDC_MAX_DIMS];
	long stride[VDC_MAX_DIMS];
	long count [VDC_MAX_DIMS];
	long size  [VDC_MAX_DIMS];

	for (int i = 0; i < rec->numVariablesAndCoords; i++) {
		if (thevar == rec->variables[i].var_name_quark) {
			int numDims = rec->variables[i].num_dimensions;
			int refLevels = VDC_GetNumRefLevels(p, NrmQuarkToString(rec->variables[i].var_name_quark));
			int refLevel = refLevels - 1;

			// If not time varying, add fake flat time dimension for convenience
			if (!VDC_IsTimeVarying(p, NrmQuarkToString(rec->variables[i].var_name_quark))) {
				VDC_DEBUG_printff(" Adding fake time variable\n");
				for (int j = 0; j < numDims; j++) {
					start [j+1] = _start [j];
					finish[j+1] = _finish[j];
					stride[j+1] = _stride[j];
				}
				start[0] = finish[0] = 0;
				stride[0] = 1;
				numDims++;
				for (int j = 1; j < numDims; j++)
					size[j] = DimList_index(rec->dims, rec->variables[i].file_dim_num[j-1])->rec.dim_size;
				size[0] = 1;
			} else {
				for (int j = 0; j < numDims; j++) {
					start [j] = _start [j];
					finish[j] = _finish[j];
					stride[j] = _stride[j];
				}
				for (int j = 0; j < numDims; j++)
					size[j] = DimList_index(rec->dims, rec->variables[i].file_dim_num[j])->rec.dim_size;
			}

#ifdef VDC_DEBUG
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"start[":"", start[j], j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"finish[":"", finish[j], j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"stride[":"", stride[j], j==numDims-1?"]\n":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"size[":"", size[j], j==numDims-1?"]\n":", ");
#endif
			for (int j = 0; j < numDims; j++) count[j] = (finish[j]-start[j])/stride[j] + 1;
			assert(numDims <= VDC_MAX_DIMS);

			// Stride

			for (int j = 0; j < numDims; j++) {
				if (start[j] % stride[j] != 0 || finish[j] % stride[j] != 0) {
					NhlPError(NhlFATAL, NhlEUNKNOWN, "VDCReadVar: Stride needs to be aligned with read start and finish indexes."); // TODO VDC
					return NULL;
				}
			}

			while (refLevel > 0) {
				for (int j = 1; j < numDims; j++)
					if (stride[j] % 2 == 1)
					   goto _strideEnd;	

				for (int j = 1; j < numDims; j++) {
					stride[j] /= 2;
					size[j]   /= 2;
					finish[j]  = start[j] + ((finish[j] - start[j]) / 2);
				}
				refLevel--;
			}
_strideEnd: ;

			// else {
			// 	for (int j = 1; j < numDims; j++) {
			// 		if (stride[j] != 1) {
			// 			NhlPError(NhlWARNING,NhlEUNKNOWN, "VDCReadVar: "); // TODO VDC
			// 			break;
			// 		}
			// 	}
			// }

			size_t loadSize = 1;
			for (int j = 1; j < numDims; j++) loadSize *= size[j];
			float *in = malloc(sizeof(float)*loadSize);

			for (int t = start[0]; t <= finish[0]; t += stride[0]) {
				VDC_GetVarAtTimeStep(p, t, NrmQuarkToString(thevar), refLevel, rec->levelOfDetail, in);

				if (numDims == 2) { // Start at 2D because fake time dimension is added to 1D data
					long ti = ((t - start[0]) / stride[0]) * count[1];
					for (int x = start[1]; x <= finish[1]; x += stride[1]) {
						out[ti + ((x - start[1]) / stride[1])] = in[x];
					}
				}
				else if (numDims == 3) {
					long ti = ((t - start[0]) / stride[0]) * count[1] * count[2];
					for (int y = start[1]; y <= finish[1]; y += stride[1]) {
						long yi = ti + ((y - start[1]) / stride[1]) * count[2];
						for (int x = start[2]; x <= finish[2]; x += stride[2]) {
							out[yi + ((x - start[2]) / stride[2])] = in[y*size[2] + x];
						}
					}
				}
				else if (numDims == 4) {
					long ti = ((t - start[0]) / stride[0]) * count[1] * count[2] * count[3];
					for (int z = start[1]; z <= finish[1]; z += stride[1]) {
						long zi = ti + ((z - start[1]) / stride[1]) * count[2] * count[3];
						for (int y = start[2]; y <= finish[2]; y += stride[2]) {
							long yi = zi + ((y - start[2]) / stride[2]) * count[3];
							for (int x = start[2]; x <= finish[2]; x += stride[2]) {
								out[yi + ((x - start[3]) / stride[3])] = in[z*size[2]*size[3] + y*size[3] + x];
							}
						}
					}
				}
			}

			free(in);
			return out;
		}
	}

	return NULL;
}


static void *VDCReadCoord (void* therec, NclQuark thevar, long* start, long* finish,long* stride,void* storage)
{
    VDC_DEBUG_printff("('%s')\n", NrmQuarkToString(thevar));

	return VDCReadVar(therec, thevar, start, finish,stride,storage);
}

static void *VDCReadAtt (void *therec,NclQuark theatt,void* storage)
{
    VDC_DEBUG_printff("(\"%s\") = ", NrmQuarkToString(theatt));
	VDCRecord *rec = (VDCRecord*)therec;

    for (int i=0; i<rec->numAtts; i++) {
        if (theatt == rec->globalAtts[i].att_name_quark) {
#ifdef VDC_DEBUG
        	if (rec->globalAtts[i].data_type == NCL_string)
				fprintf(stderr, "\"%s\"\n", NrmQuarkToString(rec->globalAttsValues[i]));
        	if (rec->globalAtts[i].data_type == NCL_int || rec->globalAtts[i].data_type == NCL_int64)
				fprintf(stderr, "%li\n", rec->globalAttsValues[i]);
        	if (rec->globalAtts[i].data_type == NCL_float || rec->globalAtts[i].data_type == NCL_double)
				fprintf(stderr, "%f\n", ((double*)rec->globalAttsValues)[i]);
#endif
        	if (rec->globalAtts[i].data_type == NCL_string || rec->globalAtts[i].data_type == NCL_int64)
        		*(long*) storage = rec->globalAttsValues[i];
        	else if (rec->globalAtts[i].data_type == NCL_int)
        		*(int*) storage = (int) rec->globalAttsValues[i];
        	else if (rec->globalAtts[i].data_type == NCL_float)
        		*(float*) storage = (float) rec->globalAttsValues[i];
        	else if (rec->globalAtts[i].data_type == NCL_double)
        		*(double*) storage = ((double*)rec->globalAttsValues)[i];
			return (storage);
        }
    }

	return NULL;
}


static void *VDCReadVarAtt (void * therec, NclQuark thevar, NclQuark theatt, void * storage)
{
    VDC_DEBUG_printff("('%s', '%s')\n", NrmQuarkToString(thevar), NrmQuarkToString(theatt));

	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;

	if (theatt == NrmStringToQuark("C_Ratios")) {
		size_t *data;
		int count;
		int ret = VDC_GetCRatios(p, NrmQuarkToString(thevar), &data, &count);
		for (int i = 0; i < count; i++)
			((int*)storage)[i] = data[i];
		VDC_FreeSize_tArray(&data);
		return storage;
	}

	int xt = VDC_GetAttType(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt));

	if (xt >= 0) {
		if (xt == VDC_XType_TEXT) {
			char *text;
			VDC_GetAtt_text(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &text);
        	*(long*) storage = NrmStringToQuark(text);
			VDC_FreeString(&text);
		}
		else if (xt == VDC_XType_INT64) {
			long *data;
			int count;
			VDC_GetAtt_long(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &data, &count);
			for (int i = 0; i < count; i++)
				((long*)storage)[i] = data[i];
			VDC_FreeLongArray(&data);
		}
		else if (xt == VDC_XType_INT32) {
			long *data;
			int count;
			VDC_GetAtt_long(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &data, &count);
			for (int i = 0; i < count; i++)
				((int*)storage)[i] = (int) data[i];
			VDC_FreeLongArray(&data);
		}
		else if (xt == VDC_XType_FLOAT) {
			double *data;
			int count;
			VDC_GetAtt_double(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &data, &count);
			for (int i = 0; i < count; i++)
				((float*)storage)[i] = (float) data[i];
			VDC_FreeDoubleArray(&data);
		}
		else if (xt == VDC_XType_DOUBLE) {
			double *data;
			int count;
			VDC_GetAtt_double(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &data, &count);
			for (int i = 0; i < count; i++)
				((double*)storage)[i] = data[i];
			VDC_FreeDoubleArray(&data);
		}
		return storage;
	}

    VDC_DEBUG_printff(" Warning: '%s'->'%s' not found\n", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
	return NULL; // xt = INVALID
}


static NhlErrorTypes VDCWriteVar (void * therec, NclQuark thevar, void *_data, long* _start, long *_finish, long *_stride)
{
    VDC_DEBUG_printff("('%s')\n", NrmQuarkToString(thevar));
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	float *data = (float*)_data;
	long start [VDC_MAX_DIMS];
	long finish[VDC_MAX_DIMS];
	long stride[VDC_MAX_DIMS];
	long count [VDC_MAX_DIMS];
	long size  [VDC_MAX_DIMS];
	long _size  [VDC_MAX_DIMS];
	AssertWriteMode(rec);

	for (int i = 0; i < rec->numVariablesAndCoords; i++) {
		if (thevar == rec->variables[i].var_name_quark) {
			int numDims = rec->variables[i].num_dimensions;
			int refLevels = VDC_GetNumRefLevels(p, NrmQuarkToString(rec->variables[i].var_name_quark));
			int refLevel = refLevels - 1;
			int dimIndexOffset = rec->hasScalarDim ? 1 : 0;

			for (int j = 0; j < numDims; j++)
				_size[j] = DimList_index(rec->dims, rec->variables[i].file_dim_num[j] + dimIndexOffset)->rec.dim_size;

			if (numDims == 0) {
				// Add fake dimension of size 1 for scalar variable
				numDims++;
				start [0] = 0;
				finish[0] = 0;
				stride[0] = 1;
				size  [0] = 1;
			} else {
				// If not time varying, add fake flat time dimension for convenience
				if (!VDC_IsTimeVarying(p, NrmQuarkToString(rec->variables[i].var_name_quark))) {
					VDC_DEBUG_printff(" Adding fake time variable\n");
					for (int j = 0; j < numDims; j++) {
						start [j+1] = _start [j];
						finish[j+1] = _finish[j];
						stride[j+1] = _stride[j];
						size  [j+1] = _size  [j];
					}
					start[0] = finish[0] = 0;
					stride[0] = 1;
					size[0] = 1;
					numDims++;
				} else {
					for (int j = 0; j < numDims; j++) {
						start [j] = _start [j];
						finish[j] = _finish[j];
						stride[j] = _stride[j];
						size  [j] = _size  [j];
					}

				}
			}

#ifdef VDC_DEBUG
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"start[":"",  start[j],  j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"finish[":"", finish[j], j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"stride[":"", stride[j], j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"size[":"",   size[j],   j==numDims-1?"]\n":", ");
#endif
			for (int j = 0; j < numDims; j++) count[j] = (finish[j]-start[j])/stride[j] + 1;
			assert(numDims <= VDC_MAX_DIMS);

			// Stride

			for (int j = 0; j < numDims; j++) {
				if (start[j] % stride[j] != 0 || finish[j] % stride[j] != 0) {
					NhlPError(NhlFATAL, NhlEUNKNOWN, "VDCWriteVar: Stride needs to be aligned with read start and finish indexes."); // TODO VDC
					return NhlFATAL;
				}
			}

			while (refLevel > 0) {
				for (int j = 1; j < numDims; j++)
					if (stride[j] % 2 == 1)
					   goto _strideEnd;	

				for (int j = 1; j < numDims; j++) {
					stride[j] /= 2;
					size[j]   /= 2;
					finish[j]  = start[j] + ((finish[j] - start[j]) / 2);
				}
				refLevel--;
			}
_strideEnd: ;

			// else {
			// 	for (int j = 1; j < numDims; j++) {
			// 		if (stride[j] != 1) {
			// 			NhlPError(NhlWARNING,NhlEUNKNOWN, "VDCReadVar: "); // TODO VDC
			// 			break;
			// 		}
			// 	}
			// }

			size_t loadSize = 1;
			for (int j = 1; j < numDims; j++) loadSize *= size[j];
			float *in = malloc(sizeof(float)*loadSize);

			for (int t = start[0]; t <= finish[0]; t += stride[0]) {
				if (numDims == 2) { // Start at 2D because fake time dimension is added to 1D data
					long ti = ((t - start[0]) / stride[0]) * count[1];
					for (int x = start[1]; x <= finish[1]; x += stride[1]) {
						in[x] = data[ti + ((x - start[1]) / stride[1])];
					}
				}
				else if (numDims == 3) {
					long ti = ((t - start[0]) / stride[0]) * count[1] * count[2];
					for (int y = start[1]; y <= finish[1]; y += stride[1]) {
						long yi = ti + ((y - start[1]) / stride[1]) * count[2];
						for (int x = start[2]; x <= finish[2]; x += stride[2]) {
							in[y*size[2] + x] = data[yi + ((x - start[2]) / stride[2])];
						}
					}
				}
				else if (numDims == 4) {
					long ti = ((t - start[0]) / stride[0]) * count[1] * count[2] * count[3];
					for (int z = start[1]; z <= finish[1]; z += stride[1]) {
						long zi = ti + ((z - start[1]) / stride[1]) * count[2] * count[3];
						for (int y = start[2]; y <= finish[2]; y += stride[2]) {
							long yi = zi + ((y - start[2]) / stride[2]) * count[3];
							for (int x = start[2]; x <= finish[2]; x += stride[2]) {
								in[z*size[2]*size[3] + y*size[3] + x] = data[yi + ((x - start[3]) / stride[3])];
							}
						}
					}
				}
				if (VDC_PutVarAtTimeStep(p, t, NrmQuarkToString(thevar), rec->levelOfDetail, in) < 0) {
					NhlPError(NhlFATAL, NhlEUNKNOWN, "VDCWriteVar: Error writing data to file: %s\n", VDC_GetErrMsg(p));
					return NhlFATAL;
				}
				// if (VDC_PutVarAtTimeStep(p, t, "lon", rec->levelOfDetail, in) < 0) {
				// 	NhlPError(NhlFATAL, NhlEUNKNOWN, "VDCWriteVar: Error writing data to file: %s\n", VDC_GetErrMsg(p));
				// 	return NhlFATAL;
				// }
			}

			free(in);
			return NhlNOERROR;
		}
	}

	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCWriteVar: Variable \"%s\" does not exist.", NrmQuarkToString(thevar));
	return NhlFATAL;
}


static NhlErrorTypes VDCWriteVarAtt (void *therec, NclQuark thevar, NclQuark theatt, void* data);
static NhlErrorTypes VDCWriteAtt (void *therec, NclQuark theatt, void *data )
{
    VDC_DEBUG_printff("('%s')\n", NrmQuarkToString(theatt));
	return VDCWriteVarAtt(therec, NrmStringToQuark(""), theatt, data);
}


static NhlErrorTypes VDCWriteVarAtt (void *therec, NclQuark thevar, NclQuark theatt, void* values)
{
    VDC_DEBUG_printff("('%s', '%s')\n", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	AssertDefineMode(rec);

	int xt = VDC_GetAttType(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt));
	if (xt < 0) {
		NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCWriteVarAtt: Attritube '%s'->'%s' does not exist.", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
		return NhlFATAL;
	}
	NclBasicDataTypes data_type = _VDCXTypeToNCLDataType(xt);

	void *valuesSansQuark;
	if (data_type == NCL_string) {
		valuesSansQuark = malloc(strlen(NrmQuarkToString(*(long*)values)) + 1);
		strcpy((char *)valuesSansQuark, NrmQuarkToString(*(long*)values));
		VDC_DEBUG_printff(": <data> is string \"%s\"\n", valuesSansQuark); // TODO VDC check
	}
	else valuesSansQuark = values;

	int n_items;
	VDC_GetAtt_Count(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), &n_items);

	VDC_DEBUG_printff(": Writing to VDC\n");
	if (VDC_PutAtt(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), xt, valuesSansQuark, n_items) < 0) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCWriteVarAtt: failed to write variable (\"%s\") attribute (\"%s\").", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
		return NhlFATAL;
	}

	if (data_type == NCL_string)
		free(valuesSansQuark);

	if (thevar == NrmStringToQuark("")) {
		int index;
		for (int i = 0; i < rec->numAtts; i++) {
			if (rec->globalAtts[i].att_name_quark == theatt) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			VDC_DEBUG_printff(": Error: index was not set.\n");
			NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCWriteVarAtt: failed to write variable (\"%s\") attribute (\"%s\").", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
			return NhlFATAL;
		}


		VDC_DEBUG_printff(": Writing to global atts in record\n");
		if (data_type == NCL_float || data_type == NCL_int)
			rec->globalAttsValues[index] = *(int *)values; // TODO VDC do I need to copy? / multiple items
		else
			rec->globalAttsValues[index] = *(long *)values; // TODO VDC do I need to copy? / multiple items
	}

	return NhlNOERROR;
}


static NhlErrorTypes VDCAddDim (void* therec, NclQuark thedim, ng_size_t size,int is_unlimited)
{
    VDC_DEBUG_printff("('%s', %li, %s)\n", NrmQuarkToString(thedim), size, is_unlimited?"True":"False");
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	AssertDefineMode(rec);
	int is_scalar_dim = 0;

	if (is_unlimited) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddDim: VDC does not support dimensions of unlimited length."); // TODO VDC
		return NhlFATAL;
	}
	if (size < 1) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddDim: dimension size must be greater than 0.");
		return NhlFATAL;
	}

	if (thedim == NrmStringToQuark("ncl_scalar")) {
		if (size != 1) {
			NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddDim: \"ncl_scalar\" is a reserved file dimension name in NCL, this name can only represent dimensions of size 1");
			return NhlFATAL;
		}
		rec->hasScalarDim = 1;
		is_scalar_dim = 1;

	} else {
		if (VDC_DefineDimension(p, NrmQuarkToString(thedim), size) < 0) {
			NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddDim: failed to create dimension \"%s\".", NrmQuarkToString(thedim));
			return NhlFATAL;
		}
	}

	DimList *d = DimList_new();
	d->rec.dim_name_quark = thedim;
	d->rec.dim_size = size;
	d->rec.is_unlimited = is_unlimited;

	// ncl_scalar needs to be first
	if (is_scalar_dim)
		DimList_pushFront(&rec->dims, d);
	else
		DimList_push(&rec->dims, d);
	rec->dimsCount++;

	/*
	printf("--- dims ITerate list ---\n");
	d = rec->dims;
	int i = 0;
	while (d) {
		fprintf(stderr, "dims[%i] = '%s'\n", i, NrmQuarkToString(d->rec.dim_name_quark));
		d = d->next; i++;
	}
	printf("--- dims iterate index ---\n");
	for (i = 0; i < rec->dimsCount; i++) {
		fprintf(stderr, "dims[%i] = '%s'[%li]\n", i, NrmQuarkToString(DimList_index(rec->dims, i)->rec.dim_name_quark), DimList_index(rec->dims, i)->rec.dim_size);
	}
	*/

	return NhlNOERROR;
}


const char *_heuristicAxis(const char *name)
{
	char n[256];
	assert(strlen(name) < 255);
	strcpy(n, name);
	{char *p = n; for (; *p; ++p) *p = tolower(*p);}

	if (strstr(n, "lon"))    return "X";
	if (strstr(n, "west"))   return "X";
	if (strstr(n, "east"))   return "X";

	if (strstr(n, "lat"))    return "Y";
	if (strstr(n, "north"))  return "Y";
	if (strstr(n, "south"))  return "Y";

	if (!strcmp(n, "x"))       return "X";
	if (!strcmp(n, "y"))       return "Y";
	if (!strcmp(n, "z"))       return "Z";

	if (!strcmp(n, "t"))       return "T";
	if (!strcmp(n, "time"))    return "T";
	if (!strcmp(n, "days"))    return "T";
	if (!strcmp(n, "hours"))   return "T";
	if (!strcmp(n, "minutes")) return "T";
	if (!strcmp(n, "seconds")) return "T";

	if (strstr(n, "z_") == n) return "Z";

	return "X";
}


static NhlErrorTypes VDCAddVar (void* therec, NclQuark thevar, NclBasicDataTypes data_type, int n_dims, NclQuark *dim_names, long* dim_sizes)
{
#ifdef VDC_DEBUG
    VDC_DEBUG_printff("('%s', %s, %i, [", NrmQuarkToString(thevar), _NCLDataTypeToString(data_type), n_dims);
	for (int i = 0; i < n_dims; i++) VDC_DEBUG_printf("\"%s\"%s", NrmQuarkToString(dim_names[i]), i+1==n_dims?"], [":", ");
	for (int i = 0; i < n_dims; i++) VDC_DEBUG_printf("\"%li\"%s", dim_sizes[i], i+1==n_dims?"])\n":", ");
#endif
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	AssertDefineMode(rec);
	int add_scalar_dim = 0;



	// Check for implicit coordinate variables
	if (n_dims == 1 && thevar == dim_names[0]) {
		NclQuark timeDimName = NrmStringToQuark("");
		NclQuark units = NrmStringToQuark("");
		NclQuark axis  = NrmStringToQuark(_heuristicAxis(NrmQuarkToString(thevar)));
		if (axis == NrmStringToQuark("T")) {
			timeDimName = thevar;
			n_dims = 0;
		}
		return VDCAddCoordVarCustom(therec, thevar, data_type, n_dims, dim_names, timeDimName, units, axis);
	}

	// Check for ncl_scalar which implies a dimensionless variable
	if (n_dims == 1 && dim_names[0] == NrmStringToQuark("ncl_scalar")) {
		n_dims = 0;
		add_scalar_dim = 1;
	}

	// Manually add implicit coordinates here instead of in VDC to use heuristics for axes
	for (int i = 0; i < n_dims; i++) {
		if (VDC_CoordVarExists(p, NrmQuarkToString(dim_names[i])))
			continue;

		// VDCAddCoordVarCustom(therec, dim_names[i], NCL_float, 

		// TODO VDC add variables to NCL cache
	}

	// Swap dimension order to comply with VDC conventions
	for (int i = 0; i < n_dims / 2; i++) {
		NclQuark *tmp = dim_names[i];
		dim_names[i] = dim_names[n_dims - i - 1];
		dim_names[n_dims - i - 1] = tmp;
	}
	
	char **dims = (char **)malloc(sizeof(char*) * n_dims);
	for (int i = 0; i < n_dims; i++) {
		dims[i] = (char *)malloc(sizeof(char) * (strlen(NrmQuarkToString(dim_names[i])) + 1));
		strcpy(dims[i], NrmQuarkToString(dim_names[i]));
	}

	int ret = VDC_DefineDataVar(p, NrmQuarkToString(thevar), (const char **)dims, n_dims, NULL, 0, "", _NCLDataTypeToVDCXType(data_type), rec->compressionEnabled);

	for (int i = 0; i < n_dims; i++) free(dims[i]);
	free(dims);

	if (ret < 0) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddVar: failed to add variable \"%s\" with error code %i.", NrmQuarkToString(thevar), ret);
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddVar: error message: \"%s\"", VDC_GetErrMsg());
		return NhlFATAL;
	}

	NclFVarRec *oldVariables = rec->variables;
	rec->variables = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec) * (rec->numVariablesAndCoords + 1));

	memcpy(rec->variables, oldVariables, sizeof(NclFVarRec) * rec->numVariables);
	memcpy(&rec->variables[rec->numVariables + 1], &oldVariables[rec->numVariables], sizeof(NclFVarRec) * (rec->numVariablesAndCoords - rec->numVariables));

	rec->variables[rec->numVariables].var_name_quark = thevar;
	rec->variables[rec->numVariables].data_type = data_type;

	assert(n_dims <= NCL_MAX_DIMENSIONS);
	rec->variables[rec->numVariables].num_dimensions = n_dims;
	for (int i = 0; i < n_dims; i++)
		rec->variables[rec->numVariables].file_dim_num[i] = _dimensionNameToId(rec, NrmQuarkToString(dim_names[n_dims - i - 1]));

	rec->numVariables++;
	rec->numVariablesAndCoords++;

	NclFree(oldVariables);

	if (add_scalar_dim)
		if (!rec->hasScalarDim)
			VDCAddDim(therec, NrmStringToQuark("ncl_scalar"), 1, 0);

	return NhlNOERROR;
}


static NhlErrorTypes VDCSetVarCompressLevel (void* therec, NclQuark thevar, int compress_level)
{
    VDC_DEBUG_printff("('%s', %i, )\n", NrmQuarkToString(thevar), compress_level);
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;


    VDC_DEBUG_printff(": Not Implemented\n", NrmQuarkToString(thevar), compress_level);
	return NhlFATAL;
}


static NhlErrorTypes VDCAddVarAtt (void *therec,NclQuark thevar, NclQuark theatt, NclBasicDataTypes data_type, int n_items, void * values);
static NhlErrorTypes VDCAddAtt (void *therec,NclQuark theatt, NclBasicDataTypes data_type, int n_items, void * values)
{
    // VDC_DEBUG_printff("('%s', %s, %i, <data>)\n", NrmQuarkToString(theatt), _NCLDataTypeToString(data_type), n_items);
	return VDCAddVarAtt(therec, NrmStringToQuark(""), theatt, data_type, n_items, values);
}


static NhlErrorTypes VDCAddVarAtt (void *therec, NclQuark thevar, NclQuark theatt, NclBasicDataTypes data_type, int n_items, void * values)
{
    // VDC_DEBUG_printff("('%s', '%s', %s, %i, <data>)\n", NrmQuarkToString(thevar), NrmQuarkToString(theatt), _NCLDataTypeToString(data_type), n_items);
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	AssertDefineMode(rec);

	void *valuesSansQuark;
	if (data_type == NCL_string) {
		valuesSansQuark = malloc(strlen(NrmQuarkToString(*(long*)values)) + 1);
		strcpy((char *)valuesSansQuark, NrmQuarkToString(*(long*)values));
		// VDC_DEBUG_printff(": <data> is string \"%s\"\n", valuesSansQuark);
	}
	else valuesSansQuark = values;

	// VDC_DEBUG_printff(": Adding to VDC\n");
	if (VDC_PutAtt(p, NrmQuarkToString(thevar), NrmQuarkToString(theatt), _NCLDataTypeToVDCXType(data_type), valuesSansQuark, n_items) < 0) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddVarAtt: failed to add variable (\"%s\") attribute (\"%s\").", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
		return NhlFATAL;
	}

	if (data_type == NCL_string)
		free(valuesSansQuark);

	if (thevar == NrmStringToQuark("")) {
		VDC_DEBUG_printff(": Adding to global atts in record\n");

		NclFAttRec *oldAtts = rec->globalAtts;
		long *oldAttsValues = rec->globalAttsValues;

		rec->globalAtts = (NclFAttRec*)NclMalloc(sizeof(NclFAttRec) * (rec->numAtts + 1));
		rec->globalAttsValues = NclMalloc(sizeof(long) * (rec->numAtts + 1));

		memcpy(rec->globalAtts, oldAtts, sizeof(NclFAttRec) * rec->numAtts);
		memcpy(rec->globalAttsValues, oldAttsValues, sizeof(long) * rec->numAtts);

		NclFree(oldAtts);
		NclFree(oldAttsValues);

		rec->globalAtts[rec->numAtts].att_name_quark = theatt;
		rec->globalAtts[rec->numAtts].data_type = data_type;
		rec->globalAtts[rec->numAtts].num_elements = n_items;
		if (data_type == NCL_float || data_type == NCL_int)
			rec->globalAttsValues[rec->numAtts] = *(int *)values; // TODO VDC do I need to copy? / multiple items
		else
			rec->globalAttsValues[rec->numAtts] = *(long *)values; // TODO VDC do I need to copy? / multiple items

		rec->numAtts++;
	}

	return NhlNOERROR;
}


static void *MapNclTypeToFormat (NclBasicDataTypes nclType)
{
	VDC_XType *ret = (VDC_XType *)NclMalloc(sizeof(VDC_XType));
	*ret = _NCLDataTypeToVDCXType(nclType);
	return ret;
}


static NclBasicDataTypes MapFormatTypeToNcl (void *type)
{
	return _VDCXTypeToNCLDataType(*(VDC_XType *)type);
}


static NhlErrorTypes VDCSetOption (void *therec,NclQuark option, NclBasicDataTypes data_type, int n_items, void * values)
{
    VDC_DEBUG_printff("('%s')\n", NrmQuarkToString(option));

	VDCRecord *rec = (VDCRecord*)therec;

	if (option ==  NrmStringToQuark("levelofdetail")) {
		int val = *(int*)values;
		if (val >= -1) {
			rec->levelOfDetail = val;
			VDC_DEBUG_printff(": %s = %i\n", NrmQuarkToString(option), rec->levelOfDetail);
		} else {
	 		NhlPError(NhlWARNING,NhlEUNKNOWN, "VDCSetOption: option (%s) value cannot be less than -1", NrmQuarkToString(option));
	 		return NhlWARNING;
		}
	}
	else if (option ==  NrmStringToQuark("compressionenabled")) {
		int val = *(int*)values;
		if (val == 0 || val == 1) {
			rec->compressionEnabled = val;
			VDC_DEBUG_printff(": %s = %s\n", NrmQuarkToString(option), rec->compressionEnabled ? "true" : "false");
			// TODO VDC
			if (val == 1 && rec->dataSource) {
				// size_t bs[3] = {64,64,64};
				size_t cratios[4] = {1,4,21,62};
				VDC_SetCompressionBlock(rec->dataSource, "bior4.4", cratios, 4);
			}
		} else {
	 		NhlPError(NhlWARNING,NhlEUNKNOWN, "VDCSetOption: option (%s) value must be 0 or 1.", NrmQuarkToString(option));
	 		return NhlWARNING;
		}
	}

	/* Handled by parent */
	// else {
	// 	NhlPError(NhlWARNING,NhlEUNKNOWN, "VDCSetOption: option (%s) is invalid.",NrmQuarkToString(option));
	//  	return NhlWARNING;
	// }

	return NhlNOERROR;
}


#ifdef VDC_DEBUG_OFF
#define _NULLFUNC(x) \
	void NULL_##x (void) { \
		printf("\n=======================================\nERROR: NULL VDC function %s called\n", #x); \
		exit(1); \
	}
#define NULLFUNC(x) NULL_##x
#else
#define _NULLFUNC(x)
#define NULLFUNC(x) NULL
#endif
_NULLFUNC(VDCWriteCoord)
_NULLFUNC(VDCAddChunkDim)
_NULLFUNC(VDCAddVarChunk)
_NULLFUNC(VDCAddVarChunkCache)
_NULLFUNC(VDCSetVarCompressLevel)

_NULLFUNC(read_coord2)
_NULLFUNC(read_var2)
_NULLFUNC(write_coord2)
_NULLFUNC(write_var2)
_NULLFUNC(VDCRenameDim)
_NULLFUNC(VDCAddCoordVar) // Not used according to NclNetCdf.c #2871
_NULLFUNC(VDCDelAtt)
_NULLFUNC(VDCDelVarAtt)


NclFormatFunctionRec VDCRec = {
/* NclInitializeFileRecFunc initialize_file_rec */      VDCInitializeFileRec,
/* NclCreateFileFunc	   create_file; */		VDCCreateFile,
/* NclOpenFileFunc         open_file; */		VDCOpenFile,
/* NclFreeFileRecFunc      free_file_rec; */	VDCFreeFileRec,
/* NclGetVarNamesFunc      get_var_names; */	VDCGetVarNames,
/* NclGetVarInfoFunc       get_var_info; */		VDCGetVarInfo,
/* NclGetDimNamesFunc      get_dim_names; */	VDCGetDimNames,
/* NclGetDimInfoFunc       get_dim_info; */		VDCGetDimInfo,
/* NclGetAttNamesFunc      get_att_names; */	VDCGetAttNames,
/* NclGetAttInfoFunc       get_att_info; */		VDCGetAttInfo,
/* NclGetVarAttNamesFunc   get_var_att_names; */VDCGetVarAttNames,
/* NclGetVarAttInfoFunc    get_var_att_info; */	VDCGetVarAttInfo,
/* NclGetCoordInfoFunc     get_coord_info; */	VDCGetCoordInfo,
/* NclReadCoordFunc        read_coord; */		VDCReadCoord,
/* NclReadCoordFunc        read_coord; */		NULLFUNC(read_coord2),
/* NclReadVarFunc          read_var; */			VDCReadVar,
/* NclReadVarFunc          read_var; */			NULLFUNC(read_var2),
/* NclReadAttFunc          read_att; */			VDCReadAtt,
/* NclReadVarAttFunc       read_var_att; */		VDCReadVarAtt,
/* NclWriteCoordFunc       write_coord; */		NULLFUNC(VDCWriteCoord),
/* NclWriteCoordFunc       write_coord; */		NULLFUNC(write_coord2),
/* NclWriteVarFunc         write_var; */		VDCWriteVar,
/* NclWriteVarFunc         write_var; */		NULLFUNC(write_var2),
/* NclWriteAttFunc         write_att; */		VDCWriteAtt,
/* NclWriteVarAttFunc      write_var_att; */	VDCWriteVarAtt,
/* NclAddDimFunc           add_dim; */			VDCAddDim,
/* NclAddChunkDimFunc      add_chunk_dim; */	NULLFUNC(VDCAddChunkDim),
/* NclRenameDimFunc        rename_dim; */		NULLFUNC(VDCRenameDim),
/* NclAddVarFunc           add_var; */			VDCAddVar,
/* NclAddVarChunkFunc      add_var_chunk; */	NULLFUNC(VDCAddVarChunk),
/* NclAddVarChunkCacheFunc add_var_chunk_cache; */	NULLFUNC(VDCAddVarChunkCache),
/* NclSetVarCompressLevelFunc set_var_compress_level; */ NULLFUNC(VDCSetVarCompressLevel),
/* NclAddVarFunc           add_coord_var; */	NULLFUNC(VDCAddCoordVar),
/* NclAddAttFunc           add_att; */			VDCAddAtt,
/* NclAddVarAttFunc        add_var_att; */		VDCAddVarAtt,
/* NclMapFormatTypeToNcl   map_format_type_to_ncl; */	MapFormatTypeToNcl,
/* NclMapNclTypeToFormat   map_ncl_type_to_format; */	MapNclTypeToFormat,
/* NclDelAttFunc           del_att; */			NULLFUNC(VDCDelAtt),
/* NclDelVarAttFunc        del_var_att; */		NULLFUNC(VDCDelVarAtt),
#include "NclGrpFuncs.null"
/* NclSetOptionFunc        set_option;  */      VDCSetOption
};

NclFormatFunctionRecPtr VDCAddFileFormat (void)
{	
	return(&VDCRec);
}


//
// -----------------------------
//       Custom Functions
// -----------------------------
//


static NhlErrorTypes VDCAddCoordVarCustom(void* therec, NclQuark thevar, NclBasicDataTypes data_type, int n_dims, NclQuark *dim_names, NclQuark timeDimName, NclQuark units, NclQuark axisStr)
{
    VDC_DEBUG_printff("('%s', %s, %i, [dim_names], [dim_sizes])\n", NrmQuarkToString(thevar), _NCLDataTypeToString(data_type), n_dims);
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	AssertDefineMode(rec);

	int axis = -1;
	if      (axisStr == NrmStringToQuark("X")    || axisStr == NrmStringToQuark("x")) axis = 0;
	else if (axisStr == NrmStringToQuark("Y")    || axisStr == NrmStringToQuark("y")) axis = 1;
	else if (axisStr == NrmStringToQuark("Z")    || axisStr == NrmStringToQuark("z")) axis = 2;
	else if (axisStr == NrmStringToQuark("T")    || axisStr == NrmStringToQuark("y")) axis = 3;
	else if (axisStr == NrmStringToQuark("TIME") || axisStr == NrmStringToQuark("time") || axisStr == NrmStringToQuark("Time")) axis = 3;
	else {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: invalid axis \"%s\".", NrmQuarkToString(axisStr));
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: valid axes are: X, Y, Z, T", VDC_GetErrMsg());
		return NhlFATAL;
	}

	// VDC expects empty array for defining time coord but I don't know how to pass <empty> in NCL so will use "" as <empty>
	if (n_dims == 1 && dim_names[0] == NrmStringToQuark(""))
		n_dims = 0;

	// VDC does not allow coordinate variables with the same names as dimensions to be compressed
	// The VDC error message is cryptic so adding a check here with clearer message
	if (rec->compressionEnabled) {
		char **names;
		int namesCount;
		VDC_GetDimensionNames(p, &names, &namesCount);
		for (int i = 0; i < namesCount; i++) {
			if (NrmStringToQuark(names[i]) == thevar) {
				VDC_FreeStringArray(&names, &namesCount);
				NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: coordinate variables with the same names as dimensions cannot be compressed.");
				return NhlFATAL;
			}
		}
		VDC_FreeStringArray(&names, &namesCount);
	}

	// Swap dimension order to comply with VDC conventions
	for (int i = 0; i < n_dims / 2; i++) {
		NclQuark *tmp = dim_names[i];
		dim_names[i] = dim_names[n_dims - i - 1];
		dim_names[n_dims - i - 1] = tmp;
	}

	char **dims = n_dims ? (char **)malloc(sizeof(char*) * n_dims) : NULL;
	for (int i = 0; i < n_dims; i++) {
		dims[i] = (char *)malloc(sizeof(char) * (strlen(NrmQuarkToString(dim_names[i])) + 1));
		strcpy(dims[i], NrmQuarkToString(dim_names[i]));
	}
	
	// int ret1 = VDC_DefineCoordVarUniform(p, NrmQuarkToString(thevar), dims, n_dims, NrmQuarkToString(timeDimName), NrmQuarkToString(units), axis, _NCLDataTypeToVDCXType(data_type), rec->compressionEnabled);
	int ret2 = VDC_DefineCoordVar(p, NrmQuarkToString(thevar), dims, n_dims, NrmQuarkToString(timeDimName), NrmQuarkToString(units), axis, _NCLDataTypeToVDCXType(data_type), rec->compressionEnabled);

	for (int i = 0; i < n_dims; i++) free(dims[i]);
	free(dims);

	if (ret2 < 0) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: failed to add variable \"%s\" with error code %i.", NrmQuarkToString(thevar), ret2);
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: error message: \"%s\"", VDC_GetErrMsg());
		return NhlFATAL;
	}

	NclFVarRec *oldVariables = rec->variables;
	rec->variables = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec) * (rec->numVariablesAndCoords + 1));
	memcpy(rec->variables, oldVariables, sizeof(NclFVarRec) * rec->numVariablesAndCoords);

	rec->variables[rec->numVariablesAndCoords].var_name_quark = thevar;
	rec->variables[rec->numVariablesAndCoords].data_type = data_type;

	assert(n_dims <= NCL_MAX_DIMENSIONS);
	rec->variables[rec->numVariablesAndCoords].num_dimensions = n_dims;
	for (int i = 0; i < n_dims; i++)
		rec->variables[rec->numVariablesAndCoords].file_dim_num[i] = _dimensionNameToId(rec, NrmQuarkToString(dim_names[n_dims - i - 1]));

	// Add time dim since it is store separately
	if (timeDimName && timeDimName != NrmStringToQuark("")) {
		assert(n_dims +1 <= NCL_MAX_DIMENSIONS);
		rec->variables[rec->numVariablesAndCoords].file_dim_num[rec->variables[rec->numVariablesAndCoords].num_dimensions] = _dimensionNameToId(rec, NrmQuarkToString(timeDimName));
		rec->variables[rec->numVariablesAndCoords].num_dimensions++;
	}

	rec->numVariablesAndCoords++;

	NclFree(oldVariables);

	return NhlNOERROR;
}

NhlErrorTypes _VDC_FileCoordDef(NclFile thefile, NclQuark varname, NclQuark *dimnames, int dimnames_count, NclQuark timeDimName, NclQuark units, NclQuark axis, NclQuark type)
{
	NclFileClass fc = NULL;
	if (thefile == NULL)
		return NhlFATAL;

	void *rec = thefile->file.private_rec;
	//fc = (NclFileClass)thefile->obj.class_ptr;
	if (NrmStringToQuark("vdc") != thefile->file.file_ext_q) {
		NhlPError(NhlFATAL, NhlEUNKNOWN, "_VDC_FileCoordDef: file is not a VDC file.");
		return NhlFATAL;
	}
	NclBasicDataTypes nclType = _nameToNclBasicDataType(type);
	if (nclType == NCL_none) {
		NhlPError(NhlFATAL, NhlEUNKNOWN, "_VDC_FileCoordDef: invalid data type.");
		return NhlFATAL;
	}

	NhlErrorTypes ret = VDCAddCoordVarCustom(rec, varname, nclType, dimnames_count, dimnames, timeDimName, units, axis);
	if (ret != NhlNOERROR)
		return ret;

	if(thefile->file.n_vars >= thefile->file.max_vars)
	{
		_NclReallocFilePart(&(thefile->file), -1, thefile->file.n_vars, -1, -1);
	}

	thefile->file.var_info[thefile->file.n_vars] = (*thefile->file.format_funcs->get_var_info)(thefile->file.private_rec,varname);
	thefile->file.var_att_info[thefile->file.n_vars] = NULL;
	thefile->file.var_att_ids[thefile->file.n_vars] = -1;
	
	thefile->file.n_vars++;
	UpdateCoordInfo(thefile, varname);

	return NhlNOERROR;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
NhlErrorTypes _VDC_IFileCoordDef(void)
{
    VDC_DEBUG_printff("()\n");
	NhlErrorTypes ret, ret0;

	NclFile thefile;
	obj *thefile_id;

	NclQuark *dimnames;
	int dimnames_count;
	NclQuark *types;
	NclQuark *varnames;
	NclQuark *timeDimNames;
	NclQuark *units;
	NclQuark *axis;

	ng_size_t dimsize;
	NclScalar missing;
	int has_missing;
	ng_size_t tmp_dimsize;
	NclScalar tmp_missing;
	int tmp_has_missing;

	thefile_id = (obj*)NclGetArgValue(0, 7, NULL, NULL, NULL, NULL, NULL, 0);
	thefile = (NclFile)_NclGetObj((int)*thefile_id);
	if(thefile == NULL) return(NhlFATAL);

	varnames = (NclQuark*)NclGetArgValue(1, 7, NULL, &dimsize, &missing, &has_missing, NULL, 0);
	if(has_missing) for(int i = 0; i < dimsize; i++) if(varnames[i] == missing.stringval) return(NhlFATAL);

	dimnames = (void*)NclGetArgValue(2, 7, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	dimnames_count = tmp_dimsize;
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(dimnames[i] == tmp_missing.stringval) return(NhlFATAL);

	timeDimNames = (NclQuark*)NclGetArgValue(3, 7, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(timeDimNames[i] == tmp_missing.stringval) return(NhlFATAL);

	units = (NclQuark*)NclGetArgValue(4, 7, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(units[i] == tmp_missing.stringval) return(NhlFATAL);

	axis = (NclQuark*)NclGetArgValue(5, 7, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(axis[i] == tmp_missing.stringval) return(NhlFATAL);

	types = (NclQuark*)NclGetArgValue(6, 7, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0);
    VDC_DEBUG_printff("\n");
	if(tmp_dimsize != dimsize) {
		return(NhlFATAL);
	} else if(tmp_has_missing) {
		for(int i = 0; i < dimsize; i++) {
			if(types[i] == tmp_missing.stringval)  {
				return(NhlFATAL);
			}
		}
	}

	for(int i = 0; i < dimsize; i ++) {
		ret = _VDC_FileCoordDef(thefile, varnames[i], dimnames, dimnames_count, timeDimNames[i], units[i], axis[i], types[i]);
		if(ret < NhlINFO) {
			ret0 = ret;
		}
	}
	return(ret0);
}

static NhlErrorTypes VDCAddDataVarCustom(void* therec, NclQuark thevar, NclBasicDataTypes data_type, int n_dims, NclQuark *dim_names, int n_coords, NclQuark *coord_names, NclQuark units)
{
    VDC_DEBUG_printff("('%s', %s, %i, [dim_names], %i, [coords], '%s')\n", NrmQuarkToString(thevar), _NCLDataTypeToString(data_type), n_dims, n_coords, NrmQuarkToString(units));
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	AssertDefineMode(rec);

	// Swap dimension order to comply with VDC conventions
	for (int i = 0; i < n_dims / 2; i++) {
		NclQuark *tmp  = dim_names[i];
		dim_names[i] = dim_names[n_dims - i - 1];
		dim_names[n_dims - i - 1] = tmp;

		NclQuark *tmp2 = coord_names[i];
		coord_names[i] = coord_names[n_coords - i - 1];
		coord_names[n_coords - i - 1] = tmp2;
	}

	char **dims = (char **)malloc(sizeof(char*) * n_dims);
	for (int i = 0; i < n_dims; i++) {
		dims[i] = (char *)malloc(sizeof(char) * (strlen(NrmQuarkToString(dim_names[i])) + 1));
		strcpy(dims[i], NrmQuarkToString(dim_names[i]));
	}
	char **coords = (char **)malloc(sizeof(char*) * n_coords);
	for (int i = 0; i < n_coords; i++) {
		coords[i] = (char *)malloc(sizeof(char) * (strlen(NrmQuarkToString(coord_names[i])) + 1));
		strcpy(coords[i], NrmQuarkToString(coord_names[i]));
	}
	
	int ret2 = VDC_DefineDataVar(p, NrmQuarkToString(thevar), dims, n_dims, coords, n_coords, NrmQuarkToString(units), _NCLDataTypeToVDCXType(data_type), rec->compressionEnabled);

	for (int i = 0; i < n_dims; i++) free(dims[i]);
	for (int i = 0; i < n_coords; i++) free(coords[i]);
	free(dims);
	free(coords);

	if (ret2 < 0) {
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: failed to add variable \"%s\" with error code %i.", NrmQuarkToString(thevar), ret2);
	 	NhlPError(NhlFATAL,NhlEUNKNOWN, "VDCAddCoordVar: error message: \"%s\"", VDC_GetErrMsg());
		return NhlFATAL;
	}

	NclFVarRec *oldVariables = rec->variables;
	rec->variables = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec) * (rec->numVariablesAndCoords + 1));

	memcpy(rec->variables, oldVariables, sizeof(NclFVarRec) * rec->numVariables);
	memcpy(&rec->variables[rec->numVariables + 1], &oldVariables[rec->numVariables], sizeof(NclFVarRec) * (rec->numVariablesAndCoords - rec->numVariables));

	rec->variables[rec->numVariables].var_name_quark = thevar;
	rec->variables[rec->numVariables].data_type = data_type;

	assert(n_dims <= NCL_MAX_DIMENSIONS);
	rec->variables[rec->numVariables].num_dimensions = n_dims;
	for (int i = 0; i < n_dims; i++)
		rec->variables[rec->numVariables].file_dim_num[i] = _dimensionNameToId(rec, NrmQuarkToString(dim_names[n_dims - i - 1]));

	rec->numVariables++;
	rec->numVariablesAndCoords++;

	NclFree(oldVariables);

	return NhlNOERROR;
}

NhlErrorTypes _VDC_FileDataDef(NclFile thefile, NclQuark varname, NclQuark *dimnames, int dimnames_count, NclQuark *coordnames, int coordnames_count, NclQuark units, NclQuark type)
{
	NclFileClass fc = NULL;
	if (thefile == NULL)
		return NhlFATAL;

	void *rec = thefile->file.private_rec;
	//fc = (NclFileClass)thefile->obj.class_ptr;
	if (NrmStringToQuark("vdc") != thefile->file.file_ext_q) {
		NhlPError(NhlFATAL, NhlEUNKNOWN, "_VDC_FileCoordDef: file is not a VDC file.");
		return NhlFATAL;
	}
	NclBasicDataTypes nclType = _nameToNclBasicDataType(type);
	if (nclType == NCL_none) {
		NhlPError(NhlFATAL, NhlEUNKNOWN, "_VDC_FileCoordDef: invalid data type.");
		return NhlFATAL;
	}

	NhlErrorTypes ret = VDCAddDataVarCustom(rec, varname, nclType, dimnames_count, dimnames, coordnames_count, coordnames, units);
	if (ret != NhlNOERROR)
		return ret;

	if(thefile->file.n_vars >= thefile->file.max_vars)
	{
		_NclReallocFilePart(&(thefile->file), -1, thefile->file.n_vars, -1, -1);
	}

	thefile->file.var_info[thefile->file.n_vars] = (*thefile->file.format_funcs->get_var_info)(thefile->file.private_rec,varname);
	thefile->file.var_att_info[thefile->file.n_vars] = NULL;
	thefile->file.var_att_ids[thefile->file.n_vars] = -1;
	
	thefile->file.n_vars++;
	UpdateCoordInfo(thefile, varname);

	return NhlNOERROR;
}

NhlErrorTypes _VDC_IFileDataDef(void)
{
    VDC_DEBUG_printff("()\n");
	NhlErrorTypes ret, ret0;

	NclFile thefile;
	obj *thefile_id;

	NclQuark *dimnames;
	int dimnames_count;
	NclQuark *types;
	NclQuark *varnames;
	NclQuark *coordVarNames;
	int coordVarNames_count;
	NclQuark *units;

	ng_size_t dimsize;
	NclScalar missing;
	int has_missing;
	ng_size_t tmp_dimsize;
	NclScalar tmp_missing;
	int tmp_has_missing;

	thefile_id = (obj*)NclGetArgValue(0, 6, NULL, NULL, NULL, NULL, NULL, 0);
	thefile = (NclFile)_NclGetObj((int)*thefile_id);
	if(thefile == NULL) return(NhlFATAL);

	varnames = (NclQuark*)NclGetArgValue(1, 6, NULL, &dimsize, &missing, &has_missing, NULL, 0);
	if(has_missing) for(int i = 0; i < dimsize; i++) if(varnames[i] == missing.stringval) return(NhlFATAL);

	dimnames = (void*)NclGetArgValue(2, 6, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	dimnames_count = tmp_dimsize;
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(dimnames[i] == tmp_missing.stringval) return(NhlFATAL);

	coordVarNames = (NclQuark*)NclGetArgValue(3, 6, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	coordVarNames_count = tmp_dimsize;
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(coordVarNames[i] == tmp_missing.stringval) return(NhlFATAL);

	units = (NclQuark*)NclGetArgValue(4, 6, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0); 
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(units[i] == tmp_missing.stringval) return(NhlFATAL);

	types = (NclQuark*)NclGetArgValue(5, 6, NULL, &tmp_dimsize, &tmp_missing, &tmp_has_missing, NULL, 0);
	if(tmp_has_missing) for(int i = 0; i < dimsize; i++) if(types[i] == tmp_missing.stringval) return(NhlFATAL);

    VDC_DEBUG_printff("\n");
	if(tmp_dimsize != dimsize) {
		return(NhlFATAL);
	} else if(tmp_has_missing) {
		for(int i = 0; i < dimsize; i++) {
			if(types[i] == tmp_missing.stringval)  {
				return(NhlFATAL);
			}
		}
	}

	for(int i = 0; i < dimsize; i ++) {
		ret = _VDC_FileDataDef(thefile, varnames[i], dimnames, dimnames_count, coordVarNames, coordVarNames_count, units[i], types[i]);
		if(ret < NhlINFO) {
			ret0 = ret;
		}
	}
	return(ret0);
}
#pragma GCC diagnostic pop
