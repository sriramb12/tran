#ifndef m2client_msg_h
#define m2client_msg_h

#define CAP_MSG \
"To add mask set name to a cap wafer list:  Type 'client -C -a <cap_wafer> <mask>'\n\
To retrieve a cap wafer file:              Type 'client -C -r <cap_wafer>'\n\
To list masks associated with a cap wafer: Type 'client -C -l <cap_wafer>'\n\
To display the cap wafer for a mask set:   Type 'client -C -R <mask>'\n\n"

#define CRON_MSG \
"To resend data not previously sent, Type 'client -c -r'\n\
To send administrator copy of weekly audit log, Type 'client -c -a'\n\n\
To send group of administrators copy of weekly audit log, Type 'client -c -g'\n\n\
To audit specified directory: Type 'client -c -a'\n"

#define USERGROUP_MSG \
"To create a group:             Type 'client -u -c <group name>'\n\
To add users to a group:       Type 'client -u -a <group> <email1 ... email5'\n\
To delete users from a group:  Type 'client -u -d <group> <email1> ...email5>'\n\
To delete the entire group:    Type 'client -u -D <group>'\n\
To view users in a group:      Type 'client -u -l <group>'\n\n"

#define FILELIST_MSG \
"To add a file to the file list:       Type 'client -L -a\n\
To Delete a file from the file list:  Type 'client -L -d\n\n"

#define REGISTER_MSG \
"To check registration:  Type 'client -r -c'\n\
To perform auto registration:  Type 'client -r -a\n\
For manual registration:  Type 'client -r -m <login> <subnet> <email>'\n\
To modify your registration PIN:  Type 'client -r -p <old pin> <new pin>'\n\n"

#define DATA_MSG \
"For mask data:               Type 'client -m'\n\
For global tables:           Type 'client -g'\n\
For official global tables:  Type 'client -G'\n\
For Fab data:                Type 'client -f'\n\
For machine tables:          Type 'client -M'\n\
For template data:           Type 'client -t'\n\
For official template data:  Type 'client -T'\n\
For PC data:                 Type 'client -p'\n\
For contract/serial numbers: Type client -s\n\
For user group functions:    Type 'client -u'\n\
For network requests:        Type 'client -n'\n"

#define FAB_MSG \
"To create a new fab:        Type 'client -f -c <fab>'\n\
To delete an existing fab:  Type 'client -f -d fab override'\n\
To rename an existing fab:  Type 'client -f -r <new fab> <old fab>'\n\
For a list of fabs:         Type 'client -f -l'\n\n"

#define MACHINE_MSG \
"To retrieve machine table log:  Type 'client -M -l <fab>'\n\
To check out machine tables:    Type 'client -M -o <fab> <override>'\n\
To read machine tables:         Type 'client -M -r <fab> <override>'\n\
To retrieve official copy:      Type 'client -M -R <override>'\n\
To cancel check out:            Type 'client -M -b <fab>'\n\
To check in machine tables:     Type 'client -M -i <fab> <override'\n\
To update machine tables:       Type 'client -M -u <fab> <override>'\n\
To approve machine tables:      Type 'client -M -a <fab>'\n\
To shadow machine tables:       Type 'client -M -h <fab>'\n\
To perform audit on all data:   Type 'client -M -A'\n\
To perform weekly audit:        Type 'client -M -W'\n\
To perform daily audit:         Type 'client -M -D'\n\
To perform hourly audit:        Type 'client -M -H'\n\n"

#define GLOBAL_MSG \
"To create a global table:      Type 'client -g -c <table>'\n\
To check out a global table:   Type 'client -g -o <table> <override>'\n\
To read a global table:        Type 'client -g -r <table> <override>'\n\
To retrieve official copy:     Type 'client -g -R <file> <override>'\n\
To retrieve all global files:  Type 'client -g -A'\n\
To cancel check out:           Type 'client -g -b <table> <override>'\n\
To check in a global table:    Type 'client -g -i <table> <override>'\n\
To update a global table:      Type 'client -g -u <table>, <override>'\n\
To approve a global table:     Type 'client -g -a <table>'\n\
To delete a global table:      Type 'client -g -d <table> <override>'\n\
To shadow a global table:      Type 'client -g -h <table>'\n\n"

