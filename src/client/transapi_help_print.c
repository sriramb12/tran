#ifdef TRANSAPI
#ifdef TRANSCEND

/****************************************************************************
 *
 * Function:	trans_help_print
 *
 * Description:	Routines to print help for Transcend customers 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "global_defines.h"
#include "rtn.h"
#include "trans_client_errs.h"

#include "global_debug.h"


int trans_help_print(char *ptr, char command, char help_option)
{
DBG();

switch(command)
{
case 'a': sprintf(rtn.msg, "%s -a     Print a help screen for advanced commands.\n\n", ptr); break;
case 'A': sprintf(rtn.msg, "%s -A     Display advanced put/get options.\n\n", ptr); break;
case 'b': sprintf(rtn.msg, "%s -b keyword\n\n\
     Display list of files in directories for data with specified keyword.\n\n",
	ptr); break;
case 'B': sprintf(rtn.msg, "%s -b keyword\n\n\
     Retrieve list of files in directories for data with specified keyword.\n\n",
	ptr); break;
case 'c': sprintf(rtn.msg, "%s -c  [-site] all | keyword\n\n\
     Cancel previously set email notification.  If a keyword\n\
     is  given, email notification is cancelled only for the\n\
     data associated with the keyword.  If the word 'all' is\n\
     used, email notification is cancelled for all files the\n\
     user has placed in Transcend  except  those  for  which\n\
     email  notification  has  been explicitly set using the\n\
     file keyword.\n\n", ptr); break;
case 'C': sprintf(rtn.msg, "%s -CC _c_o_r_e___i_d\n\n\
              Display  friendly e-mail address and name for specified Core ID.\n\n", ptr); break;
case'd': sprintf(rtn.msg, "%s -d keyword | all\n\n\
          Delete file or directory associated with the  specified\n\
          keyword.  This command is executed after all users have\n\
          retrieved a copy of the data.  If one of the users with\n\
          the  file  keyword  does  not  delete  the  data, it is\n\
          automatically deleted three working days after  it  was\n\
          transcended thus keeping the disk free for the transfer\n\
          of other large files.  If the customer types 'trans  -d\n\
          all', data for all keywords the user has transferred to\n\
          the Transcend repository is deleted.\n\n", ptr); break;
case 'D':
     sprintf(rtn.msg, "%s -D keyword\n\n\
     Set delete lock for file or directory  associated  with\n\
     the  specified  keyword.  Only the user who transcended\n\
     the data, referred to  as  the  owner,   can  use  this\n\
     option.  When the delete lock is set, no one except the\n\
     owner can delete the data.  This option is useful  when\n\
     a  user  is distributing data to more than one user and\n\
     wants to prevent one of the  users  from  deleting  the\n\
     data  before  all of the users have retrieved the data.\n\
     However, even with the delete lock  set,  the  data  is\n\
     still  deleted by the Transcend audits when the expira-\n\
     tion date is reached.\n\n", ptr); break;
case 'e': sprintf(rtn.msg, "%s -e keyword [days]\n\n\
     Extend the expiration date on a file or directory asso-\n\
     ciated  with  the specified keyword  one to three addi-\n\
     tional working days.  If the  user  does  not  enter  a\n\
     value  for days on the command line, he is prompted for\n\
     a value.\n\n", ptr); break;
case 'E':
printf("%s -E keyword\n\n\
     Cancel delete lock for the file or directory associated\n\
     with  the  specified  keyword.  Only the user who tran-\n\
     scended the data, referred to as the owner,  can cancel\n\
     the  delete lock.  This option is  used  when the owner\n\
     no longer needs to protect data from being  deleted  by\n\
     other users.\n\n", ptr); break;
case 'f': sprintf(rtn.msg, "%s -f  [-site] file... | directory...\n\n\
     Put files or directories specified on the command  line\n\
     into  the  Transcend  repository.  The files and direc-\n\
     tories are tarred and compressed during  data  transfer\n\
     and  untarred  and  uncompressed during data retrieval.\n\
     The data is put in the receiving user's current working\n\
     directory as separate files and directories during data\n\
     retrieval.  Wild cards can be used to specify the  file\n\
     and directory  names.\n\n", ptr); break;
case 'F': sprintf(rtn.msg, "%s -F  [-site] file... | directory...\n\n\
     Put multiple files  or  directories  in  the  Transcend\n\
     repository   and   specify  deletion  after  the  first\n\
     retrieval.  The files are tarred and compressed  during\n\
     data transfer and untarred and uncompressed during data\n\
     retrieval.  The files and directories are  put  in  the\n\
     receiving  user's current working directory as separate\n\
     files and  directories  during  data  retrieval.   Wild\n\
     cards can be used to specify the file names.\n\n", ptr); break;
case 'g': sprintf(rtn.msg, "%s -g keyword\n\n\
     Retrieve the file or directory from  Transcend  associ-\n\
     ated  with  the  specified  keyword.  Data retrieved is\n\
     placed in the user's current working directory.  If the\n\
     data  is already present, the user is asked if he wants\n\
     to overwrite the existing data.\n\n", ptr); break;
case 'G': sprintf(rtn.msg, "%s -G keyword\n\n\
     Get the file or  directory  from  Transcend  associated\n\
     with  the  specified  keyword and delete from Transcend\n\
     after the current retrieval.\n\n", ptr); break;
case 'h': sprintf(rtn.msg, "%s -h keyword\n\n\
     Retrieve history for file or directory associated  with\n\
     the  specified keyword.  A summary of all commands per-\n\
     formed on the data is shown.  History can be  retrieved\n\
     for  up to two weeks after the original expiration date\n\
     of a file or directory.\n\n", ptr); break;
case 'H': sprintf(rtn.msg, "%s -H  -command\n\n\
     Display detailed help for specified command i.e. \n\
     trans -H -p'\n\n", ptr); break;
case 'i': sprintf(rtn.msg, "%s -i keyword | all\n\n\
          Retrieve information about a file or directory  associ-\n\
          ated with the specified keyword.  Information retrieved\n\
          includes file name, size,  creator,  date  transcended,\n\
          expiration date, data type, platform, and site.  If the\n\
          customer types 'trans -i all', a list of  keywords  for\n\
          data  the  customer  has  transferred  to all Transcend\n\
          repositories is shown.\n\n", ptr); break;
case 'I': sprintf(rtn.msg, "%s -I    Display defined iCAP classifications.\n\n", ptr); break;
case 'j': sprintf(rtn.msg, "%s -j customer-e-mail \n\n\
    Request approval for specified customer.", ptr); break;
case 'J': sprintf(rtn.msg, "%s -J keyword    Display file name for specified keyword.\n\n", ptr); break;
case 'k': sprintf(rtn.msg, "%s -k  [-site] directory file...\n\n\
     Package files listed on the command line into a  direc-\n\
     tory and put the directory in the Transcend repository.\n\
     This option is like the -f option except the files  are\n\
     bundled  together  in  a directory during data transfer\n\
     and  are  retrieved  in  the  directory   during   data\n\
     retrieval.   The directory the files are placed in is a\n\
     symbolic directory and does not need to be  present  on\n\
     the user's disk.  Wild cards can be used to specify the\n\
     file names.  Only files can be placed in the directory.\n\
     Other directories cannot be included.\n\n", ptr); break;
case 'K': sprintf(rtn.msg, "%s -K  [-site] directory file...\n\n\
     Package files in  specified  directory,  transcend  the\n\
     directory,   and   specify  deletion  after  the  first\n\
     retrieval.  This option is like the  -F  option  except\n\
     the  files  are  bundled together in a directory during\n\
     data transfer and are retrieved in the directory during\n\
     data  retrieval.  The directory the files are placed in\n\
     is a symbolic directory and does not need to be present\n\
     on  the user's disk.  Wild cards can be used to specify\n\
     the file names.  Only files can be placed in the direc-\n\
     tory.  Other directories cannot be included.\n\n", ptr); break;
case 'l': sprintf(rtn.msg, "%s -l keyword\n\n\
     Display original, compressed, and stored file length of\n\
     file  or  directory  associated with the keyword.  This\n\
     option provides a method for the user to verify data in\n\
     the  Transcend  repository is the correct size.  If the\n\
     compressed and stored data size are 0, the data was not\n\
     successfully transferred.\n\n", ptr); break;
case 'L': sprintf(rtn.msg, "%s -L keyword\n\n\
     Lock data to prevent retrieval.  This command is useful\n\
     when  the user has placed a file/directory in Transcend\n\
     and wants to wait for a period of time before  allowing\n\
     other users to retrieve the data.\n\n", ptr); break;
case 'm': sprintf(rtn.msg, "%s -m keyword email...\n\n\
     Mail multiple users a 9 character secure keyword  for\n\
     the  file  or  directory  associated with the specified\n\
     keyword.   Each  user  is  specified  by  his  friendly\n\
     NXP email address i.e. transbug@nxp.com.  If a speci-\n\
     fied user is not a registered  Transcend  user,  or  an\n\
     entry  for  the  user is not present in the Active Directory\n\
     directory, he is not sent email.  The secure  key-\n\
     words  sent  to  users may only be used by the user who\n\
     receives the keyword.  Secure keywords may be  used  to\n\
     perform  all  transactions  file keywords are used for.\n\
     Secure keywords can also be sent using the -N, -P,  -T,\n\
     -Z, -n, -p, -t, and -z options.\n\n", ptr); break;
	case 'M': sprintf(rtn.msg, "%s -M\n\n\
     Release  shared  memory  blocks  no  longer being used.\n\
     This command is executed to clean up shared memory  not\n\
     properly  released  when  parallel  data  transfers are\n\
     aborted or interrupted.\n\n", ptr); break;
case 'n': sprintf(rtn.msg, "%s -n  [-site] file-list [email...]\n\n\
     Transcend files or  directories  listed  in  the  named\n\
     file.   this  option  is  like the -f option except the\n\
     file names are listed in a file rather  than  specified\n\
     on the command line.  Names within the file must be put\n\
     on separate lines.  Wild cards cannot  be  used  within\n\
     the  file.   Secure  keywords  may  also be sent to multiple\n\
     users (see the -m option).\n\n", ptr); break;
case 'N': sprintf(rtn.msg, "%s -N  [-site] file-list [email...]\n\n\
     Transfer files listed in  the  named  file  and  delete\n\
     after  the first retrieval.  this option is like the -F\n\
     option except the file  names  are  listed  in  a  file\n\
     rather  than specified on the command line.  Wild cards\n\
     cannot be used within the file.\n\n", ptr); break;
case 'o':  sprintf(rtn.msg, "%s -o [-site]\n\n\
     Obtain user's registration status.  This feature is \n\
     useful if the user wants to check his registration status\n\
     without performing registration.\n\n", ptr); break;
case 'O': sprintf(rtn.msg, "%s -O  [-site] file | directory [email...]\n\n\
              Transfer  duplicate files to the mask shop.  Checks are in place\n\
              to prevent duplicate files from being sent  to  the  mask  shop.\n\
              Using  this  option  overrides  the  checks  in place and allows\n\
              duplicate files to be transferred.\n\n", ptr); break;
case 'q':  sprintf(rtn.msg, "%s -q | -ql [-<site>] | [all]\n\n\
          Display  list  of  file  keywords  for  data  user  has\n\
          uploaded to the Transcend repository.  If a site is not\n\
          specified, keywords for the closest site are shown.  If\n\
          the  site  is  set to 'all', keywords for all sites are\n\
          displayed.  The list of keywords is  shown  in  reverse\n\
          chronological  order,  and  the symbol '+' or '-' indi-\n\
          cates if the file data is still present in the  reposi-\n\
          tory.   If  the -ql option is used, a long list of file\n\
          keywords for data the user has uploaded  is  displayed.\n\
          This  list includes a history of transactions performed\n\
          for each keyword.  If the user has write permission  in\n\
          his  current  working directory, the list of key- words\n\
          is saved in a file called 'file_keywords'.\n\n", ptr); break;
case 'Q':  sprintf(rtn.msg, "%s -Q | -Ql [-<site>] | [all]\n\n\
          display list of personal secure keywords received  from\n\
          other  users.  If a site is not specified, keywords for\n\
          the closest site are shown.  If  the  site  is  set  to\n\
          'all',  keywords for all sites are displayed.  The list\n\
          of keywords is shown in  reverse  chronological  order,\n\
          and the symbol '+' or '-' indicates if the file data is\n\
          still present in the repository.  If the -Ql option  is\n\
          used, the list of secure keywords includes a history of\n\
          transactions performed for each keyword .  If the  user\n\
          has  write permission in his current working directory,\n\
          the  list  of  keywords  is  saved  in  a  file  called\n\
          'secure_keywords'.  Only secure keywords for the user's\n\
          registered e-mail address are shown.   Secure  keywords\n\
          sent to other users are not displayed.  This feature is\n\
          useful when the user has lost or has  not  received  e-\n\
          mail messages with secure keywords from other users.\n\n", ptr); break;
case 'p': sprintf(rtn.msg, "%s -p  [-site] file | directory [email...]\n\n\
          Put a file or directory in  the  Transcend  repository.\n\
          Transcend  repositories  are present in Austin, Munich,\n\
          Noida, Israel, and DTMS.  During data transfer,\n\
          the  default  site  is set to the server nearest to the\n\
          customer's site.  To specify a specific Transcend site,\n\
          use  the  abbreviations for the Austin,  Munich, Noida,\n\
          Israel, and DTMS  sites respectively.\n\
\n\
          Files, directories, and groups of files of any size may\n\
          be  transferred.   Files    are    compressed    during\n\
          data    transfer    and   uncompressed   during    data\n\
          retrieval.   Directories  are tarred   and   compressed\n\
          during  data  transfer  and uncompressed  and  untarred\n\
          during data retrieval.\n\
\n\
          After data transfer,  the user is given a randomly gen-\n\
          erated 9 character file keyword  he  can  give  to  any\n\
          user[s]  wishing  to retrieve the data.  If one or more\n\
          email addresses are specified, a secure keyword is sent\n\
          to  the specified email addresses.  Secure keywords are\n\
          valid only for the users who receive  them  via  email.\n\
          Both  file  and  secure  keywords  contain 4 lower case\n\
          alphabetic characters (a-z), followed by  four  numeric\n\
          characters  (0-9),  and  terminated  with an alphabetic\n\
          character.\n\
\n\
          During directory transfer,  gtar or gnutar is used   to\n\
          bundle/unbundle the data if it is present on the user's\n\
          machine.  Otherwise, tar is used.     Incompatibilities\n\
          are  sometimes  present  if  the sender of the data has\n\
          gtar installed on their machine,  and the receiver does\n\
          not.   The receiver can avoid incompatibilities by ins-\n\
          talling gtar on his machine.  Customers may also  force\n\
          the  use  of tar instead of gtar by using the '-pt' and\n\
          '-gt' options when putting  and  getting  data  respec-\n\
          tively.\n\n", ptr); break;
case 'P': sprintf(rtn.msg, "%s -P  [-site] file | directory [email...]\n\n\
     Put a file or directory in Transcend and specify  dele-\n\
     tion after the first successful retrieval.  this option\n\
     is used when a file will be retrieved by only one user.\n\n", ptr); break;
case 'r': sprintf(rtn.msg, "%s -r  [release] | [documentation]\n\n\
     Display list of commands which can be used to  retrieve\n\
     Transcend  releases and documents.  Transcend is avail-\n\
     able on the following platforms:  Solaris, \n\
     Linux, Ubuntu, AIX, Windows 7, and MacOS.\n\n", ptr); break;
case 'R': sprintf(rtn.msg, "%s -R [-site] core-ID [Active Directory-password]\n\n\
     Perform Transcend registration.  This option is  useful\n\
     when a user wants to register without executing another\n\
     Transcend command.  If the  user  does  not  enter  his\n\
     Active Directory  Password on the command line, he is prompted for\n\
     the password, and the password is  not  echoed  to  the\n\
     screen.\n\n", ptr); break;
case 's': sprintf(rtn.msg, "%s -s  [-site] all | keyword\n\n\
     Set email notification for one  or  more  files.   When\n\
     email notification is set, the user receives email each\n\
     time one  of his files is retrieved from Transcend.  If\n\
     a  keyword  is used, email notification is set only for\n\
     the associated data.  If the word 'all' is used,  email\n\
     notification  is  set  for all of the user's data.  The\n\
     user may set Email notification only for  data  he  has\n\
     transcended.\n\n", ptr); break;
	case 'S': sprintf(rtn.msg, "%s -s\n\n\
View status of InterCom servers.  This command is used by the \n\
GUI to verify supported servers are active.\n\n",
 ptr); break;
case 't': sprintf(rtn.msg, "%s -t  [-site] file | directory [email]\n\n\
     Tar a  directory  tree  following  symbolic  links  and\n\
     transfer  to  the Transcend repository.  This option is\n\
     different from the 'p' option because  symbolic  links\n\
     are not followed with the 'P' option.\n\n", ptr); break;
case 'T': sprintf(rtn.msg, "%s -T  [-site] directory [email...]\n\n\
     Tar a directory tree following symbolic links, transfer\n\
     to the Transcend repository, and delete after the first\n\
     successful retrieval.  This option  is  different  from\n\
     the  -P  option because symbolic links are not followed\n\
     with the -P option.\n\n", ptr); break;
case 'u': sprintf(rtn.msg, "%s -u [-site] <core user ID> | <first_name.last_name>\n\n\
     Display information about  user  retrieved  from  LDAP\n\
     directory.   The  user's  name, email address, telephone\n\
     number, department, location code, and  mail  drop  are\n\
     displayed.   Registered Transcend sites are also shown.\n\
     If the user is not present in the  LDAP  directory,  no\n\
     information   is   returned  even  if  the  user  is  a\n\
     registered Transcend user.\n\n", ptr); break;
case 'U': sprintf(rtn.msg, "%s -U keyword\n\n\
     Unlock data to allow data retrieval.   This  option  is\n\
     used  to  unlock  data  previously locked for retrieval\n\
     with the 'L' option.\n\n", ptr); break;
case 'v': sprintf(rtn.msg, "%s -v [-site]\n\n\
     Display program version, default server machine,  local\n\
     machine, trans.log directory, etc.  If a site is speci-\n\
     fied, information about the site is shown.\n\n", ptr);
break;
case 'V': sprintf(rtn.msg, "%s -V | -Vl [-<site>]\n\n\
          Display list of secure keywords received.  The keywords are shown in\n\
              reverse chronological order, and the symbol '+' or '-' indicates\n\
              if the file data is still present in the repository.  If the -Ql\n\
              option  is  used, a long list of secure keywords for data the user\n\
              has received is displayed.  This  list  includes  a  history  of\n\
              transactions  performed for each keyword.  If the user has write\n\
              permission in his current working directory, the  list  of  key-\n\
              words is saved in a file called 'secure_keywords'.\n\
          Only   secure   keywords  associated  with  the  user's\n\
          registered primary e-mail address  are  shown.   Secure\n\
          keywords  sent  to other users are not shown.  This  feature  is  \n\
          useful when the user has lost or has not received e-mail messages with\n\
          secure  keywords  from  other  users.   This  option is\n\
          identical to the '-Q' option.\n\n", ptr); break;
/*
     other users.  If a site is not specified, keywords for\n\
     the closest site are displayed.\n\n", ptr); break;
*/
case 'w': sprintf(rtn.msg, "%s -w     Ping Transcend/InterCom server\n\
          This command is used to verify the server is active\n\n", ptr); break;
