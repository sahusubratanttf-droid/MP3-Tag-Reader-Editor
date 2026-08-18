#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "tag_editor.h"
#include "constants.h"

#define RED    "\033[1;31m"   // Defining colour codes
#define GREEN  "\033[1;32m"
#define BLUE   "\033[34m"
#define RESET  "\033[0m"

Status edit_tags(const char *inputFile,
                const char *outputFile,
                const char *frameID,
                const char *newData)
{
    FILE *fp = fopen(inputFile, "rb");
    if (!fp) 
    {
        printf(RED "Error: Cannot open %s\n" RESET, inputFile);
        return FAILURE;
    }

    FILE *newfp = fopen(outputFile, "wb");
    if (!newfp) 
    {
        printf(RED "Error: Cannot create %s\n" RESET, outputFile);
        fclose(fp);
        return FAILURE;
    }

    printf(GREEN "File editing started...\n" RESET);

    // Copy 10-byte ID3 header
    unsigned char header[10];
    fread(header, 1, 10, fp);
    fwrite(header, 1, 10, newfp);

    long bytesRead = 0;
    int totalTagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7)  | header[9];

    while (bytesRead < totalTagSize) 
    {
        char currentID[5];
        fread(currentID, 1, 4, fp);
        currentID[4] = '\0';

        if (currentID[0] == 0) 
           break; // padding

        unsigned char sizeBytes[4];
        fread(sizeBytes, 1, 4, fp);
        int frameSize = (sizeBytes[0] << 24) | (sizeBytes[1] << 16) | (sizeBytes[2] << 8)  | sizeBytes[3];

        unsigned char flags[2];
        fread(flags, 1, 2, fp);

        unsigned char *data = (unsigned char *)malloc(frameSize);
        fread(data, 1, frameSize, fp);

        if (strcmp(currentID, frameID) == 0) 
        {
            // Build new frame
            int newSize = strlen(newData) + 1; // +1 for encoding byte
            unsigned char newSizeBytes[4] = {(newSize >> 24) & 0xFF, (newSize >> 16) & 0xFF, (newSize >> 8) & 0xFF, newSize & 0xFF};

            fwrite(currentID, 1, 4, newfp);
            fwrite(newSizeBytes, 1, 4, newfp);
            fwrite(flags, 1, 2, newfp);

            unsigned char encoding = 0; 
            fwrite(&encoding, 1, 1, newfp);
            fwrite(newData, 1, strlen(newData), newfp);
        } 
        else 
        {
            // Copy original frame
            fwrite(currentID, 1, 4, newfp);
            fwrite(sizeBytes, 1, 4, newfp);
            fwrite(flags, 1, 2, newfp);
            fwrite(data, 1, frameSize, newfp);
        }

        free(data);
        bytesRead += 10 + frameSize;
    }

    // Copy remaining audio data
    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) 
    {
        fwrite(buffer, 1, n, newfp);
    }

    fclose(fp);
    fclose(newfp);

    // Delete old file and rename new one
    if (remove(inputFile) == 0 && rename(outputFile, inputFile) == 0) 
    {
        return SUCCESS;
    }

    return SUCCESS;
}