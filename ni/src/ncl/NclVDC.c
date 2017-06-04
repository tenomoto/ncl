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
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include <vapor/VDC_c.h>

// TODO
#define VDC_DEBUG
#ifdef VDC_DEBUG
#define VDC_DEBUG_printf(...) fprintf (stderr, __VA_ARGS__)
#else
#define VDC_DEBUG_printf(...)
#endif

static void dump(void *d, int size) {
	printf("%p\n==============\n", d);
	for (int i = 0; i < size; i++) {
		printf("%x", (((char*)d)[i])&0xf);
		if (i%2==1)printf(" ");
	}
	printf("\n");
}

typedef struct _VDCRecord VDCRecord;

struct _VDCRecord {
        NclQuark		file_path_q;
        int				wr_status;
        VDC*			dataSource;

        NclFVarRec      *variables;
		int				*variablesIsTimeVarying;
		int				numVariables;
		int				numVariablesAndCoords;

        NclFAttRec		*globalAtts;
        long			*globalAttsValues;
        int				numAtts;

        NclFDimRec      *dimensions;
        int             numDimensions;

		int levelOfDetail;
};

static int _VDCXTypeToNCLDataType(int xtype)
{
	switch (xtype) {
		case VDC_XType_TEXT: return NCL_string;
		case VDC_XType_INT32: return NCL_int;
		case VDC_XType_INT64: return NCL_int64;
		case VDC_XType_FLOAT: return NCL_float;
		case VDC_XType_DOUBLE: return NCL_double;
		case VDC_XType_INVALID: return NCL_none;
		default: return NCL_none;
	}
}

static int _dimensionNameToId(VDCRecord *rec, const char *name)
{
	NclQuark name_q = NrmStringToQuark(name);
	for (int i = 0; i < rec->numDimensions; i++) {
		if (rec->dimensions[i].dim_name_quark == name_q)
			return i;
	}
	return -1;
}

static void *VDCInitializeFileRec (NclFileFormat *format)
{
    VDCRecord*  therec = (VDCRecord*) NULL;
    
    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCInitializeFileRec...\n");
    #endif

	therec = (VDCRecord*)NclCalloc(1, sizeof(VDCRecord));
	if (! therec) {
		NhlPError(NhlFATAL,ENOMEM,NULL);
		return NULL;
	}
	therec->levelOfDetail = -1;

	*format = _NclVDC;
	return (void *) therec;
}


static void *VDCCreateFile (void *rec,NclQuark path)
{
	VDCRecord *tmp = (VDCRecord*)rec;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCCreateFile...\n");
    #endif

    return((void*) tmp);
}

static void _defineGlobalAttributes (VDCRecord *rec)
{
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s\n", __FILE__, __LINE__, __func__);
    #endif
	VDC *p = rec->dataSource;

	char **names;
	int count;
	VDC_GetAttNames(p, "", &names, &count);

	// TODO VDC num_elements > 1

    rec->numAtts = count;
    rec->globalAtts = (NclFAttRec*)NclMalloc(sizeof(NclFAttRec) * (rec->numAtts +1)); // NOTE +1 is in OGR code so I'm keeping it.
    rec->globalAttsValues = NclMalloc(sizeof(long) * (rec->numAtts +1));
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
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s\n", __FILE__, __LINE__, __func__);
    #endif

	VDC *p = rec->dataSource;

	char **names;
	int count;
	VDC_GetDimensionNames(p, &names, &count);
	VDCDimension *dim = VDCDimension_new();
    rec->dimensions = (NclFDimRec*)NclMalloc(sizeof(NclFDimRec) * count);
	rec->numDimensions = count;

	for (int i = 0; i < count; i++) {
		VDC_GetDimension(p, names[i], dim);
        rec->dimensions[i].dim_name_quark = NrmStringToQuark(names[i]);
        rec->dimensions[i].dim_size = VDCDimension_GetLength(dim);
        rec->dimensions[i].is_unlimited = 0;
	}

	VDCDimension_delete(dim);
	VDC_FreeStringArray(&names, &count);
}

