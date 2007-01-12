/*
 *      $Id: NclHDF.c,v 1.23 2007-01-12 20:09:35 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1994			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Wed Jul 13 10:15:21 MDT 1994
 *
 *	Description:	
 */
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#include "defs.h"
#define HAVE_NETCDF
#include <hdf/mfhdf.h>
#include <hdf/netcdf.h>
#include "NclData.h"
#include "NclFileInterfaces.h"
#include <math.h>
#include <ctype.h>

typedef struct _HDFFileRecord HDFFileRecord;
typedef struct _HDFVarInqRec HDFVarInqRec;
typedef struct _HDFDimInqRec HDFDimInqRec;
typedef struct _HDFAttInqRec HDFAttInqRec;
typedef struct _HDFVarInqRecList HDFVarInqRecList;
typedef struct _HDFDimInqRecList HDFDimInqRecList;
typedef struct _HDFAttInqRecList HDFAttInqRecList;

struct _HDFVarInqRecList {
	HDFVarInqRec *var_inq;
	HDFVarInqRecList *next;
};

struct _HDFDimInqRecList {
	HDFDimInqRec *dim_inq;
	HDFDimInqRecList *next;
};

struct _HDFAttInqRecList {
	HDFAttInqRec *att_inq;
	HDFAttInqRecList *next;
};

struct _HDFVarInqRec {
	int varid;
	int32 id_ref;
	int32 vg_ref;
	NclQuark name;
	NclQuark hdf_name;
	NclQuark class_name;
	NclQuark var_path;
	nc_type	data_type;
	int	n_dims;
	int	dim[MAX_VAR_DIMS];
	int	natts;
	HDFAttInqRecList *att_list;
};

struct _HDFDimInqRec {
	int dimid;
	NclQuark name;
	NclQuark hdf_name;
	long size;
	int is_unlimited;
};
	
struct _HDFAttInqRec {
	int att_num;
	NclQuark name;
	NclQuark hdf_name;
	int	varid;
	nc_type data_type;
	int	len;
	void *value;
	int attr_ix;
};


struct _HDFFileRecord {
NclQuark	file_path_q;
int		wr_status;
int		n_vars;
HDFVarInqRecList *vars;
int		n_dims;
HDFDimInqRecList *dims;
int		has_scalar_dim;
int		n_file_atts;
HDFAttInqRecList *file_atts;
};


static void HDFGetAttrVal
#if	NhlNeedProto
(int ncid,HDFAttInqRec* att_inq)
#else
(ncid,att_inq)
int ncid,
HDFAttInqRec* att_inq
#endif
{
	char *tmp;
	int ret;

	if (att_inq->data_type < 1) {
		att_inq->value = NULL;
	}
	else if(att_inq->data_type == NC_CHAR) {
		tmp = (char*)NclMalloc(att_inq->len+1);
		tmp[att_inq->len] = '\0';
		ret = sd_ncattget(ncid,att_inq->varid,NrmQuarkToString(att_inq->name),tmp);
		att_inq->value = NclMalloc(sizeof(NclQuark));
		*(string *)att_inq->value = NrmStringToQuark(tmp);
		NclFree(tmp);
	} 
	else {
		att_inq->value = NclMalloc(nctypelen(att_inq->data_type)*att_inq->len);
		ret = sd_ncattget(ncid,att_inq->varid,NrmQuarkToString(att_inq->name),att_inq->value);
	}
}

static void HDF_SDGetAttrVal
#if	NhlNeedProto
(int sd_id,HDFAttInqRec* att_inq)
#else
(sd_id,att_inq)
int sdid,
HDFAttInqRec* att_inq
#endif
{
	char *tmp;
	int ret;

	if (att_inq->data_type < 1) {
		att_inq->value = NULL;
	}
	else if(att_inq->data_type == NC_CHAR) {
		int llen = att_inq->len - 1;
		tmp = (char*)NclMalloc(att_inq->len+1);
		ret = SDreadattr(sd_id,att_inq->attr_ix,tmp);
		att_inq->value = NclMalloc(sizeof(NclQuark));
		while (tmp[llen] == '\0' || isspace(tmp[llen])) {
			llen--;
		}
		while (strlen(tmp) < llen) {
			tmp[strlen(tmp)] = ' ';
		}
		tmp[MAX(att_inq->len,llen+1)] = '\0';
		*(string *)att_inq->value = NrmStringToQuark(tmp);
		NclFree(tmp);
	} 
	else {
		att_inq->value = NclMalloc(nctypelen(att_inq->data_type)*att_inq->len);
		ret = SDreadattr(sd_id,att_inq->attr_ix,att_inq->value);
		
	}
}


static void HDFCacheAttValue
#if	NhlNeedProto
(HDFAttInqRec *att_inq,void *value)
#else
(att_inq,value)
	HDFAttInqRec *att_inq;
	void *value;
#endif
{
	if (att_inq->data_type < 1 || value == NULL) {
		att_inq->value = NULL;
	}
	else if (att_inq->data_type == NC_CHAR) {
		char *tmp = NclMalloc(att_inq->len + 1);
		strncpy(tmp,value,att_inq->len);
		tmp[att_inq->len] = '\0';
		att_inq->value = NclMalloc(sizeof(NclQuark));
		*(string*)att_inq->value = NrmStringToQuark(tmp);
		NclFree(tmp);
	}
	else {
		att_inq->value = NclMalloc(nctypelen(att_inq->data_type) * att_inq->len);
		memcpy(att_inq->value,value,nctypelen(att_inq->data_type) * att_inq->len);
	}
	return;
}


static NclBasicDataTypes HDFMapToNcl 
#if	NhlNeedProto
(void* the_type)
#else
(the_type)
	void *the_type;
#endif
{
	static int first = 1;
	static NclBasicDataTypes long_type;
	if(first) {
		if(sizeof(nclong) == _NclSizeOf(NCL_int)) {
			long_type = NCL_int;
		} else if(sizeof(nclong) == _NclSizeOf(NCL_long)) {
			long_type = NCL_long;
		} 
		else {
			long_type = NCL_none;
		}
		first = 0;
	}
	switch(*(nc_type*)the_type) {
	case NC_BYTE:
		return(NCL_byte);
	case NC_CHAR:
		return(NCL_char);
	case NC_SHORT:
		return(NCL_short);
	case NC_LONG:
		return(long_type);
	case NC_FLOAT:
		return(NCL_float);
	case NC_DOUBLE:
		return(NCL_double);
	default:
		return(NCL_none);
	}
}

static void *HDFMapFromNcl
#if	NhlNeedProto
(NclBasicDataTypes the_type)
#else
(the_type)
	NclBasicDataTypes the_type;
#endif
{
	static int first = 1;
	static NclBasicDataTypes long_type;
	void *out_type = (void*)NclMalloc((unsigned)sizeof(nc_type));;
	if(first) {
		if(sizeof(nclong) == _NclSizeOf(NCL_long)) {
			long_type = NCL_long;
		} else if(sizeof(nclong) == _NclSizeOf(NCL_int)) {
			long_type = NCL_int;
		} else {
			long_type = NCL_none;
		}
		first = 0;
	}

	switch(the_type) {
	case NCL_byte:
		*(nc_type*)out_type = NC_BYTE;
                break;
	case NCL_char:
		*(nc_type*)out_type = NC_CHAR;
                break;
	case NCL_short:
		*(nc_type*)out_type = NC_SHORT;
                break;
	case NCL_int:
		*(nc_type*)out_type = NC_LONG;
                break;
	case NCL_long:
		if(long_type == the_type) {
			*(nc_type*)out_type = NC_LONG;
		} else {
			NhlPError(NhlWARNING,NhlEUNKNOWN,"Can't map type, HDF 4 does not support 64 bit longs: try converting to integer or double");
			NclFree(out_type);
			out_type = NULL;
		}
		break;
	case NCL_float:
		*(nc_type*)out_type = NC_FLOAT;
		break;
	case NCL_double:
		*(nc_type*)out_type = NC_DOUBLE;
		break;
        default:
		NclFree(out_type);
		out_type = NULL;
	}
	return(out_type);
}

static NclQuark HDFToNCLName
#if	NhlNeedProto
(
	char *hdf_name,
	char *ncl_class,
	NhlBoolean compress
)
#else
(hdf_name,hdf_class,compress)
	char *hdf_name;
	char *ncl_class;
	NhlBoolean compress;

#endif
{
	char buffer[MAX_NC_NAME];
	char *tp, *cp;
	char len = MIN(MAX_NC_NAME - 2,strlen(hdf_name));
	
	strncpy(buffer,hdf_name,len);
	cp = tp = buffer;

	if (! compress) {
		while (cp < buffer + len) {
			if (! isalnum(*cp)) {
				*cp = '_';
			}
			cp++;
		}
	}
	else {
		while (tp < buffer + len) {
			if (isalnum(*tp)) {
				*cp = *tp;
				cp++;
			}
			else {
				*cp = '_';
				if (cp == buffer || *(cp-1) != '_')
					cp++;
			}
			tp++;
		}
		if (cp > buffer && *(cp-1) == '_')
			cp--;
	}

	*cp = '\0';
	if (ncl_class) {
		len = MIN(MAX_NC_NAME - 1,len + 2 + strlen(ncl_class));
		strncat(cp,"__",len);
		cp += 2;
		*cp = '\0';
		strncat(buffer,ncl_class,len);
		cp += strlen(ncl_class);
		*cp = '\0';
	}
	buffer[len] = '\0';
	return NrmStringToQuark(buffer);
}
		  
