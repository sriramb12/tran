#ifndef m2server_errs_h
#define m2server_errs_h
#include "server_caution_errs.h"
#include "global_server_errs.h"
/* Defines for database errors follow */

#define xx ""
#define bad_index "Error -603:  Invalid parent fab and process flow name."
#define BAD_INDEX -603
/* Authorization errors */
#define m2own_err "Error -616:  You are not authorized to perform updates for:"
#define M2OWN_ERR -616
#define build_auth "Error -617:    You are not authorized to build templates."
#define BUILD_AUTH -617
#define approve_auth "Error -618:    You are not authorized to approve templates."
#define APPROVE_AUTH -617
#define pc_approve_err "Error -618:    You are not authorized to approve PC libraries."
#define PC_APPROVE_ERR -618
#define pc_build_err "Error -619:    You are not authorized to build PC libraries."
#define PC_BUILD_ERR 619

#define bad_time_flag  "Error -633:    Invalid time flag"
#define BAD_TIME_FLAG -633
#define control_err "Error -635:  You are not authorized to access\n\n\
               %s\n\n\
               Access to this library is restricted to:\n\n\
               Control Group '%s'"
#define CONTROL_ERR -635

#define non_wl_err "Error -638:    Fab '%s' must be created before templates for\n\
               '%s' can be created."
#define NON_WL_ERR -638

/* Server directory and table of contents  errors */

/* Server file errors */
#define bad_file_name "Error -659:    Bad file name of 0 length received from user."
#define BAD_FILE_NAME -659

/* Warning messages with successful data retrieval not from state table */
#define pc_wrn "\n\nWarning -660:  You have not retrieved the PC library for:"
#define PC_WRN -660
#define oldpc_wrn "\n\nWarning -661:  The PC library for %s\n\
               has been updated since your last retrieval."
#define OLDPC_WRN -661
#define no_pc_wrn "\n\nWarning -662:  No PC library present for:"
#define NO_PC_WRN -662
#define newtmpl "\n\nWarning -664:  The fab templates used to\n\
               create mask set '%s' are newer than the mask set data."
#define NEWTMPL -664

/* Warnings that template data mask created from nonexistent */
#define fab_wrn "\n\nWarning -667:  The fab templates used\n\
               to create mask set '%s' no longer exist."
#define FAB_WRN -667
/* End of warning messages not from state table */

/* State table warnings sent with successful DB transaction */
/* Warnings requiring no parameters */
#define ngi_wrn "\n\nWarning -669:  the Mask Designer Database cannot garantee data integrity\n\
           of the data retrieved."
#define NGI_WRN -669
#define ovr_wrn "\n\nWarning -670:  Your previous transaction was cancelled."
#define OVR_WRN -670

/* Warnings requiring 1 parameter */
/* The following warning messages are not handled as a single unit in */
/* get_state because there are only three that are handled alike */
#define upd1upd "\n\nWarning -671:  Previous update of %s overwritten.\n\
               Check-in of data recommended."
#define UPD1UPD -671
#define upd1out "\n\nWarning -672:  Update of %s cancelled and data checked out.\n\
                However, the Mask Designer Database cannot guarantee integrity of data retrieved."
#define UPD1OUT -672
#define upd1cin "\n\nWarning -673:  Update of %s cancelled and data checked in."
#define UPD1CIN -673
#define ovr1out "\n\nWarning -674:  Previous check-out of %s \n\
               overwritten."
#define OVR1OUT -674
/* End of warning messages which require suffix to be printed */
#define rec_out "\n\nWarning -675:  %s re-checked out\n\
               Previous check-out was in directory:\n\n\
                '%s'"
#define REC_OUT -675
#define app2wrn "\n\nWarning -676:  %s re-checked out for approval.\n\
               Previous check-out was in directory:\n\n\
                '%s'"
#define APP2WRN -676
/* Warning printed when user retrieves data in an unstable update state by another user  */
#define upd4wrn "\n\nWarning -677:  %s retrieved, but  currently being updated by:\n\
               Name:  %s\n\
               Phone:  %s\n\
               Email:  %s@nxp.com"
