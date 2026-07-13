#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "common/gopro_json.h"

int tfp_sprintf(char *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    const int written = vsprintf(s, fmt, ap);
    va_end(ap);
    return written;
}

static char *readFile(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }

    long size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }

    char *buffer = (char *)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    size_t readBytes = fread(buffer, 1, (size_t)size, fp);
    fclose(fp);

    if (readBytes != (size_t)size) {
        free(buffer);
        return NULL;
    }

    buffer[size] = '\0';
    return buffer;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <json-file> <status|settings> <key>\n", argv[0]);
        return 1;
    }

    char *json = readFile(argv[1]);
    if (!json) {
        fprintf(stderr, "Failed to read JSON file: %s\n", argv[1]);
        return 2;
    }

    const char *objectName = argv[2];
    const char *key = argv[3];
    const char *objectStart = NULL;
    const char *objectEnd = NULL;
    char value[32];

    if (strcmp(objectName, "status") != 0 && strcmp(objectName, "settings") != 0) {
        fprintf(stderr, "Invalid object: %s (expected status or settings)\n", objectName);
        free(json);
        return 3;
    }

    if (goproJsonExtractObjectRange(json, objectName, &objectStart, &objectEnd) &&
        goproJsonExtractValue(objectStart, objectEnd, key, value, sizeof(value))) {
        printf("%s[%s]: %s\n", objectName, key, value);
    } else {
        printf("%s[%s] not found\n", objectName, key);
    }

    free(json);
    return 0;
}