static void 
ProcessVgroup(HDFFileRecord *hdf,int32 hid,int32 vg_ref,int level,char *vpath) 
{
	char vname[VGNAMELENMAX];
	char vclass[VGNAMELENMAX];
	int32 n_entries;
	int status;
	int i;
	int32 *tag_array,*ref_array;
	char levelstr[30];
	int vgid = Vattach(hid,vg_ref,"r");
	int len = strlen(vpath);

	status = Vgetclass(vgid,vclass);
	if (! strcmp(vclass,"CDF0.0")) {
		/* we're not interested in the SDS data vgroup, 
		   because it won't show the hierarchy */
		status = Vdetach(vgid);
		return;
	}
		
	status = Vinquire(vgid,&n_entries,vname);

	levelstr[0] = '\0';
	for (i = 0; i < level; i++) {
		strcat(levelstr,"\t");
	}
#if 0
	printf("%svgroup %d: %d %s %s\n",levelstr,vg_ref,n_entries,vclass,vname);
#endif
	tag_array = NclMalloc(n_entries*sizeof(int));
	ref_array = NclMalloc(n_entries*sizeof(int));
	status = Vgettagrefs(vgid,tag_array,ref_array,n_entries);
	if (len)
		strncat(vpath,"/",1023);
	strncat(vpath,vname,1023); 
	for (i = 0; i < n_entries; i++) {
		if (tag_array[i] == DFTAG_NDG) {
			HDFVarInqRecList *varp;
#if 0
			printf("%s\tDFTAG_NDG %d %d %d %d\n",levelstr,tag_array[i],ref_array[i],vg_ref,level);
#endif
			for (varp = hdf->vars; varp != NULL; varp = varp->next) {
				if (ref_array[i] == varp->var_inq->id_ref && varp->var_inq->var_path == NrmNULLQUARK) {
#if 0
					varp->var_inq->var_path = HDFToNCLName(vpath,NULL,True);
#endif
					varp->var_inq->var_path = NrmStringToQuark(vpath);
					varp->var_inq->vg_ref = vg_ref;
				}
			}
		}
/*
		else if (tag_array[i] == DFTAG_SD)
			printf("%s\tDFTAG_SD %d %d\n",levelstr,tag_array[i],ref_array[i]);
		else if (tag_array[i] == DFTAG_VG)
			printf("%s\tDGTAG_VG %d %d\n",levelstr,tag_array[i],ref_array[i]);
		else 
			printf("%s\tOTHER %d %d\n",levelstr,tag_array[i],ref_array[i]);
*/

		if (Visvg(vgid,ref_array[i])) {
			ProcessVgroup(hdf,hid,ref_array[i],level +1,vpath);
		}
	}
	vpath[len] = '\0';
	NclFree(tag_array);
	NclFree(ref_array);
	status = Vdetach(vgid);
	return;
}


static void ProcessVgroups(HDFFileRecord *hdf,char *path)
{
	int hid = Hopen(path,DFACC_READ,0);
	int32 *vls = NULL;
	int status = Vstart(hid);
	int vid,i,j;
	int vcount = Vlone(hid,vls,0);
	char vname[VGNAMELENMAX];
	char vclass[VGNAMELENMAX];
	char vpath[1024];
	int n_entries;
	int *tag_array,*ref_array;

	if (vcount) {
		vpath[0] = '\0';
		vls = NclMalloc(vcount * sizeof(int));
		/*
		 * get top level Vgroups
		 */
		vcount = Vlone(hid,vls,vcount);
		for (i = 0; i < vcount; i++) {
			ProcessVgroup(hdf,hid,vls[i],0,vpath);
		}
		NclFree(vls);
	}
}
	
static void GetSDInfo(char *path)
{
	
	int32 sd_id, sds_id, sds_ref;
      intn  status;
      int32 n_datasets, n_file_attrs, index;
      int32 dim_sizes[MAX_VAR_DIMS];
      int32 rank, data_type, n_attrs;
      char  name[MAX_NC_NAME];    int   i;    
      /********************* End of variable declaration ***********************/    
      /*    * Open the file and initialize the SD interface.    */    
      sd_id = SDstart (path, DFACC_READ); 

      /* Determine the number of data sets in the file and the number    
       * of file attributes.     */    
      status = SDfileinfo (sd_id, &n_datasets, &n_file_attrs);    
      /*      
       * Access every data set and print its name, rank, dimension sizes,    
       * data type, and number of attributes.     
       * The following information should be displayed:    *    
       *               name = SDStemplate    
       *               rank = 2    
       *               dimension sizes are : 16  5      
       *               data type is  24    
       *               number of attributes is  0    */    
      for (index = 0; index < n_datasets; index++)    {        
	      sds_id = SDselect (sd_id, index);     
	      sds_ref = SDidtoref(sds_id);
	      status = SDgetinfo (sds_id, name, &rank, dim_sizes,&data_type, &n_attrs);        
	      printf("----------------id %d ref %d\n",sds_id,sds_ref);
	      printf ("name = %s\n", name);        
	      printf ("rank = %d\n", rank);        
	      printf ("dimension sizes are : ");        
	      for (i=0; i< rank; i++) 
		      printf ("%d  ", dim_sizes[i]);        
	      printf ("\n");        
	      printf ("data type is  %d\n", data_type);        
	      printf ("number of attributes is  %d\n", n_attrs);        
	      /*        
	       * Terminate access to the data set.        
	       */        
	      status = SDendaccess (sds_id);    }    
      /*    
       * Terminate access to the SD interface and close the file.    
       */    
      status = SDend (sd_id); 
} 



static void ProcessDuplicateNames
#if	NhlNeedProto
(
	HDFFileRecord *tmp
)
#else
(tmp)
	HDFFileRecord *tmp;
#endif
{
	
	HDFVarInqRecList *varp,*tvarp;

	ProcessVgroups(tmp,NrmQuarkToString(tmp->file_path_q));
	for (varp = tmp->vars; varp != NULL; varp = varp->next) {
		char vgid_string[12];
		int dup_count = 0;
		if (varp->var_inq->name != NrmNULLQUARK)
			continue;
		for (tvarp = varp->next; tvarp != NULL; tvarp = tvarp->next) {
			if (varp->var_inq->hdf_name != tvarp->var_inq->hdf_name) 
				continue;
			/*
			 * More than one variable with this name
			 * Must do something
			 */
			dup_count++;
			if (tvarp->var_inq->vg_ref > 0)
				sprintf(vgid_string,"%d",tvarp->var_inq->vg_ref);
			else 
				sprintf(vgid_string,"%d",dup_count);
			tvarp->var_inq->name = HDFToNCLName
				(NrmQuarkToString(tvarp->var_inq->hdf_name),vgid_string,True);

			if (tvarp->var_inq->var_path != NrmNULLQUARK) {
				int i;
				HDFAttInqRecList **atlp;

				for (atlp = &(tvarp->var_inq->att_list);;atlp = &((*atlp)->next)) {
					if (*atlp != NULL)
						continue;
					(*atlp) = (HDFAttInqRecList*)NclMalloc((unsigned)sizeof(HDFAttInqRecList));
					(*atlp)->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
					(*atlp)->next = NULL;
					(*atlp)->att_inq->att_num = tvarp->var_inq->natts;
					(*atlp)->att_inq->varid = tvarp->var_inq->varid;
					(*atlp)->att_inq->name = (*atlp)->att_inq->hdf_name = NrmStringToQuark("hdf_group");
					(*atlp)->att_inq->data_type = NC_CHAR;
					(*atlp)->att_inq->len = strlen(NrmQuarkToString(tvarp->var_inq->var_path));
					(*atlp)->att_inq->value = NULL;
					break;
				}
				tvarp->var_inq->natts++;
				atlp = &((*atlp)->next);
				(*atlp) = (HDFAttInqRecList*)NclMalloc((unsigned)sizeof(HDFAttInqRecList));
				(*atlp)->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
				(*atlp)->next = NULL;
				(*atlp)->att_inq->att_num = tvarp->var_inq->natts;
				(*atlp)->att_inq->varid = tvarp->var_inq->varid;
				(*atlp)->att_inq->name = (*atlp)->att_inq->hdf_name = NrmStringToQuark("hdf_group_id");
				(*atlp)->att_inq->data_type = NC_LONG;
				(*atlp)->att_inq->len = 1;
				(*atlp)->att_inq->value = NULL;
				tvarp->var_inq->natts++;
			}

		}
		if (varp->var_inq->vg_ref > 0) {
			sprintf(vgid_string,"%d",varp->var_inq->vg_ref);
		}
		else {
			sprintf(vgid_string,"%d",dup_count);
		}
		varp->var_inq->name = HDFToNCLName(NrmQuarkToString(varp->var_inq->hdf_name),vgid_string,True);
		if (varp->var_inq->var_path != NrmNULLQUARK) {
			int i;
			HDFAttInqRecList **atlp;


			for (atlp = &(varp->var_inq->att_list);;atlp = &((*atlp)->next)) {
				if (*atlp != NULL)
					continue;
				(*atlp) = (HDFAttInqRecList*)NclMalloc((unsigned)sizeof(HDFAttInqRecList));
				(*atlp)->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
				(*atlp)->next = NULL;
				(*atlp)->att_inq->att_num = varp->var_inq->natts;
				(*atlp)->att_inq->varid = varp->var_inq->varid;
				(*atlp)->att_inq->name = (*atlp)->att_inq->hdf_name = NrmStringToQuark("hdf_group");
				(*atlp)->att_inq->data_type = NC_CHAR;
				(*atlp)->att_inq->len = strlen(NrmQuarkToString(varp->var_inq->var_path));
				(*atlp)->att_inq->value = NULL;
				break;
			}
			varp->var_inq->natts++;
			atlp = &((*atlp)->next);
			(*atlp) = (HDFAttInqRecList*)NclMalloc((unsigned)sizeof(HDFAttInqRecList));
			(*atlp)->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
			(*atlp)->next = NULL;
			(*atlp)->att_inq->att_num = varp->var_inq->natts;
			(*atlp)->att_inq->varid = varp->var_inq->varid;
			(*atlp)->att_inq->name = (*atlp)->att_inq->hdf_name = NrmStringToQuark("hdf_group_id");
			(*atlp)->att_inq->data_type = NC_LONG;
			(*atlp)->att_inq->len = 1;
			(*atlp)->att_inq->value = NULL;
			varp->var_inq->natts++;
		}

			
	}
}