#define UPD4WRN -677
#define cho4wrn "\n\nWarning -678:  %s retrieved, but already checked out to:\n\n\
               Name:  %s\n\
               Phone:  %s\n\
               Email:  %s@nxp.com"
#define CHO4WRN -678
/* End of warning messages from state table */

/* State table errors */
#define ERR_OFFSET 380

/* State table errors which require prefix to be printed */
#define UPPER_PREFIX_RANGE -680
#define nci1err "Error -682:    %s cannot be checked in\n\
               because it was not previously checked out."
#define NCI1ERR -682
#define ncb1err "Error -683:    %s is not checked out."
#define NCB1ERR -683
#define nca1err "Error -684:  %s is not checked out for approval."
#define NCA1ERR -684

/* State table errors which can be corrected by turning overwrite flag on */
#define UPPER_OVERRIDE -685
#define rmt2upd "\n\nOverride -685:  %s is currently being updated by a remote database server.\n\n\
                Press 'Continue' to proceed with transaction\n\
                after remote update completes.  This may take up to one minute.\n\n\
                Press 'Cancel' to abort."
#define RMT2UPD -685
#define cho1upd "\n\nOverride -686:  %s is in an unstable update state.\n\n\
                Press 'Continue' to proceed with check-out.\n\
                Press 'Cancel' to abort."
#define CHO1UPD -686
#define ovr2upd "\n\nOverride -687:  %s is in an unstable update state.\n\n\
                Press 'Continue' to proceed with update.\n\
                Press 'Cancel' to abort."
#define OVR2UPD -687
#define del2upd "\n\nOverride -688:  %s is in an unstable update state.\n\n\
                Press 'Continue' to proceed with delete.\n\
                Press 'Cancel' to abort."
#define DEL2UPD -688
#define rd2upd "\n\nOverride -689:  %s is in an unstable update state.\n\n\
                Press 'Continue' to proceed with read.\n\
                Press 'Cancel' to abort."
#define RD2UPD -689
#define cin2upd "\n\nOverride -690:  %s is in an unstable update state.\n\n\
                Press 'Continue' to proceed with check-in.\n\
                Press 'Cancel' to abort."
#define CIN2UPD -690
#define can2upd "\n\nOverride -691:  %s is in an unstable update state.\n\n\
                Press 'Continue' to proceed with cancellation of check-out.\n\
                Press 'Cancel' to abort."
#define CAN2UPD -691
#define pc2exists "\n\nOverride -692:  %s is already present.\n\n\
                Press 'Continue' to put latest copy of PC library in the \n\
               Mask Designer Database.\n\
                Press 'Cancel' to abort."
#define PC2EXISTS -692

#define dlt2ovr "\n\nOverride -693:  You have requested deletion of:\n\n\
                %s\n\n\
                Press 'Continue' to proceed with delete.\n\
                Press 'Cancel' to abort."
#define DLT2OVR -693
#define LOWER_PREFIX_RANGE -693
/* End of state table errors which require prefix to be printed */
#define ovr2cho "\n\nOverride -694:  You have already checked out:\n\n\
                %s\n\n\
                Previous check-out was in directory:\n\n\
                '%s'\n\n\
                If you re-check-out the data in the same directory,\n\
                the data already present will be DESTROYED.\n\n\
                Press 'Continue' to proceed with check-out.\n\
                Press 'Cancel' to abort."
#define OVR2CHO -694
#define can1ovr "Override -695:  You have requested cancel check out on \n\n\
                %s\n\n\
                If you perform this transaction the data on \n\
                your local disk will be deleted, and none of \n\
                changes you have made will be saved in the \n\
                Mask Designer Database.\n\n\
                Press 'Continue' to proceed with transaction\n\
                Press 'Cancel' to abort."
#define CAN1OVR -695
#define no1offdata "Override -696:  Official data for %s does not exist.\n\n\
                Press 'Continue' to use unofficial data.\n\
                Press 'Cancel' to abort."
