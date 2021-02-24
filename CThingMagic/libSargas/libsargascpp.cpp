#include "libsargascpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <unistd.h>
#include <queue>
//#include "../events/error_event.h"
//#include "../events/libitkcpp_event.h"
//#include "../events/event_types.h"

#define     BSIZE               5000
#define     INSIZE              1024
#define     RED                 "\e[31m"
#define     GRN                 "\e[32m"
#define     SRC                 "\e[0m"
#ifndef     forever
#define     forever             while(1)
#endif

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

LibSargasCpp::LibSargasCpp() :
    m_Readerp(&m_Reader),
    m_AntennaList(NULL),
    m_AntennaCount(0x0),
    m_IsReaderCreate(false),
    m_IsStop(true),
    m_IsBreak(false),
    m_IsStartThread(false),
    m_IsThreadCreateSuccess(false),
    m_ItkListener(NULL),
    m_Index(0)
{

}

LibSargasCpp::~LibSargasCpp() {

}

void LibSargasCpp::errx(int exitval, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);

    exit(exitval);
}

void LibSargasCpp::checkerr(TMR_Reader *rp, TMR_Status ret, int exitval, const char *msg) {
    if (TMR_SUCCESS != ret) {
        errx(exitval, "Error %s: %s\n", msg, TMR_strerr(rp, ret));
    }
}

void LibSargasCpp::serialPrinter(bool tx, uint32_t dataLen, const uint8_t *data, uint32_t timeout, void *cookie) {
    FILE *out = (FILE *)cookie;
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

void LibSargasCpp::stringPrinter(bool tx, uint32_t dataLen, const uint8_t *data, uint32_t timeout, void *cookie) {
    FILE *out = (FILE *)cookie;

    fprintf(out, "%s", tx ? "Sending: " : "Received:");
    fprintf(out, "%s\n", data);
}

void LibSargasCpp::parseAntennaList(uint8_t *antenna, uint8_t *antennaCount, char *args) {
    char *token = NULL;
    char str[] = ",";
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
        scans = sscanf(token, "%hhu"/*SCNu8*/, &antenna[i]);
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

void LibSargasCpp::printDebug(const char *tag, const char *format, ...) {
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

    std::cout << GRN;
    printf("%s [%s]: ", buftime, tag);
    // debug log
    va_list argp;
    va_start(argp, format);
    vsprintf(buf, format, argp);
    std::cout << buf << SRC << std::endl;
}

void LibSargasCpp::printError(const char *tag, const char *format, ...) {
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

    std::cout << RED;
    printf("%s [%s]: ", buftime, tag);
    // debug log
    va_list argp;
    va_start(argp, format);
    vsprintf(buf, format, argp);
    std::cout << buf << SRC << std::endl;
}

void LibSargasCpp::printT(const char *format, ...) {
    if(NULL == format) {
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

    std::cout << GRN;
    printf("%s: ", buftime);
    // debug log
    va_list argp;
    va_start(argp, format);
    vsprintf(buf, format, argp);
    std::cout << "=> " << buf << SRC << std::endl;
}

int LibSargasCpp::getStdin() {
    char stdIn[INSIZE];
    printf("Please enter options: ");
    int intValue = 0;
    if(NULL != fgets(stdIn, INSIZE, stdin)) {
        stdIn[strlen(stdIn)] = '\0';
        intValue = atoi(stdIn);
    }

    return intValue;
}

void LibSargasCpp::cleanScreen() {
    system("clear");
}

void LibSargasCpp::noticeScreen(const char *str) {
    LOGD("Notice", "%s", str);
    printf("Enter any key to continue ... ");
    getchar();
}

void LibSargasCpp::readAllMemBanks(TMR_TagOp *op, TMR_TagFilter *filter, TMR_Reader *rp) {
    TMR_ReadPlan plan;
    uint8_t data[258];
    TMR_Status ret;
    TMR_uint8List dataList;
    dataList.len = dataList.max = 258;
    dataList.list = data;
    const char TAG[] = "readAllMemBanks";

    TMR_RP_init_simple(&plan, m_AntennaCount, m_AntennaList, TMR_TAG_PROTOCOL_GEN2, 1000);

    ret = TMR_RP_set_filter(&plan, filter);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "setting tag filter failed at %d in %s", __LINE__, __FILE__);
        return ;
    }

    ret = TMR_RP_set_tagop(&plan, op);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "setting tagop failed at %d in %s", __LINE__, __FILE__);
        return ;
    }

    /* Commit read plan */
    ret = TMR_paramSet(rp, TMR_PARAM_READ_PLAN, &plan);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "setting read plan failed at %d in %s", __LINE__, __FILE__);
        return ;
    }

    ret = TMR_read(rp, 500, NULL);
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret) {
        /* In case of TAG ID Buffer Full, extract the tags present
         * in buffer.
         */
        LOGE(TAG, "reading tags: %s", TMR_strerr(rp, ret));
    }
    else {
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "reading tags failed at %d in %s", __LINE__, __FILE__);
            return ;
        }
    }

    while (TMR_SUCCESS == TMR_hasMoreTags(rp)) {
        TMR_TagReadData trd;
        uint8_t dataBuf[258];
        uint8_t dataBuf1[258];
        uint8_t dataBuf2[258];
        uint8_t dataBuf3[258];
        uint8_t dataBuf4[258];
        char epcStr[128];

        ret = TMR_TRD_init_data(&trd, sizeof(dataBuf)/sizeof(uint8_t), dataBuf);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "creating tag read data failed at %d in %s", __LINE__, __FILE__);
            return ;
        }

        trd.userMemData.list = dataBuf1;
        trd.epcMemData.list = dataBuf2;
        trd.reservedMemData.list = dataBuf3;
        trd.tidMemData.list = dataBuf4;

        trd.userMemData.max = 258;
        trd.userMemData.len = 0;
        trd.epcMemData.max = 258;
        trd.epcMemData.len = 0;
        trd.reservedMemData.max = 258;
        trd.reservedMemData.len = 0;
        trd.tidMemData.max = 258;
        trd.tidMemData.len = 0;

        ret = TMR_getNextTag(rp, &trd);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "fetching tag failed at %d in %s", __LINE__, __FILE__);
            return ;
        }

        TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);
        LOG("%s", epcStr);