static void *HDFInitializeFileRec
#if	NhlNeedProto
(NclFileFormat *format)
#else
(format)
NclFileFormatType *format;
#endif
{
	HDFFileRecord *therec = NULL;

	therec = (HDFFileRecord*)NclCalloc(1, sizeof(HDFFileRecord));
	if (! therec) {
		NhlPError(NhlFATAL,ENOMEM,NULL);
		return NULL;
	}
	*format = _NclHDF;
	return (void *) therec;
}

static void *HDFOpenFile
#if	NhlNeedProto
(void *rec,NclQuark path,int wr_status)
#else
(rec,path,wr_status)
void *rec;
NclQuark path;
int wr_status;
#endif
{
	HDFFileRecord *tmp = (HDFFileRecord*) rec;
	int cdfid;
	int dummy;
	char buffer[MAX_NC_NAME];
	char buffer2[MAX_NC_NAME];
	int i,j,has_scalar_dim = 0,nvars = 0;
	long tmp_size;
	HDFAttInqRecList **stepalptr;
	HDFVarInqRecList **stepvlptr;
	HDFVarInqRecList *tmpvlptr;
	HDFVarInqRecList *tmpvlptr2;
	HDFDimInqRecList **stepdlptr;
	HDFDimInqRecList *tmpdlptr;
	int32 sd_id;
	intn status;
	
	if(tmp == NULL) {
		return(NULL);
	}
	tmp->file_path_q = path;
	tmp->wr_status = wr_status;
	tmp->n_vars = 0;
	tmp->vars= NULL;
	tmp->n_dims = 0;
	tmp->dims = NULL;
	tmp->n_file_atts = 0;
	tmp->file_atts= NULL;

#if 0
	/* print out SD dataset info using SD interface */
	GetSDInfo(NrmQuarkToString(path)); 
#endif

	if(wr_status > 0) {
		cdfid = sd_ncopen(NrmQuarkToString(path),NC_NOWRITE);
		sd_id = SDstart (NrmQuarkToString(path), DFACC_READ); 
	} else {
		cdfid = sd_ncopen(NrmQuarkToString(path),NC_WRITE);
		sd_id = SDstart (NrmQuarkToString(path), DFACC_WRITE); 
	}

	if(cdfid == -1) {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"The specified HDF file (%s) does not exist or can't be opened\n",NrmQuarkToString(path));
		NclFree(tmp);
		return(NULL);
	}
#if 0
	SDfileinfo (sd_id, &tmp->n_vars, &tmp->n_file_atts);    
#endif
	sd_ncinquire(cdfid,&(tmp->n_dims),&(tmp->n_vars),&(tmp->n_file_atts),&dummy);


	stepdlptr = &(tmp->dims);
	if(tmp->n_dims != 0) {
		for(i = 0 ; i < tmp->n_dims; i++) {
			*stepdlptr = (HDFDimInqRecList*)NclMalloc(
					(unsigned) sizeof(HDFDimInqRecList));
			(*stepdlptr)->dim_inq = (HDFDimInqRec*)NclMalloc(
					(unsigned)sizeof(HDFDimInqRec));
			(*stepdlptr)->next = NULL;
			(*stepdlptr)->dim_inq->dimid = i;
			(*stepdlptr)->dim_inq->is_unlimited = (i==dummy)?1:0;

			sd_ncdiminq(cdfid,i,buffer,&((*stepdlptr)->dim_inq->size));
			if((*stepdlptr)->dim_inq->size == 0) {
				NhlPError(NhlWARNING,NhlEUNKNOWN,"HDF: %s is a zero length dimension some variables may be ignored",buffer);
			}
			
			(*stepdlptr)->dim_inq->hdf_name = NrmStringToQuark(buffer);
			(*stepdlptr)->dim_inq->name = HDFToNCLName(buffer,NULL,True);
			stepdlptr = &((*stepdlptr)->next);
		}
	} else {
		tmp->dims = NULL;
	}
	stepvlptr = &(tmp->vars);
	nvars = tmp->n_vars;
	if(tmp->n_vars != 0 ) {
		NhlBoolean has_duplicate_names = False;
		int32 sds_id;
		for(i = 0 ; i < nvars; i++) {
			int dim_sizes[32];
			*stepvlptr = (HDFVarInqRecList*)NclMalloc(
					(unsigned) sizeof(HDFVarInqRecList));
			(*stepvlptr)->var_inq = (HDFVarInqRec*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRec));
			(*stepvlptr)->next = NULL;
			(*stepvlptr)->var_inq->varid = i;
			sd_ncvarinq(cdfid,i,buffer,
				&((*stepvlptr)->var_inq->data_type),
				&((*stepvlptr)->var_inq->n_dims),
				((*stepvlptr)->var_inq->dim),
				&((*stepvlptr)->var_inq->natts)
				);
			sds_id = SDselect(sd_id,i);
			(*stepvlptr)->var_inq->id_ref = SDidtoref(sds_id);
			(*stepvlptr)->var_inq->vg_ref = -1;
			(*stepvlptr)->var_inq->var_path = NrmNULLQUARK;
			(*stepvlptr)->var_inq->name = NrmNULLQUARK;
			(*stepvlptr)->var_inq->hdf_name = NrmStringToQuark(buffer);
			/*
			status = SDgetinfo(sds_id,buffer,
					   &((*stepvlptr)->var_inq->n_dims),
					   dim_sizes,
					   &((*stepvlptr)->var_inq->data_type),
					   &((*stepvlptr)->var_inq->natts));
			*/

			for(j = 0; j < ((*stepvlptr)->var_inq->n_dims); j++) {
				tmp_size = 0;
				sd_ncdiminq(cdfid,((*stepvlptr)->var_inq->dim)[j],buffer2,&tmp_size);
				if(tmp_size == 0 ) {
					NhlPError(NhlWARNING,NhlEUNKNOWN,"HDF: A zero length dimension was found in variable (%s), ignoring this variable",buffer);
					break;
				}
			}
			if(j != ((*stepvlptr)->var_inq->n_dims)) {
				tmpvlptr = *stepvlptr;
				*stepvlptr = NULL;
				tmp->n_vars--;
				NclFree(tmpvlptr->var_inq);
				NclFree(tmpvlptr);
				
			} else {
				if(((*stepvlptr)->var_inq->n_dims) == 0) {
					((*stepvlptr)->var_inq->dim)[0] = -5;
					((*stepvlptr)->var_inq->n_dims) = 1;
					has_scalar_dim = 1;
				}
				stepalptr = &((*stepvlptr)->var_inq->att_list);
				(*stepvlptr)->var_inq->natts++;
				if(((*stepvlptr)->var_inq->natts) != 0) {
					for(j = 0; j < ((*stepvlptr)->var_inq->natts); j++) {
						(*stepalptr) = (HDFAttInqRecList*)NclMalloc(
							(unsigned)sizeof(HDFAttInqRecList));
						(*stepalptr)->att_inq = (HDFAttInqRec*)NclMalloc(
							(unsigned)sizeof(HDFAttInqRec));
						(*stepalptr)->next = NULL;
						(*stepalptr)->att_inq->att_num = j;
						(*stepalptr)->att_inq->varid = i;
						if (j < (*stepvlptr)->var_inq->natts - 1) {
							sd_ncattname(cdfid,i,j,buffer);
							(*stepalptr)->att_inq->hdf_name = NrmStringToQuark(buffer);
							(*stepalptr)->att_inq->name = HDFToNCLName(buffer,NULL,False);
							sd_ncattinq(cdfid,i,buffer,
								    &((*stepalptr)->att_inq->data_type),
								    &((*stepalptr)->att_inq->len));
							(*stepalptr)->att_inq->attr_ix = SDfindattr(sds_id,buffer);

							HDF_SDGetAttrVal(sds_id,(*stepalptr)->att_inq);
						}
						else {
							(*stepalptr)->att_inq->name =
								(*stepalptr)->att_inq->hdf_name = NrmStringToQuark("hdf_name");
							(*stepalptr)->att_inq->data_type = NC_CHAR;
							(*stepalptr)->att_inq->len = 
								strlen(NrmQuarkToString((*stepvlptr)->var_inq->hdf_name));
							(*stepalptr)->att_inq->value = NULL;
						}
						stepalptr = &((*stepalptr)->next);
					}
				} else {
					((*stepvlptr)->var_inq->att_list) = NULL;
				}
				status = SDendaccess (sds_id);
				stepvlptr = &((*stepvlptr)->next);
			}
		}
		if(has_scalar_dim) {
			tmp->has_scalar_dim = 1;
			tmpdlptr = tmp->dims;
			tmp->dims = (HDFDimInqRecList*)NclMalloc(
					(unsigned) sizeof(HDFDimInqRecList));
			tmp->dims->dim_inq = (HDFDimInqRec*)NclMalloc(
					(unsigned)sizeof(HDFDimInqRec));
			tmp->dims->next = tmpdlptr;
			tmp->dims->dim_inq->dimid = -5;
			tmp->dims->dim_inq->size = 1;
			tmp->dims->dim_inq->name = NrmStringToQuark("ncl_scalar");
			tmp->dims->dim_inq->hdf_name = NrmNULLQUARK;
			tmp->dims->dim_inq->is_unlimited = 0;
			tmp->n_dims++;
		} else {
			tmp->has_scalar_dim = 0;
		}
		/* see if there are duplicate names */

		for (tmpvlptr = tmp->vars; tmpvlptr != NULL; tmpvlptr = tmpvlptr->next) {
			for (tmpvlptr2 = tmpvlptr->next; tmpvlptr2 != NULL; tmpvlptr2 = tmpvlptr2->next) {
				if (tmpvlptr->var_inq->hdf_name == tmpvlptr2->var_inq->hdf_name) {
				        has_duplicate_names = True;
					break;
				}
			}
			if (has_duplicate_names)
				break;
		}
		if (has_duplicate_names) {
			ProcessDuplicateNames(tmp);
		}
		else {
			for (tmpvlptr = tmp->vars; tmpvlptr != NULL; tmpvlptr = tmpvlptr->next) {
				tmpvlptr->var_inq->name = HDFToNCLName(NrmQuarkToString(tmpvlptr->var_inq->hdf_name),NULL,True);
			}
		}
		
	} else {
		tmp->vars = NULL;
		tmp->has_scalar_dim = 0;
	}
	if(tmp->n_file_atts != 0 ) {
		int attr_ix;
		stepalptr = &(tmp->file_atts);
		for(i = 0; i < tmp->n_file_atts; i++) {
			*stepalptr = (HDFAttInqRecList*)NclMalloc(
				(unsigned)sizeof(HDFAttInqRecList));
			(*stepalptr)->att_inq = (HDFAttInqRec*)NclMalloc(
				(unsigned)sizeof(HDFAttInqRec));
			(*stepalptr)->next = NULL;
			sd_ncattname(cdfid,NC_GLOBAL,i,buffer);
			(*stepalptr)->att_inq->att_num = i;
			(*stepalptr)->att_inq->hdf_name = NrmStringToQuark(buffer);
			(*stepalptr)->att_inq->name = HDFToNCLName(buffer,NULL,False);
			(*stepalptr)->att_inq->varid = NC_GLOBAL;
			(*stepalptr)->att_inq->attr_ix = SDfindattr(sd_id,buffer);
			sd_ncattinq(cdfid,NC_GLOBAL,buffer,
					&((*stepalptr)->att_inq->data_type),
                                	&((*stepalptr)->att_inq->len));
			HDF_SDGetAttrVal(sd_id,(*stepalptr)->att_inq);
       	        	stepalptr = &((*stepalptr)->next);
		}
	} else {
		tmp->file_atts = NULL;
	}
	status = SDend (sd_id);
	sd_ncclose(cdfid);
	return((void*)tmp);
}

