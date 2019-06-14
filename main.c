//
// Created by snow on Jun 13th, 2019.
// https://github.com/penguin806/WordsCount.git
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
// https://gist.github.com/roychen/1710968
// @kazutomo ���ں˵�list.h��ֲ�����û�ģʽ Thanks!


#define _In_	// SAL (Microsoft source code annotation language) ΢��ע�� ϰ�߼����� ʹ�������������������
#define _Out_	// _In_ -> ����һ���������   _Out_ -> �������   _Inout_ -> �������
#define _Inout_

#define INPUT_BUFFER_SIZE 1048576	// ����ļ۸��ǾŰ�ʮ���ʿ
#define WORD_BUFFER_SIZE 512

struct word_info{					// ����ڵ� �洢ÿ���ʵ���Ϣ
    char *wordCharactors;			// ������ĸ��
    unsigned wordAppearTimes;		// ���ĳ��ִ���
    struct list_head internalList;	// 2��Сβ�� internalList.prev�����ϸ��ڵ��β��(internalList) internalList.next���¸��ڵ�β��
};
typedef struct word_info WORD_INFO_NODE; // WORD_INFO_NODE  <==>  struct word_info

// ��������������û��������� (�Ƿ��ظ�)
// @theWord: ����鵥��
// @wordList: ����źܶ�ܶ൥�ʵ�����
bool checkIfWordExist(_In_ const char *theWord, _In_ struct list_head *wordList)
{
    struct list_head *pTemp;
	// �ں�list.h�ṩlist_for_each���� ����򻯱���������
	// ��������wordList������10���ڵ� ��ѭ��10�� ÿ���ڵ��internalList�Զ������β�pTemp
    list_for_each(pTemp, wordList)
    {
		// ������pTemp�ǽڵ���internalList ��������Ҫ�� �ǽڵ��wordCharactors��wordAppearTimes ��ô��?
		// û���� ������list_entry (����ƫ��ֵ �Զ�������ڵ��׵�ַpNode)  ��pNode->wordCharactors pNode->wordAppearTimes  OKla
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);
        if(strcmp(theWord, pNode->wordCharactors) == 0)	// strcmp(A,B) ��A=B����0 -> �ⵥ�� ���������и���������һģһ��
        {
            pNode->wordAppearTimes++;   // �ظ�����+1
            return true;				// ������ԥ�ش�: �ظ�!
        }
    }

    return false;   // ���ظ�!
}

// ����ĳ�����ʷŽ���������
// @theWord: ���洢����
// @wordList: �ܶ�ܶ൥�ʵ�����
void appendWordToWordsList(_In_ char *theWord, _Inout_ struct list_head *wordList)
{
	// �Ž�ȥǰ �ȼ�������û�к��Լ�һ���� �о�����
    bool bWordExists = checkIfWordExist(theWord, wordList);
    if(true == bWordExists)
    {
        return;
    }

	// �·���һ���ڴ�newWordBuffer �Ѿ�theWord�������¼�
    char *newWordBuffer = malloc(strlen(theWord) + 1);
    strcpy(newWordBuffer, theWord);

	// Ϊ����wordList�½��ڵ�(����)
    WORD_INFO_NODE *pWordInfo;
    pWordInfo = malloc(sizeof(WORD_INFO_NODE));	// ����һ����WORD_INFO_NODE�ṹһ���������
    pWordInfo->wordCharactors = newWordBuffer;	// ���
    pWordInfo->wordAppearTimes = 1;
    INIT_LIST_HEAD(&pWordInfo->internalList);	// INIT_LIST_HEAD����: ��internalList.prevָ��internalList internalList.nextҲָ���Լ� �Լ�ָ�Լ�(��ʼ��)
    list_add_tail(&pWordInfo->internalList, wordList);	// ���������(�ڵ�)�ᵽ����ĩβ: ��һ���ڵ�internalList.next = ����ڵ�internalList  ��������ڵ�internalList.next = ͷ�ڵ�internalList
}

