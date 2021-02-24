#ifndef LIBSARGASC_H
#define LIBSARGASC_H

//#include <tm_reader.h>
#include "../c/src/api/tm_reader.h"
//#include "../include/tm_reader.h"

#ifndef         BOOL
typedef         int    BOOL;
#endif
#define         TRUE   1
#define         FALSE  0
#define         LOGD   printDebug
#define         LOGE   printDebug

enum            OPTIONS {
                EXIT_OPT       = 0,    CONNECT_OPT    = 1,
                DISCONNECT_OPT = 2,    READTAG_OPT    = 3
};

void            errx(int exitval, const char *fmt, ...);
void            checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg);
void            serialPrinter(bool tx, uint32_t dataLen, const uint8_t data[],
                   uint32_t timeout, void *cookie);
void            stringPrinter(bool tx,uint32_t dataLen, const uint8_t data[],uint32_t timeout, void *cookie);
void            parseAntennaList(uint8_t *antenna, uint8_t *antennaCount, char *args);
int             init(int argc, char *argv[], TMR_Reader *rp, int flag);
void            DeInit(TMR_Reader *reader);
int             readtag(int argc, char *argv[], TMR_Reader *rp);
void            printDebug(const char *tag, const char *format, ...);
int             getStdin();
void            cleanScreen();
void            noticeScreen(const char *str);


#endif // LIBSARGASC_H
