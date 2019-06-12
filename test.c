//
// Created by snow on Jun 13th, 2019.
// https://github.com/penguin806/WordsCount.git
//

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct word_info{
    char *wordCharactors;
    unsigned wordAppearTimes;
    struct list_head internalList;
};
typedef struct word_info WORD_INFO_NODE;

int _main()
{
    LIST_HEAD(wordList);

    WORD_INFO_NODE *pWordInfo;
    pWordInfo = malloc(sizeof(WORD_INFO_NODE));
    pWordInfo->wordCharactors = "Test1";
    pWordInfo->wordAppearTimes = 1;
    INIT_LIST_HEAD(&pWordInfo->internalList);
    list_add_tail(&pWordInfo->internalList, &wordList);

    WORD_INFO_NODE *pWordInfo2;
    pWordInfo2 = malloc(sizeof(WORD_INFO_NODE));
    pWordInfo2->wordCharactors = "Test2";
    pWordInfo2->wordAppearTimes = 1;
    INIT_LIST_HEAD(&pWordInfo2->internalList);
    list_add_tail(&pWordInfo2->internalList, &wordList);


    struct list_head *pTemp;
    list_for_each(pTemp, &wordList){
        WORD_INFO_NODE *node = list_entry(pTemp,WORD_INFO_NODE,internalList);
        printf("%s\n",node->wordCharactors);
    };

    return 0;
}