#if 1
        if (0 < trd.data.len) {
            char dataStr[258];
            TMR_bytesToHex(trd.data.list, trd.data.len, dataStr);
            LOG("data(%d): %s", trd.data.len, dataStr);
        }

        if (0 < trd.userMemData.len) {
            char dataStr[258];
            TMR_bytesToHex(trd.userMemData.list, trd.userMemData.len, dataStr);
            LOG("userData(%d): %s", trd.userMemData.len, dataStr);
        }
        if (0 < trd.epcMemData.len) {
            char dataStr[258];
            TMR_bytesToHex(trd.epcMemData.list, trd.epcMemData.len, dataStr);
            LOG("epcData(%d): %s", trd.epcMemData.len, dataStr);
        }
        if (0 < trd.reservedMemData.len) {
            char dataStr[258];
            TMR_bytesToHex(trd.reservedMemData.list, trd.reservedMemData.len, dataStr);
            LOG("reservedData(%d): %s", trd.reservedMemData.len, dataStr);
        }
        if (0 < trd.tidMemData.len) {
            char dataStr[258];
            TMR_bytesToHex(trd.tidMemData.list, trd.tidMemData.len, dataStr);
            LOG("tidData(%d): %s", trd.tidMemData.len, dataStr);
        }
#endif
    }
#if 1
    ret = TMR_executeTagOp(rp, op, filter, &dataList);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "executing the read all mem bank failed at %d in %s", __LINE__, __FILE__);
        return ;
    }
    if (0 < dataList.len) {
        char dataStr[258];
        TMR_bytesToHex(dataList.list, dataList.len, dataStr);
        LOG("Data(%d): %s", dataList.len, dataStr);
    }
#endif
}

bool LibSargasCpp::isConnectedToReader() {
    return m_IsReaderCreate;
}

void *LibSargasCpp::encodingThread(void *thread) {
    pthread_detach(pthread_self());
    LibSargasCpp *task = (LibSargasCpp *)thread;
    task->m_IsThreadCreateSuccess = true;
    char method[64];
    char msg[64];
    forever {
        if(task->m_IsBreak) {
            break;
        }
        while(!task->m_IsStop) {
            if(task->m_IsStartThread && task->m_IsReaderCreate && task->m_PubDataVct.size() > 0) {
                if(task->m_Index >= 4) {
                    LOGD("encodingThread", "encoding end and vector is run out of");
                    task->stopEncodingThread();
                    break;
                }

                if(0 == task->enCoding(task->m_PubDataVct.at(task->m_Index).tid, task->m_PubDataVct.at(task->m_Index).epc)) {
                    LOGD("encodingThread", "encoding finished index = %d", task->m_Index);
                    sprintf(method, "method OK: index = %d", task->m_Index);
                    sprintf(msg, "message OK: index =%d", task->m_Index);
                    task->m_Index ++;
                    task->m_IsStartThread = false;
                    task->publishMessage(method, msg);
                }
            }
            ::sleep(1);
        }
        ::usleep(500000);
    }

    task->m_IsThreadCreateSuccess = false;

    pthread_exit(0);
}