#define MASK_MSG \
"To create a mask set:        Type 'client -m -c <mask> <fab> <technology> <flow> <design>'\n\
To copy a mask set:          Type 'client -m -y <mask> <old mask>'\n\
To check out a mask set:     Type 'client -m -o <mask> <override> <autosave>  '\n\
To read a mask set:          Type 'client -m -r <mask> <override> <autosave>'\n\
To read prior version:       Type 'client -m -p <mask> <override> <version>'\n\
To check out prior version:  Type 'client -m -P <mask> <override> <version>'\n\
To cancel check out:         Type 'client -m -b <mask>'\n\
To check in a mask set:      Type 'client -m -i <mask> <override>'\n\
To update a mask set:        Type 'client -m -u <mask> <override>'\n\
To save the .db files:       Type 'client -s <mask>'\n\
To append a mask set, type 'client -m -a <mask>'\n\
To retrieve mask check out status:  type client -m -q <mask>'\n\
To delete a mask set:        Type 'client -m -d <mask> <override>'\n\
To shadow mask data:         Type 'client -m -h <mask>'\n\
To list all mask sets:       Type client -m -L'\n\
To list mask sets with specified criteria:  Type 'client -m -l'\n\
To add/modify users/usergroups: Type 'client -m -e'\n\
To audit all mask set data:  Type 'client -m -A'\n\
To perform weekly audit:     Type 'client -m -W'\n\
To perform daily audit:      Type 'client -m -D'\n\
To perform hourly audit:     Type 'client -m -H'\n\n"

#define TECH_MSG \
"To create a template:        Type 'client -t -c <new fab> <new tech> < old fab> <old technology>'\n\
To copy a template:          Type'client -t -y <fab> <tech> <mask>'\n\
To check out a template:     Type'client -t -o <fab> <tech> <override> \n\
To read a template:          Type'client -t -r <fab> <tech> <override>'\n\
To retrieve official copy:   Type 'client -t -R <fab> <tech> <override>'\n\
To read prior version:       Type 'client -t -p <fab> <tech> <override> <version>'\n\
To check out prior version:  Type 'client -t -P <fab> <tech> <override> <version>'\n\
To cancel check out:         Type  'client -t -b <fab> <tech> <override>'\n\
To cancel read:              Type  'client -t -B <fab> <tech>'\n\
To check in a template:      Type 'client -t -i <fab> <tech> <override>'\n\
To update a template:        Type 'client -t -u <fab> <tech>' <override>\n\
To save the .db files:       Type 'client -s <fab> <tech>'\n\
To retrieve template check out status:  type client -t -q <fab> <tech>'\n\
To delete a template:        Type 'client -t -d <fab> <tech> <override>'\n\
To shadow template data:     Type 'client -t -h <fab> <tech>'\n\
To approve a template:       Type 'client -t -a <fab> <tech>'\n\
To unapprove a template:     Type 'client -t -U <fab> <tech>'\n\
For a list of technologies:  Type 'client -t -l <fab>'\n\
To audit all template data:  Type 'client -t -A'\n\
To perform weekly audit:     Type 'client -t -W'\n\
To perform daily audit:      Type 'client -t -D'\n\
To perform hourly audit:     Type 'client -t -H'\n\n"

#define OFF_TECH_MSG \
"To perform audit on all data:  Type 'client -T -A'\n\
To perform weekly audit:  Type 'client -T -W'\n\
To perform daily audit:  Type 'client -T -D'\n\
To perform hourly audit:  Type 'client -T -H'\n\n"

#define OFF_GLOBAL_MSG \
"To perform audit on all data:  Type 'client -g -A'\n\
To perform weekly audit:  Type 'client -G -W'\n\
To perform daily audit:  Type 'client -G -D'\n\
To perform hourly audit:  Type 'client -G -H'\n\n"

#define PC_MSG \
"To create a PC library:       Type 'client -p -c <pc> <control group> <files>'\n\
To read a PC library:         Type 'client -p -r <pc>'\n\
To update a PC library:       Type 'client -p -u <pc> <control group> <files>'\n\
To list files in PC library:  Type 'client -p -l <pc>'\n\
To list links in PC library:  Type 'client -p -R <pc>'\n\
To list all PC libraries:     Type 'client -p -L'\n\
To delete a PC library:       Type 'client -p -d <pc> <override>'\n\n"

#define PARTITION_MSG \
"To create a Partition:       Type 'client -P -c <pc> <control group> <files>'\n\
To read a PC library history:         Type 'client -P -r <pc>'\n\
To update a PC library history:       Type 'client -P -u <pc> <control group> <files>'\n\
To list files in Partition:  Type 'client -P -l <pc>'\n\
To list all PC libraries:     Type 'client -P -L'\n\
To delete a Partition:       Type 'client -P -d <pc> <override>'\n\n"

#define LINK_MSG \
"To add a link:  Type 'client -l -a <pc> <fab> <tech> <override>'\n\
To delete a link:  Type 'client -l -d <pc> <fab> <tech>'\n\
To modify a link:  Type 'client -l -m <pc> <fab> <tech>'\n\n"
#define CONTRACT_SERIAL_MSG \
"For com1 serial numbers:       Type 'client -s -1\n\
For Mos6 serial numbers:      Type client -s -s\n\
For Mos16 serial numbers:      Type client -s -6\n\
For Mos17 serial numbers:      Type client -s -7\n\
For Mos21 serial numbers:      Type client -s -2\n\
For mask set names:          Type 'client -s -m'\n\
For PC names:                  Type 'client -s -p '\n\
For Photronics serial numbers: Type client -s -n <mask>'\n\
For serial numbers for Toppan, Type 'client -s -d <mask>'\n\n"

#endif
