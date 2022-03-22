#ifndef transrc_h
#define transrc_h
extern int rcread (char *rcfile);

struct transrc_t {
  int hashBySize;
  int hashByTime;
  char logDir[1024];
  char historyDir[1024];
    char machine[32];
    char address[20];
    int socket;
    unsigned long long parallel_zip_size;
    unsigned long long separate_block_size;
};

extern struct transrc_t transrc;

#endif
