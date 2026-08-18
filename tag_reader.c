#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "tag_reader.h"
#include "constants.h"

#define RED    "\033[1;31m"   // Defining colour codes
#define GREEN  "\033[1;32m"
#define BLUE   "\033[34m"
#define RESET  "\033[0m"

const char* get_label(const char *frameID) 
{
    if (strcmp(frameID, "TIT2") == 0) return BLUE "Title"    RESET "(" GREEN "TIT2" RESET ")";
    if (strcmp(frameID, "TALB") == 0) return BLUE "Album"    RESET "(" GREEN "TALB" RESET ")";
    if (strcmp(frameID, "TPE1") == 0) return BLUE "Artist"   RESET "(" GREEN "TPE1" RESET ")";
    if (strcmp(frameID, "TYER") == 0) return BLUE "Year"     RESET "(" GREEN "TYER" RESET ")";
    if (strcmp(frameID, "TCON") == 0) return BLUE "Genre"    RESET "(" GREEN "TCON" RESET ")";
    if (strcmp(frameID, "COMM") == 0) return BLUE "Comments" RESET "(" GREEN "COMM" RESET ")";
    return frameID; 
}

Status read_tags(const char *filename) 
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) 
    {
        printf(RED "Error: Cannot open file %s\n" RESET, filename);
        return FAILURE;
    }

    printf(GREEN "File reading started...\n" RESET);

    // Read 10-byte ID3 header
    unsigned char header[10];
    fread(header, 1, 10, fp);

    // Extract total tag size 
    int totalTagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7)  | header[9];

    long bytesRead = 0;
    while (bytesRead < totalTagSize) 
    {
        char frameID[5];
        fread(frameID, 1, 4, fp);
        frameID[4] = '\0';

        // Stop if padding (all zeros)
        if (frameID[0] == 0) 
           break;

        unsigned char sizeBytes[4];
        fread(sizeBytes, 1, 4, fp);
        int frameSize = (sizeBytes[0] << 24) | (sizeBytes[1] << 16) | (sizeBytes[2] << 8) | sizeBytes[3];

        fseek(fp, 2, SEEK_CUR); // skip frame flags

        unsigned char *data = (unsigned char *)malloc(frameSize);
        fread(data, 1, frameSize, fp);

        // Only processing the frames i want
        if (strcmp(frameID, "TIT2") == 0 ||
            strcmp(frameID, "TALB") == 0 ||
            strcmp(frameID, "TYER") == 0 ||
            strcmp(frameID, "TCON") == 0 ||
            strcmp(frameID, "COMM") == 0 ||
            strcmp(frameID, "TPE1") == 0) 
        {
            printf("%s: ", get_label(frameID));
        
            unsigned char encoding = data[0];
            if (encoding == 0) 
            {
                for (int i = 1; i < frameSize; i++) 
                {
                    if (data[i] != 0) 
                      putchar(data[i]);
                }
                putchar('\n');
            } 
            else if (encoding == 1) 
            {
                int start = 3;    // Skip (2 bytes after encoding)
                for (int i = start; i < frameSize; i += 2) 
                {
                    if (data[i] != 0) putchar(data[i]);
                }
                putchar('\n');
            } 
            else if (encoding == 3) 
            {
                printf("%s\n", (char *)(data + 1));
            } 
            else 
            {
                printf(RED "Unsupported encoding: %d\n" RESET, encoding);
            }
        }

        free(data);
        bytesRead += 10 + frameSize; // frame header + data
    }

    fclose(fp);

    printf(GREEN "File reading successful!\n" RESET);
    return SUCCESS;
}