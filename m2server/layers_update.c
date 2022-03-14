#ifdef garbage
/****************************************************************************
 *
 * Function:	common_maskset_layers_update, common_template_layers_update, etc.
 *
 * Description:	Updates common_template_layers, common_maskset_layers, etc.
 *
 * Author:	Renee Carter/richard Lee
 *
 ***************************************************************************/

#define CELL_LIST_MAX 500
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_sql_lib.h"
#include "m2globals.h"
#include "m2server_errs.h"
#include "m2server_globals.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "server_params.h"
#include "strcpy.h"
#include "user_params.h"
#include <cell_int.h>
#include <contract_lineitem_int.h>
#include <data_revision_int.h>
#include <design_group_int.h>
#include <errno.h>
#include <flow_int.h>
#include <fracture_plate_int.h>
#include <fracture_step_int.h>
#include <fracture_style_int.h>
#include <fractured_pattern_int.h>
#include <malloc.h>
#include <mask2000.h>
#include <mask2000_int.h>
#include <maskset_int.h>
#include <maskshop_int.h>
#include <maskshop_table_int.h>
#include <mysql.h>
#include <orderform_int.h>
#include <plate_int.h>
#include <process_group_int.h>
#include <process_step_int.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "m2globals.h"
#include "m2req_types.h"
#include <mask2000.h>
#include <mask2000_int.h>
#include <maskset_int.h>
#include <maskshop_int.h>
#include <maskshop_table_int.h>
#include <flow_int.h>
#include <process_step_int.h>
#include <data_revision_int.h>
#include <plate_int.h>
#include <cell_int.h>
#include <fracture_style_int.h>
#include <fracture_plate_int.h>
#include <fracture_step_int.h>
#include <fractured_pattern_int.h>
#include <process_group_int.h>
#include <design_group_int.h>
#include <contract_lineitem_int.h>
#include <orderform_int.h>

int CURRENT_CELL_COUNT=0;
static struct cell_class *CellList[CELL_LIST_MAX];

/*  Function: CellListAdd()                                        */
/*  Purpose: This function is used to build a unique list of cell  */
/*           A return of '0' means the cell was already on the list*/
/*           A return of '1' means the cell was added.             */
int CellListAdd(struct cell_class *cell) 
{
   
    int index;
    /* Always initialized the next slot to be assigned. 
       This will avoid having the init require outside
       this routine.
   */
    CellList[CURRENT_CELL_COUNT] = (struct cell_class *)NULL;
    for(index = 0;index < CELL_LIST_MAX;index++) {
       /* We are looking through the list to see if this
          cell has been assigned already. If it has, return() */
       if (CellList[index] == cell) return(0);
       if (index == CURRENT_CELL_COUNT) { /* This is the next available slot */
          CellList[index] = cell;
          CURRENT_CELL_COUNT++;
          return(1);
       }
    }
    if (index == CELL_LIST_MAX) {
/*           printf("ERROR: CELL_LIST_MAX=%d exceeded! Cell List not complete!\n",CELL_LIST_MAX); */
          return(-1);
    }
    return(1);
} /* end cellListAdd */

/*  Function: load_cell_list(void)                                 */
/*  Purpose: This function is used to load the global CellList.    */
int load_cell_list(void) 
{

   struct flow_class *flow;
   struct process_step_class *ps;
   struct data_revision_class *dr;
   struct cell_class *currentCell;
   int    rotation = 0;

   /* Maskset is assumed to be loaded */
   flow = get_flow();
   currentCell = (struct cell_class *)NULL;
   ps = flow_get_first_process_step(flow);
   while(ps) {
      dr = process_step_get_first_current_revision(ps);
      while(dr) {
          data_revision_get_next_cell(dr,&currentCell,&rotation);
          while(currentCell) {
             CellListAdd(currentCell);
             data_revision_get_next_cell(dr,&currentCell,&rotation);
          }
          dr = process_step_get_next_current_revision(ps);
      }
      ps = flow_get_next_process_step(flow);
   }
   return(1);
}