void LibSargasCpp::startEncodingThread() {
    m_IsBreak = false;
    m_IsStop  = false;
    m_IsStartThread = true;

    // test data
    if(0 == m_Index || m_Index >= 4) {
        m_Index = 0;
        m_PubDataVct.clear();
        PubDataItem pubData1;
        pubData1.tid = "112233445566";
        pubData1.epc = "668866886688";
        m_PubDataVct.push_back(pubData1);
        PubDataItem pubData2;
        pubData2.tid = "223344556677";
        pubData2.epc = "112211221122";
        m_PubDataVct.push_back(pubData2);
        PubDataItem pubData3;
        pubData3.tid = "334455667788";
        pubData3.epc = "223322332233";
        m_PubDataVct.push_back(pubData3);
        PubDataItem pubData4;
        pubData4.tid = "445566778899";
        pubData4.epc = "556655665566";
        m_PubDataVct.push_back(pubData4);
    }
}

void LibSargasCpp::stopEncodingThread() {
    m_IsStartThread = false;
    m_IsStop        = true;
}

void LibSargasCpp::setStartThreadFlag() {
    m_IsStartThread = true;
}

void LibSargasCpp::publishMessage(std::string method, std::string message) {
    if(m_ItkListener) {
        m_ItkListener->onSendEvent(method, message);
    }
}

void LibSargasCpp::addListener(SargasEventListener *listener) {
    m_ItkListener = listener;
}

int LibSargasCpp::getReaderInfo() {
    if(!m_IsReaderCreate) {
		LOGE("ERROR", "Please init before get Reader Information !!!");
		return -1;
	}
	TMR_Region region;
    TMR_Status ret;
    const char TAG[] = "LibSargasCpp";
    region = TMR_REGION_NONE;
    ret = TMR_paramGet(m_Readerp, TMR_PARAM_REGION_ID, &region);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "getting region failed !!!");
        return -1;
    }

    if (TMR_REGION_NONE == region) {
      TMR_RegionList regions;
      TMR_Region _regionStore[32];
      regions.list = _regionStore;
      regions.max = sizeof(_regionStore)/sizeof(_regionStore[0]);
      regions.len = 0;

      ret = TMR_paramGet(m_Readerp, TMR_PARAM_REGION_SUPPORTEDREGIONS, &regions);
      if(TMR_SUCCESS != ret) {
          LOGE(TAG, "getting supported regions failed !!!");
          return -1;
      }

      if (regions.len < 1) {
          LOGE(TAG, "Reader doesn't supportany regions");
          return -1;
          //checkerr(m_Readerp, TMR_ERROR_INVALID_REGION, __LINE__, "Reader doesn't supportany regions");
      }
      region = regions.list[0];
      ret = TMR_paramSet(m_Readerp, TMR_PARAM_REGION_ID, &region);
      if(TMR_SUCCESS != ret) {
          LOGE(TAG, "setting region failed !!!");
          return -1;
      }
    }

    /// display reader information
    {
        uint16_t productGroupID, productID;
        TMR_String str;
        char string[50];
        str.value = string;
        str.max = 50;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_VERSION_HARDWARE, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/hardware: %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/hardware not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting version hardware", TMR_strerr(m_Readerp, ret));
            }
        }
        /**
         * for failure case API is modifying the str.value to some constant string,
         * to fix that, restoring the str.value variable
         **/
        str.value = string;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_VERSION_SERIAL, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/serial: %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/serial not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting version serial", TMR_strerr(m_Readerp, ret));
            }
        }
        /**
       * for failure case API is modifying the str.value to some constant string,
       * to fix that, restoring the str.value variable
       **/
        str.value=string;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_VERSION_MODEL, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/model:  %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/model not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting version model", TMR_strerr(m_Readerp, ret));
            }
        }
        /**
         * for failure case API is modifying the str.value to some constant string,
         * to fix that, restoring the str.value variable
         **/
        str.value=string;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_VERSION_SOFTWARE, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/software: %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/software not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting version software", TMR_strerr(m_Readerp, ret));
            }
        }
        /**
         * for failure case API is modifying the str.value to some constant string,
         * to fix that, restoring the str.value variable
         **/
        str.value=string;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_URI, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/uri:  %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/uri:  Unsupported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting reader URI", TMR_strerr(m_Readerp, ret));
            }
        }
        /**
         * for failure case API is modifying the str.value to some constant string,
         * to fix that, restoring the str.value variable
         **/
        str.value=string;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_PRODUCT_ID, &productID);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/productID: %d", productID);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/productID not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting product id", TMR_strerr(m_Readerp, ret));
            }
        }

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_PRODUCT_GROUP_ID, &productGroupID);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/productGroupID: %d", productGroupID);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/productGroupID not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting product group id", TMR_strerr(m_Readerp, ret));
            }
        }

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_PRODUCT_GROUP, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/version/productGroup: %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/version/productGroup not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting product group", TMR_strerr(m_Readerp, ret));
            }
        }
        /**
         * for failure case API is modifying the str.value to some constant string,
         * to fix that, restoring the str.value variable
         **/
        str.value=string;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_READER_DESCRIPTION, &str);
        if (TMR_SUCCESS == ret) {
            LOG("/reader/description:  %s", str.value);
        }
        else {
            if (TMR_ERROR_NOT_FOUND == ret) {
                LOGE(TAG, "/reader/description not supported");
            }
            else {
                LOGE(TAG, "Error %s: %s", "getting reader description", TMR_strerr(m_Readerp, ret));
            }
        }
    }

    return 0;
}

