/*
**      $Id: cn09c.c,v 1.1 1995-09-28 16:36:36 haley Exp $
*/
/***********************************************************************
*                                                                      *
*                Copyright (C)  1995                                   *
*        University Corporation for Atmospheric Research               *
*     The use of this Software is governed by a License Agreement      *
*                                                                      *
***********************************************************************/
/*
**  File:       cn09c.c
**
**  Author:     Ethan Alpert (converted to C by Mary Haley)
**          National Center for Atmospheric Research
**          PO 3000, Boulder, Colorado
**
**  Date:       Thu Sep 28 08:32:16 MDT 1995
**
**  Description:    Reads a netCDF file and produces a series of
**                  surface pressure contour plots.
**
*/


#include <stdio.h>
#include <math.h>
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/ResList.h>
#include <ncarg/hlu/App.h>
#include <ncarg/hlu/XWorkstation.h>
#include <ncarg/hlu/NcgmWorkstation.h>
#include <ncarg/hlu/PSWorkstation.h>
#include <ncarg/hlu/ContourPlot.h>
#include <ncarg/hlu/ScalarField.h>
#include <netcdf.h>

main()
{
/*
 * Declare variables for the HLU routine calls.
 */
    int     appid, workid, field1, con1;
    int     srlist, i, j, k;
/*
 * Declare variables for getting information from netCDF file.
 */
    int     ncid, lon_id, lat_id, frtime_id, press_id, frtime[7];
    float   press[33][36], special_value;
    float   lon[36], lat[33];
    long    start[3], count[3], lonlen, latlen, frtimelen;
    char    filename[256], string[20];
    const char *dir = _NGGetNCARGEnv("data");
/*
 * Default is to create an NCGM file.
 */
    int NCGM=1, X11=0, PS=0;
/*
 * Initialize the HLU library and set up resource template.
 */
    NhlInitialize();
    srlist = NhlRLCreate(NhlSETRL);
/*
 * Create Application object.
 */
    NhlRLClear(srlist);
    NhlRLSetString(srlist,NhlNappDefaultParent,"True");
    NhlRLSetString(srlist,NhlNappUsrDir,"./");
    NhlCreate(&appid,"cn09",NhlappClass,NhlDEFAULT_APP,srlist);

    if (NCGM) {
/*
 * Create a meta file object.
 */
        NhlRLClear(srlist);
        NhlRLSetString(srlist,NhlNwkMetaName,"./cn09c.ncgm");
        NhlCreate(&workid,"cn09Work",NhlncgmWorkstationClass,
                  NhlDEFAULT_APP,srlist);
    }
    else if (X11) {
/*
 * Create an XWorkstation object.
 */
        NhlRLClear(srlist);
        NhlRLSetInteger(srlist,NhlNwkPause,True);
        NhlCreate(&workid,"cn09Work",NhlxWorkstationClass,
              NhlDEFAULT_APP,srlist);
    }
    else if (PS) {
/*
 * Create a PS workstation.
 */
        NhlRLClear(srlist);
        NhlRLSetString(srlist,NhlNwkPSFileName,"./cn09c.ps");
        NhlCreate(&workid,"cn09Work",NhlpsWorkstationClass,
                  NhlDEFAULT_APP,srlist);
    }
/*
 * Open data file containing surface pressure data for
 * entire globe.
 */
    sprintf( filename, "%s/cdf/contour.cdf", dir );
    ncid = ncopen(filename,NC_NOWRITE);
/*
 * Get the lat/lon dimensions.
 */
    lat_id = ncdimid(ncid,"lat");
    lon_id = ncdimid(ncid,"lon");
    frtime_id  = ncdimid(ncid,"frtime");
    ncdiminq(ncid,lat_id,(char *)0,&latlen);
    ncdiminq(ncid,lon_id,(char *)0,&lonlen);
    ncdiminq(ncid,frtime_id,(char *)0,&frtimelen);
/*
 * Read in surface pressure and convert it to millibars.
 */
    press_id = ncvarid(ncid,"Psl");
    start[0] = start[1] = start[2] = 0;
    count[0] = 1; count[1] = latlen; count[2] = lonlen;
    ncvarget(ncid,press_id,(long const *)start,(long const *)count,press);
    ncattget(ncid,press_id,"_FillValue",&special_value);
    for( j = 0; j < latlen; j++ ) {
        for( k = 0; k < lonlen; k++ ) {
            press[j][k] /= 100.;
        }
    }
/*
 * Read in lat/lon/frtime values.
 */
    lat_id = ncvarid(ncid,"lat");
    count[0] = latlen;
    ncvarget(ncid,lat_id,(long const *)start,(long const *)count,lat);

    lon_id = ncvarid(ncid,"lon");
    count[0] = lonlen;
    ncvarget(ncid,lon_id,(long const *)start,(long const *)count,lon);

    frtime_id = ncvarid(ncid,"frtime");
    count[0] = frtimelen;
    ncvarget(ncid,frtime_id,(long const *)start,(long const *)count,frtime);
/*
 * Create scalar field configured with first time step
 * of pressure data.
 */
    count[0] = latlen; count[1] = lonlen;
    NhlRLClear(srlist);
    NhlRLSetMDFloatArray(srlist,NhlNsfDataArray,&press[0][0],2,(int *)count);
    NhlRLSetFloat(srlist,NhlNsfMissingValueV,special_value);
    NhlRLSetFloat(srlist,NhlNsfXCStartV,lon[0]);
    NhlRLSetFloat(srlist,NhlNsfXCEndV,lon[lonlen-1]);
    NhlRLSetFloat(srlist,NhlNsfYCStartV,lat[0]);
    NhlRLSetFloat(srlist,NhlNsfYCEndV,lat[latlen-1]);
    NhlCreate(&field1,"field1",NhlscalarFieldClass,appid,srlist);
/*
 * Create contour object using manual level spacing and solid
 * color fill
 */
    NhlRLClear(srlist);
    NhlRLSetInteger(srlist,NhlNcnScalarFieldData,field1);
    NhlRLSetString(srlist,NhlNcnLevelSelectionMode, "ManualLevels");
    NhlRLSetFloat(srlist,NhlNcnMinLevelValF,960.0);
    NhlRLSetFloat(srlist,NhlNcnMaxLevelValF,1040.0);
    NhlRLSetFloat(srlist,NhlNcnLevelSpacingF,5.0);
    NhlRLSetInteger(srlist,NhlNcnMaxLevelCount,25);
    NhlRLSetString(srlist,NhlNcnInfoLabelOn,"OFF");
    NhlRLSetString(srlist,NhlNcnHighLabelsOn,"OFF");
    NhlRLSetString(srlist,NhlNcnLowLabelsOn,"OFF");
    NhlRLSetString(srlist,NhlNcnLineLabelsOn,"False");
    NhlRLSetString(srlist,NhlNcnLinesOn,"False");
    NhlRLSetString(srlist,NhlNcnFillOn,"True");
    NhlRLSetString(srlist,NhlNtiMainString,"Forecast Time 0");
    NhlCreate(&con1,"con1",NhlcontourPlotClass,workid,srlist);
/* 
 * Draw first frame
 */
    NhlDraw(con1);
    NhlFrame(workid);
/*
 * Loop and draw remaining frames reseting the scalar field object
 * with a new array for each iteration
 */
    for( i = 1; i <= frtimelen-1; i++ ) {
/*
 * Read in surface pressure and convert it to millibars.
 */
        start[0] = i; start[1] = start[2] = 0;
        count[0] = 1; count[1] = latlen; count[2] = lonlen;
        ncvarget(ncid,press_id,(long const *)start,(long const *)count,press);
        for( j = 0; j < latlen; j++ ) {
            for( k = 0; k < lonlen; k++ ) {
                press[j][k] /= 100.;
            }
        }
        NhlRLClear(srlist);
        count[0] = latlen; count[1] = lonlen;
        NhlRLSetMDFloatArray(srlist,NhlNsfDataArray,&press[0][0],2,
                            (int *)count);
/*
 * Create new scalar field.
 */
        NhlSetValues(field1,srlist);
        NhlRLClear(srlist);
        sprintf(string,"Forecast Time %d", frtime[i] );
        NhlRLSetString(srlist,NhlNtiMainString,string);
        NhlSetValues(con1,srlist);
        NhlDraw(con1);
        NhlFrame(workid);
    }
/*
 * Close the netCDF file.
 */
    ncclose(ncid);
/*
 * NhlDestroy destroys the given id and all of its children.
 */
    NhlRLDestroy(srlist);
    NhlDestroy(appid);
/*
 * Restores state.
 */
    NhlClose();
    exit(0);
}
