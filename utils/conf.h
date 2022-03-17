typedef enum { EXT_SERVER, INT_SERVER, EXT_CLIENT, INT_CLIENT} Role;
struct param
{
  Role role;
  char key[20];
  char value[20];
};

extern struct param confParams [];
