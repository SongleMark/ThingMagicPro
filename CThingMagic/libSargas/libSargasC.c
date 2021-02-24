#include "libSargasC.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <fcntl.h>

#define  BSIZE  2048
#define  INSIZE 1024

#ifndef  BARE_METAL
#if WIN32
#define snprintf sprintf_s
#endif

/* Enable this to use transportListener */
#ifndef USE_TRANSPORT_LISTENER
#define USE_TRANSPORT_LISTENER 0
#endif

#define usage() {errx(1, "Please provide reader URL, such as:\n"\
    "tmr:///com4 or tmr:///com4 --ant 1,2\n"\
    "tmr://my-reader.example.com or tmr://my-reader.example.com --ant 1,2\n");}

void errx(int exitval, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);

    exit(exitval);
}

void checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg) {
    if (TMR_SUCCESS != ret) {
        errx(exitval, "Error %s: %s\n", msg, TMR_strerr(rp, ret));
    }
}

void serialPrinter(bool tx, uint32_t dataLen, const uint8_t data[],
                   uint32_t timeout, void *cookie)
{
    FILE *out = cookie;
    uint32_t i;

    fprintf(out, "%s", tx ? "Sending: " : "Received:");
    for (i = 0; i < dataLen; i++)
    {
        if (i > 0 && (i & 15) == 0)
        {
            fprintf(out, "\n         ");
        }
        fprintf(out, " %02x", data[i]);
    }
    fprintf(out, "\n");
}

void stringPrinter(bool tx,uint32_t dataLen, const uint8_t data[],uint32_t timeout, void *cookie) {
    FILE *out = cookie;

    fprintf(out, "%s", tx ? "Sending: " : "Received:");
    fprintf(out, "%s\n", data);
}

void parseAntennaList(uint8_t *antenna, uint8_t *antennaCount, char *args) {
    char *token = NULL;
    char *str = ",";
    uint8_t i = 0x00;
    int scans;

    /* get the first token */
    if (NULL == args) {
        fprintf(stdout, "Missing argument\n");
        usage();
    }

    token = strtok(args, str);
    if (NULL == token) {
        fprintf(stdout, "Missing argument after %s\n", args);
        usage();
    }

    while(NULL != token) {
        scans = sscanf(token, "%"SCNu8, &antenna[i]);
        if (1 != scans) {
            fprintf(stdout, "Can't parse '%s' as an 8-bit unsigned integer value\n", token);
            usage();
        }
        i ++;
        token = strtok(NULL, str);
    }
    *antennaCount = i;
}
#endif