/*  Function: return_cell_from_cell_list                           */
/*  Purpose: This function will return the matching cell from the  */
/*           global CellList.                                      */
struct cell_class *return_cell_from_cell_list(char *cell_name) 
{

   struct cell_class *cell;
   int index;
   
   cell = (struct cell_class *)NULL;
   for(index = 0;index < CURRENT_CELL_COUNT;index++) {
       /* We are looking through the list to see if this */
       if (strcmp(cell_get_name(CellList[index]),cell_name) == 0) {
          cell = CellList[index];
          break;
       }
   }
   return(cell);
}

/* Update the common_maskset_layers table */
int common_maskset_layers_update(void)
{  
    int fab_id;
    int tech_id;
    char *maskset_name;
    char *fab_name;
    char *tech_name;
    char *flow_name;
    char * ps_name;
    char * data_revision_name;
    char * fracture_style;
    char * plate_name;
    char * cell_name;
    char * description;
    MYSQL sql;
   struct maskset_class *ms;
   struct flow_class *flow;
   struct process_step_class *ps;
   struct data_revision_class *dr;
   struct fracture_style_class *fs;
   struct fracture_plate_class *fp;
   struct fracture_step_class *fstep;
   struct cell_class *cell,*main_cell;
   char   cell_type[32];
   char   *mlayer, booleans[1024];
     char *ptr1;
   int    i;
   
    /* Verify this is a valid mask set */
    if (is_mask(mask_msg.key1) == DBNO) 
    {
	sprintf(rtn.msg, invalid_mask, mask_msg.key1);
	data_log(tr.log_file, rtn.msg);
	return(INVALID_MASK);
    }
    /* Change to directory where mask set is stored */
    chdir(mti.data_dir);
   /* Restore the maskset */
   maskset_restore();
   ms = get_maskset();
    maskset_name = maskset_get_name(ms);
    fab_name = maskset_get_created_from_fab(ms);
    tech_name = maskset_get_created_from_technology(ms);
    chdir("../..");
    /* Map the fab and technology names to the corresponding defined IDs */
    /* Find the fab num */
    fab_id = fab_lookup(fab_name);
    /* If this is an invalid fab, return */
    if (fab_id <= 0)
    {
	sprintf(rtn.msg, fab_num_err, fab_name);
	data_log(tr.log_file, rtn.msg);
	maskset_free_all();
	return(FAB_NUM_ERR);
    } /* end if */
    /* find the technology number */
    tech_id = sql_tech_num_find(tech_name, DBYES);
    if (tech_id == 0)
    {
	sprintf(rtn.msg, tech_num_err, tech_name);
	data_log(tr.log_file, rtn.msg);
/*
	maskset_free_all();
	return(TECH_NUM_ERR);
*/
    }
    /* Open a connection */
    mysql_init(&sql);
    /* Connect */
     mysql_real_connect(&sql, DTMS_DB, "rxfh80", "DR2dr33m", "common", 0, 0, 0);
   flow = maskset_get_flow(ms);
    flow_name = flow_get_name(flow);
	/* Update the maskset_layers table */
	    sql_common_maskset_layers_record_update(&sql, maskset_name, 
    fab_id, tech_id, tech_name, flow_name);
   ps = flow_get_first_process_step(flow);
   while(ps) 
    {
	ps_name = process_step_get_name(ps); 
	description = process_step_get_description(ps);
      dr = process_step_get_current_revision(ps);
    /* The following gets revision with process step name */
/*
data_revision_name = 	data_revision_get_name(dr);
*/
	/* This retrieves revision without the process step name */
	data_revision_name = data_revision_get_process_step_revision(dr); 
      ps = flow_get_next_process_step(flow);
   }
   load_cell_list();
   fracture_style_init_search();
   main_cell = (struct cell_class *)NULL;
   while((fs = fracture_style_next_object())) {
    fracture_style = fracture_style_get_name(fs);
       cell = fracture_style_get_first_cell(fs);
       while (cell) 
    {
          if ((cell = return_cell_from_cell_list(cell_get_name(cell)))) 
	{
             switch (cell_get_type(cell)) {
                case MAIN_CELL: 
                   strcpy(cell_type,"Main Cell");
                   strcpy(fracture_style,fracture_style_get_name(fs));
                   main_cell = cell;
                   break;
                case REGULAR_CELL:
                   strcpy(cell_type,"Regular Cell");
                   break;
                case CLEAR_COLLAR:
                   strcpy(cell_type,"Clear/Collar");
                   break;
                case PC_PATTERN:
                   strcpy(cell_type,"PC Pattern");
                   break;
                case FIDUCIAL:
                   strcpy(cell_type,"Fiducial");
                   break;
                case DISEMBODIED:
                   strcpy(cell_type,"Disembodied Cell");
                   break;
                case NCAR_CELL:
                   strcpy(cell_type,"NCAR Cell");
                   break;
                case WAFER_CELL:
                   strcpy(cell_type,"Wafer Cell");
                   break;
                default:
                   strcpy(cell_type,"Unknown");
                   break;
    } /* end switch */
	    cell_name = cell_get_name(cell);
	    /* Update the common_maskset_cell table */
	    sql_common_maskset_cell_record_update(&sql, maskset_name,
	    cell_name, cell_type, fracture_style, data_revision_name);
          };
          cell = fracture_style_get_next_cell(fs);
       }
   }
   /* Now go through each fracture style with the process_steps */
   fracture_style_init_search();
   while((fs = fracture_style_next_object())) 
    {
	fracture_style = fracture_style_get_name(fs);
      ps = flow_get_first_process_step(flow);
      while(ps) 
    {
	/* Retrieve the current data revision */
         dr = process_step_get_current_revision(ps);
	data_revision_name = data_revision_get_process_step_revision(dr); 
	/* Retrieve the current process step name */
	ps_name = process_step_get_name(ps); 
	/* Retrieve the description */
	description = process_step_get_description(ps); 
         if (((fp = fracture_style_match_fracture_plate_by_data_revision(fs,dr))) 
    && (dr != NULL))
	{
	plate_name = data_revision_get_name(dr);
            fstep = fracture_plate_get_first_fracture_step(fp);
            i = 0;
            strcpy(booleans," ");
            while (fstep) {
/*                printf("\t\tStep %d Input 1: %s\n",i,fracture_step_get_input1(fstep)); */
               /* Avoid adding the Temp-Layer-Name input fields... */
               if (strncmp(fracture_step_get_input1(fstep),"T",1)) {
                  /* Avoid adding multiple inputs... */
                  if (strlen(booleans)) 
    {
ptr1 = fracture_step_get_input1(fstep);
	if (ptr1 != NULL)
    {
                     strcat(booleans,fracture_step_get_input1(fstep));
                     strcat(booleans," ");
                  }
    }
               }
/*                printf("\t\tStep %d Input 2: %s\n",i++,fracture_step_get_input2(fstep)); */
               /* Avoid adding the Temp-Layer-Name input fields... */
               if (strncmp(fracture_step_get_input2(fstep),"T",1)) {
                  /* Avoid adding multiple inputs... */
                  if (strlen(booleans) && !strstr(booleans,fracture_step_get_input1(fstep))) {
                     strcat(booleans,fracture_step_get_input2(fstep));
                     strcat(booleans," ");
                  }
               }
               fstep = fracture_plate_get_next_fracture_step(fp);
            }
	    sql_common_maskset_booleans_record_update(&sql, maskset_name, 
    ps_name, data_revision_name, description, fracture_style, booleans);
            /*
            mlayer = fracture_plate_create_mlayer(fp,data_revision_get_name(dr), FS_BT_NONE);
             printf("\t\tFracture Plate Booleans: %s\n",mlayer); 
            fracture_plate_free_mlayer(mlayer);
            */
         }
         ps = flow_get_next_process_step(flow);
      }
   }
    maskset_free_all();
    sprintf(rtn.msg, "Common layers updated for %s", mti.suffix);
    data_log(tr.log_file, rtn.msg);
   return(SUCCESS);
} /* End common_maskset_layers_update */