int LibSargasCpp::init(char url[], char ant[]) {
    if(m_IsReaderCreate) {
        LOGE("ERROR", "Can't connect before destory the pre reader");
        return -1;
    }
    memset(m_Url, '\0', 256);
    memset(m_AntList, '\0', 64);
    strcpy(m_Url, url);
    strcpy(m_AntList, ant);

    TMR_Status ret;

    parseAntennaList(m_Buffer, &m_AntennaCount, m_AntList);
    m_AntennaList = m_Buffer;

    ret = TMR_create(m_Readerp, m_Url);
    if(TMR_SUCCESS != ret) {
        LOGE("TMR_create", "TMR_create failed !!!");
        return -1;
    }

    ret = TMR_connect(m_Readerp);
    if(TMR_SUCCESS != ret) {
        LOGE("TMR_connect", "TMR_connect failed !!!");
        return -1;
    }

    if(0 != pthread_create(&m_pthreadT, NULL, encodingThread, this)) {
        LOGE("thread create", "pthread_create failed !!!");
        return -1;
    }

    m_IsReaderCreate = true;
    LOGD("init test", "init successed !!!");

    return 0;
}

void LibSargasCpp::deInit() {
    TMR_destroy(m_Readerp);
    m_IsReaderCreate = false;
    deInitParamters();
}

void LibSargasCpp::deInitParamters() {
    m_IsStop = true;
    m_IsBreak = true;
    m_IsStartThread = false;
    m_IsThreadCreateSuccess = false;
}

int LibSargasCpp::setParamters() {
    TMR_Status ret;
    m_Region = TMR_REGION_NONE;
    ret = TMR_paramGet(m_Readerp, TMR_PARAM_REGION_ID, &m_Region);
    if(TMR_SUCCESS != ret) {
        LOGE("TMR_paramGet", "getting region failed !!!");
        return -1;
    }

    if (TMR_REGION_NONE == m_Region) {
        TMR_RegionList regions;
        TMR_Region _regionStore[32];
        regions.list = _regionStore;
        regions.max = sizeof(_regionStore)/sizeof(_regionStore[0]);
        regions.len = 0;

        ret = TMR_paramGet(m_Readerp, TMR_PARAM_REGION_SUPPORTEDREGIONS, &regions);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramGet", "getting supported regions failed !!!");
            return -1;
        }
        if (regions.len < 1) {
            LOGE("regions.len < 1", "Reader doesn't supportany regions");
            //checkerr(rp, TMR_ERROR_INVALID_REGION, __LINE__, "Reader doesn't supportany regions");
        }

        m_Region = regions.list[0];
        ret = TMR_paramSet(m_Readerp, TMR_PARAM_REGION_ID, &m_Region);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramSet", "setting region failed !!!");
            return -1;
        }
    }

    m_Model.value = m_Modelstr;
    m_Model.max = 64;
    TMR_paramGet(m_Readerp, TMR_PARAM_VERSION_MODEL, &m_Model);
    if (((0 == strcmp("Sargas", m_Model.value)) || (0 == strcmp("M6e Micro", m_Model.value)) ||(0 == strcmp("M6e Nano", m_Model.value)))
            && (NULL == m_AntennaList)) {
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
    ret = TMR_RP_init_simple(&m_Plan, m_AntennaCount, m_AntennaList, TMR_TAG_PROTOCOL_GEN2, 1000);
    if(ret != TMR_SUCCESS) {
        LOGE("TMR_RP_init_simple", "initializing the  read plan failed !!!");
        return -1;
    }
    /* Commit read plan */
    ret = TMR_paramSet(m_Readerp, TMR_PARAM_READ_PLAN, &m_Plan);
    if(ret != TMR_SUCCESS) {
        LOGE("TMR_paramSet", "setting read plan failed !!!");
        return -1;
    }

    return 0;
}