static void _defineVariables (VDCRecord *rec)
{
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s\n", __FILE__, __LINE__, __func__);
    #endif

	VDC *p = rec->dataSource;
	assert(rec->dimensions); // Dimensions need to be defined first

	char **dataNames, **coordNames;
	int dataCount, coordCount;
	VDC_GetDataVarNames(p, &dataNames, &dataCount);
	VDC_GetCoordVarNames(p, &coordNames, &coordCount);
	VDCBaseVar *v = VDCBaseVar_new();

	rec->numVariables = dataCount;
	rec->numVariablesAndCoords = dataCount + coordCount;
    rec->variables = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec) * (rec->numVariablesAndCoords));
	rec->variablesIsTimeVarying = (int*)NclMalloc(sizeof(int) * (rec->numVariables));

	for (int i = 0; i < rec->numVariablesAndCoords; i++) {
		const char *name = i < dataCount ? dataNames[i] : coordNames[i - dataCount];
		VDC_GetBaseVarInfo(p, name, v);

        rec->variables[i].var_name_quark = NrmStringToQuark(name);
        rec->variables[i].data_type = _VDCXTypeToNCLDataType(VDCBaseVar_GetXType(v));
		if (i < rec->numVariables)
			rec->variablesIsTimeVarying[i] = VDC_IsTimeVarying(p, name);

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
	VDCRecord *rec = (VDCRecord*) therec;
    //ng_size_t numGeometry = 0;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCOpenFile...\n");
    #endif

	if(rec == NULL) {
		#ifdef VDC_DEBUG
		fprintf(stderr, "VDCOpenFile > rec is NULL\n");
		#endif
		return(NULL);
	}

    /* NOTE: for now we only support read-only access */
    if (wr_status == 0) {
        NhlPError(NhlWARNING, NhlEUNKNOWN, "writing to VDC files is not currently implemented.");
        wr_status = 1;
    }

    rec->dataSource = VDC_new();
	int success = VDC_Initialize(rec->dataSource, NrmQuarkToString(path), VDC_AccessMode_R);// NOTE read only access

    if (success < 0) {
		VDC_delete(rec->dataSource);
        NhlPError(NhlFATAL, NhlEUNKNOWN, "Failed to open VDC file: ");
        NhlPError(NhlFATAL, NhlEUNKNOWN, NrmQuarkToString(path));
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
	VDCRecord *rec = (VDCRecord*)therec;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCFreeFileRec...\n");
    #endif

    if (rec == NULL)
            return;

	// TODO free sub variables and any file pointers
    if (rec->variables)  NclFree(rec->variables);
    if (rec->variablesIsTimeVarying) NclFree(rec->variablesIsTimeVarying);     
    if (rec->globalAtts) NclFree(rec->globalAtts);
    if (rec->globalAttsValues) NclFree(rec->globalAttsValues);
    if (rec->dimensions) NclFree(rec->dimensions);     

    VDC_delete(rec->dataSource);

    NclFree(rec);
	rec = NULL;

	return;
}


static NclQuark* VDCGetVarNames (void* therec, int *num_vars)
{
        VDCRecord *rec = (VDCRecord*)therec;
        int i;

        #ifdef VDC_DEBUG
        fprintf(stderr, "VDCGetVarNames...\n");
        #endif

        NclQuark* names = (NclQuark*) NclMalloc(sizeof(NclQuark) * rec->numVariables);
        if (names == NULL) {
                NhlPError(NhlFATAL,ENOMEM,NULL);
                return NULL;
        }

        for (i=0; i<rec->numVariables; i++) {
                names[i] = rec->variables[i].var_name_quark;
        }

        *num_vars = rec->numVariables;
        return names;
}


static NclFVarRec *VDCGetVarInfo (void *therec, NclQuark var_name)
{
	VDCRecord *rec = (VDCRecord*)therec;
        int i=0;

        #ifdef VDC_DEBUG
        fprintf(stderr, "VDCGetVarInfo...\n");	
        #endif
        
        for (i=0; i<rec->numVariables; i++) {
                if (var_name == rec->variables[i].var_name_quark) {
                        int j;
                        NclFVarRec *ret = (NclFVarRec*)NclMalloc(sizeof(NclFVarRec));
                        ret->var_name_quark = rec->variables[i].var_name_quark;
                        ret->var_full_name_quark = rec->variables[i].var_name_quark;
                        ret->var_real_name_quark = rec->variables[i].var_name_quark;
                        ret->data_type = rec->variables[i].data_type;
                        ret->num_dimensions = rec->variables[i].num_dimensions;
                        for (j=0; j<ret->num_dimensions; j++)
                                ret->file_dim_num[j] = rec->variables[i].file_dim_num[j];
                        return ret;
                }
        }

        return NULL;
}


static NclQuark *VDCGetDimNames (void* therec, int* num_dims)
{
	VDCRecord *rec = (VDCRecord*)therec;
    int i;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetDimNames...");
    #endif

    NclQuark* names = (NclQuark*) NclMalloc(sizeof(NclQuark) * rec->numDimensions);
    if (names == NULL) {
            NhlPError(NhlFATAL,ENOMEM,NULL);
            return NULL;
    }

    for (i=0; i<rec->numDimensions; i++) {
            names[i] = rec->dimensions[i].dim_name_quark;
    }

    *num_dims = rec->numDimensions;
    #ifdef VDC_DEBUG
    fprintf(stderr, "returning %i dims\n", *num_dims);
    #endif
    return names;        
}


static NclFDimRec *VDCGetDimInfo (void* therec, NclQuark dim_name_q)
{
	VDCRecord* rec = (VDCRecord*)therec;
    int i;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetDimInfo(%s)...\n", NrmQuarkToString(dim_name_q));
    #endif

    for (i=0; i<rec->numDimensions; i++) {
        if (dim_name_q == rec->dimensions[i].dim_name_quark) {
            NclFDimRec* ret = (NclFDimRec*) NclMalloc(sizeof(NclFDimRec));
            *ret = rec->dimensions[i];
            return ret;
        }
    }

	return NULL;
}


static NclQuark *VDCGetAttNames (void* therec, int *num_atts)
{	
	VDCRecord* rec = (VDCRecord*)therec;
    int i;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetAttNames...");
    #endif

    NclQuark* names = (NclQuark*) NclMalloc(sizeof(NclQuark) * rec->numAtts);
    if (names == NULL) {
        NhlPError(NhlFATAL,ENOMEM,NULL);
        return NULL;
    }

    for (i=0; i<rec->numAtts; i++) {
        names[i] = rec->globalAtts[i].att_name_quark;
    }

    *num_atts = rec->numAtts;

    #ifdef VDC_DEBUG
    fprintf(stderr, " returning %i atts\n", *num_atts);
    #endif

    return names;
}


static NclFAttRec* VDCGetAttInfo (void* therec, NclQuark att_name_q)
{
	VDCRecord* rec = (VDCRecord*)therec;
    int i;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetAttInfo(\"%s\")... ", NrmQuarkToString(att_name_q));
    #endif

    for (i=0; i<rec->numAtts; i++) {
        if (att_name_q == rec->globalAtts[i].att_name_quark) {
            NclFAttRec* ret = (NclFAttRec*) NclMalloc(sizeof(NclFAttRec));
            *ret = rec->globalAtts[i];

			#ifdef VDC_DEBUG
			fprintf(stderr, "rec->globalAtts[%i]\n", i);
			#endif
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
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
    *num_atts = 0;
	NclQuark *names_q = NULL;

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetVarAttNames(%s)...\n", NrmQuarkToString(thevar));
    #endif

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
	VDCRecord *rec = (VDCRecord*)therec;
	VDC *p = rec->dataSource;
	NclFAttRec *att = NULL; // (NclFAttRec*)NclMalloc((unsigned) sizeof(NclFAttRec));

    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetVarAttInfo(%s, %s)...\n", NrmQuarkToString(thevar), NrmQuarkToString(theatt));
    #endif

	if (theatt == NrmStringToQuark("C_Ratios")) {
		att = (NclFAttRec*)NclMalloc(sizeof(NclFAttRec));
		att->att_name_quark = theatt;
		att->data_type = NCL_int;;
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
    #ifdef VDC_DEBUG
    fprintf(stderr, "VDCGetCoordInfo(\"%s\")...\n", NrmQuarkToString(thevar));
    #endif
	return(VDCGetVarInfo(therec, thevar));
}

void _temp_copy(float *i,float *o, int w, int h) {
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			o[y*w+x] = i[y*w+x];
		}
	}
}

int _ispowerof(long n, long x) {
	if (x == 0) return 0;
	while (x % n == 0)
		x /= n;
	return x == 1;
}

int _xPowY(int x, int y) {
	int acc = 1;
	while (y-- > 0)
		acc *= x;
	return acc;
}

static void *VDCReadVar (void* therec, NclQuark thevar, long* _start, long* _finish,long* _stride,void* storage)
{
#ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s')\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thevar));
#endif

#define VDC_MAX_DIMS 5
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
			if (!rec->variablesIsTimeVarying[i]) {
				VDC_DEBUG_printf("Adding fake time variable\n");
				for (int j = 0; j < numDims; j++) {
					start [j+1] = _start [j];
					finish[j+1] = _finish[j];
					stride[j+1] = _stride[j];
				}
				start[0] = finish[0] = 0;
				stride[0] = 1;
				numDims++;
				for (int j = 1; j < numDims; j++)
					size[j] = rec->dimensions[rec->variables[i].file_dim_num[j-1]].dim_size;
				size[0] = 1;
			} else {
				for (int j = 0; j < numDims; j++) {
					start [j] = _start [j];
					finish[j] = _finish[j];
					stride[j] = _stride[j];
				}
				for (int j = 0; j < numDims; j++)
					size[j] = rec->dimensions[rec->variables[i].file_dim_num[j]].dim_size;
			}

#ifdef VDC_DEBUG
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"start[":"", start[j], j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"finish[":"", finish[j], j==numDims-1?"], ":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"stride[":"", stride[j], j==numDims-1?"]\n":", ");
			for (int j = 0; j < numDims; j++) VDC_DEBUG_printf("%s%li%s", j==0?"size[":"", size[j], j==numDims-1?"]\n":", ");
#endif
			for (int j = 0; j < numDims; j++) count[j] = (finish[j]-start[j])/stride[j] + 1;
			assert(numDims <= 4);

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
#ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s')\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thevar));
#endif

	return VDCReadVar(therec, thevar, start, finish,stride,storage);
}

static void *VDCReadAtt (void *therec,NclQuark theatt,void* storage)
{
	VDCRecord *rec = (VDCRecord*)therec;

#ifdef VDC_DEBUG
    fprintf(stderr, "VDCReadAtt(\"%s\") = ", NrmQuarkToString(theatt));
#endif

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
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s', '%s')\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thevar), NrmQuarkToString(theatt));
    #endif

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

    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s Warning: '%s'->'%s' not found\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thevar), NrmQuarkToString(theatt));
    #endif
	return NULL; // xt = INVALID
}


