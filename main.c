#include <stdio.h>
#include "wav.h"

int main( int argc, char** argv )
{
    //get data
    struct WAV_Data aud;
        if (argc == 2) {
            if (WAV_Load(&aud, argv[1]) != 0) {
                printf("Error: Wav_Load() Failed\n");
                return -1;
            }
        }
        else {
            printf("Hey, I need an input\n");
            return -1;
        }

    WAV_PrintHeader(&aud);
    WAV_PrintChannels(&aud);

    //free data
    WAV_Destroy(&aud);
}