case 'W': sprintf(rtn.msg, "%s -w     Silent ping to Transcend/InterCom server\n\
          This command is used to verify the server is active\n\n", ptr); break;
case 'X': sprintf(rtn.msg, "%s -XX     Display definedTranscend/InterCom server  repositories.   \n\
              Currently  defined  server  repositories  include  Austin, Munich,\n\
              Noida, DTMS, External TransWeb Customer, Send2foundry, and ITAR.\n\n", ptr); break;
case 'x': sprintf(rtn.msg, "%s -xx     Display  Transcend  sites  and  external companies.  The company\n\
              name and site abbreviation are shown.", ptr); break;
case 'y': sprintf(rtn.msg, "%s -y customer-e-mail \n\n\
    Perform approval for specified customer.", ptr); break;
case 'Y':  strcpy(rtn.msg, "Administrative command to extract registry\n\n"); break;
case 'z': sprintf(rtn.msg, "%s -z  [-site] directory file-list [email...]\n\n\
Zip/package files listed in a named file  into  a  directory\n\
and  put  the  directory  in the Transcend repository.  This\n\
option is like the -k option except the files are listed  in\n\
a  named file rather than on the command line.  Names within\n\
the file must be put  on  separate  lines,  and  wild  cards\n\
cannot  be used within the file.  The files are bundled into\n\
a  directory  during  data  transfer  and  retrieved  as   a\n\
directory  during  data  retrieval.  The directory the files\n\
are placed in is a symbolic directory and does not  need  to\n\
be  present  on  the sending user's disk.  Only files can be\n\
placed  in  the  directory.   Other  directories  cannot  be\n\
included.   Secure  keywords  may  also be sent to multiple users\n\
(see the -m option).\n\n", ptr); break;
case 'Z': sprintf(rtn.msg, "%s -Z  [-site] directory file [email...]\n\n\
     Zip/package listed files into the specified  directory,\n\
     transcend the directory, and specify deletion after the\n\
     first retrieval.\n\n", ptr); break;
case '1': sprintf(rtn.msg, "     %s -1 [-_s_i_t_e] _C_o_r_e _U_s_e_r _I_D | _f_i_r_s_t-_n_a_m_e._l_a_s_t-_n_a_m_e\n\
          Display information about NXP users with an  entry  in\n\
          the  LDAP  directory.   Data  displayed includes name,\n\
          phone,  department,  etc.  This command is identical to\n\
          the  'User  command' with the exception that registered\n\
          Transcend sites are not displayed.   this command  runs\n\
          faster  than the 'User command' because time to look up\n\
          the user in the Transcend registry is eliminated.\n\n",
ptr); break;
default:
{
    if (help_option == DBYES)
    {
	strcpy(rtn.msg, help_err);
	return(HELP_ERR);
    }
    else return(-1);
} /* End default */
} /* End switch */
    return(SUCCESS);
} /* End help_print */
#endif
 /* for Transcend */
#endif /* for TRANSAPI */