static NhlErrorTypes VDCWriteVarAtt (void *therec, NclQuark thevar, NclQuark theatt, void* data);

static NhlErrorTypes VDCWriteAtt (void *therec, NclQuark theatt, void *data )
{
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s')\n", __FILE__, __LINE__, __func__, NrmQuarkToString(theatt));
    #endif

	return VDCWriteVarAtt(therec, NrmStringToQuark(""), theatt, data);
}


static NhlErrorTypes VDCWriteVarAtt (void *therec, NclQuark thevar, NclQuark theatt, void* data)
{
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s', '%s')\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thevar), NrmQuarkToString(theatt));
    #endif
	VDCRecord *rec = (VDCRecord*)therec;

	printf("Not implemented\n");
	return NhlFATAL;
}


static NhlErrorTypes VDCAddDim (void* therec, NclQuark thedim, ng_size_t size,int is_unlimited)
{
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s', %li, %s)\n", __FILE__, __LINE__, __func__, NrmQuarkToString(thedim), size, is_unlimited?"True":"False");
    #endif
	VDCRecord *rec = (VDCRecord*)therec;

	printf("Not implemented\n");
	return NhlFATAL;
}


static NhlErrorTypes VDCSetOption (void *therec,NclQuark option, NclBasicDataTypes data_type, int n_items, void * values)
{
    #ifdef VDC_DEBUG
    fprintf(stderr, "[%s:%i]%s('%s')\n", __FILE__, __LINE__, __func__, NrmQuarkToString(option));
    #endif

	VDCRecord *rec = (VDCRecord*)therec;

	if (option ==  NrmStringToQuark("levelofdetail")) {
		int val = *(int*)values;
		if (val >= -1) {
			rec->levelOfDetail = val;
			#ifdef VDC_DEBUG
			fprintf(stderr, "[%s:%i]%s levelOfDetail = %i\n", __FILE__, __LINE__, __func__, rec->levelOfDetail);
			#endif
		} else {
	 		NhlPError(NhlWARNING,NhlEUNKNOWN, "VDCSetOption: option (%s) value cannot be less than -1",NrmQuarkToString(option));
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
/* NclReadCoordFunc        read_coord; */		NULL,
/* NclReadVarFunc          read_var; */			VDCReadVar,
/* NclReadVarFunc          read_var; */			NULL,
/* NclReadAttFunc          read_att; */			VDCReadAtt,
/* NclReadVarAttFunc       read_var_att; */		VDCReadVarAtt,
/* NclWriteCoordFunc       write_coord; */		NULL,
/* NclWriteCoordFunc       write_coord; */		NULL,
/* NclWriteVarFunc         write_var; */		NULL,
/* NclWriteVarFunc         write_var; */		NULL,
/* NclWriteAttFunc         write_att; */		VDCWriteAtt,
/* NclWriteVarAttFunc      write_var_att; */	VDCWriteVarAtt, // TODO VDC
/* NclAddDimFunc           add_dim; */			VDCAddDim, // TODO VDC
/* NclAddChunkDimFunc      add_chunk_dim; */	NULL,
/* NclRenameDimFunc        rename_dim; */		NULL,
/* NclAddVarFunc           add_var; */			NULL,
/* NclAddVarChunkFunc      add_var_chunk; */	NULL,
/* NclAddVarChunkCacheFunc add_var_chunk_cache; */	NULL,
/* NclSetVarCompressLevelFunc set_var_compress_level; */ NULL,
/* NclAddVarFunc           add_coord_var; */	NULL,
/* NclAddAttFunc           add_att; */			NULL,
/* NclAddVarAttFunc        add_var_att; */		NULL,
/* NclMapFormatTypeToNcl   map_format_type_to_ncl; */	NULL,
/* NclMapNclTypeToFormat   map_ncl_type_to_format; */	NULL,
/* NclDelAttFunc           del_att; */			NULL,
/* NclDelVarAttFunc        del_var_att; */		NULL,
#include "NclGrpFuncs.null"
/* NclSetOptionFunc        set_option;  */      VDCSetOption
};

NclFormatFunctionRecPtr VDCAddFileFormat 
#if	NhlNeedProto
(void)
#else 
()
#endif
{	
	return(&VDCRec);
}
