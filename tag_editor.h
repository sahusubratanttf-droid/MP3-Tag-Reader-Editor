#ifndef TAG_EDITOR_H
#define TAG_EDITOR_H

#include "constants.h"

Status edit_tags(const char *inputFile,
                const char *outputFile,
                const char *frameID,
                const char *newData);

#endif