static void *HDFCreateFile
#if	NhlNeedProto
(void *rec,NclQuark path)
#else
(rec,path)
void *rec;
NclQuark path;
#endif
{
	int id = 0;
	
	id = sd_nccreate(NrmQuarkToString(path),NC_NOCLOBBER);

	if(id > -1) {
		sd_ncendef(id);
		sd_ncclose(id);
		return(HDFOpenFile(rec,path,-1));
	} else {
		return(NULL);
	}
}

static void HDFFreeFileRec
#if	NhlNeedProto
(void* therec)
#else
(therec)
void *therec;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
        HDFVarInqRecList *stepvl;
        HDFDimInqRecList *stepdl;

	stepal = rec->file_atts;
	while(rec->file_atts != NULL) {
		stepal = rec->file_atts;
		if (stepal->att_inq->value)
			NclFree(stepal->att_inq->value);
		NclFree(stepal->att_inq);
		rec->file_atts = rec->file_atts->next;
		NclFree(stepal);
	}
	stepdl = rec->dims;
	while(rec->dims != NULL) {
		stepdl = rec->dims;
		NclFree(stepdl->dim_inq);
		rec->dims= rec->dims->next;
		NclFree(stepdl);
	}

	while(rec->vars != NULL) {
		stepvl = rec->vars;
		while(stepvl->var_inq->att_list != NULL) {
			stepal = stepvl->var_inq->att_list;
			if (stepvl->var_inq->att_list->att_inq->value)
				NclFree(stepvl->var_inq->att_list->att_inq->value);
			NclFree(stepvl->var_inq->att_list->att_inq);
			stepvl->var_inq->att_list = stepal->next;
			NclFree(stepal);
		}
		NclFree(stepvl->var_inq);
		rec->vars = rec->vars->next;
		NclFree(stepvl);
	}
	NclFree(rec);
	return;
}

static NclQuark* HDFGetVarNames
#if	NhlNeedProto
(void* therec, int *num_vars)
#else
(therec, num_vars)
void* therec;
int *num_vars;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	NclQuark *out_quarks;
	HDFVarInqRecList *stepvl;
	int i;

	out_quarks = (NclQuark*)NclMalloc((unsigned)sizeof(NclQuark)*rec->n_vars);
	stepvl = rec->vars;
	for(i = 0; i < rec->n_vars; i++) {
		out_quarks[i] = stepvl->var_inq->name;
		stepvl=stepvl->next;
	}
	*num_vars = rec->n_vars;;
	return(out_quarks);
}

static NclFVarRec *HDFGetVarInfo
#if	NhlNeedProto
(void *therec, NclQuark var_name)
#else
(therec, var_name)
void *therec;
NclQuark var_name;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFVarInqRecList *stepvl;
	HDFDimInqRecList *stepdl;
	NclFVarRec *tmp;
	int i,j;

	stepvl = rec->vars;
	while(stepvl != NULL) {
		if(stepvl->var_inq->name == var_name) {
			tmp = (NclFVarRec*)NclMalloc((unsigned)sizeof(NclFVarRec));
			tmp->var_name_quark = stepvl->var_inq->name;
			tmp->data_type = HDFMapToNcl((void*)&(stepvl->var_inq->data_type));
			tmp->num_dimensions = stepvl->var_inq->n_dims;
			for(j=0; j< stepvl->var_inq->n_dims; j++) {
				stepdl = rec->dims;
				while(stepdl->dim_inq->dimid != stepvl->var_inq->dim[j]) {
					stepdl = stepdl->next;
				}
/*
				tmp->dim_sizes[j] = stepdl->dim_inq->size;
*/
				if(stepdl->dim_inq->dimid == -5) {
					tmp->file_dim_num[j] = 0;
				} else if(rec->has_scalar_dim) {
					tmp->file_dim_num[j] = stepdl->dim_inq->dimid + 1;
				} else {
					tmp->file_dim_num[j] = stepdl->dim_inq->dimid;
				}
			}
			return(tmp);
		} else {
			stepvl = stepvl->next;
		}
	}
	return(NULL);
}

static NclQuark *HDFGetDimNames
#if	NhlNeedProto
(void* therec, int* num_dims)
#else
(therec,num_dims)
void *therec;
int *num_dims;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	NclQuark *out_quarks;
	HDFDimInqRecList *stepdl;
	int i;

	out_quarks = (NclQuark*)NclMalloc((unsigned)sizeof(NclQuark)*rec->n_dims);
	stepdl = rec->dims;
	for(i = 0; i < rec->n_dims; i++) {
		out_quarks[i] = stepdl->dim_inq->name;
		stepdl=stepdl->next;
	}
	*num_dims = rec->n_dims;;
	return(out_quarks);
}

static NclFDimRec *HDFGetDimInfo
#if	NhlNeedProto
(void* therec, NclQuark dim_name_q)
#else
(therec,dim_name_q)
void* therec;
NclQuark dim_name_q;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	NclFDimRec *tmp;
	HDFDimInqRecList *stepdl;

	stepdl = rec->dims;
	while(stepdl != NULL) {
		if(stepdl->dim_inq->name == dim_name_q) {
			tmp = (NclFDimRec*)NclMalloc((unsigned)sizeof(NclFDimRec));
			tmp->dim_name_quark = dim_name_q;
			tmp->dim_size = stepdl->dim_inq->size;
			tmp->is_unlimited  = stepdl->dim_inq->is_unlimited;
			return(tmp);
		} else {
			stepdl = stepdl->next;
		}
	}
	return(NULL);
}
static NclQuark *HDFGetAttNames
#if	NhlNeedProto
(void* therec,int *num_atts)
#else
(therec,num_atts)
void* therec;
int *num_atts;
#endif
{	
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
	NclQuark *out_list = NULL;
	int i;

	out_list = (NclQuark*)NclMalloc((unsigned)sizeof(NclQuark)*rec->n_file_atts);
	stepal = rec->file_atts;
	for(i = 0; i< rec->n_file_atts; i++) {
		out_list[i] = stepal->att_inq->name;
		stepal = stepal->next;
	}
	*num_atts = rec->n_file_atts;
	return(out_list);
}

static NclFAttRec* HDFGetAttInfo
#if	NhlNeedProto
(void* therec, NclQuark att_name_q)
#else
(therec, att_name_q)
void* therec;
NclQuark att_name_q;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
	NclFAttRec *tmp;

	stepal = rec->file_atts;
	while(stepal != NULL) {
		if(stepal->att_inq->name == att_name_q) {
			tmp=(NclFAttRec*)NclMalloc((unsigned)sizeof(NclFAttRec));
			tmp->att_name_quark = att_name_q;
/*
* For conveniesd_nce I make all character attributes strings
*/
			if(stepal->att_inq->data_type == NC_CHAR) {
				tmp->data_type = NCL_string;
				tmp->num_elements = 1;
			} else {
				tmp->data_type = HDFMapToNcl((void*)&(stepal->att_inq->data_type));
				tmp->num_elements = stepal->att_inq->len;
			}
			return(tmp);
		} else {
			stepal = stepal->next;
		}
	}

	return(NULL);
}

static NclQuark *HDFGetVarAttNames
#if	NhlNeedProto
(void *therec , NclQuark thevar, int* num_atts)
#else
(therec , thevar, num_atts)
void *therec;
NclQuark thevar;
int* num_atts;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFVarInqRecList *stepvl;
	HDFAttInqRecList *stepal;
	NclQuark *out_list = NULL;	
	int i;
	stepvl = rec->vars;
	while(stepvl != NULL) {
		if(stepvl->var_inq->name== thevar) {
			stepal = stepvl->var_inq->att_list;
			out_list = (NclQuark*)NclMalloc((unsigned)sizeof(NclQuark) * stepvl->var_inq->natts);
			*num_atts = stepvl->var_inq->natts;
			for(i = 0; i< stepvl->var_inq->natts; i++) {
				out_list[i] = stepal->att_inq->name;
				stepal = stepal->next;
			}
			return(out_list);
		} else {
			stepvl = stepvl->next;
		}
	}
		
	return(NULL);
}

