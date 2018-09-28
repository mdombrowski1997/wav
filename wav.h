#include<stdio.h>
#include<stdlib.h>

const int BUF_SIZE = 0x120;

struct WAV_Data {
    int fs;                         //file size
    int fmts;                       //size of fmt chunk
    int chan;                       //number of channels
    int samp;                       //sample rate (8k-44.1k)
    int bps;                        //bits per sample
    int ds;                         //data size
    unsigned char* data;            //actual data
};

//Basic Tools
int WAV_Load( struct WAV_Data* wav, const char* filename );
void WAV_PrintHeader( const struct WAV_Data* wav );
void WAV_PrintChannels( const struct WAV_Data* wav );
void WAV_DFT( const struct WAV_Data* wav );
void WAV_Destroy( struct WAV_Data* wav );

//Basic Tools
int WAV_Load( struct WAV_Data* wav, const char* filename ) {
    unsigned char fb[BUF_SIZE];     //buffer to hold file contents
    int m;                          //mark for relative offset
    int i;                          //iterator
    int err;        //for catching error codes when I need to reuse them

    //read file into buffer
    FILE* fin = fopen( filename, "rb" );    //open file readonly/binary
    if (fin == NULL)
    {
        printf( "Error: Failed to open %s\n", filename );
        return -1;
    }
    err = fread( fb, 1, BUF_SIZE, fin );
    //check for valid read
    if (err < BUF_SIZE)
    {
        printf( "Error: only %i of %i bytes read - ", err, BUF_SIZE );
        if (feof( fin ))
        {
            printf( "EOF reached unexpectedly, code(%i)", feof( fin ) );
            perror( "" );
            printf( "\n" );
            return -4;
        }
        if (ferror( fin ))
        {
            printf( "ERROR flag set, code(%i)", ferror( fin ) );
            perror( "" );
            printf( "\n" );
            return -5;
        }
        else
        {
            printf( "Unknown fread error\n" );
            perror( "" );
            return -6;
        }
    }

    //parse header
    //'RIFF' verification
    if (fb[0] != 'R' || fb[1] != 'I'|| fb[2] != 'F' || fb[3] != 'F')
    {
        printf( "Error: RIFF chunk incorrect\n" );
        return -2;
    }
    //file size
    wav->fs = fb[4] + (fb[5]<<8) + (fb[6]<<16) + (fb[7]<<24);
    m = 8;
    //'WAVE' verification
    for (i = m; i < BUF_SIZE; ++i)
    {
        if (fb[i+0] == 'W' && fb[i+1] == 'A' && fb[i+2] == 'V' && fb[i+3] == 'E')
        {
            m = i+4;
            break;
        }
        if (i > (BUF_SIZE - 4))
        {
            printf( "Error: WAVE chunk not found\n" );
            return -3;
        }
    }
    //'fmt<space>' verification
    for (i = m; i < BUF_SIZE; ++i)
    {
        if (fb[i+0] == 'f' && fb[i+1] == 'm' && fb[i+2] == 't' && fb[i+3] == ' ')
        {
            m = i+4;
            break;
        }
        if (i > (BUF_SIZE - 4))
        {
            printf( "Error: fmt chunk not found\n" );
            return -3;
        }
    }
    //fmt size
    wav->fmts = fb[m] + (fb[m+1]<<8) + (fb[m+2]<<16) + (fb[m+3]<<24);
    m += 4;
    //ignoring Audio Format (PCM) chunk
    m += 2;
    //no. of channels
    wav->chan = fb[m] + (fb[m+1]<<8);
    m += 2;
    //sample rate (Hz)
    wav->samp = fb[m] + (fb[m+1]<<8) + (fb[m+2]<<16) + (fb[m+3]<<24);
    m += 4;
    //ignoring Byte Rate and Block Align chunks (4 + 2)
    m += 6;
    //bits / sample
    wav->bps = fb[m] + (fb[m+1]<<8);
    m += 2;
    //'data' verification
    for (i = m; i < BUF_SIZE; ++i)
    {
        if (fb[i+0] == 'd' && fb[i+1] == 'a' && fb[i+2] == 't' && fb[i+3] == 'a')
        {
            m = i+4;
            break;
        }
        if (i > (BUF_SIZE - 4))
        {
            printf( "Error: data chunk not found\n" );
            return -3;
        }
    }
    //data size
    wav->ds = fb[m] + (fb[m+1]<<8) + (fb[m+2]<<16) + (fb[m+3]<<24);
    m += 4;

    //allocate data space
    wav->data = (unsigned char*)malloc( wav->ds );
    //fill data
    fseek( fin, m, SEEK_SET );
    err = fread(wav->data, sizeof(unsigned char), wav->ds, fin);
    if (err < wav->ds)
    {
        printf( "Error: only %i of %i bytes read - ", err, wav->ds );
        //in the event of error, make sure struct is still semi-usable
        wav->ds = err;
        if (feof( fin ))
        {
            printf( "EOF reached unexpectedly, code(%i)\n", feof( fin ) );
            return -4;
        }
        if (ferror( fin ))
        {
            printf( "ERROR flag set, code(%i)", ferror( fin ) );
            perror( "" );
            printf( "\n" );
            return -5;
        }
        else
        {
            printf( "Unknown fread error\n" );
            return -6;
        }
    }
    //close file
    fclose( fin );

    return 0;
}
void WAV_PrintHeader( const struct WAV_Data* wav ) {
    //print relevant header data
    printf( "file size:\t\t0x%X\n", wav->fs );
    printf( "fmt chunk size:\t\t0x%X\n", wav->fmts );
    printf( "# of channels:\t\t0x%X\n", wav->chan );
    printf( "sample rate:\t\t0x%X\n", wav->samp );
    printf( "bits per sample:\t0x%X\n", wav->bps );
    printf( "data size:\t\t0x%X\n", wav->ds );
}
void WAV_PrintChannels( const struct WAV_Data* wav ) {
    int Bps = wav->bps%8 ? wav->bps/8+1 : wav->bps/8;//Btyes per Sample
    unsigned char** cd = (unsigned char**)malloc(wav->chan*sizeof(unsigned char*));//container for each channel
    int i, j;   //iterators
    int spc;    //SamplesPerChannel - number of samples each channel has
        spc = 8*wav->ds / (wav->chan * wav->bps);
    //allocate space for channel data
    for (i = 0; i < wav->chan; ++i) {
        cd[i] = (unsigned char*)malloc( spc*Bps );
    }

    //organize data by channel into their containers
    for (i = 0; i < wav->chan; ++i) {
        for (j = 0; j < spc*Bps; ++j) {
            cd[i][j] = (unsigned char)wav->data[j*Bps];
        }
    }
    //print all the samples for a channel
    for (i = 0; i < spc; ++i) {
        printf( "%i,", cd[0][i] );
    }
    printf("\n");

    //give back my space
    for (i = 0; i < wav->chan-1; ++i) {
        free( cd[i] );
    }
    free( cd );
}
void WAV_Destroy( struct WAV_Data* wav ) {
    free(wav->data);
}
