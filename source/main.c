// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

#define URL "https://fast.com"

void showError(char* errorText1, char* errorText2, Result outrc)
{
    AppletHolder err;
    AppletStorage errStor;
    LibAppletArgs errArgs;

    appletCreateLibraryApplet(&err, AppletId_error, LibAppletMode_AllForeground);
    libappletArgsCreate(&errArgs, 1);
    libappletArgsPush(&errArgs, &err);
    appletCreateStorage(&errStor, 4120);
    u8 args[4120] = {0};
    args[0] = 1;

    u64 e = (((outrc & 0x1ffu) + 2000) | (((outrc >> 9) & 0x1fff & 0x1fffll) << 32));
    *(u64*)&args[8] = e;
    strcpy((char*) &args[24], errorText1);
    strcpy((char*) &args[2072], errorText2);
    appletStorageWrite(&errStor, 0, args, 4120);
    appletHolderPushInData(&err, &errStor);

    appletHolderStart(&err);
    appletHolderJoin(&err);
    exit(1);
}

int main(int argc, char* argv[])
{
    Result rc=0;
    char url[0xc00] = {0};
    strcpy(url, URL);
    int at = appletGetAppletType();
    if(at == AppletType_Application) { // Running as a title
        WebCommonConfig conf;
        WebCommonReply out;
        rc = webPageCreate(&conf, url);
        if (R_FAILED(rc))
            showError("Error starting Browser\nLookup error code for more info", "", rc);
        webConfigSetJsExtension(&conf, true);
        webConfigSetPageCache(&conf, true);
        webConfigSetBootLoadingIcon(&conf, true);
        webConfigSetWhitelist(&conf, ".*");
        rc = webConfigShow(&conf, &out);
        if (R_FAILED(rc))
            showError("Error starting Browser\nLookup error code for more info", "", rc);
    } else { // Running under applet
        showError("Running in applet mode\nPlease launch hbmenu by holding R on an APP (e.g. a game) NOT an applet (e.g. Gallery)", "", 0);
    }
    return 0;
}
