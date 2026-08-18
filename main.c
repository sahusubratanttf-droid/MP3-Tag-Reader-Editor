#include <stdio.h>
#include <string.h>
#include "types.h"
#include "tag_reader.h"
#include "tag_editor.h"

#define RED     "\033[1;31m"   // Defining colour codes
#define GREEN   "\033[1;32m"
#define BLUE   "\033[34m"
#define RESET   "\033[0m"

int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage:\n");
        printf("  %s -r input.mp3\n", argv[0]);
        printf("  %s -e -<flag> new_data input.mp3 [output.mp3]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-r") == 0) 
    {
        // Read mode
        const char *inputFile = argv[2];
        read_tags(inputFile);
    }

    else if (strcmp(argv[1], "-e") == 0) 
    {
        // Edit mode
       if (argc < 5) 
       {
        printf("Usage: %s -e -<flag> new_data input.mp3 [output.mp3]\n", argv[0]);
        return 1;
       }

        const char *frameID = NULL;
        // Mapping flags to frame IDs
        if (strcmp(argv[2], "-t") == 0) frameID = "TIT2";      // Title
        else if (strcmp(argv[2], "-A") == 0) frameID = "TALB"; // Album
        else if (strcmp(argv[2], "-a") == 0) frameID = "TPE1"; // Artist
        else if (strcmp(argv[2], "-y") == 0) frameID = "TYER"; // Year
        else if (strcmp(argv[2], "-c") == 0) frameID = "TCON"; // Genre
        else if (strcmp(argv[2], "-m") == 0) frameID = "COMM"; // Comment
        else {
            printf(RED "Unknown flag: %s\n" RESET, argv[2]);
            return 1;
        }

        const char *newData   = argv[3]; // new tag value
        const char *inputFile = argv[4]; // original MP3
        const char *outputFile = (argc >= 6) ? argv[5] : "new.mp3"; // If user provided an output filename, use it; otherwise default

        Status result = edit_tags(inputFile, outputFile, frameID, newData);

        if (result == SUCCESS) 
        {
            printf(BLUE "Tag %s updated successfully in %s\n" RESET, frameID, inputFile);
            printf(GREEN "File editing successful!\n" RESET);
        } 
        else 
        {
            printf(RED "Failed to edit tag %s\n" RESET, frameID);
        }
    }

    return 0;
}