// �������û����� ��ȡÿ���� ͳ��
// @theWord: �û������ַ���
// @wordList: �洢ͳ�ƽ��������
void parseInputString(_In_ char *inputString, _Inout_ struct list_head *wordList)
{
    char *pInputStr = inputString;		// pInputStr��ָ���û������ַ�������ʼλ��
    char *wordBuffer = malloc(WORD_BUFFER_SIZE); // �ٶ�һ���ʲ��ᳬ��512�ֽ�             (������ܲ�)

    while(*pInputStr == ' ' || *pInputStr == '\t' || *pInputStr == '\n')
    {
        pInputStr++;
		// ������ͷ�հײ���
		// ����: " \t\n   Wow,YinHongai is amazing."  ==>  "Wow,YinHongai is amazing."
    }

    while(*pInputStr)
    {
		// �����ǿո��Ʊ�������С����š���ŵ������ַ� �����֧
		// "Wow,YinHongai is amazing."
		//  ^
        if(*pInputStr != ' ' && *pInputStr != '\t' && *pInputStr != '\n' && *pInputStr != ',' && *pInputStr != '.')
        {
			// ��wordBuffer���512�ֽ��ڴ�ȫ�����0 ����pWordBufָ��wordBuffer�׵�ַ
            memset(wordBuffer, 0, WORD_BUFFER_SIZE);
            char *pWordBuf = wordBuffer;

			// ������ѽ�� �����˿ո��Ʊ�������С����Ż��� ͣ����
			// "Wow,YinHongai is amazing."
			//     ^
            while(*pInputStr && *pInputStr != ' ' && *pInputStr != '\t' && *pInputStr != '\n' && *pInputStr != ',' && *pInputStr != '.')
            {
                // ��*pInputStr����ַ�������pWordBuf�ڴ�
                *pWordBuf = *pInputStr;
                pWordBuf++;
                pInputStr++;
            }

            *pWordBuf = '\0';
			// pWordBuf: "Wow\0" ����appendWordToWordsList��Wow�Ž�����
            appendWordToWordsList(wordBuffer, wordList);
        } else {
            pInputStr++;
        }
    }

	// �黹512�ֽ��ڴ�
    free(wordBuffer);
}

// ���ҳ��ظ�Ƶ����ߵ��ʵĽڵ� Ȼ���������ɾ��
// @wordList: �洢���ս��������
// @highestAppearTimesWordNode: ����߽ڵ㸴�Ƶ�����ڴ�
void dumpWordOfHighestAppearTimesAndDeleteFromList(_Inout_ struct list_head *wordList,
        _Out_ WORD_INFO_NODE *highestAppearTimesWordNode)
{
	// ��¼��ǰ����������ظ�����
    unsigned maxWordAppearTimes = 0;
    struct list_head *pTemp, *pHighestTag = NULL;
    WORD_INFO_NODE *pHighestNode = NULL;

	// ��ѽ��...
    list_for_each(pTemp, wordList)
    {
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);

        if(pNode->wordAppearTimes > maxWordAppearTimes)
        {
			// �ҵ�һ���ظ�������maxWordAppearTimes�ߵ� ������
            maxWordAppearTimes = pNode->wordAppearTimes;
            pHighestTag = pTemp;
            pHighestNode = pNode;
        }
    }

	// ������߽ڵ���highestAppearTimesWordNode
    memcpy(highestAppearTimesWordNode, pHighestNode, sizeof(WORD_INFO_NODE));
	// ɾ����߽ڵ�
    list_del(pHighestTag);
}