int LibSargasCpp::readEPC(int intTime) {
    if(!m_IsReaderCreate) {
        LOGE("readTest", "should connect successed before read");
        return -1;
    }
    TMR_Status ret;

    if(0 != setParamters()) {
        return -1;
    }
    ret = TMR_read(m_Readerp, intTime, NULL);
    if(ret == TMR_ERROR_TAG_ID_BUFFER_FULL) {
        /* In case of TAG ID Buffer Full, extract the tags present
         * in buffer.
         */
        LOGE("TMR_read", "reading tags: %s", TMR_strerr(m_Readerp, ret));
    }
    else if(TMR_SUCCESS == ret) {
        LOGD("TMR_read", "TMR_read successed !!!");
    }
    else {
        LOGE("TMR_read", "TMR_read failed: %s !!!", TMR_strerr(m_Readerp, ret));
        return -1;
    }
    while (TMR_SUCCESS == TMR_hasMoreTags(m_Readerp)) {
        TMR_TagReadData trd;
        char epcStr[128];
        ret = TMR_getNextTag(m_Readerp, &trd);
        if(TMR_SUCCESS != ret) {
            LOGE("TMR_getNextTag", "fetching tag failed !!!");
            return -1;
        }
        TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);
        LOGD("Read_Tag", "EPC:%s ant:%d count:%d ", epcStr, trd.antenna, trd.readCount);
    }
    LOGD("read test", "read end ...");

    return 0;
}

int LibSargasCpp::readTID() {
    if(!m_IsReaderCreate) {
        LOGE("ERROR", "Can't read tid because it havn't connect");
        return -1;
    }
    TMR_Status ret;
    TMR_TagFilter filter;
    TMR_TagReadData trd;
    char epcString[128];
    const char TAG[] = "readTID";
    //Use first antenna for tag operation
    if (NULL != m_AntennaList) {
        ret = TMR_paramSet(m_Readerp, TMR_PARAM_TAGOP_ANTENNA, &m_AntennaList[0]);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramSet", "setting tagop antenna failed !!!");
            return -1;
        }
    }
    if(0 != setParamters()) {
        return -1;
    }

    ret = TMR_read(m_Readerp, 100, NULL);
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret) {
        /* In case of TAG ID Buffer Full, extract the tags present
         * in buffer.
         */
        LOGD(TAG, "reading tags: %s", TMR_strerr(m_Readerp, ret));
    }
    else {
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "reading tags failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
    }

    if (TMR_ERROR_NO_TAGS == TMR_hasMoreTags(m_Readerp)) {
        LOGE(TAG, "No tags found for test");
        return -1;
    }

    ret = TMR_getNextTag(m_Readerp, &trd);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "getting tags failed at %d in %s !!!", __LINE__, __FILE__);
        return -1;
    }

    TMR_TF_init_tag(&filter, &trd.tag);
    TMR_bytesToHex(filter.u.tagData.epc, filter.u.tagData.epcByteCount,
                   epcString);
    {
        TMR_TagOp tagop;
        uint8_t readLength = 0x00;

        if ((0 == strcmp("M6e", m_Model.value)) || (0 == strcmp("M6e PRC", m_Model.value))
                || (0 == strcmp("M6e Micro", m_Model.value)) || (0 == strcmp("Mercury6", m_Model.value))
                || (0 == strcmp("Astra-EX", m_Model.value)) || (0 == strcmp("M6e JIC", m_Model.value)) || (0 == strcmp("Sargas", m_Model.value)))
        {
            /**
             * Specifying the readLength = 0 will retutrn full Memory bank data for any
             * tag read in case of M6e  and its varients and M6 reader.
             **/
            readLength = 0;
        }
        else {
            /**
             * In other case readLen is minimum.i.e 2 words
             **/
            readLength = 2;
        }

        LOG("Perform embedded and standalone tag operation - read TID memory bank without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, TMR_GEN2_BANK_TID, 0, readLength);
        readAllMemBanks(&tagop, NULL, m_Readerp);
    }

    return 0;
}