/* Update the common_template_layers table */
int common_template_layers_update(char *approve_status)
{  
    int ps_order = 0;
    int fab_id;
    int cnt;
    int tech_cnt;
    int tech_id_array[10];
    char *fab_name;
    char *tech_name;
    char *flow_name;
    char * ps_name;
    char * data_revision_name;
    char * fracture_style;
    char * description;
    MYSQL sql;
   struct maskset_class *ms;
   struct flow_class *flow;
   struct technology_class *technology;
   struct process_step_class *ps;
   struct data_revision_class *dr;
   struct fracture_style_class *fs;
   struct fracture_plate_class *fp;
   struct fracture_step_class *fstep;
   struct cell_class *cell,*main_cell;
   char   *mlayer, booleans[1024];
     char *ptr1;
    char mds_tech[32];
   int    i;
   
    /* Change to directory where template is stored */
    chdir(mti.data_dir);
   /* Restore the template */
   maskset_restore();
   ms = get_maskset();
    /* Change back to the server directory */
    chdir("../../..");
    fab_name = maskset_get_created_from_fab(ms);
    tech_name = maskset_get_created_from_technology(ms);
    printf("tech_name is %s\n", tech_name);
    /* Map the fab and technology names to the corresponding defined IDs */
    /* first see if a special mapping from MDS to common technologies present */
    if ((tech_cnt = sql_mds_tech_name_map(tech_id_array, tech_name)) == 0)
    {
    /* find the technology number in common database */
    tech_id_array[0] = sql_tech_num_find(tech_name, DBYES);
    tech_cnt = 1;
    } /* End if for mapping not found */
    printf("tech id is %d\n", tech_id_array[0]);
    if (tech_id_array[0] == 0)
    {
	sprintf(rtn.msg, tech_num_err, tech_name);
	data_log(tr.log_file, rtn.msg);
	maskset_free_all();
	return(TECH_NUM_ERR);
    }
    /* Find the fab num */
    printf("fab_name is %s\n", fab_name);
    fab_id = fab_lookup(fab_name);
    printf("fab_id is %d\n", fab_id);
    /* If this is an invalid fab, return */
    if (fab_id <= 0)
    {
	sprintf(rtn.msg, fab_num_err, fab_name);
	data_log(tr.log_file, rtn.msg);
	maskset_free_all();
	return(FAB_NUM_ERR);
    } /* end if */
   flow = maskset_get_flow(ms);
    technology = maskset_get_technology(ms);                     
    if (technology == 0)
    {
	sprintf(rtn.msg, invalid_technology, mti.suffix);
	data_log(tr.log_file, rtn.msg);
	return(INVALID_TECHNOLOGY);
    }
    /* Open a connection */
    mysql_init(&sql);
    /* Connect */
     mysql_real_connect(&sql, DTMS_DB, "rxfh80", "DR2dr33m", "common", 0, 0, 0);
    for (cnt = 0; cnt < tech_cnt; ++cnt)
    {
	/* Delete the old entries before adding the new entries */
	sql_current_table_delete(&sql, "common_template_layers", fab_id, tech_id_array[cnt]);
	sql_current_table_delete(&sql, "common_template_booleans", fab_id, tech_id_array[cnt]);
    } /* end for */
    flow = technology_get_first_flow(technology);      
    while (flow)                                      
    {
	flow_name = flow_get_name(flow);
      ps = flow_get_first_process_step(flow);          
      while (ps)                                      
	{
	ps_name = process_step_get_name(ps); 
	description = process_step_get_description(ps);
        dr = process_step_get_current_revision(ps);    
    /* this gets the process step name and revision */
	data_revision_name = data_revision_get_name(dr);  
    /* This gets the revision without the process step name */
	data_revision_name = data_revision_get_process_step_revision(dr); 
    for (cnt = 0; cnt < tech_cnt; ++cnt)
	    sql_common_template_layers_record_update(&sql, fab_id, tech_id_array[cnt], tech_name, flow_name, 
	ps_name, ps_order++, data_revision_name, description, approve_status);
        ps = flow_get_next_process_step(flow);         
      }                                                
      flow = technology_get_next_flow(technology);     
    }                                                  
   /* Now go through each fracture style with the process_steps */
    /* Must reset the flow */
   flow = maskset_get_flow(ms);
   fracture_style_init_search();
   while((fs = fracture_style_next_object())) {
	fracture_style = fracture_style_get_name(fs);
      ps = flow_get_first_process_step(flow);
      while(ps) 
    {
	ps_name = process_step_get_name(ps); 
         dr = process_step_get_current_revision(ps);
         if ((fp = fracture_style_match_fracture_plate_by_data_revision(fs,dr))) {
/*             printf("\tFracture Plate for: %s\n",data_revision_get_name(dr)); */
	data_revision_name = data_revision_get_name(dr);
            fstep = fracture_plate_get_first_fracture_step(fp);
            i = 0;
            strcpy(booleans," ");
            while (fstep) {
/*                printf("\t\tStep %d Input 1: %s\n",i,fracture_step_get_input1(fstep)); */
               /* Avoid adding the Temp-Layer-Name input fields... */
               if (strncmp(fracture_step_get_input1(fstep),"T",1)) {
                  /* Avoid adding multiple inputs... */
                  if (strlen(booleans)) 
    {
ptr1 = fracture_step_get_input1(fstep);
	if (ptr1 != NULL)
    {
                     strcat(booleans,fracture_step_get_input1(fstep));
                     strcat(booleans," ");
                  }
    }
               }
/*                printf("\t\tStep %d Input 2: %s\n",i++,fracture_step_get_input2(fstep)); */
               /* Avoid adding the Temp-Layer-Name input fields... */
               if (strncmp(fracture_step_get_input2(fstep),"T",1)) {
                  /* Avoid adding multiple inputs... */
                  if (strlen(booleans) && !strstr(booleans,fracture_step_get_input1(fstep))) {
                     strcat(booleans,fracture_step_get_input2(fstep));
                     strcat(booleans," ");
                  }
               }
               fstep = fracture_plate_get_next_fracture_step(fp);
            }
/*             printf("\t\tFinal Bools: %s\n",booleans); */
    for (cnt = 0; cnt < tech_cnt; ++cnt)
sql_common_template_booleans_record_update(&sql, fab_id, tech_id_array[cnt], 
fracture_style, ps, booleans);
            /*
            mlayer = fracture_plate_create_mlayer(fp,data_revision_get_name(dr), FS_BT_NONE);
             printf("\t\tFracture Plate Booleans: %s\n",mlayer); 
            fracture_plate_free_mlayer(mlayer);
            */
         }
         ps = flow_get_next_process_step(flow);
      }
   }
    maskset_free_all();
    sprintf(rtn.msg, "Common layers updated for ", mti.suffix);
	data_log(tr.log_file, rtn.msg);
   return(SUCCESS);
} /* end common_template_layers_update */

    /* Update the common_layers table */