// ��������
// @wordList: �洢���ս��������
void printWordsList(_In_ struct list_head *wordList)
{
	// ��ʦҪ��ȥ����� ���ﶨ��һ����ʱ�
	// �������� ��������� �ٽ�functionWordsTableSize��Ϊ�����С
    const unsigned functionWordsTableSize = 6;
    const char* functionWordsTable[] = {
        "the", "a", "an", "of", "at", "in"
    };
	// ͳ����ʳ��ִ���
    unsigned functionWordsCount = 0;

	// ��ʦҪ��Ƶ�ʴӸߵ����������
	// �ҵ�˼·:  ÿ�����Ƶ�����ڵ� ��ɾ����
	//            ������ڵ�һ����ʣ ������
    puts("WORD        \t\t\tTIMES");
	// �������û?
    while(false == list_empty(wordList))
    {
		// ����dumpWordOfHighestAppearTimesAndDeleteFromList�ҵ���߽ڵ㲢ɾ�� ����֮ǰ��һ�ݵ���ʱ�ڵ��Ա����
        WORD_INFO_NODE *pHighestNode = malloc(sizeof(WORD_INFO_NODE));
        dumpWordOfHighestAppearTimesAndDeleteFromList(wordList, pHighestNode);

		// �ǲ������?
        bool isFunctionWord = false;
        for(int i=0; i<functionWordsTableSize; i++)
        {
            if(strcmp(functionWordsTable[i],pHighestNode->wordCharactors) == 0)
            {
				// ��!
                isFunctionWord = true;
                functionWordsCount++;
            }
        }

		// ����!
        if(!isFunctionWord)
        {
			// �������ڵ�
            printf("%-12s\t\t\t%d\n", pHighestNode->wordCharactors, pHighestNode->wordAppearTimes);
        }

		// �ͷ���ʱ�ڵ�
        free(pHighestNode);
    }

    if(functionWordsCount)
    {
		// �����>0 ���
        printf("%-12s\t\t\t%d\n", "FUNCTION_WORD", functionWordsCount);
    }
}

/***                              _
*  _._ _..._ .-',     _.._(`))
* '-. `     '  /-._.-'    ',/
*    )         \            '.
*   / _    _    |             \
*  |  a    a    /              |
*  \   .-.                     ;
*   '-('' ).-'       ,'       ;
*      '-;           |      .'
*         \           \    /
*         | 7  .__  _.-\   \
*         | |  |  ``/  /`  /
*        /,_|  |   /,_/   /
*           /,_/      '`-'
*
*    Hi С��Ů ������ڵ������~ ��
***/

int main()
{
    char *inputBuffer = malloc(INPUT_BUFFER_SIZE); // ���ڴ�ѷ���һ��1MB(1024�ֽ�*1024)�ĵ��� ���ڽ��������ַ�
	
	LIST_HEAD(wordList);	// �ȼ���-> struct list_head wordList; �½�һ��list_head�ṹ��ʵ�� ����wordList
							// �ṹ������2��ָ�����: *prev��*next �����ǽ��и�ֵ ָ���Լ��ĵ�ַ
							// �����һ��˫��ѭ������

    fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin);	// �ӱ�׼���������stdin ��ȡ������1MB���ַ� �Ž���һ�еĵ���
    
	// ����������������� ������Ž�����wordList
	// @inputBuffer: �ղ�������ַ����׵�ַ
	// @wordList: ���������ַ
	parseInputString(inputBuffer, &wordList);
    
	// ��������������ÿ������ �Լ����ִ��� ���ǰѽ�������
	// @wordList: ���������ַ
	printWordsList(&wordList);

    free(inputBuffer); // ��1MB�󵰸⻹��ϵͳ
    return 0;
}

// ������: gcc 7.3.0
// �÷�:
//		1. ./������ <cr> �����ַ��� <cr>
//		2. ./������ < �ı��ļ� <cr>
//		3. cat �ı��ļ� | ./������ <cr>

/***
*	����:
*	/home/snow/CLionProjects/WordsCount/cmake-build-debug/WordsCount
*	YinHongai is the most perfect and beautiful and beautiful and gorgeous and gorgeous and gorgeous and clever and clever and clever and clever girl in the world. <�س�>
*	WORD        			TIMES
*	and						9
*	clever      			4
*	gorgeous    			3
*	beautiful   			2
*	YinHongai   			1
*	is          			1
*	most        			1
*	perfect     			1
*	girl        			1
*	world       			1
*	FUNCTION_WORD			2
*
***/