int LibSargasCpp::inventory() {
    if(!m_IsReaderCreate) {
        LOGE("ERROR", "Can't inventory  because it havn't connect");
        return -1;
    }
    TMR_Status ret;
    TMR_TagFilter filter;
    TMR_TagReadData trd;
    char epcString[128];
    const char TAG[] = "inventory";
    //Use first antenna for tag operation
    if (NULL != m_AntennaList) {
        ret = TMR_paramSet(m_Readerp, TMR_PARAM_TAGOP_ANTENNA, &m_AntennaList[0]);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramSet", "setting tagop antenna failed !!!");
            return -1;
        }
    }

    if(0 != setParamters()) {
        return -1;
    }

    ret = TMR_read(m_Readerp, 100, NULL);
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret) {
        /* In case of TAG ID Buffer Full, extract the tags present
         * in buffer.
         */
        LOGD(TAG, "reading tags: %s", TMR_strerr(m_Readerp, ret));
    }
    else {
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "reading tags failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
    }

    if (TMR_ERROR_NO_TAGS == TMR_hasMoreTags(m_Readerp)) {
        LOGE(TAG, "No tags found for test");
        return -1;
    }

    ret = TMR_getNextTag(m_Readerp, &trd);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "getting tags failed at %d in %s !!!", __LINE__, __FILE__);
        return -1;
    }

    TMR_TF_init_tag(&filter, &trd.tag);
    TMR_bytesToHex(filter.u.tagData.epc, filter.u.tagData.epcByteCount,
                   epcString);
    {
        TMR_TagOp tagop;
        TMR_uint16List writeData;
        TMR_TagData epc;
        uint8_t readLength = 0x00;
        uint16_t data[]   = {0x1234, 0x5678};
        uint16_t data1[]  = {0xFFF1, 0x1122};
        uint8_t epcData[] = {
            0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,
            0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67,
        };

        if ((0 == strcmp("M6e", m_Model.value)) || (0 == strcmp("M6e PRC", m_Model.value))
                || (0 == strcmp("M6e Micro", m_Model.value)) || (0 == strcmp("Mercury6", m_Model.value))
                || (0 == strcmp("Astra-EX", m_Model.value)) || (0 == strcmp("M6e JIC", m_Model.value)) || (0 == strcmp("Sargas", m_Model.value)))
        {
            /**
             * Specifying the readLength = 0 will retutrn full Memory bank data for any
             * tag read in case of M6e  and its varients and M6 reader.
             **/
            readLength = 0;
        }
        else {
            /**
             * In other case readLen is minimum.i.e 2 words
             **/
            readLength = 2;
        }

        /* write Data on EPC bank */
        LOG("start write epc ...");
        epc.epcByteCount = sizeof(epcData) / sizeof(epcData[0]);
        memcpy(epc.epc, epcData, epc.epcByteCount * sizeof(uint8_t));
        ret = TMR_TagOp_init_GEN2_WriteTag(&tagop, &epc);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing GEN2_WriteTag failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing the write tag operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        LOG("Writing on EPC bank success !!!");

        /* Write Data on reserved bank */
        writeData.list = data;
        writeData.max = writeData.len = sizeof(data) / sizeof(data[0]);
        ret = TMR_TagOp_init_GEN2_BlockWrite(&tagop, TMR_GEN2_BANK_RESERVED, 0, &writeData);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "Initializing the write operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing the write operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        LOG("Writing on RESERVED bank success !!!");

        /* Write data on user bank */
        writeData.list = data1;
        writeData.max = writeData.len = sizeof(data1) / sizeof(data1[0]);
        ret = TMR_TagOp_init_GEN2_BlockWrite(&tagop, TMR_GEN2_BANK_USER, 0, &writeData);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "Initializing the write operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing the write operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        LOG("Writing on USER bank success");

        LOG("Perform embedded and standalone tag operation - read only user memory without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_BANK_USER), 0, readLength);
        readAllMemBanks(&tagop, NULL, m_Readerp);

        LOG("Perform embedded and standalone tag operation - read all memory bank without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_Bank)(TMR_GEN2_BANK_USER | TMR_GEN2_BANK_EPC_ENABLED | TMR_GEN2_BANK_RESERVED_ENABLED |TMR_GEN2_BANK_TID_ENABLED |TMR_GEN2_BANK_USER_ENABLED), 0, readLength);
        readAllMemBanks(&tagop, NULL, m_Readerp);

        LOG("Perform embedded and standalone tag operation - read only user memory with filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_BANK_USER), 0, readLength);
        readAllMemBanks(&tagop, &filter, m_Readerp);

        LOG("Perform embedded and standalone tag operation - read user memory, reserved memory with filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_Bank)(TMR_GEN2_BANK_USER | TMR_GEN2_BANK_RESERVED_ENABLED |TMR_GEN2_BANK_USER_ENABLED), 0, readLength);
        readAllMemBanks(&tagop, &filter, m_Readerp);

        LOG("Perform embedded and standalone tag operation - read user memory, reserved memory and tid memory with filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_Bank)(TMR_GEN2_BANK_USER | TMR_GEN2_BANK_RESERVED_ENABLED |TMR_GEN2_BANK_USER_ENABLED |TMR_GEN2_BANK_TID_ENABLED ), 0, readLength);
        readAllMemBanks(&tagop, &filter, m_Readerp);

        LOG("Perform embedded and standalone tag operation - read user memory, reserved memory, tid memory and epc memory with filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_Bank)(TMR_GEN2_BANK_USER | TMR_GEN2_BANK_RESERVED_ENABLED |TMR_GEN2_BANK_USER_ENABLED |TMR_GEN2_BANK_TID_ENABLED | TMR_GEN2_BANK_EPC_ENABLED), 0, readLength);
        readAllMemBanks(&tagop, &filter, m_Readerp);
    }

    return 0;
}

int LibSargasCpp::writeEPCseveralTimes() {
    if(!m_IsReaderCreate) {
        LOGE("error", "Please init before inventory !!!");
        return -1;
    }
    TMR_Status ret;
    TMR_TagFilter filter;
    TMR_TagReadData trd;
    char epcString[128];
    const char TAG[] = "writeEPCseveralTimes";

    if(0 != setParamters()) {
        return -1;
    }

    //Use first antenna for tag operation
    if (NULL != m_AntennaList) {
        ret = TMR_paramSet(m_Readerp, TMR_PARAM_TAGOP_ANTENNA, &m_AntennaList[0]);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramSet", "setting tagop antenna failed !!!");
            return -1;
        }
    }

    ret = TMR_read(m_Readerp, 100, NULL);
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret) {
        /* In case of TAG ID Buffer Full, extract the tags present
         * in buffer.
         */
        LOGD(TAG, "reading tags: %s", TMR_strerr(m_Readerp, ret));
    }
    else {
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "reading tags failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
    }

    if (TMR_ERROR_NO_TAGS == TMR_hasMoreTags(m_Readerp)) {
        LOGE(TAG, "No tags found for test");
        return -1;
    }

    ret = TMR_getNextTag(m_Readerp, &trd);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "getting tags failed at %d in %s !!!", __LINE__, __FILE__);
        return -1;
    }

    TMR_TF_init_tag(&filter, &trd.tag);
    TMR_bytesToHex(filter.u.tagData.epc, filter.u.tagData.epcByteCount,
                   epcString);
    {
        TMR_TagOp tagop;
        TMR_TagData epc;
        TMR_TagData epc1;
        TMR_TagData epc2;
        uint8_t readLength = 0x00;
        uint8_t epcData[] = {
            0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,
            0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67,
        };
        uint8_t epcData1[] = {
            0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
            0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
        };
        uint8_t epcData2[] = {
            0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
            0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
        };

        if ((0 == strcmp("M6e", m_Model.value)) || (0 == strcmp("M6e PRC", m_Model.value))
                || (0 == strcmp("M6e Micro", m_Model.value)) || (0 == strcmp("Mercury6", m_Model.value))
                || (0 == strcmp("Astra-EX", m_Model.value)) || (0 == strcmp("M6e JIC", m_Model.value)) || (0 == strcmp("Sargas", m_Model.value)))
        {
            /**
             * Specifying the readLength = 0 will retutrn full Memory bank data for any
             * tag read in case of M6e  and its varients and M6 reader.
             **/
            readLength = 0;
        }
        else {
            /**
             * In other case readLen is minimum.i.e 2 words
             **/
            readLength = 2;
        }

        /* write Data on EPC bank */
        LOG("start write epc1 ...");
        epc.epcByteCount = sizeof(epcData) / sizeof(epcData[0]);
        memcpy(epc.epc, epcData, epc.epcByteCount * sizeof(uint8_t));
        ret = TMR_TagOp_init_GEN2_WriteTag(&tagop, &epc);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing GEN2_WriteTag failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing the write tag operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        LOG("Writing on EPC1 bank success !!!");

        LOG("read only epc1 memory without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_BANK_EPC), 0, readLength);
        readAllMemBanks(&tagop, NULL, m_Readerp);

        LOG("start write epc2 ...");
        epc1.epcByteCount = sizeof(epcData1) / sizeof(epcData1[0]);
        memcpy(epc1.epc, epcData1, epc1.epcByteCount * sizeof(uint8_t));
        ret = TMR_TagOp_init_GEN2_WriteTag(&tagop, &epc1);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing GEN2_WriteTag failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing the write tag operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        LOG("Writing on EPC2 bank success !!!");

        LOG("read only epc memory without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_BANK_EPC), 0, readLength);
        readAllMemBanks(&tagop, NULL, m_Readerp);

        LOG("start write epc3 ...");
        epc2.epcByteCount = sizeof(epcData2) / sizeof(epcData2[0]);
        memcpy(epc2.epc, epcData2, epc2.epcByteCount * sizeof(uint8_t));
        ret = TMR_TagOp_init_GEN2_WriteTag(&tagop, &epc2);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing GEN2_WriteTag failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing the write tag operation failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
        LOG("Writing on EPC3 bank success !!!");

        LOG("read only epc3 memory without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&tagop, (TMR_GEN2_BANK_EPC), 0, readLength);
        readAllMemBanks(&tagop, NULL, m_Readerp);

    }

    return 0;
}

int LibSargasCpp::enCoding(TMR_TagData tid, TMR_TagData newepc) {
    if(!m_IsReaderCreate) {
        LOGE("ERROR", "Please init before inventory !!!");
        return -1;
    }
    TMR_Status ret;
    TMR_TagFilter filter;
    TMR_TagReadData trd;
    char epcString[128];
    const char TAG[] = "enCoding";

    if(0 != setParamters()) {
        return -1;
    }

    //Use first antenna for tag operation
    if (NULL != m_AntennaList) {
        ret = TMR_paramSet(m_Readerp, TMR_PARAM_TAGOP_ANTENNA, &m_AntennaList[0]);
        if(ret != TMR_SUCCESS) {
            LOGE("TMR_paramSet", "setting tagop antenna failed !!!");
            return -1;
        }
    }

    ret = TMR_read(m_Readerp, 100, NULL);
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret) {
        /* In case of TAG ID Buffer Full, extract the tags present
         * in buffer.
         */
        LOGD(TAG, "reading tags: %s", TMR_strerr(m_Readerp, ret));
    }
    else {
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "reading tags failed at %d in %s !!!", __LINE__, __FILE__);
            return -1;
        }
    }

    if (TMR_ERROR_NO_TAGS == TMR_hasMoreTags(m_Readerp)) {
        LOGE(TAG, "No tags found for test");
        return -1;
    }

    ret = TMR_getNextTag(m_Readerp, &trd);
    if(TMR_SUCCESS != ret) {
        LOGE(TAG, "getting tags failed at %d in %s !!!", __LINE__, __FILE__);
        return -1;
    }

    TMR_TF_init_tag(&filter, &trd.tag);
    TMR_bytesToHex(filter.u.tagData.epc, filter.u.tagData.epcByteCount,
                   epcString);
    {
        TMR_TagOp tagop;
        TMR_TagOp newtagop;
        uint8_t readLength = 0x00;

        if ((0 == strcmp("M6e", m_Model.value)) || (0 == strcmp("M6e PRC", m_Model.value))
                || (0 == strcmp("M6e Micro", m_Model.value)) || (0 == strcmp("Mercury6", m_Model.value))
                || (0 == strcmp("Astra-EX", m_Model.value)) || (0 == strcmp("M6e JIC", m_Model.value)) || (0 == strcmp("Sargas", m_Model.value)))
        {
            /**
             * Specifying the readLength = 0 will retutrn full Memory bank data for any
             * tag read in case of M6e  and its varients and M6 reader.
             **/
            readLength = 0;
        }
        else {
            /**
             * In other case readLen is minimum.i.e 2 words
             **/
            readLength = 2;
        }

        /* write Data on EPC bank */
        LOG("start write epc ...");
        // 1. set tid
        ret = TMR_TagOp_init_GEN2_WriteTag(&tagop, &tid);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing GEN2_WriteTag");
            return -1;
        }
        ret = TMR_executeTagOp(m_Readerp, &tagop, NULL, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing GEN2_WriteTag failed !!!");
            return -1;
        }

        // 2. write newepc by tid filter
        /* Initialize the new tagop to write the new epc*/
        ret = TMR_TagOp_init_GEN2_WriteTag(&newtagop, &newepc);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing GEN2_WriteTag failed !!!");
            return -1;
        }

        // 3. set tid filter
        /* Initialize the filter with the tid of the tag which is set earlier*/
        ret = TMR_TF_init_tag(&filter, &tid);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "initializing TMR_TagFilter failed !!!");
            return -1;
        }

        /* Execute the tag operation Gen2 writeTag with select filter applied*/
        ret = TMR_executeTagOp(m_Readerp, &newtagop, &filter, NULL);
        if(TMR_SUCCESS != ret) {
            LOGE(TAG, "executing GEN2_WriteTag failed !!!");
            return -1;
        }
        LOG("Writing on epc(new epc) bank success !!!");

        LOG("read only epc memory without filter");
        ret = TMR_TagOp_init_GEN2_ReadData(&newtagop, TMR_GEN2_BANK_EPC, 0, readLength);
        readAllMemBanks(&newtagop, NULL, m_Readerp);

    }

    return 0;
}