#define NO1OFFDATA -696
#define rdo2cho "\n\nOverride -697:  You have already checked out:\n\n\
                %s\n\n\
                Previous check-out was in directory:\n\n\
                '%s'\n\n\
                If you read the data in the same directory,\n\
                the data already present will be DESTROYED.\n\n\
                Press 'Continue' to proceed with check-out.\n\
                Press 'Cancel' to abort."
#define RDO2CHO -697
#define suffix2err "\n\nOverride -698:  File with invalid suffix found.\n\n\
                '%s'\n\n\
                Press 'Continue' to proceed with transaction.\n\
                Press 'Cancel' to abort."
#define SUFFIX2ERR -698
#define unused2err "\n\nOverride -699:  Essential file not found in file list:\n\n\
                '%s%s'\n\n\
                Press 'Continue to proceed with transaction.\n\
                Press 'Cancel' to abort."
#define UNUSED_ERR -699
#define off_upd_ovr "Override -700:  You have already began updating %s.\n\n\
                Press 'Continue' to proceed with transaction.\n\
                Press 'Cancel' to abort."
#define OFF_UPD_OVR -700
#define alr2linked "\n\nOverride -701:  %s is already linked to:\n\n\
                PC library '%s'\n\n\
                Press 'Continue' to overwrite the existing link.\n\
                Press 'Cancel to abort."
#define ALR2linked -701
#define user_lock_err "Error -702:  %s is being accessed by:\n\n\
                Name:  %s\n\
                Phone:  %s\n\
                Email:  %s@nxp.com\n\
                Press 'Continue' to wait up to 5 minutes to access data.\n\
                Press 'Cancel' to cancel transaction."
#define USER_LOCK_ERR -702
#define dlt1ovr "\n\nOverride -703:  %s is currently checked out to you.\n\
                Press 'Continue' to proceed with delete.\n\
                Press 'Cancel' to abort."
#define DLT1OVR -703
#define LOWER_OVERRIDE -703
/* End of errors which can be corrected by turniing OVERRIDE_FLAG on */
/* State table errors which require suffix to be printed */
#define UPPER_SUFFIX_RANGE -704
#define nco1err "Error -705:    You cannot check-in %s\n\
               because you have not checked it out."
#define NCO1ERR -705
#define bad1upd "Error -706:    You cannot update %s\n\
               because you have not checked it out."
#define BAD1UPD -706
#define bam1err "Error -707:  You have already checked out %s for edit.\n\
             The data must be checked in before it can be checked out for read."
#define BAM1ERR -707
#define cha1cho "Error -708:  You have checked out %s.\n\
You cannot  approve the template until you check-in the data or\n\
               cancel check-out."
#define CHA1CHO -708
#define LOWER_SUFFIX_RANGE -708
/* End of state table errors which require suffix to be printed */

/* State table errors which require 4 parameters */
#define UPPER_RANGE5 -709
#define upd4err "Error -709:    %s is being updated by:\n\n\
               Name:    %s\n\
               Phone:    %s\n\
               Email:    %s@nxp.com"
#define UPD4ERR -709
#define out4err "Error -710:    %s is checked out to:\n\n\
               Name:    %s\n\
               Phone:    %s\n\
               Email:    %s@nxp.com"
#define OUT4ERR -710
#define mod_owner_err "Error -711:  You cannot modify the owner for data you do not own.  Data is owned by:\n\n\
               Name:   %s\n\
               Phone:  %s\n\
               Email:  %s@nxp.com\n"
#define MOD_OWNER_ERR -711
#define dlt4data "Error -712:    You may not delete data you did not create.\n\
               %s was created by:\n\n\
               Name:    %s\n\
               Phone:    %s\n\
               Email:    %s@nxp.com\n"
#define DLT4DATA -712
#define LOWER_RANGE5 -712
/* End of state table errors which require 5 parameters */
/* End of state table errors */

#define bad4owner "Error -713:  You cannot delete data you do not own.  Data is owned by:\n\n\
               Name:  %s\n\
               Phone:  %s\n\
               Email: %s@nxp.com\n"
