#include "sargascpp_sample.h"
#include <assert.h>
#include <stdio.h>

#define     	TAG         "Main"
TMR_Reader      gReader;
TMR_Reader      *gReader_P;

int main(int argc, char *argv[]) {
    BOOL isStop = FALSE;
    int  sel = 0;
    gReader_P = &gReader;

    while(!isStop) {
        cleanScreen();
        printf("%s", MENU);
        sel = getStdin();
        switch(sel) {
        case EXIT_OPT:
            isStop = true;
            break;
        case CONNECT_OPT:
            if(0 == init(argc, argv, gReader_P, CONNECT_OPT)) {
                noticeScreen("\n");
            }
            break;
        case DISCONNECT_OPT:
            DeInit(gReader_P);
            break;
        case READTAG_OPT:
            readtag(argc, argv, gReader_P);
            noticeScreen("read end ");
            break;
        default:
            break;
        }
    }
    cleanScreen();

    return 0;
}