static NclFAttRec *HDFGetVarAttInfo
#if	NhlNeedProto
(void *therec, NclQuark thevar, NclQuark theatt)
#else
(therec, thevar, theatt)
void *therec;
NclQuark thevar;
NclQuark theatt;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFVarInqRecList *stepvl;
	HDFAttInqRecList *stepal;
	NclFAttRec *tmp = NULL;

	stepvl = rec->vars;
	while(stepvl != NULL) {
		if(stepvl->var_inq->name == thevar) {
			stepal = stepvl->var_inq->att_list;
			while(stepal != NULL) {
				if(stepal->att_inq->name == theatt) {
					tmp= (NclFAttRec*)NclMalloc((unsigned)
						sizeof(NclFAttRec));
					tmp->att_name_quark = theatt;
					if(stepal->att_inq->data_type == NC_CHAR) {

						tmp->data_type = NCL_string;
						tmp->num_elements = 1;
					} else {
						tmp->data_type = HDFMapToNcl((void*)&stepal->att_inq->data_type);
						tmp->num_elements = stepal->att_inq->len;
					}
					return(tmp);
				} else {
					stepal = stepal->next;
				}
			}
		} else {
			stepvl = stepvl->next;
		}
	}
		
	return(NULL);
}

static NclFVarRec *HDFGetCoordInfo
#if	NhlNeedProto
(void* therec, NclQuark thevar)
#else
(therec, thevar)
void* therec;
NclQuark thevar;
#endif
{
	return(HDFGetVarInfo(therec,thevar));
}


static void *HDFReadVar
#if	NhlNeedProto
(void* therec, NclQuark thevar, long* start, long* finish,long* stride,void* storage)
#else
(therec, thevar, start, finish,stride,storage)
void* therec;
NclQuark thevar;
long* start;
long* finish;
long* stride;
void* storage;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*) therec;
	HDFVarInqRecList *stepvl;
	void *out_data;
	int n_elem = 1;
	int cdfid = -1;
	int ret = -1,i;
	int no_stride = 1;
	long count[MAX_NC_DIMS];

	stepvl = rec->vars;
	while(stepvl != NULL) {
		if(stepvl->var_inq->name == thevar) {
			for(i= 0; i< stepvl->var_inq->n_dims; i++) {
				count[i] = (int)floor((float)(finish[i] - start[i])/(float)stride[i]) + 1;
				n_elem *= count[i];
				if(stride[i] != 1) {
					no_stride = 0;
				}
			}
			out_data = storage;
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_NOWRITE);
				
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for reading",NrmQuarkToString(rec->file_path_q));
				return(NULL);
			}


			if(no_stride) {	
				ret = sd_ncvargetg(cdfid,
					stepvl->var_inq->varid,
					start,
					count,
					NULL,
					NULL,
					out_data);
			} else {
				ret = sd_ncvargetg(cdfid,
					stepvl->var_inq->varid,
					start,
					count,
					stride,
					NULL,
					out_data);
			}
	
			sd_ncclose(cdfid);
			if(ret == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to read variable (%s) from file (%s)",NrmQuarkToString(thevar),NrmQuarkToString(rec->file_path_q));
				return(NULL);
			} else {
				return(storage);
			}
		} else {
			stepvl = stepvl->next;
		}
	}
	NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Variable (%s) is not an element of file (%s)",NrmQuarkToString(thevar),NrmQuarkToString(rec->file_path_q));
	return(NULL);
}

static void *HDFReadCoord
#if	NhlNeedProto
(void* therec, NclQuark thevar, long* start, long* finish,long* stride,void* storage)
#else
(therec, thevar, start, finish,stride,storage)
void* therec;
NclQuark thevar;
long* start;
long* finish;
long* stride;
void* storage;
#endif
{
	return(HDFReadVar(therec,thevar,start,finish,stride,storage));
}


static void *HDFReadAtt
#if	NhlNeedProto
(void *therec,NclQuark theatt,void* storage)
#else
(therec,theatt,storage)
void *therec;
NclQuark theatt;
void* storage;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
	int cdfid,ret ;
	char *tmp;

	stepal = rec->file_atts;
	while(stepal != NULL) {
		if(stepal->att_inq->name == theatt) {
			if (stepal->att_inq->value != NULL) {
				if(stepal->att_inq->data_type == NC_CHAR) {
					*(string*)storage = *(string*)(stepal->att_inq->value);
				} else {
					memcpy(storage,stepal->att_inq->value,
					       nctypelen(stepal->att_inq->data_type)*stepal->att_inq->len);
				}
				return(storage);
			}
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_NOWRITE);
			
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for reading",NrmQuarkToString(rec->file_path_q));
				return(NULL);
			}
			if(stepal->att_inq->data_type == NC_CHAR) {
				tmp = (char*)NclMalloc(stepal->att_inq->len+1);
				tmp[stepal->att_inq->len] = '\0';
				ret = sd_ncattget(cdfid,NC_GLOBAL,NrmQuarkToString(theatt),tmp);
				*(string*)storage = NrmStringToQuark(tmp);
				NclFree(tmp);
			} else {
				ret = sd_ncattget(cdfid,NC_GLOBAL,NrmQuarkToString(theatt),storage);
			}
			sd_ncclose(cdfid);
			return(storage);
		} else {
			stepal = stepal->next;
		}
	}
	NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Attribute (%s) is not a global attribute of file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(rec->file_path_q));
	return(NULL);
}

static void *HDFReadVarAtt
#if	NhlNeedProto
(void * therec, NclQuark thevar, NclQuark theatt, void * storage)
#else
(therec, thevar, theatt, storage)
void * therec;
NclQuark thevar;
NclQuark theatt;
void* storage;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
	HDFVarInqRecList *stepvl;
	int cdfid;
	int ret;
	char *tmp;

	stepvl = rec->vars;
	while(stepvl != NULL) {
		if(stepvl->var_inq->name == thevar) {
			if (theatt == NrmStringToQuark("hdf_name")) {
				*(string*)storage = stepvl->var_inq->hdf_name;
				return storage;
			}
			else if (theatt == NrmStringToQuark("hdf_group")) {
				*(string*)storage = stepvl->var_inq->var_path;
				return storage;
			}
			stepal = stepvl->var_inq->att_list;
			
			while(stepal != NULL) {
				if(stepal->att_inq->name == theatt) {
					if (theatt == NrmStringToQuark("hdf_group_id")) {
						memcpy(storage,&(stepvl->var_inq->vg_ref),
						       nctypelen(stepal->att_inq->data_type)*stepal->att_inq->len);
						return storage;
					}
					if (stepal->att_inq->value != NULL) {
						if(stepal->att_inq->data_type == NC_CHAR) {
							*(string*)storage = *(string*)(stepal->att_inq->value);
						} else {
							memcpy(storage,stepal->att_inq->value,
							       nctypelen(stepal->att_inq->data_type)*stepal->att_inq->len);
						}
						return(storage);
					}
					cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_NOWRITE);
			
					if(cdfid == -1) {
						NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for reading",NrmQuarkToString(rec->file_path_q));
						return(NULL);
					}
					if(stepal->att_inq->data_type == NC_CHAR) {
	
						tmp = (char*)NclMalloc(stepal->att_inq->len + 1);
						tmp[stepal->att_inq->len] = '\0';
						ret = sd_ncattget(cdfid,stepvl->var_inq->varid,NrmQuarkToString(theatt),tmp);
						*(string*)storage = NrmStringToQuark(tmp);
						NclFree(tmp);
					
						
						
					} else {
						ret = sd_ncattget(cdfid,stepvl->var_inq->varid,NrmQuarkToString(theatt),storage);
					}
					sd_ncclose(cdfid);
					if(ret != -1)
						return(storage);
				} else {
					stepal = stepal->next;
				}
			}
			break;
		} else {
			stepvl = stepvl->next;
		}
	}
	NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Attribute (%s) is not a variable attribute of (%s->%s)",NrmQuarkToString(theatt),NrmQuarkToString(rec->file_path_q),NrmQuarkToString(thevar));
	return(NULL);
}
static NhlErrorTypes HDFWriteVar
#if	NhlNeedProto
(void * therec, NclQuark thevar, void *data, long* start, long *finish,long *stride)
#else
(therec, thevar, data, start, finish,stride)
void * therec;
NclQuark thevar;
void *data;
long *start;
long *finish;
long *stride;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	int cdfid;
	HDFVarInqRecList *stepvl; 
	long count[MAX_NC_DIMS];
	int i,n_elem = 1,no_stride = 1;
	int ret;

	if(rec->wr_status <= 0) {
		stepvl = rec->vars;
		while(stepvl != NULL) {
			if(stepvl->var_inq->name == thevar) {
				for(i= 0; i< stepvl->var_inq->n_dims; i++) {
					count[i] = (int)floor((float)(finish[i] - start[i])/(float)stride[i]) + 1;
					n_elem *= count[i];
					if(stride[i] != 1) {
						no_stride = 0;
					}
				}
				cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
				
				if(cdfid == -1) {
					NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
					return(NhlFATAL);
				}

	
				if(no_stride) {
					ret = sd_ncvarputg(cdfid,
						stepvl->var_inq->varid,
						start,
						count,
						NULL,
						NULL,
						data);
				} else {
					ret = sd_ncvarputg(cdfid,
						stepvl->var_inq->varid,
						start,
						count,
						stride,
						NULL,
						data);
				}
	
				sd_ncclose(cdfid);
				if(ret == -1) {
					NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to write variable (%s) from file (%s)",NrmQuarkToString(thevar),NrmQuarkToString(rec->file_path_q));
					return(NhlFATAL);
				} else {
					return(NhlNOERROR);
				}
			} else {
				stepvl = stepvl->next;
			}
		}
		
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);

	
}
static NhlErrorTypes HDFWriteCoord
#if	NhlNeedProto
(void *therec, NclQuark thevar, void* data, long* start, long* finish,long* stride)
#else
(therec, thevar, data, start, finish,stride)
void *therec;
NclQuark thevar;
void* data;
long* start;
long* finish;
long* stride;
#endif
{
	return(HDFWriteVar(therec,thevar,data,start,finish,stride));
}