#define BAD4OWNER -713
#define grant4err "Error -714:      You cannot grant overwrite for data you do not own.  \n\
               %s is owned by:\n\n\
               Name:   %s\n\
               Phone:  %s\n\
               Email:  %s@nxp.com\n"
#define GRANT4ERR -714
#define no_ovrwr "Error -715:  You are not authorized to overwrite %s.\n\
               Overwrite privileges granted to:\n\n\
               Email: %s@nxp.com"
#define NO_OVRWR -715
#define max_lock_err "Error -717:  %s is still being accessed by:\n\n\
               Name:  %s\n\
               Email:  %s@nxp.com\n"
#define MAX_LOCK_ERR -717
#define approve_err "Error -718:  You are not authorized to approve master templates."
#define APPROVE_ERR -718
#define off1err "Error -719:  %s\n\
               has not been approved for official use."
#define OFF1ERR -719
#define no_masks "Warning -720:  No mask sets found to meet specified criteria."
#define NO_MASKS -720

#define owner_not_fnd "Error -721:  Unable to find address information for server\n\
               '%s'"
#define OWNER_NOT_FND -721
#define not_server "Error -722: %s is owned by the %s Mask Designer Database."
#define NOT_SERVER -722
#define bad_sock_oepn "Error -723:  Unable to open socket to remote server."
#define BAD_SOCK_OPEN -723
/* This value cannot be changed unless the corresponding one in dbclient_errs.h is changed */
#define invalid_req "Error -726:  NO data requested from the Mask Designer Database."
#define INVALID_REQ -726
#define dup_name "Error -727:    A fab with the specified name already\n\
               exists.  Please select another name and try your request again."
#define DUP_NAME -727
#define transfer_err "Error -729:  Received following message while trying to transfer data to '%s'\n\n%s"
#define TRANSFER_ERR -729

/* Warnings that unofficial template data mask created from nonexistent */
#define unoff_fab_wrn "\n\nWarning -731:  The unofficial fab templates used\n\
               to create mask set '%s' no longer exist."
#define UNOFF_FAB_WRN -731
#define off_fab_wrn "\n\nWarning -734:  The official fab templates used\n\
               to create mask set '%s' no longer exist.\n\
               However, unofficial copy still exists."
#define OFF_FAB_WRN -734
#define new_unoff_tmpl "\n\nWarning -737:  The unofficial fab templates are newer than the official fab templates."
#define NEW_UNOFF_TMPL -737
#define unoff_wrn "\n\nWarning -739:  %s was created from\n\
               unofficial copy of %s."
#define UNOFF_WRN -739

/* Invalid prefix and suffix errors */
#define off_tmpl_wrn "\n\nWarning -746:  %s\n\
               does not exist.  Unofficial copy retrieved."
#define OFF_TMPL_WRN -746
#define off_mask_err "Error -748:  %s has not been approved.\n\
               Mask sets cannot be created from unapproved templates."
#define OFF_MASK_ERR -748
#define no_files_err "Error -749:  Files for Version '%s' not present for %s."
#define NO_FILES_ERR -749
/* PC link errors and warnings */
#define empty_link_file "Error -750:  PC library '%s' is not linked to any fab data."
#define EMPTY_LINK_FILE -750
#define pc_unlink_wrn "\n\nWarning -751:  Master templates for fab '%s', process flow '%s'\n\
               are no longer linked to a PC library.\n\
               Thus %s is also no longer linked to a PC library."
#define PC_UNLINK_WRN -751
#define no_link_err "Error -752:  %s is not linked to a PC library."
#define NO_LINK_ERR -752
/*
#define invalid_mask_dir "Caution -753:  Found invalid mask directory '"
*/
/*
#define INVALID_MASK_DIR -753
*/
#define link_err "Error -754:  Links to fab/process flow data still exist for PC library:"
#define LINK_ERR -754
#define lnk2_err "Error -755:  PC library '%s' is not linked to:\n\n\
               %s"
#define LNK2_ERR -755
#define pc_link_wrn "\n\nWarning -756:  Fab templates for fab '%s', process flow '%s'\n\
               have been linked to PC library '%s'.\n\
               Thus %s has been linked to the same library."