int init(int argc, char *argv[], TMR_Reader *rp, int flag) {
    //TMR_Reader r, *rp;
    TMR_Status ret;
    TMR_ReadPlan plan;
    TMR_Region region;
    uint8_t *antennaList = NULL;
    uint8_t buffer[20];
    uint8_t i;
    uint8_t antennaCount = 0x0;
    TMR_String model;
    char str[64];

    if (argc < 2) {
        usage();
    }

    for (i = 2; i < argc; i+=2) {
        if(0x00 == strcmp("--ant", argv[i])) {
            if (NULL != antennaList) {
                fprintf(stdout, "Duplicate argument: --ant specified more than once\n");
                usage();
            }
            parseAntennaList(buffer, &antennaCount, argv[i+1]);
            antennaList = buffer;
        }
        else {
            fprintf(stdout, "Argument %s is not recognized\n", argv[i]);
            usage();
        }
    }

    ret = TMR_create(rp, argv[1]);
    if(TMR_SUCCESS != ret) {
        LOGE("TMR_create", "TMR_create failed !!!");
        return -1;
    }

    ret = TMR_connect(rp);
    if(TMR_SUCCESS != ret) {
        LOGE("TMR_connect", "TMR_connect failed !!!");
        return -1;
    }

    region = TMR_REGION_NONE;
    ret = TMR_paramGet(rp, TMR_PARAM_REGION_ID, &region);
    if(TMR_SUCCESS != ret) {
        LOGE("TMR_paramGet", "getting region failed !!!");
        return -1;
    }

    if (TMR_REGION_NONE == region) {
        TMR_RegionList regions;
        TMR_Region _regionStore[32];
        regions.list = _regionStore;
        regions.max = sizeof(_regionStore)/sizeof(_regionStore[0]);
        regions.len = 0;

        ret = TMR_paramGet(rp, TMR_PARAM_REGION_SUPPORTEDREGIONS, &regions);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramGet", "getting supported regions failed !!!");
            return -1;
        }
        if (regions.len < 1) {
            LOGE("regions.len < 1", "Reader doesn't supportany regions");
            //checkerr(rp, TMR_ERROR_INVALID_REGION, __LINE__, "Reader doesn't supportany regions");
        }

        region = regions.list[0];
        ret = TMR_paramSet(rp, TMR_PARAM_REGION_ID, &region);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramSet", "setting region failed !!!");
            return -1;
        }
    }

    model.value = str;
    model.max = 64;
    TMR_paramGet(rp, TMR_PARAM_VERSION_MODEL, &model);
    if (((0 == strcmp("Sargas", model.value)) || (0 == strcmp("M6e Micro", model.value)) ||(0 == strcmp("M6e Nano", model.value)))
            && (NULL == antennaList)) {
        LOGE("TMR_paramGet Module", "Module doesn't has antenna detection support please provide antenna list !!!");
        return -1;
    }

    /**
    * for antenna configuration we need two parameters
    * 1. antennaCount : specifies the no of antennas should
    *    be included in the read plan, out of the provided antenna list.
    * 2. antennaList  : specifies  a list of antennas for the read plan.
    **/

    // initialize the read plan
    ret = TMR_RP_init_simple(&plan, antennaCount, antennaList, TMR_TAG_PROTOCOL_GEN2, 1000);
    if(ret != TMR_SUCCESS) {
        LOGE("TMR_RP_init_simple", "initializing the  read plan failed !!!");
        return -1;
    }

    /* Commit read plan */
    ret = TMR_paramSet(rp, TMR_PARAM_READ_PLAN, &plan);
    if(ret != TMR_SUCCESS) {
        LOGE("TMR_paramSet", "setting read plan failed !!!");
        return -1;
    }
    if(READTAG_OPT == flag) {
        printf("Please input read's time(ms): ");
        char stdIn[INSIZE];
        int intTime = 0;
        if(NULL != fgets(stdIn, INSIZE, stdin)) {
            stdIn[strlen(stdIn)] = '\0';
            intTime = atoi(stdIn);
        }
        ret = TMR_read(rp, intTime, NULL);
        if(ret == TMR_ERROR_TAG_ID_BUFFER_FULL) {
            /* In case of TAG ID Buffer Full, extract the tags present
             * in buffer.
             */
            LOGE("TMR_read", "reading tags: %s", TMR_strerr(rp, ret));
        }
        else if(TMR_SUCCESS == ret) {
            LOGD("TMR_read", "TMR_read successed !!!");
        }
        else {
            LOGE("TMR_read", "TMR_read failed !!!");
            return -1;
        }
    }
    LOGD("Init", "Init successed !!!");
    if(CONNECT_OPT == flag) {
        TMR_destroy(rp);
    }

    return 0;
}

void DeInit(TMR_Reader *reader) {
    TMR_destroy(reader);
}

int readtag(int argc, char *argv[], TMR_Reader *rp) {
    if(0 != init(argc, argv, rp, READTAG_OPT)) {
        LOGE("Readtag", "init reader failed don't read ...");
        return -1;
    }
    TMR_Status ret;
    while (TMR_SUCCESS == TMR_hasMoreTags(rp)) {
        TMR_TagReadData trd;
        char epcStr[128];

        ret = TMR_getNextTag(rp, &trd);
        if(TMR_SUCCESS != ret) {
            LOGE("TMR_getNextTag", "fetching tag failed !!!");
            return -1;
        }
        TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);
        LOGD("Read_Tag", "EPC:%s ant:%d count:%d ", epcStr, trd.antenna, trd.readCount);
    }
    TMR_destroy(rp);

    return 0;
}

void printDebug(const char *tag, const char *format, ...) {
    if(NULL == tag || NULL == format) {
        return ;
    }
    // show time
    struct tm    *ptm = NULL;
    struct timeb stTimeb;
    ftime(&stTimeb);
    ptm = localtime(&stTimeb.time);
    char buf[BSIZE];
    char buftime[1024];
    int year = ptm->tm_year + 1900;
    int mon = ptm->tm_mon + 1;
    sprintf(buftime, "%d/%02d/%02d %02d:%02d:%02d.%03d",
            year, mon, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);

    printf("%s [%s]: ", buftime, tag);
    // debug log
    va_list argp;
    va_start(argp, format);
    vsprintf(buf, format, argp);
    printf("%s\n", buf);
}

int getStdin() {
    char stdIn[INSIZE];
    printf("Please enter options: ");
    int intValue = 0;
    if(NULL != fgets(stdIn, INSIZE, stdin)) {
        stdIn[strlen(stdIn)] = '\0';
        intValue = atoi(stdIn);
    }

    return intValue;
}

void cleanScreen() {
    system("clear");
}

void noticeScreen(const char *str) {
    //LOGD(TAG, "%s", str);
    printf("%s\n", str);
    printf("Enter any key to continue ... ");
    getchar();
}