static NhlErrorTypes HDFWriteAtt
#if	NhlNeedProto
(void *therec, NclQuark theatt, void *data )
#else
(therec, theatt, data )
void *therec;
NclQuark theatt;
void *data;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
	int cdfid;
	int ret = -1;
	int redef;
	char *buffer=NULL;

	if(rec->wr_status <= 0) {
		stepal = rec->file_atts;
		while(stepal != NULL) {
			if(stepal->att_inq->name == theatt) {
				cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
				if(cdfid == -1) {
					NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
					return(NhlFATAL);
				}
				if(stepal->att_inq->data_type == NC_CHAR) {
					buffer = NrmQuarkToString(*(NclQuark*)data);
					redef = 0;
					if(strlen(buffer)+1 > stepal->att_inq->len) {
						sd_ncredef(cdfid);
						redef = 1;
					}
					ret = sd_ncattput(cdfid,NC_GLOBAL,NrmQuarkToString(theatt),stepal->att_inq->data_type,strlen(buffer)+1,(void*)buffer);
					if (redef)
						sd_ncendef(cdfid);
					if (stepal->att_inq->value != NULL)
						memcpy(stepal->att_inq->value,data,sizeof(NclQuark));
					
				} else {
					ret = sd_ncattput(cdfid,NC_GLOBAL,NrmQuarkToString(theatt),stepal->att_inq->data_type,stepal->att_inq->len,data);
					memcpy(stepal->att_inq->value,data,
					       nctypelen(stepal->att_inq->data_type)*stepal->att_inq->len);
				}
	
	
				sd_ncclose(cdfid);
				if(ret == -1) {
					NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to write the attribute (%s) to file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(rec->file_path_q));
					return(NhlFATAL);
				}
				return(NhlNOERROR);
			} else {
				stepal = stepal->next;
			}
		}	
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFDelAtt
#if 	NhlNeedProto
(void *therec, NclQuark theatt)
#else 
(therec, theatt)
void *therec;
NclQuark thevar;
NclQuark theatt;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal,*tmpal;
	int cdfid;
	int ret;

	if(rec->wr_status <= 0) {
		stepal = rec->file_atts;
		if((stepal != NULL) && (stepal->att_inq->name == theatt)) {
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
				return(NhlFATAL);
			}
			sd_ncredef(cdfid);
			ret = sd_ncattdel(cdfid,NC_GLOBAL,(const char*)NrmQuarkToString(theatt));
			sd_ncendef(cdfid);
	
			tmpal = stepal;
			rec->file_atts = stepal->next;
			if(tmpal->att_inq->value) 
				NclFree(tmpal->att_inq->value);
			NclFree(tmpal->att_inq);
			NclFree(tmpal);
			sd_ncclose(cdfid);
			if(ret == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to delete the attribute (%s) from file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(rec->file_path_q));
				return(NhlFATAL);
			}
			return(NhlNOERROR);
		} else {
			while(stepal->next != NULL) {
				if(stepal->next->att_inq->name == theatt) {
					cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
					if(cdfid == -1) {
						NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
						return(NhlFATAL);
					}

					sd_ncredef(cdfid);
					ret = sd_ncattdel(cdfid,NC_GLOBAL,(const char*)NrmQuarkToString(theatt));
					sd_ncendef(cdfid);
					tmpal = stepal->next;
					stepal->next = stepal->next->next;
					if(tmpal->att_inq->value) 
						NclFree(tmpal->att_inq->value);
					NclFree(tmpal->att_inq);
					NclFree(tmpal);
					sd_ncclose(cdfid);
					if(ret == -1) {
						NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to delete the attribute (%s) from file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(rec->file_path_q));
						return(NhlFATAL);
					}
					return(NhlNOERROR);
				} else {	
					stepal = stepal->next;
				}
			}	
		}
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFDelVarAtt
#if 	NhlNeedProto
(void *therec, NclQuark thevar, NclQuark theatt)
#else 
(therec, thevar, theatt)
void *therec;
NclQuark thevar;
NclQuark theatt;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal,*tmpal;
	HDFVarInqRecList *stepvl;
	int cdfid;
	int ret;

	if(rec->wr_status <= 0) {
		stepvl = rec->vars;
		while(stepvl != NULL) {
			if(stepvl->var_inq->name == thevar) {
				stepal = stepvl->var_inq->att_list;
				if((stepal != NULL) && (stepal->att_inq->name == theatt)) {
					cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
					if(cdfid == -1) {
						NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
						return(NhlFATAL);
					}
					sd_ncredef(cdfid);
					ret = sd_ncattdel(cdfid,stepvl->var_inq->varid,(const char*)NrmQuarkToString(theatt));
					sd_ncendef(cdfid);
			
					tmpal = stepal;
					stepvl->var_inq->att_list = stepal->next;
					if(tmpal->att_inq->value) 
						NclFree(tmpal->att_inq->value);
					NclFree(tmpal->att_inq);
					NclFree(tmpal);
					sd_ncclose(cdfid);
					if(ret == -1) {
						NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to delete the attribute (%s) from variable (%s) in file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(thevar),NrmQuarkToString(rec->file_path_q));
						return(NhlFATAL);
					}
					return(NhlNOERROR);
				} else {
					while(stepal->next != NULL) {
						if(stepal->next->att_inq->name == theatt) {
							cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
							if(cdfid == -1) {
								NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
								return(NhlFATAL);
							}

							sd_ncredef(cdfid);
							ret = sd_ncattdel(cdfid,stepvl->var_inq->varid,(const char*)NrmQuarkToString(theatt));
							sd_ncendef(cdfid);
							tmpal = stepal->next;
							stepal->next = stepal->next->next;
							if(tmpal->att_inq->value) 
								NclFree(tmpal->att_inq->value);
							NclFree(tmpal->att_inq);
							NclFree(tmpal);
							sd_ncclose(cdfid);
							if(ret == -1) {
								NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to delete the attribute (%s) from variable (%s) in file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(thevar),NrmQuarkToString(rec->file_path_q));
								return(NhlFATAL);
							}
							return(NhlNOERROR);
						} else {	
							stepal = stepal->next;
						}
					}	
				}
			} else {
				stepvl = stepvl->next;
			}
		} 
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}
static NhlErrorTypes HDFWriteVarAtt 
#if	NhlNeedProto
(void *therec, NclQuark thevar, NclQuark theatt, void* data)
#else
(therec,thevar, theatt,  data )
void *therec;
NclQuark thevar;
NclQuark theatt;
void* data;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFAttInqRecList *stepal;
	HDFVarInqRecList *stepvl;
	int cdfid;
	int ret;
	char * buffer = NULL;
	
	
	/*
	 * The "hdf_name" attribute does not exist for writing
	 */
	if (theatt == NrmStringToQuark("hdf_name"))
		return(NhlNOERROR);
	else if (theatt == NrmStringToQuark("hdf_group"))
		return(NhlNOERROR);
	else if (theatt == NrmStringToQuark("hdf_group_id"))
		return(NhlNOERROR);
	if(rec->wr_status <= 0) {
		stepvl = rec->vars;
		while(stepvl != NULL) {
			if(stepvl->var_inq->name == thevar) {
				stepal = stepvl->var_inq->att_list;
				while(stepal != NULL) {
					if(stepal->att_inq->name == theatt) {
						cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
						if(cdfid == -1) {
							NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
							return(NhlFATAL);
						}
						if(stepal->att_inq->data_type == NC_CHAR) {	
							int redef = 0;
							buffer = NrmQuarkToString(*(NclQuark*)data);
							if(strlen(buffer)  > stepal->att_inq->len) {
								ncredef(cdfid);
								redef = 1;
							}
							ret = sd_ncattput(cdfid,stepvl->var_inq->varid,NrmQuarkToString(theatt),stepal->att_inq->data_type,strlen(buffer),buffer);
							if (redef)
								ncendef(cdfid);

							if (stepal->att_inq->value != NULL)
								memcpy(stepal->att_inq->value,data,sizeof(NclQuark));
						} else {
							ret = sd_ncattput(cdfid,stepvl->var_inq->varid,NrmQuarkToString(theatt),stepal->att_inq->data_type,stepal->att_inq->len,data);
							if (stepal->att_inq->value != NULL) {
								memcpy(stepal->att_inq->value,data,
								       nctypelen(stepal->att_inq->data_type)*stepal->att_inq->len);
							}
						}
		
						sd_ncclose(cdfid);
						if(ret == -1) {
							NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: An error occurred while attempting to write the attribute (%s) to variable (%s) in file (%s)",NrmQuarkToString(theatt),NrmQuarkToString(thevar),NrmQuarkToString(rec->file_path_q));
							return(NhlFATAL);
						}
						return(NhlNOERROR);
					} else {	
						stepal = stepal->next;
					}
				}	
			} else {
				stepvl = stepvl->next;
			}
		} 
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFAddDim
#if	NhlNeedProto
(void* therec, NclQuark thedim, int size,int is_unlimited)
#else
(therec, thedim, size)
void* therec;
NclQuark thedim;
int size;
int is_unlimited;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*) therec;
	int cdfid;
	HDFDimInqRecList *stepdl;
	int ret = -1;
	int add_scalar = 0;

	if(rec->wr_status <=  0) {
		
		if(thedim == NrmStringToQuark("ncl_scalar")) {
			if (size != 1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,
					  "HDF: \"ncl_scalar\" is a reserved file dimension name in NCL, this name can only represent dimensions of size 1");
				return(NhlFATAL);
			}
			add_scalar = 1;
		}
		else {
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
				return(NhlFATAL);
			}
			sd_ncredef(cdfid);
			if (is_unlimited) {
				ret = sd_ncdimdef(cdfid,NrmQuarkToString(thedim),(long)0);
			}
			else {
				ret = sd_ncdimdef(cdfid,NrmQuarkToString(thedim),(long)size);
			}
			sd_ncendef(cdfid);
			sd_ncclose(cdfid);
			if(ret == -1) {
				return(NhlFATAL);
			}

		}
		stepdl = rec->dims;

		if (add_scalar) {
			rec->has_scalar_dim = 1;
			rec->dims = (HDFDimInqRecList*)NclMalloc(
				(unsigned) sizeof(HDFDimInqRecList));
			rec->dims->dim_inq = (HDFDimInqRec*)NclMalloc(
				(unsigned)sizeof(HDFDimInqRec));
			rec->dims->next = stepdl;
			rec->dims->dim_inq->dimid = -5;
			rec->dims->dim_inq->size = 1;
			rec->dims->dim_inq->is_unlimited = 0;
			rec->dims->dim_inq->name = NrmStringToQuark("ncl_scalar");
			rec->n_dims++;
		}
		else if(stepdl == NULL) {
			rec->dims = (HDFDimInqRecList*)NclMalloc((unsigned)sizeof(HDFDimInqRecList));
			rec->dims->dim_inq = (HDFDimInqRec*)NclMalloc((unsigned)sizeof(HDFDimInqRec));
			rec->dims->dim_inq->dimid = ret;
			rec->dims->dim_inq->name = thedim;
			rec->dims->dim_inq->size = (long)size;
			rec->dims->dim_inq->is_unlimited = is_unlimited;
			rec->dims->next = NULL;
			rec->n_dims = 1;
		} else {
			while(stepdl->next != NULL) {
				stepdl = stepdl->next;
			}
			stepdl->next = (HDFDimInqRecList*)NclMalloc((unsigned)sizeof(HDFDimInqRecList));
			stepdl->next->dim_inq = (HDFDimInqRec*)NclMalloc((unsigned)sizeof(HDFDimInqRec));
			stepdl->next->dim_inq->dimid = ret;
			stepdl->next->dim_inq->name = thedim;
			stepdl->next->dim_inq->size = (long)size;
			stepdl->next->dim_inq->is_unlimited = is_unlimited;
			stepdl->next->next = NULL;
			rec->n_dims++;
		}	
		return(NhlNOERROR);
	} else {	
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}
/*ARGSUSED*/
static NhlErrorTypes HDFAddVar
#if	NhlNeedProto
(void* therec, NclQuark thevar, NclBasicDataTypes data_type, int n_dims,NclQuark *dim_names, long* dim_sizes)
#else
(therec,thevar,data_type,n_dims,dim_names,dim_sizes)
void* therec;
NclQuark thevar;
NclBasicDataTypes data_type;
int n_dims;
NclQuark *dim_names;
long* dim_sizes;
#endif
{
	HDFFileRecord* rec = (HDFFileRecord*)therec;
	HDFVarInqRecList *stepvl = NULL;
	int cdfid,i,ret;
	nc_type *the_data_type;
	int dim_ids[MAX_NC_DIMS];
	HDFDimInqRecList* stepdl = NULL;
	int add_scalar_dim = 0;

	if(rec->wr_status <= 0) {
		cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
		if(cdfid == -1) {
			NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
			return(NhlFATAL);
		}
		the_data_type = HDFMapFromNcl(data_type);
/*
* All dimensions are correct dimensions for the file
*/
		dim_ids[0] = -999;
		for(i = 0; i < n_dims; i++) {
			stepdl = rec->dims;
			while(stepdl != NULL) {
				if(stepdl->dim_inq->name == dim_names[i]){
					if((n_dims > 1)&&(dim_names[i] == NrmStringToQuark("ncl_scalar"))) {
						NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: the reserved dimension name \"ncl_scalar\" was used in a value with more than one dimension, can not add variable");
						return(NhlFATAL);
					}
					dim_ids[i] = stepdl->dim_inq->dimid;
					break;
				} else {
					stepdl = stepdl->next;
				}
			}
		} 
		if (dim_ids[0] == -999) {
			if (n_dims == 1 && dim_sizes[0] == 1 && dim_names[0] == NrmStringToQuark("ncl_scalar")) {
				dim_ids[0] = -5;
				add_scalar_dim = 1;
			}
			else {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: internal error adding variable");
				return(NhlFATAL);
			}
		}

		if(the_data_type != NULL) {
			sd_ncredef(cdfid);
			if((n_dims == 1)&&(dim_ids[0] == -5)) {
				ret = sd_ncvardef(cdfid,NrmQuarkToString(thevar),*the_data_type, 0, NULL);
			} else {
				ret = sd_ncvardef(cdfid,NrmQuarkToString(thevar),*the_data_type, n_dims, dim_ids);
			}
			sd_ncendef(cdfid);
			sd_ncclose(cdfid);
			if(ret == -1) {
				NclFree(the_data_type);
				return(NhlFATAL);
			} 
	
			stepvl = rec->vars;
			if(stepvl == NULL) {
				rec->vars = (HDFVarInqRecList*)NclMalloc(
                                        (unsigned)sizeof(HDFVarInqRecList));
				rec->vars->next = NULL;
				rec->vars->var_inq = (HDFVarInqRec*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRec));
				rec->vars->var_inq->varid = ret;
				rec->vars->var_inq->name = thevar;
				rec->vars->var_inq->data_type = *the_data_type;
				rec->vars->var_inq->n_dims = n_dims;
				rec->vars->var_inq->natts = 0;
				rec->vars->var_inq->att_list = NULL;
				for(i = 0 ; i< n_dims; i++) {
					rec->vars->var_inq->dim[i] = dim_ids[i];
				}
				rec->n_vars = 1;
			} else {
				while(stepvl->next != NULL) {
					stepvl= stepvl->next;
				}
				stepvl->next = (HDFVarInqRecList*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRecList));
				stepvl->next->var_inq = (HDFVarInqRec*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRec));
				stepvl->next->next = NULL;
				stepvl->next->var_inq->varid = ret;
				stepvl->next->var_inq->name = thevar;
				stepvl->next->var_inq->data_type = *the_data_type;
				stepvl->next->var_inq->n_dims = n_dims;
				stepvl->next->var_inq->natts = 0;
				stepvl->next->var_inq->att_list = NULL;
				for(i = 0 ; i< n_dims; i++) {
					stepvl->next->var_inq->dim[i] = dim_ids[i];
				}
				rec->n_vars++;
			}
			if (add_scalar_dim) {
				rec->has_scalar_dim = 1;
				stepdl = rec->dims;
				rec->dims = (HDFDimInqRecList*)NclMalloc(
					(unsigned) sizeof(HDFDimInqRecList));
				rec->dims->dim_inq = (HDFDimInqRec*)NclMalloc(
					(unsigned)sizeof(HDFDimInqRec));
				rec->dims->next = stepdl;
				rec->dims->dim_inq->dimid = -5;
				rec->dims->dim_inq->size = 1;
				rec->dims->dim_inq->is_unlimited = 0;
				rec->dims->dim_inq->name = NrmStringToQuark("ncl_scalar");
				rec->n_dims++;
			}

			NclFree(the_data_type);
			return(NhlNOERROR);
		} else {
			sd_ncclose(cdfid);
		}
	} else {	
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFAddCoordVar
#if	NhlNeedProto
(void *therec, NclQuark thevar,NclBasicDataTypes data_type)
#else
(therec,thevar,data_type)
void *therec;
NclQuark thevar;
NclBasicDataTypes data_type;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFDimInqRecList *stepdl = NULL;
	HDFVarInqRecList *stepvl = NULL;
	int cdfid;
	int ret,size;
	nc_type *the_data_type;
	

	if(rec->wr_status <= 0) {
		cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
		if(cdfid == -1) {
			NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
			return(NhlFATAL);
		}
		the_data_type = HDFMapFromNcl(data_type);
		if(the_data_type != NULL) {
			stepdl = rec->dims;
			while(stepdl != NULL ) {
				if(stepdl->dim_inq->name == thevar){
					sd_ncredef(cdfid);
					size = stepdl->dim_inq->size;
					ret = sd_ncvardef(cdfid,NrmQuarkToString(thevar),*the_data_type,1,&size);
					if(ret == -1) {
						sd_ncabort(cdfid);
						sd_ncclose(cdfid);
						NclFree(the_data_type);
						return(NhlFATAL);
					} 
				}
			} 
			stepvl = rec->vars;
			if(stepvl == NULL) {
				rec->vars = (HDFVarInqRecList*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRecList));
				rec->vars->next = NULL;
				rec->vars->var_inq = (HDFVarInqRec*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRec*));
				rec->vars->var_inq->varid = ret;
				rec->vars->var_inq->name = thevar;
				rec->vars->var_inq->data_type = *the_data_type;
				rec->vars->var_inq->n_dims = 1;
				rec->vars->var_inq->dim[0] = stepdl->dim_inq->dimid;
				rec->vars->var_inq->natts = 0;
				rec->vars->var_inq->att_list = NULL;
				rec->n_vars++;
			} else {
				while(stepvl->next != NULL) {
					stepvl = stepvl->next;
				}
				stepvl->next = (HDFVarInqRecList*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRecList));
				stepvl->next->next = NULL;
				stepvl->next->var_inq = (HDFVarInqRec*)NclMalloc(
					(unsigned)sizeof(HDFVarInqRec*));
				stepvl->next->var_inq->varid = ret;
				stepvl->next->var_inq->name = thevar;
				stepvl->next->var_inq->data_type = *the_data_type;
				stepvl->next->var_inq->n_dims = 1;
				stepvl->next->var_inq->dim[0] = stepdl->dim_inq->dimid;
				stepvl->next->var_inq->natts = 0;
				stepvl->next->var_inq->att_list = NULL;
				rec->n_vars++;
			}
			NclFree(the_data_type);
		} else {
			sd_ncclose(cdfid);
		}
	} else {	
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFRenameDim
#if	NhlNeedProto
(void* therec, NclQuark from, NclQuark to)
#else
(therec, from, to)
void* therec;
NclQuark from;
NclQuark to;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFDimInqRecList *stepdl;
	int cdfid,ret;

	if(to == NrmStringToQuark("ncl_scalar")) {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF : \"ncl_scalar\" is a reserved file dimension name in NCL: other dimensions can not be changed to it");
                return(NhlFATAL);
	}
	stepdl = rec->dims;
	while(stepdl != NULL) {
		if(stepdl->dim_inq->name == from) {
			if(stepdl->dim_inq->dimid == -5) {
				stepdl->dim_inq->name = to;
				return(NhlNOERROR);
			}
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
				return(NhlFATAL);
			}
			sd_ncredef(cdfid);
			ret = sd_ncdimrename(cdfid,stepdl->dim_inq->dimid,NrmQuarkToString(to));
			sd_ncclose(cdfid);
			if(ret == -1) {
				return(NhlFATAL);
			} else {
				stepdl->dim_inq->name = to;
				return(NhlNOERROR);
			}
		} else {
			stepdl = stepdl->next;
		}
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFAddAtt
#if	NhlNeedProto
(void *therec,NclQuark theatt, NclBasicDataTypes data_type, int n_items, void * values)
#else
(therec,theatt,data_type,n_items,values)
	void *therec;
	NclQuark theatt;
	NclBasicDataTypes data_type;
	int n_items;
	void * values;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFAttInqRecList* stepal;
	nc_type *the_data_type;
	int i,ret;
	int cdfid;
	

	if(rec->wr_status <= 0) {
		the_data_type = (nc_type*)HDFMapFromNcl(data_type);
		if(the_data_type != NULL) {
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
				NclFree(the_data_type);
				return(NhlFATAL);
			}
			sd_ncredef(cdfid);
			ret = sd_ncattput(cdfid,NC_GLOBAL,NrmQuarkToString(theatt),*the_data_type,n_items,values);
			sd_ncendef(cdfid);
			sd_ncclose(cdfid);
			if(ret != -1 ) {
				stepal = rec->file_atts;
				if(stepal == NULL) {
					rec->file_atts = (HDFAttInqRecList*)NclMalloc((unsigned)
						sizeof(HDFAttInqRecList));
					rec->file_atts->att_inq= (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
					rec->file_atts->next = NULL;
					rec->file_atts->att_inq->att_num = 1;
					rec->file_atts->att_inq->name = theatt;
					rec->file_atts->att_inq->data_type = *the_data_type;
					rec->file_atts->att_inq->len = n_items;
					HDFCacheAttValue(rec->file_atts->att_inq,values);
				} else {	
					i = 0;
					while(stepal->next != NULL) {
						stepal = stepal->next; 
						i++;
					}
					stepal->next = (HDFAttInqRecList*)NclMalloc((unsigned)sizeof(HDFAttInqRecList));
					stepal->next->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
					stepal->next->att_inq->att_num = i;
					stepal->next->att_inq->name = theatt;
					stepal->next->att_inq->data_type = *the_data_type;
					stepal->next->att_inq->len = n_items;
					stepal->next->next = NULL;
					HDFCacheAttValue(stepal->next->att_inq,values);
				}
				rec->n_file_atts++;
				NclFree(the_data_type);
				return(NhlNOERROR);
			} 
		} 
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}