#define PC_LINK_WRN -756
#define bad_pc_link "\n\nWarning -757:  New PC library linked to master templates for\n\
               fab '%s', process flow '%s'\n\
               cannot be found in the Mask Designer Database."
#define BAD_PC_LINK -757
#define time_range_err "Error -759:  Invalid time range flag: ' '"
#define TIME_RANGE_ERR -759

/* Info file errors */
#define new_mask_fab_wrn "\n\nWarning -767:  The unofficial fab templates are newer than the official fab\n\
               templates.  Thus the mask set does NOT have the LATEST data\n\
               from the unofficial fab templates."
#define NEW_MASK_FAB_WRN -767
#define bad_serial "Error -769:  Invalid serial number"
#define BAD_SERIAL -769
#define crt2err "Error -780:  %s is already present in the Mask Designer Database."
#define CRT2ERR - 780

#define RD_WAIT -798     /* Wait for read to complete before doing an update */
#define UP_WAIT -799     /* Wait for update to complete before reading or updating data */
#define contract_range_err "Error -801:    There are no more available contract release numbers\n\
               in the Mask Designer Database."
#define CONTRACT_RANGE_ERR -801
#define bamsp_err "Error -802:    You have already checked out %s.\n\
               The data must be checked in before it can be checked out again."
#define BAMSP_ERR -802
#define bad_site "Error -806:  Invalid site:"
#define BAD_SITE -806
#define cnb_err "Error -608:    Unable to find match in CNB table for :\n\n\
               Mask Shop '%s'"
#define CNB_ERR -608
#define contract_tbl_err "Error -611:  Contract numbers have not been defined for specified mask shop."
#define CONTRACT_TBL_ERR -611
#define tmplbld_err "Error -612:  Unable to add user to site registration table on '"
#define TMPLBLD_ERR -612
#define array_overflow_err "Error -615:    More than 3000 mask sets are present.  The\n\
               size of the mask_key array in audit_mask_file\n\
               must be increased."
#define ARRAY_OVERFLOW_err -615
#define archive "Warning -628:  %s has not been accessed\n\
               for a year and has been archived."
#define ARCHIVE -628
#define alr_loc "Error -629:     %s is already locked for archived."
#define ALR_LOC -629
#define alr1arc "Error -630:     %s has already been archived."
#define ALR1ARC -630
#define not1arc "Error -631:     %s is not archived."
#define NOT1ARC -631
#define NOT_LOC -631
#define arc1err "Error -632:    %s has been archived.\n\
               The requested transaction cannot be performed on archived data."
#define ARC1ERR -632
#define loc2ovr "\n\nOverride -633:  %s has been archived.\n\n\
                Press 'Continue' to restore data from archive\n\n\
                and check out the data.\n\n\
                Press 'Cancel' to abort check-out."
#define LOC2OVR -633
#define RESTORE -634
#define not_archived "Error -635:  %s is not archived."
#define NOT_ARCHIVED -635





#define crt_msg "\n\n\
               Mask set created from:\n\n\
               Waferline: %s\n\
               Process flow: %s\n\
               Design group:  %s"
#define mask_entry_add_err "Error -635:   Unable to add entry for mask set %s to mask.sets file."
#define MASK_ENTRY_ADD_ERR -635
/* Used in MARS */
#define mask_name_err "Error -638:    No more mask set names are present."
#define MASK_NAME_ERR -638
#define pc_name_err "Error -639:    No more PC names are present."
#define PC_NAME_ERR -639
#define fiducial_err "Error -640:    No more fiducials are present."
#define FIDUCIAL_ERR -640
#define requester_cid_err "Error -641:  Invalid commerce ID enter for requestor."
#define REQUESTOR_CID_ERR -641
#define requester_uid_err "Error -642:  Invalid user ID '%s' entered for requestor."
#define REQUESTOR_UID_ERR -642
#define receiver_cid_wrn "\nWarning -643:  Invalid commerce ID entered for receiver.\n\
               No email sent to receiver."
