#ifndef mmbdsql_h
#define mmbdsql_h

/* This file contains definitions required to interface MMBD software to
 * the MySQL interface library.
 *
 * This is a joint development effort between Renee Carter and Pavel Ivanov.
 *    Renee Carter - development of MySQL Interface code.
 *    Pavel Ivanov - development of XL reading and MDS Implemention.
 *
 * Copyright: 11 November 2008
 */

#include <mysql.h>

/* This enum defines the sheets of the MMBD that are captured in MySQL. */
typedef enum mmbdSheetNumber{
    MMBD_SHEET_BOOLEANS,
    MMBD_SHEET_SIZING,
    MMBD_NumMMBDSheets,       /* Real sheets must go above this entry */
    MMBD_SHEET_NONE,
    MMBD_SHEET_UNKNOWN
} mmbdSheetType;

/* This enum defines each record in the Boolean Sheet.
 *
 *    Each element is stored as a string. This allows maximum flexibility.
 *    The string will be either a text string or a 255-character string MySQL.
 *
 */
typedef enum _mmbdsql_booleans_sheet_record {
				/* Begin General Information */
    BooleanProcessStep,		/* 255 */
    BooleanDescription,		/* text */
    BooleanOptions,		/* 255 */
    BooleanRETTech,		/* 255 */
    BooleanRETProcessStep,	/* 255 */
    BooleanProxLayer,		/* 255 */
    BooleanDummy1,		/* Begin OPC Boolean Information */
    BooleanOPCBoolean,		/* text */
    BooleanOPCSGBoolean,	/* text */
    BooleanOPCPassThrough,	/* text */
    BooleanNoOPCLayer,		/* text */
    BooleanOPCOutputLayer,	/* 255 */
    BooleanDummy2,		/* Begin RET Tiling Boolean Information */
    BooleanTilingBoolean,	/* text */
    BooleanRETTilingBoolean,	/* text */
    BooleanTilingExclusion,	/* text */
    BooleanTilingPassThrough,	/* text */
    BooleanNoTilingLayer,	/* text */
    BooleanTilingOutputLayer,	/* text */
    BooleanDummy3,		/* Begin FIN Boolean Information */
    BooleanFINGrid,		/* 255 */
    BooleanFINFractureBoolean,	/* text */
    BooleanFINSGBoolean,	/* text */
    BooleanDummy4,		/* Begin SGPC Boolean Information */
    BooleanSGPCGrid,		/* 255 */
    BooleanSGPCFrctureBoolean,	/* text */
    BooleanSGPCSGBoolean,	/* text */
    BooleanDummy5,		/* Begin Key Boolean Information */
    BooleanKeyGrid,		/* 255 */
    BooleanInputGDSFiles,	/* text */
    BooleanKeyFractureBoolean,	/* text */
    BooleanDummy6,		/* Begin CD Boolean Information */
    BooleanCDFracturePanelGrid, /* 255 */
    BooleanCDInputGDSFiles,	/* text */
    BooleanCDFractureBoolean,	/* text */
    BooleanDummy7,		/* Begin OPC General Information */
    BooleanOPCFormName,		/* 255 */
    BooleanOPCType,		/* 255 */
    BooleanDummy8,		/* Begin Tiling General Information */
    BooleanTilingFormName,	/* 255 */
    BooleanTilingType,		/* 255 */
    BooleantilingVersions,	/* 255 */
    BooleanDummy9,		/* Begin Comments Section*/
    Booleancomments,		/* text */
    NumBooleanFields
} mmbdBooleansSheetFields;

/* This enum defines each record in the Sizing Sheet. *
 *
 *    Each element is stored as a string. This allows maximum flexibility.
 *    The string will be either a text string or a 255-character string MySQL.
 *
 */