static NhlErrorTypes HDFAddVarAtt
#if	NhlNeedProto
(void *therec,NclQuark thevar, NclQuark theatt, NclBasicDataTypes data_type, int n_items, void * values)
#else
(therec,thevar,theatt,data_type,n_items,values)
	void *therec;
	NclQuark thevar;
	NclQuark theatt;
	NclBasicDataTypes data_type;
	int n_items;
	void * values;
#endif
{
	HDFFileRecord *rec = (HDFFileRecord*)therec;
	HDFAttInqRecList* stepal;
	HDFVarInqRecList* stepvl;
	nc_type *the_data_type;
	int i;
	int cdfid,ret;
	
	if(rec->wr_status <= 0) {
		the_data_type = (nc_type*)HDFMapFromNcl(data_type);
		if(the_data_type != NULL) {
			cdfid = sd_ncopen(NrmQuarkToString(rec->file_path_q),NC_WRITE);
			if(cdfid == -1) {
				NhlPError(NhlFATAL,NhlEUNKNOWN,"HDF: Could not reopen the file (%s) for writing",NrmQuarkToString(rec->file_path_q));
				NclFree(the_data_type);
				return(NhlFATAL);
			}
			stepvl = rec->vars;	
			while(stepvl != NULL) {
				if(stepvl->var_inq->name == thevar) {
					break;
				} else {
					stepvl = stepvl->next;
				}
			}
			sd_ncredef(cdfid);
			ret = sd_ncattput(cdfid,stepvl->var_inq->varid,NrmQuarkToString(theatt),*the_data_type,n_items,values);
			sd_ncendef(cdfid);
			sd_ncclose(cdfid);
			if(ret != -1 ) {
				stepal = stepvl->var_inq->att_list;
				if(stepal == NULL) {
					stepvl->var_inq->att_list= (HDFAttInqRecList*)NclMalloc((unsigned)
						sizeof(HDFAttInqRecList));
					stepvl->var_inq->att_list->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
					stepvl->var_inq->att_list->next = NULL;
					stepvl->var_inq->att_list->att_inq->att_num = 0;
					stepvl->var_inq->att_list->att_inq->name = theatt;
					stepvl->var_inq->att_list->att_inq->data_type = *the_data_type;
					stepvl->var_inq->att_list->att_inq->len = n_items;
					HDFCacheAttValue(stepvl->var_inq->att_list->att_inq,values);
					stepvl->var_inq->natts = 1;
				} else {	
					i = 0;
					while(stepal->next != NULL) {
						stepal = stepal->next; 
						i++;
					}
					stepal->next = (HDFAttInqRecList*)NclMalloc((unsigned)sizeof(HDFAttInqRecList));
					stepal->next->att_inq = (HDFAttInqRec*)NclMalloc((unsigned)sizeof(HDFAttInqRec));
					stepal->next->att_inq->att_num = i;
					stepal->next->att_inq->name = theatt;
					stepal->next->att_inq->data_type = *the_data_type;
					stepal->next->att_inq->len = n_items;
					stepal->next->next = NULL;
					HDFCacheAttValue(stepal->next->att_inq,values);
					stepvl->var_inq->natts++ ;
				}
				NclFree(the_data_type);
				return(NhlNOERROR);
			} 
		} 
	} else {
		NhlPError(NhlFATAL,NhlEUNKNOWN,"File (%s) was opened as a read only file, can not write to it",NrmQuarkToString(rec->file_path_q));
	}
	return(NhlFATAL);
}