#define RECEIVER_CID_WRN -643
#define receiver_uid_wrn "\nWarning -644:  Invalid user ID entered for receiver.\n\
               No email sent to receiver."
#define RECEIVER_UID_WRN -644
#define invalid_format "Error -645:  Invalid data entered."
#define INVALID_FORMAT -645
#define not_approved "Error -647:    Official copy of %s not present."
#define NOT_APPROVED -647
#define no_c4mask "Error -648:  %s has not been assigned a C4 mask."
#define NO_C4MASK -648
#define usergroup2ovr "Override -849:  You have requested deletion of all users in %s.\n\
                If you proceed with this transaction, %s will be deleted.\n\n\
                Press 'Continue' to proceed with deletion.\n\
                Press 'Cancel' to abort."
#define USERGROUP2OVR -849
#define add_err "Error -650:    You are not authorized to add entries to:"
#define delete_entry_err "Error -651:    You are not authorized to delete entries from:"
#define delete_auth_err "Error -856:  You are not authorized to delete:"
#define m2reg_err "Error -853:    Registration entry not present for '%s' in the\n\
               Mask Designer registry."
#define M2REG_ERR -853
#define ba_err "Error -803:  You have already checked out %s for edit.\n\
             The data must be checked in before it can be checked out for read."
#define BA_ERR -803
#define not_enough_versions \
"Error -804:    Transaction on %s was aborted because too few  \n\
               version files were present in the .db directory.  \n\n\
               Number of version files found:  %d"
#define NOT_ENOUGH_VERSIONS -804
#define mask_wrn "Warning -805:  No mask sets meeting specified criteria found."
#define MASK_WRN -805
#define no_tech_err "Error -807:    No technologies defined for:\n\n\
               %s"
#define NO_TECH_ERR -807
#define version_limit_wrn "Warning -808:  Unable to retrieve complete version history\n\
               because of memory allocation limitations."
#define VERSION_LIMIT_WRN -808
#define dlt3ovr "Override -809:  You submitted a usergroup list containg 0 names.\n\
               This will result in deletion of:\n\n\
                %s\n\n\
               Access control is currently assigned for %d mask set(s) \n\
               and %d template(s).\n\n\
               Press the 'Show Assignments' button to view \n\
               masks and templates assigned.\n\n\
                Press 'Continue' to proceed with delete.\n\
                Press 'Cancel' to abort."
#define no_users "Error -810:    You submitted a user list of 0 items for a nonexistent usergroup.\n\
               You cannot create a user group with 0 users."
#define NO_USERS -810
#define mod_usergroup_err "Error -811:  You cannot modify access control for data you do not own.  Data is owned by:\n\n\
               Name:   %s\n\
               Phone:  %s\n\
               Email:  %s@nxp.com\n"
#define MOD_USERGROUP_ERR -811
#define dlt4ovr "\n\nOverride -813:  You have requested deletion of:\n\n\
                %s\n\n\
               Access control is currently assigned for %d mask set(s) \n\
               and %d template(s).\n\n\
               Press the 'Show Assignments' button to view \n\
               masks and templates assigned.\n\n\
                Press 'Continue' to proceed with delete.\n\
                Press 'Cancel' to abort."
#define usergroup_add_wrn "\n\nWarning -814:  User group '%s' was not assigned to %s\n\
               because it is not present in the Mask Designer Database."
#define USERGROUP_ADD_WRN -814
#define non_version_err "Error -815:    Version %s not present for:\n\n\
               %s"
#define NON_VERSION_ERR -815
#define non_approve_version "Error -816:    No approved versions present for:\n\n\
               %s"
#define NON_APPROVE_VERSION -816
#define approve_dlt_err "Error -817:    Deletion not performed because one or more approved versions found for:\n\n\
               %s\n\n\
               PC libraries with approved versions cannot be deleted."

#define APPROVE_DLT_ERR -817
#define invalid_pc_name "Error -818:    Received invalid PC library name:\n\n\
               '%s'"
#define INVALID_PC_NAME -818
#define partition_not_empty "Error -819:    One or more partitions or PC libraries are present in:\n\n\
               %s\n\n\
               Partitions cannot be deleted until they are empty."
