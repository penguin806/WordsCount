//
// Created by snow on Jun 13th, 2019.
// https://github.com/penguin806/WordsCount.git
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include "list.h"

#define _In_
#define _Out_
#define _Inout_

struct word_info{
    char *wordCharactors;
    unsigned wordAppearTimes;
    struct list_head internalList;
};
typedef struct word_info WORD_INFO_NODE;

bool checkIfWordsAreSameIgnoreCase(_In_ char *wordA, _In_ char *wordB)
{
    char *pA = wordA;
    char *pB = wordB;

    while (*pA && *pB)
    {
        if(tolower(*pA) != tolower(*pB))
        {
            return false;
        }
        pA++;
        pB++;
    }

    if(*pA || *pB)
    {
        return false;
    } else {
        return true;
    }
}

bool checkIfWordExist(_In_ char *theWord, _In_ struct list_head *wordList)
{
    struct list_head *pTemp;
    list_for_each(pTemp, wordList)
    {
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);
        if(checkIfWordsAreSameIgnoreCase(theWord, pNode->wordCharactors))
        {
            pNode->wordAppearTimes++;   // Word already exists in list
            return true;
        }
    }

    return false;   // Word not exists in list
}

void appendWordToWordsList(_In_ char *theWord, _Inout_ struct list_head *wordList)
{
    bool bWordExists = checkIfWordExist(theWord, wordList);
    if(true == bWordExists)
    {
        return;
    }

    char *newWordBuffer = malloc(strlen(theWord) + 1);
    strcpy(newWordBuffer, theWord);

    WORD_INFO_NODE *pWordInfo;
    pWordInfo = malloc(sizeof(WORD_INFO_NODE));
    pWordInfo->wordCharactors = newWordBuffer;
    pWordInfo->wordAppearTimes = 1;
    INIT_LIST_HEAD(&pWordInfo->internalList);
    list_add_tail(&pWordInfo->internalList, wordList);
}

void parseInputString(_In_ char *inputString, _In_ const int inputBufferSize, _Inout_ struct list_head *wordList)
{
	if(inputBufferSize <= 0)
	{
		return;
	}
    char *pInputStr = inputString;
    char *wordBuffer = malloc(inputBufferSize);

    while(*pInputStr == ' ' || *pInputStr == '\t' || *pInputStr == '\n')
    {
        pInputStr++; // Skip blank of the beginning
    }

    while(*pInputStr && pInputStr < inputString + inputBufferSize)
    {
        // if the character is NOT 'space' or 'tab' or 'newLine' or any 'punctuationCharacter'
        if(!isspace(*pInputStr) && !ispunct(*pInputStr))
        {
            memset(wordBuffer, 0, inputBufferSize);
            char *pWordBuf = wordBuffer;
            while(*pInputStr && pInputStr < inputString + inputBufferSize && !isspace(*pInputStr) && !ispunct(*pInputStr))
            {
                // Extract word
                *pWordBuf = *pInputStr;
                pWordBuf++;
                pInputStr++;
            }

            *pWordBuf = '\0';
            appendWordToWordsList(wordBuffer, wordList);
        } else {
            pInputStr++;
        }
    }

    free(wordBuffer);
}

void dumpWordOfHighestAppearTimesAndDeleteFromList(_Inout_ struct list_head *wordList,
        _Out_ WORD_INFO_NODE *highestAppearTimesWordNode)
{
    unsigned maxWordAppearTimes = 0;
    struct list_head *pTemp, *pHighestTag = NULL;
    WORD_INFO_NODE *pHighestNode = NULL;

    list_for_each(pTemp, wordList)
    {
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);

        if(pNode->wordAppearTimes > maxWordAppearTimes)
        {
            maxWordAppearTimes = pNode->wordAppearTimes;
            pHighestTag = pTemp;
            pHighestNode = pNode;
        }
    }

    memcpy(highestAppearTimesWordNode, pHighestNode, sizeof(WORD_INFO_NODE));   // Dump
    list_del(pHighestTag);
}

void printWordsList(_In_ struct list_head *wordList, _In_ FILE *outputFileHandle)
{
    const unsigned functionWordsTableSize = 6;
    const char* functionWordsTable[] = {
        "the", "a", "an", "of", "at", "in"
    };
    unsigned functionWordsCount = 0;

    fputs("WORD                \t\t\tTIMES\n", outputFileHandle);
    while(false == list_empty(wordList))
    {
        WORD_INFO_NODE *pHighestNode = malloc(sizeof(WORD_INFO_NODE));
        dumpWordOfHighestAppearTimesAndDeleteFromList(wordList, pHighestNode);

        bool isFunctionWord = false;
        for(int i=0; i<functionWordsTableSize; i++)
        {
            if( checkIfWordsAreSameIgnoreCase((char *)functionWordsTable[i], pHighestNode->wordCharactors) )
            {
                isFunctionWord = true;
                functionWordsCount++;
            }
        }

        if(!isFunctionWord)
        {
            fprintf(outputFileHandle, "%-20s\t\t\t%d\n", pHighestNode->wordCharactors, pHighestNode->wordAppearTimes);
        }

        free(pHighestNode);
    }

    if(functionWordsCount)
    {
        fprintf(outputFileHandle, "%-20s\t\t\t%d\n", "FUNCTION_WORD", functionWordsCount);
    }
}

int main(int argc, char *argv[])
{
    char *inputFilePath = NULL;
    char *outputFilePath = NULL;

    int optionIndex = 0;
    struct option longOptions[] = {
            {"input", required_argument, NULL, 'i'},
            {"output", required_argument, NULL, 'o'},
            {0,0,0,0}
    };
    const char *shortOptionString = "i:o:";

    while (1)
    {
        int singleChar = getopt_long(argc, argv, shortOptionString, longOptions, &optionIndex);
        if( singleChar == -1)
        {
            break;
        }

        switch (singleChar)
        {
            case 'i':
                inputFilePath = optarg;
                break;
            case 'o':
                outputFilePath = optarg;
                break;
            default:
                printf("Usage: %s -i <InputFile> -o <OutputFile>\n", argv[0]);
                return 1;
        }
    }

    FILE *pInputFile = fopen(inputFilePath, "r");
//    FILE *pInputFile = fopen("test.txt", "r");
    FILE *pOutputFile = fopen(outputFilePath, "w");
    if(!pInputFile)
    {
        puts("Error: unable to access InputFile.");
        return -1;
    }
    if(!pOutputFile)
    {
        puts("Error: create OutputFile failed.");
        return -2;
    }

    // Get size of InputFile
    fseek(pInputFile, 0, SEEK_END);
    const int inputFileSize = ftell(pInputFile);
    fseek(pInputFile, 0, SEEK_SET);

    // Allocate memory equals to InputFile size
    const int inputBufferSize = inputFileSize + 1;
    char *inputBuffer = malloc(inputBufferSize);
    // Read entire InputFile and fill info inputBuffer
    fread(inputBuffer, inputFileSize, 1, pInputFile);
    *(inputBuffer + inputBufferSize - 1) = 0;

    LIST_HEAD(wordList);

    parseInputString(inputBuffer, inputBufferSize, &wordList);
    printWordsList(&wordList, pOutputFile);

    free(inputBuffer);
    return 0;
}