NclFormatFunctionRec HDFRec = {
/* NclInitializeFileRecFunc initialize_file_rec */      HDFInitializeFileRec,
/* NclCreateFileFunc	   create_file; */		HDFCreateFile,
/* NclOpenFileFunc         open_file; */		HDFOpenFile,
/* NclFreeFileRecFunc      free_file_rec; */		HDFFreeFileRec,
/* NclGetVarNamesFu_nc      get_var_names; */		HDFGetVarNames,
/* NclGetVarInfoFusd_nc       get_var_info; */		HDFGetVarInfo,
/* NclGetDimNamesFusd_nc      get_dim_names; */		HDFGetDimNames,
/* NclGetDimInfoFusd_nc       get_dim_info; */		HDFGetDimInfo,
/* NclGetAttNamesFusd_nc      get_att_names; */		HDFGetAttNames,
/* NclGetAttInfoFusd_nc       get_att_info; */		HDFGetAttInfo,
/* NclGetVarAttNamesFusd_nc   get_var_att_names; */	HDFGetVarAttNames,
/* NclGetVarAttInfoFusd_nc    get_var_att_info; */		HDFGetVarAttInfo,
/* NclGetCoordInfoFusd_nc     get_coord_info; */		HDFGetCoordInfo,
/* NclReadCoordFusd_nc        read_coord; */		HDFReadCoord,
/* NclReadCoordFusd_nc        read_coord; */		NULL,
/* NclReadVarFusd_nc          read_var; */			HDFReadVar,
/* NclReadVarFusd_nc          read_var; */			NULL,
/* NclReadAttFusd_nc          read_att; */			HDFReadAtt,
/* NclReadVarAttFusd_nc       read_var_att; */		HDFReadVarAtt,
/* NclWriteCoordFusd_nc       write_coord; */		HDFWriteCoord,
/* NclWriteCoordFusd_nc       write_coord; */		NULL,
/* NclWriteVarFusd_nc         write_var; */		HDFWriteVar,
/* NclWriteVarFusd_nc         write_var; */		NULL,
/* NclWriteAttFusd_nc         write_att; */		HDFWriteAtt,
/* NclWriteVarAttFusd_nc      write_var_att; */		HDFWriteVarAtt,
/* NclAddDimFusd_nc           add_dim; */			HDFAddDim,
/* NclAddDimFusd_nc           rename_dim; */		HDFRenameDim,
/* NclAddVarFusd_nc           add_var; */			HDFAddVar,
/* NclAddVarFusd_nc           add_coord_var; */		HDFAddCoordVar,
/* NclAddAttFusd_nc           add_att; */			HDFAddAtt,
/* NclAddVarAttFusd_nc        add_var_att; */		HDFAddVarAtt,
/* NclMapFormatTypeToNcl   map_format_type_to_sd_ncl; */	HDFMapToNcl,
/* NclMapNclTypeToFormat   map_sd_ncl_type_to_format; */	HDFMapFromNcl,
/* NclDelAttFusd_nc           del_att; */			HDFDelAtt,
/* NclDelVarAttFusd_nc        del_var_att; */		HDFDelVarAtt,
/* NclSetOptionFunc           set_option;  */           NULL
};
NclFormatFunctionRecPtr HDFAddFileFormat 
#if	NhlNeedProto
(void)
#else 
()
#endif
{
	
	return(&HDFRec);
}