typedef enum _mmbdsql_sizing_sheet_record {
    SizingProcessStep,		/* 255 */
    SizingDescription,		/* text */
    SizingOptions,		/* 255 */
    SizingBarcodeRevision,	/* 255 */
    SizingBuildPriority,	/* 255 */
    SizingDensity,		/* 255 */
    SizingChangeType,		/* 255 */
    SizingBaseLayerRef,		/* 255 */
    SizingMaskMag,		/* 255 */
    SizingCDFeature,		/* 255 */
    SizingOverlayAlignsTo,	/* 255 */
    SizingOverlayMeasuresTo,	/* 255 */
    SizingCDMinArea,		/* 255 */
    SizingCDMinWidth,		/* 255 */
    SizingCDKeySubstrate,	/* 255 */
    SizingDummy1,
    SizingTotalSizingDrawn,	/* 255 */
    SizingTotalSizingApp,	/* 255 */
    SizingTotalSizingWafer,	/* 255 */
    SizingPhotoBias,		/* 255 */
    SizingEtchBias,		/* 255 */
    SizingTotalBias,		/* 255 */
    SizingDummy2,
    SizingCDDrawnLineBefore,	/* 255 */
    SizingCDDrwanSpaceBefore,	/* 255 */
    SizingCDDrawnPitchAfter,	/* 255 */
    SizingCDDrawnLineAfter,	/* 255 */
    SizingCDDrwanSpaceAfter,	/* 255 */
    SizingCDDrawnPitchBefore,	/* 255 */
    SizingCDDenseLine,		/* 255 */
    SizingCDIsoLine,		/* 255 */
    SizingCDDenseSpace,		/* 255 */
    SizingCDDensePitch,		/* 255 */
    SizingReticleCDDenseLine,	/* 255 */
    SizingReticleCDIsoLine,	/* 255 */
    SizingReticleCDDenseSpace,	/* 255 */
    SizingReticleCDDensePitch,	/* 255 */
    SizingDummy3,
    SizingADICDWaferScale,	/* 255 */
    SizingADIPlusMinus,		/* 255 */
    SizingADITolerance,		/* 255 */
    SizingACICDWaferScale,	/* 255 */
    SizingACIPlusMinus,		/* 255 */
    SizingACITolerance,		/* 255 */
    SizingDummy4,
    Sizingcomments,		/* text */
    NumSizingFields
} mmbdSizingSheetFields;

typedef struct _mmbdsql_record {
    mmbdSheetType type;
    int field;
    char **fields;
    struct _mmbdsql_record *next;
} mmbdRecord;

/* Prototyes for routines used to manipulate records. */
mmbdSheetType mmbd_record_get_type( mmbdRecord *record );
char *mmbd_record_get_type_string( mmbdRecord *record );
char *mmbd_record_get_table_name( mmbdRecord *record );
int mmbd_record_get_num_fields( mmbdRecord *record );
char **mmbd_record_get_fields( mmbdRecord *record );
int mmbd_record_get_max_fields( mmbdRecord *record );
mmbdRecord *mmbd_record_new( mmbdSheetType type );
int mmbd_record_add_field( mmbdRecord *record, char *value );
void mmbd_record_initialize( mmbdRecord *record );
int mmbd_record_dump( mmbdRecord *record );
void mmbd_record_delete( mmbdRecord *record );
mmbdRecord *mmbd_record_get_next( mmbdRecord *record );

/* Prototypes for inserting records from MySQL. */

/* The following 2 prototypes will probably soon be deleted */
int mmbd_boolean_record_insert(char *record[NumBooleanFields], int num_fields);
int mmbd_sizing_record_insert(char *record[NumSizingFields], int num_fields);

/* Prototypes for retrieving records from MySQL. */
MYSQL_RES * mmbd_result_get(char *db, char *query1);

/* Prototypes from mmbd_sql_lib.c */
/* Get the maximum index from the mmbd_table */
int mmbd_max_index_rtv(void);

/* Map the mmbd name and user version to the mmbd index */
int mmbd_index_rtv(char *mmbd_name, char *user_version);

void core_id_rtv(char *core_id);

/* Retrieve a record from the mmbd database */
mmbdRecord *mmbd_record_rtv( mmbdSheetType type, char *process_step, char *mmbd_name, char *user_version);

/* Retrieve all records from specified table */
int mmbd_records_rtv(char *table, int total_fields);

/* Insert record into any MMBD table that is defined */
int mmbd_record_insert(mmbdRecord *record);

/* Insert info for a new MMBD */
int new_mmbd_insert(char *mmbd_name, char *user_version);

#endif
