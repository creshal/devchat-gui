#define APPNAME "DevchatGUI"
#define VERSION "1.0.0"
#define TIMEOUT 3000
int debug;
int real_debug;
int no_config;
int no_spellcheck;
#ifndef G_OS_WIN32
#define NOTIFY
#endif
#define NLS
#define SPELLCHECK