#define PARTITION_NOT_EMPTY -819
#define alr_approved "Error -820:    Version %s of %s\n\
               is the currently approved version."
#define ALR_APPROVED -820
#define no_approved_version "Error -821:    Approved version not present for:\n\n\
               %s"
#define NO_APPROVED_VERSION -821
#define not_approver "Error -822:    You are not authorized to approved:\n\n\
               %s"
#define NOT_APPROVER -822
#define invalid_year "Error -823:    Invalid year '%d' Entered.  Valid\n\
               years are from 1900 to 2037."
#define INVALID_YEAR -823
#define no_template_data "\n\nWarning -824:  The template mask set %s was created from is no longer present.\n\
          Mask set was created from:\n\n\
               Fab '%s', technology '%s'"
#define NO_TEMPLATE_DATA -824
#define template_newer_wrn "\n\nWarning -825:  The template mask set '%s' was created from is newer than the\n\
               mask set.  The mask set was created from:\n\n\
               Fab '%s', technology '%s'"
#define TEMPLATE_NEWER_WRN -825
#define unofficial_newer_wrn "\n\nWarning -825:  Unapproved version of the template mask set '%s' was created from is newer than the mask set.\n\
          Mask set was created from:\n\n\
               Fab '%s', technology '%s'"
#define UNOFFICIAL_NEWER_WRN -826
#define no_off_template_wrn "\nWarning -827:  Approved version of the template mask set '%s' was created fromis no longer present.\n\n\
          Mask set was created from:\n\n\
               Fab '%s', technology '%s'"
#define NO_OFF_TEMPLATE_WRN -827
#define admin_auth_err "Error -828:  You are not an authorized MDS administrator."
#define ADMIN_AUTH_ERR -828
#define no_version_fnd "Error -829:    No data present for version %s of:\n\n\
               %s"
#define NO_VERSION_FND -829
#define invalid_partition_name "Error -830:    The partition name you entered is invalid.  Partition\n\
               names must contain at least one alphabetic character."
#define INVALID_PARTITION_NAME -830
#define m2lock_err "Error -831:    another transaction is currently being performed on:\n\n\
               %s\n\n\
               Please retry this transaction in a few minutes."
#define M2LOCK_ERR -831
#define lock_override_err "Override -832:  Another transaction is currently be performed on:\n\n\
               %s\n\n\
               Press 'Continue' to wait for transaction to complete.\n\
               This may take up to one minute.\n\n\
               Press 'Cancel' to abort transaction."
#define LOCK_OVERRIDE_ERR -832
#define maskprep_convert_ovr \
"Override -833: %s is present in the Mask Prep database.   If\n\
               you proceed with the check-out,  the mask set will be\n\
               converted to the Mask Designer format and disabled in the\n\
               Mask Prep database\n\n\
                Press 'Continue' to proceed with check-out.\n\
                Press 'Cancel' to abort."
#define MASKPREP_CONVERT_OVR -833
#define maskprep_convert_read_wrn "\n\nWarning -834:  %s was retrieved from the Mask Prep database."
#define maskprep_convert_chkout_wrn "\n\nWarning -834:  %s was retrieved from the Mask Prep database\n\
               and has been checked out in the Mask Prep database."
#define audit_file_err \
"Error -835:  Audit log not present for '%s'"
#define AUDIT_FILE_ERR -835
#define maskprep_convert_err "Error -836:  Unable to convert %s from Mask Prep \n\
               format to Mask Designer format."
#define MASKPREP_CONVERT_ERR -836

#define bad_mask_msg \
"\nThis MOS12 SMOS7LV reticle set contains errors in the fracture\n\
                Booleans that MUST be fixed, prior to ordering any NEW reticles.\n\n"
