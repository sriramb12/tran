#define DECRYPT_COMMAND \
# echo "Running script to combine and decrypt file.\n\
cat %s/* |  gpg --s2k-mode N --output %s --decrypt -\n"

#define COMBINE_COMMAND  \
"echo "Running script to combine files.\n\
cat %s/* >%s"

#define DECRYPT_MSG \
"\nThe data has been split up into multiple small files.\n\
Type the following commands to combine the files and decrypt the data.\n\n\
cd %s%s\n\
cat %s/* |  gpg --s2k-mode N --output %s --decrypt -\n\n\
You will then be prompted for your passphrase.\n"

#define DECRYPT_MSG \
"\nThe data has been split up into multiple small files.\n\
A script which can be used to combine the files and decrypt the\n\
resulting file is present in directory:\n\
    '%s'\n\
To run this script cd to %s and type:\n\
chmod 700 combine_decrypt%s\n\
combine_decrypt%s\n\
You will then be prompted for your passphrase.\n\n\
If you would like to combine the files without decrypting them\n\
execute the following commands:\n\
cd %s\n\
chmod 700 combine_script%s\n\
combine_script%s\n\
The file created can then be decrypted at a later time.\n"

