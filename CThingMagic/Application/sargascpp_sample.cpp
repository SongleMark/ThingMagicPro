#include "../libSargas/libsargascpp.h"
#include <assert.h>
#include <stdio.h>
#include "../libSargas/libitkcpp.h"
//#include "../libSargas/libmercurycpp.h"

#define         TAG         "Main"

#define         MENU    \
                "\t\t\t****************************************************\n\n" \
                "\t\t\t*            ThingMagic Sargas's sample            *\n\n" \
                "\t\t\t****************************************************\n\n" \
                "\t\t\t*              1. Connect.                         *\n\n" \
                "\t\t\t*              2. DisConnect.                      *\n\n" \
                "\t\t\t*              3. Read epc by fixed time           *\n\n" \
                "\t\t\t*              4. Inventory.                       *\n\n" \
                "\t\t\t*              5. Read TID memory bank.            *\n\n" \
                "\t\t\t*              6. Write Serval times to epc.       *\n\n" \
                "\t\t\t*              7. Write EPC by select tid filter.  *\n\n" \
                "\t\t\t*              8. Start encoding thread.           *\n\n" \
                "\t\t\t*              9. Start thread to encode once.     *\n\n" \
                "\t\t\t*             10. get reader's information.        *\n\n" \
                "\t\t\t*              0. exit.                            *\n\n" \
                "\t\t\t****************************************************\n"

LibSargasCpp    *gLibSargascpp;
Libitkcpp       *gLibItkcpp;
#if 1
int main(int argc, char *argv[]) {
    BOOL isStop = FALSE;
    int  sel = 0;
    gLibSargascpp = new LibSargasCpp;
    gLibItkcpp    = new Libitkcpp;
    gLibSargascpp->addListener(gLibItkcpp);

    uint8_t newepcData[] = {
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
    };
    uint8_t tidData[] = {
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
        0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
    };
    TMR_TagData newepc;
    TMR_TagData tid;
    newepc.epcByteCount = sizeof(newepcData) / sizeof(newepcData[0]);
    memcpy(newepc.epc, newepcData, newepc.epcByteCount * sizeof(uint8_t));
    tid.epcByteCount = sizeof(tidData) / sizeof(tidData[0]);
    memcpy(tid.epc, tidData, tid.epcByteCount * sizeof(uint8_t));

    while(!isStop) {
        gLibSargascpp->cleanScreen();
        printf("%s", MENU);
        sel = gLibSargascpp->getStdin();
        switch(sel) {
        case LibSargasCpp::EXIT_OPT:
            isStop = true;
            break;
        case LibSargasCpp::CONNECT_OPT:
            gLibSargascpp->init((char *)"tmr://localhost", (char *)"2");
            break;
        case LibSargasCpp::DISCONNECT_OPT:
            gLibSargascpp->deInit();
            break;
        case LibSargasCpp::READTAG_OPT:
            gLibSargascpp->readEPC(100);
            break;
        case LibSargasCpp::READALLMEM_OPT:
            gLibSargascpp->inventory();
            break;
        case LibSargasCpp::READTID_OPT:
            gLibSargascpp->readTID();
            break;
        case LibSargasCpp::WRITESERVAL_OPT:
            gLibSargascpp->writeEPCseveralTimes();
            break;
        case LibSargasCpp::WRITENEWEPC_OPT:
            gLibSargascpp->enCoding(tid, newepc);
            break;
        case LibSargasCpp::STARTTHREAD_OPT:
            gLibSargascpp->startEncodingThread();
            break;
        case LibSargasCpp::ENCODEONCE_OPT:
            gLibSargascpp->setStartThreadFlag();
            break;
        case LibSargasCpp::READINFO_OPT:
            gLibSargascpp->getReaderInfo();
        default:
            break;
        }
    }
    if(gLibSargascpp->isConnectedToReader()) {
        gLibSargascpp->deInit();
        LOGD("main", "sargas deinit successed");
    }
    else {
        gLibSargascpp->deInitParamters();
        LOGD("main", "sargas deInitParamters successed");
    }
    gLibSargascpp->cleanScreen();
    if(gLibSargascpp) {
        delete gLibSargascpp;
        gLibSargascpp = NULL;
    }
    if(gLibItkcpp) {
        delete gLibItkcpp;
        gLibItkcpp = NULL;
    }

    return 0;
}
#endif

#if 0
int main(int argc, char *argv[]) {
    BOOL isStop = FALSE;
    int  sel = 0;
    gLibMercurycpp =new Libmercurycpp;
    char *antennaL[32];
    strcpy(antennaL[0], "1");

    while(!isStop) {
        gLibMercurycpp->cleanScreen();
        printf("%s", MENU);
        sel = gLibMercurycpp->getStdin();
        switch(sel) {
        case Libmercurycpp::EXIT_OPT:
            isStop = true;
            break;
        case Libmercurycpp::CONNECT_OPT:
            gLibMercurycpp->connect("tmr://localhost", antennaL, 1);
            break;
        case Libmercurycpp::DISCONNECT_OPT:
            gLibMercurycpp->disConnect();
            break;
        case Libmercurycpp::READTAG_OPT:
            gLibMercurycpp->readEPC();
            break;
        case Libmercurycpp::READALLMEM_OPT:

            break;
        default:
            break;
        }
    }
    gLibMercurycpp->cleanScreen();

    return 0;
}
#endif