int LibSargasCpp::enCoding(std::string tid, std::string newepc) {
    TMR_TagData strTid;
    TMR_TagData strEpc;
    // transfer tid data
    uint8_t tidData[tid.size()/2];
    uint32_t  iValue = 0;
    string  ad = "";
    char *str;
    for (int i = 0, j = 0; i < (int)tid.length(); i += 2, j++) {
        ad = tid.substr(i, 2);
        iValue = (int)strtol(ad.c_str(), &str, 16);     //Hexadecimal
        tidData[j] = iValue;
    }
    strTid.epcByteCount = sizeof(tidData) / sizeof(tidData[0]);
    memcpy(strTid.epc, tidData, strTid.epcByteCount * sizeof(uint8_t));
    //transfer epc data
    uint8_t epcData[newepc.size()/2];
    uint32_t  iValue1 = 0;
    string  ad1 = "";
    char *str1;
    for (int i = 0, j = 0; i < (int)newepc.length(); i += 2, j++) {
        ad1 = newepc.substr(i, 2);
        iValue1 = (int)strtol(ad1.c_str(), &str1, 16);     //Hexadecimal
        epcData[j] = iValue1;
    }
    strEpc.epcByteCount = sizeof(epcData) / sizeof(epcData[0]);
    memcpy(strEpc.epc, epcData, strEpc.epcByteCount * sizeof(uint8_t));

    return enCoding(strTid, strEpc);
}
