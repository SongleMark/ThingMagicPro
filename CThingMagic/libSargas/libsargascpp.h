#ifndef LIBSARGASCPP_H
#define LIBSARGASCPP_H

//#include <tm_reader.h>
#include "tm_reader.h"
#include <iostream>
#include <vector>
#include <pthread.h>
#include "pubdata.h"

#ifndef         BOOL
typedef         int    BOOL;
#endif
#define         TRUE   1
#define         FALSE  0
#define         LOGD   LibSargasCpp::printDebug
#define         LOGE   LibSargasCpp::printError
#define         LOG    LibSargasCpp::printT

/// @name: SargasEventListener
/// @brief: this class will used libitkcpp's function
class SargasEventListener
{
public:
    virtual void onSendEvent(std::string method, std::string message) = 0;
};

/// @name: LibSargasCpp
/// @brief: function of thingmagic reader
class LibSargasCpp
{
public:
    /// @brief : constructor of this class just initialize some paramters
    explicit LibSargasCpp();

    /// @brief : distructor of this class just deinitialize and disconnect of reader
    virtual ~LibSargasCpp();

    /// @brief : the enumeration of user's select.
    enum            OPTIONS {
                    EXIT_OPT       = 0,    CONNECT_OPT    = 1,
                    DISCONNECT_OPT = 2,    READTAG_OPT    = 3,
                    READALLMEM_OPT = 4,    READTID_OPT    = 5,
                    WRITESERVAL_OPT = 6,   WRITENEWEPC_OPT = 7,
                    STARTTHREAD_OPT = 8,   ENCODEONCE_OPT  = 9,
                    READINFO_OPT   = 10,
    };

protected:
    /// some utility function may be just for this class
    void            errx(int exitval, const char *fmt, ...);
    void            checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg);
    void            serialPrinter(bool tx, uint32_t dataLen, const uint8_t data[],
                       uint32_t timeout, void *cookie);
    void            stringPrinter(bool tx,uint32_t dataLen, const uint8_t data[],uint32_t timeout, void *cookie);
    void            parseAntennaList(uint8_t *antenna, uint8_t *antennaCount, char *args);
    void            readAllMemBanks(TMR_TagOp *op, TMR_TagFilter *filter, TMR_Reader *rp);

public:
    /// @brief : print debug log green line
    /// @param : tag    ==> the target just may be function name and class name
    /// @param : format ==> the format of log just like system function "printf()"
    static void     printDebug(const char *tag, const char *format, ...);

    /// @brief : print error log red line
    /// @param : tag    ==> the target just may be function name and class name
    /// @param : format ==> the format of log just like system function "printf()"
    static void     printError(const char *tag, const char *format, ...);

    /// @brief : print debug log green line and don't have target
    /// @param : format ==> the format of log just like system function "printf()"
    static void     printT(const char *format, ...);

    /// @brief : fgets stdin and return the integer value
    /// @retval: the integer value of user's input
    static int      getStdin();

    /// @brief : clean screen
    static void     cleanScreen();

    /// @brief : print a notice in screen and can keep screen wait before get char
    /// @param : str ==> the notice string
    static void     noticeScreen(const char *str);

    /// @brief : return the connect reader's status flag
    /// @retval: connect status flag
    bool            isConnectedToReader();

    /// @brief : Initialize the reader and connect to it
    /// @param : url ==> the connected url
    /// @param : ant ==> the list of antennas
    int             init(char url[], char ant[]);

    /// @brief : destory the tmr_reader's header and deinit some paramters
    void            deInit();

    /// @brief : deinit some paramters
    void            deInitParamters();

    /// @brief : tmr's paramters set and get
    /// @retval: 0 -- successed
    /// @retval: -1 -- failed
    int             setParamters();

    /// @brief : read epcstring by fixed time
    /// @param : ==> intTime is the read time
    /// @retval: 0 --successed
    /// @retval: -1 --failed
    int             readEPC(int intTime);

    /// @brief : print the result about read tid memory bank
    /// @retval:  0 --- successed
    /// @retval: -1 --- failed
    int             readTID();

    /// @brief : write all memory banks and read it.
    /// @retval: 0  -- successed
    /// @retval: -1 -- failed
    int             inventory();

    /// @brief : write epc data by several times
    /// @retval:  0 --- successed
    /// @retval: -1 --- failed
    int             writeEPCseveralTimes();

    /// @brief : write new epc data to tag by select tid and will read epc after write
    /// @param : tid    ==> the filter is tid value
    /// @param : newepc ==> the write tag value is newepc
    /// @retval:  0 --- successed
    /// @retval: -1 --- failed
    int             enCoding(TMR_TagData tid, TMR_TagData newepc);

    /// @brief : write new epc data to tag by select tid and will read epc after write
    /// @param : tid    ==> the filter is tid value
    /// @param : newepc ==> the write tag value is newepc
    /// @retval:  0 --- successed
    /// @retval: -1 --- failed
    int             enCoding(std::string tid, std::string newepc);

    /// @brief: the thread of enCoding
    /// @param: thread pointer
    /// @retval: void
    static void    *encodingThread(void *thread);

    /// @brief: set thread start flag by true
    /// @retval: void
    void            startEncodingThread();

    /// @brief: set thread stop flag by true
    /// @retval: void
    void            stopEncodingThread();

    /// @brief : test for  start thread
    void            setStartThreadFlag();

    /// @brief: publish message to upper machine
    /// @param: method  ==> message method
    /// @param: message ==> the send message
    void            publishMessage(std::string method, std::string message);

    /// @brief: used SargasEventListener pointer point to the libitkcpp class
    /// @param: listener ==> the pointer of the son class(libitkcpp class's pointer)
    void            addListener(SargasEventListener *listener);

    /// @brief: display reader information
    /// @retval: 0 success, -1 failed
    int             getReaderInfo();

private:
    TMR_Reader             *m_Readerp;              /* the header pointer of reader */
    TMR_Reader              m_Reader;               /* used to initialize the pointer of header*/
    uint8_t                *m_AntennaList;          /* antenna list */
    uint8_t                 m_AntennaCount;         /* count of antenna */
    bool                    m_IsReaderCreate;       /* is ThingMagic connect successed? */
    char                    m_Url[256];             /* connect to readers's url */
    char                    m_AntList[64];          /* the input of antenna */
    bool                    m_IsStop;               /* encoding thread's stop flag */
    bool                    m_IsBreak;              /* encoding thread's break flag */
    bool                    m_IsStartThread;        /* is start the encoding thread */
    bool                    m_IsThreadCreateSuccess;/* the encoding thread's create status*/
    vector<PubDataItem>		m_PubDataVct;           /* encoding data list */
    pthread_t               m_pthreadT;             /* pthread_t of encoding thread */
    uint8_t                 m_Buffer[20];           /* buffer */
    TMR_Region              m_Region;               /* RFID regulatory regions */
    TMR_String              m_Model;                /* Internal string representation with known storage size */
    char                    m_Modelstr[64];         /* used to m_Model */
    TMR_ReadPlan            m_Plan;                 /* A ReadPlan structure specifies the antennas, protocols, and filters to use for a search (read)  */
    SargasEventListener    *m_ItkListener;          /* the class can used libitkcpp's function */
    int                     m_Index;                /* the PubDataItem vector's index */
    std::string             m_Tid;                  /* tid memory bank string and used to select filter */
    std::string             m_Epc;                  /* epc memory bank string and used to encode */

};

#endif // LIBSARGASCPP_H
