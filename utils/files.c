#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

const char *directoryName = "pages";

char *getFileContent(char *fileName)
{
    // char *newFileName;
    char *filePath;
    struct dirent *directoryEntry;
    DIR *directory = opendir(directoryName);

    if (directory == NULL)
    {
        fprintf(stdout, "'%s': Directory not found\n", directoryName);
        return NULL;
    }
    else
    {
        // newFileName = malloc(strlen(fileName) + 5);
        // strcpy(newFileName, fileName);
        // strcat(newFileName, ".html");
        // fprintf(stdout, "file: %s\t%s\t%ld\n", fileName, newFileName, strlen(fileName));

        while ((directoryEntry = readdir(directory)) != NULL)
        {
            if (strcmp(fileName, directoryEntry->d_name) != 0)
            {
                continue;
            }
            filePath = malloc(strlen(directoryName) + strlen(directoryEntry->d_name) + 2);
            sprintf(filePath, "%s/%s", directoryName, directoryEntry->d_name);

            FILE *file = fopen(filePath, "r");

            if (file == NULL)
            {
                fprintf(stdout, "Unable to open file at location %s\n", filePath);
                free(filePath);
                // free(newFileName);
                closedir(directory);
                return NULL;
            }

            char buffer[2];
            size_t offset = 0;
            size_t fileSize = 1;
            char *fileContent = malloc(fileSize);

            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                fileSize += strlen(buffer);
                fileContent = realloc(fileContent, fileSize);
                snprintf(fileContent + offset, strlen(buffer) + 1, "%s", buffer);
                offset += strlen(buffer);
            }

            free(filePath);
            // free(newFileName);
            closedir(directory);
            return fileContent;
        }
    }
    fprintf(stdout, "File not found: %s\n", fileName);
    // free(newFileName);
    closedir(directory);
    return NULL;
}

char *getHtmlFileContent(const char *fileName)
{
    char *newFileName;

    newFileName = malloc(strlen(fileName) + 5);
    strcpy(newFileName, fileName);
    strcat(newFileName, ".html");

    char *fileContent = getFileContent(newFileName);

    free(newFileName);

    return fileContent;
}

char *getMimeType(char *fileName)
{
    if (fileName == NULL)
    {
        return "text/html";
    }

    // fprintf(stdout, "%s\n", fileName);
    char *fileExtension = strrchr(fileName, '.');

    if (fileExtension == NULL)
        return "text/plain";

    if (strcmp(fileExtension, ".html") == 0 || strcmp(fileExtension, ".html") == 0)
        return "text/html";
    else if (strcmp(fileExtension, ".css") == 0)
        return "text/css";
    else if (strcmp(fileExtension, ".js") == 0)
        return "application/javascript";
    else if (strcmp(fileExtension, ".json") == 0)
        return "application/json";
    else if (strcmp(fileExtension, ".xml") == 0)
        return "application/xml";

    return "text/plain";
}