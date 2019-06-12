//
// Created by snow on Jun 13th, 2019.
// https://github.com/penguin806/WordsCount.git
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"

#define _In_
#define _Out_
#define _Inout_

#define INPUT_BUFFER_SIZE 1024000
#define WORD_BUFFER_SIZE 512

struct word_info{
    char *wordCharactors;
    unsigned wordAppearTimes;
    struct list_head internalList;
};
typedef struct word_info WORD_INFO_NODE;

bool checkIfWordExist(_In_ const char *theWord, _In_ struct list_head *wordList)
{
    struct list_head *pTemp;
    list_for_each(pTemp, wordList)
    {
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);
        if(strcmp(theWord, pNode->wordCharactors) == 0)
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

void parseInputString(_In_ char *inputString, _Inout_ struct list_head *wordList)
{
    char *pInputStr = inputString;
    char *wordBuffer = malloc(WORD_BUFFER_SIZE); // 0.5KByte

    while(*pInputStr == ' ' || *pInputStr == '\t' || *pInputStr == '\n')
    {
        pInputStr++; // Skip blank of the beginning
    }

    while(*pInputStr)
    {
        if(*pInputStr != ' ' && *pInputStr != '\t' && *pInputStr != '\n' && *pInputStr != ',' && *pInputStr != '.')
        {
            memset(wordBuffer, 0, WORD_BUFFER_SIZE);
            char *pWordBuf = wordBuffer;
            while(*pInputStr && *pInputStr != ' ' && *pInputStr != '\t' && *pInputStr != '\n' && *pInputStr != ',' && *pInputStr != '.')
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

void printWordsList(_In_ struct list_head *wordList)
{
    const unsigned functionWordsTableSize = 6;
    const char* functionWordsTable[] = {
        "the", "a", "an", "of", "at", "in"
    };
    unsigned functionWordsCount = 0;

    puts("WORD        \t\t\tTIMES");
    while(false == list_empty(wordList))
    {
        WORD_INFO_NODE *pHighestNode = malloc(sizeof(WORD_INFO_NODE));
        dumpWordOfHighestAppearTimesAndDeleteFromList(wordList, pHighestNode);

        bool isFunctionWord = false;
        for(int i=0; i<functionWordsTableSize; i++)
        {
            if(strcmp(functionWordsTable[i],pHighestNode->wordCharactors) == 0)
            {
                isFunctionWord = true;
                functionWordsCount++;
            }
        }

        if(!isFunctionWord)
        {
            printf("%-12s\t\t\t%d\n", pHighestNode->wordCharactors, pHighestNode->wordAppearTimes);
        }

        free(pHighestNode);
    }

    if(functionWordsCount)
    {
        printf("%-12s\t\t\t%d\n", "FUNCTION_WORD", functionWordsCount);
    }
}

int main()
{
    char *inputBuffer = malloc(INPUT_BUFFER_SIZE); //1MByte
    LIST_HEAD(wordList);

    fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin);
    parseInputString(inputBuffer, &wordList);
    printWordsList(&wordList);

    free(inputBuffer);
    return 0;
}