#define bad_mask_ovr \
"Override -837: This MOS12 SMOS7LV reticle set contains errors in the fracture\n\
                Booleans that MUST be fixed, prior to ordering any NEW reticles.\n\
                By checking out this mask set, you take responsibility for \n\
                fixing these problems.\n\n\
                HOW TO FIX:\n\
                \n\
                1. Delete any spaces found in the Boolean algorithms.  i.e.\n\
                \"26;  11,  1, 61, 63\" change to \"26;11,1,61,63\" Depending on\n\
                your version of CATS  the spaces can  be  interpreted\n\
                incorrectly, resulting in bad fractured data.\n\
                \n\
                2. After the spaces are removed, if you don't already have a\n\
                FIN fracture style,  copy the ALLCELLS fracture style to a\n\
                FIN fracture style.\n\
                \n\
                3. In the FIN fracture style ONLY, in the last line of the\n\
                fracture Boolean,  for every layer,  remove all datatypes\n\
                listed, except 63.  i.e.  \"26;11,1,61,63\"  would change to\n\
                \"26;63\"  Datatypes 1-20 are used for reticle revisioning (-\n\
                01) but may also be used in the FIN data, thus causing bad\n\
                fracture data in the FIN for revisions.\n\
                \n\
                Contact Rob Colclasure (480-413-7057) with questions.\n\n\
                Press 'Continue' to proceed with check-out.\n\
                Press 'Cancel' to abort."
#define BAD_MASK_OVR -837
#define bad_mask_wrn \
"\n\nWarning -838:  This MOS12 SMOS7LV reticle set contains errors in the fracture\n\
                Booleans that MUST be fixed, prior to ordering any NEW reticles.\n\
                HOW TO FIX:\n\
                \n\
                1. Delete any spaces found in the Boolean algorithms.  i.e.\n\
                \"26;  11,  1, 61, 63\" change to \"26;11,1,61,63\" Depending on\n\
                your version of CATS  the spaces can  be  interpreted\n\
                incorrectly, resulting in bad fractured data.\n\
                \n\
                2. After the spaces are removed, if you don't already have a\n\
                FIN fracture style,  copy the ALLCELLS fracture style to a\n\
                FIN fracture style.\n\
                \n\
                3. In the FIN fracture style ONLY, in the last line of the\n\
                fracture Boolean,  for every layer,  remove all datatypes\n\
                listed, except 63.  i.e.  \"26;11,1,61,63\"  would change to\n\
                \"26;63\"  Datatypes 1-20 are used for reticle revisioning (-\n\
                01) but may also be used in the FIN data, thus causing bad\n\
                fracture data in the FIN for revisions.\n\
                \n\
                Contact Rob Colclasure (480-413-7057) with questions.\n"
#define BAD_MASK_WRN -838
#define not_enough_files \
"Error -839:    Transaction on %s was aborted because too few \n\
               files were present in the .db directory.  \n\n\
               Number of files found:  %d"
#define NOT_ENOUGH_FILES -839
#define autosave_present_ovr "Override -840:  Autosave directory present"
#define autosave_not_present \
"Error -841:    Autosave version of data not present for:\n\n\
               %s"
#define AUTOSAVE_NOT_PRESENT -841
#define tech_num_err  "Error -842:    Could not find technology number for \n\n\
               technology '%s'"
#define TECH_NUM_ERR -842
#define fab_num_err  "Error -842:    Could not find fab number for \n\n\
               technology '%s'"
#define FAB_NUM_ERR -843
#define cap_wafer_not_fnd \
"Error -950:  Cap wafer for mask set '%s' not found."
#define CAP_WAFER_NOT_FND -950
#define cap_wafer_err "Error -951:  Mask set '%s' is a cap wafer."
#define CAP_WAFER_ERR -951
#define cap_wafer_create_err "Error -952:    Unable to create cap wafer '%s' because a\n\
               mask set with the same name already exists."
#define CAP_WAFER_CREATE_ERR -952
#define maskset_create_err "Error -953:    Unable to create %s because a\n\
               cap wafer with the same name already exists."
#define MASKSET_CREATE_ERR -953
#define invalid_mask "Error -954:    Invalid mask set name '%s'."
#define INVALID_MASK	 -954
#define invalid_technology "Error -955:    Technology not restored for %s"
#define INVALID_TECHNOLOGY  -955

#endif