int common_layers_update(void)
{
    if (mask_msg.data_type == TEMPLATE)
    {
	/* Update the common_template_layers table */
	return(common_template_layers_update(st.version_type));
	/* Update the list of flows and options */
/*	ps_lists_update(mask_msg.key1, mask_msg.key2); */
   }
    /* If this is a mask set add entry to the common_maskshop_layers table */
    else if (mask_msg.data_type == MASK_SET)  
    {
	/* Update the common_maskset_layers table */
	sql_mask_create_add(mask_msg.key1, user.email);
	return(common_maskset_layers_update());
    }
    else return(FAILURE);
} /* end common_layers_update */
#ifdef garbage
common_layers_update() { }
flow_find_flow() { }
flow_free_process_step_list() { }
flow_get_process_step_list() { }
get_maskset() { }
maskset_free_flow_list() { }
maskset_get_created_from_fab() { }
maskset_get_created_from_technology() { }
maskset_get_flow_list() { }
maskset_restore() { }
option_get_first_process_step() { }
option_get_name() { }
option_get_next_process_step() { }
option_get_num_process_steps() { }
option_group_get_first_option() { }
option_group_get_name() { }
option_group_get_next_option() { }
option_group_get_num_options() { }
option_group_get_ret_group_of_interest() { }
option_group_init_search() { }
option_group_next_object() { }
process_step_get_name() { }
#endif
#endif /* for garbage */
