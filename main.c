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
// @kazutomo 把内核的list.h移植到了用户模式 Thanks!


#define _In_	// SAL (Microsoft source code annotation language) 微软注解 习惯加上它 使函数输入输出更加清晰
#define _Out_	// _In_ -> 这是一个输入参数   _Out_ -> 输出参数   _Inout_ -> 输入输出
#define _Inout_

#define INPUT_BUFFER_SIZE 1048576	// 蛋糕的价格是九磅十五便士
#define WORD_BUFFER_SIZE 512

struct word_info{					// 链表节点 存储每个词的信息
    char *wordCharactors;			// 单词字母们
    unsigned wordAppearTimes;		// 它的出现次数
    struct list_head internalList;	// 2条小尾巴 internalList.prev连接上个节点的尾巴(internalList) internalList.next连下个节点尾巴
};
typedef struct word_info WORD_INFO_NODE; // WORD_INFO_NODE  <==>  struct word_info

// ↓看看链表里有没有这个单词 (是否重复)
// @theWord: 待检查单词
// @wordList: 存放着很多很多单词的链表
bool checkIfWordExist(_In_ const char *theWord, _In_ struct list_head *wordList)
{
    struct list_head *pTemp;
	// 内核list.h提供list_for_each工具 大幅简化遍历链表步骤
	// 假设链表wordList现在有10个节点 将循环10次 每个节点的internalList自动传给形参pTemp
    list_for_each(pTemp, wordList)
    {
		// 传来的pTemp是节点内internalList 但我们想要的 是节点的wordCharactors与wordAppearTimes 怎么办?
		// 没问题 我们有list_entry (根据偏移值 自动计算出节点首地址pNode)  再pNode->wordCharactors pNode->wordAppearTimes  OKla
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);
        if(strcmp(theWord, pNode->wordCharactors) == 0)	// strcmp(A,B) 若A=B返回0 -> 这单词 链表里面有个和它长的一模一样
        {
            pNode->wordAppearTimes++;   // 重复次数+1
            return true;				// 毫不犹豫回答: 重复!
        }
    }

    return false;   // 不重复!
}

// ↓将某个单词放进单词链表
// @theWord: 待存储单词
// @wordList: 很多很多单词的链表
void appendWordToWordsList(_In_ char *theWord, _Inout_ struct list_head *wordList)
{
	// 放进去前 先检查表里有没有和自己一样的 有就算了
    bool bWordExists = checkIfWordExist(theWord, wordList);
    if(true == bWordExists)
    {
        return;
    }

	// 新分配一块内存newWordBuffer 把旧theWord拷贝到新家
    char *newWordBuffer = malloc(strlen(theWord) + 1);
    strcpy(newWordBuffer, theWord);

	// 为链表wordList新建节点(箱子)
    WORD_INFO_NODE *pWordInfo;
    pWordInfo = malloc(sizeof(WORD_INFO_NODE));	// 这是一个与WORD_INFO_NODE结构一样大的箱子
    pWordInfo->wordCharactors = newWordBuffer;	// 填充
    pWordInfo->wordAppearTimes = 1;
    INIT_LIST_HEAD(&pWordInfo->internalList);	// INIT_LIST_HEAD作用: 将internalList.prev指向internalList internalList.next也指向自己 自己指自己(初始化)
    list_add_tail(&pWordInfo->internalList, wordList);	// 把这个箱子(节点)搬到链表末尾: 上一个节点internalList.next = 这个节点internalList  接着这个节点internalList.next = 头节点internalList
}

// ↓分析用户输入 提取每个词 统计
// @theWord: 用户输入字符串
// @wordList: 存储统计结果的链表
void parseInputString(_In_ char *inputString, _Inout_ struct list_head *wordList)
{
    char *pInputStr = inputString;		// pInputStr先指向用户输入字符串的起始位置
    char *wordBuffer = malloc(WORD_BUFFER_SIZE); // 假定一个词不会超过512字节             (超过会很惨)

    while(*pInputStr == ' ' || *pInputStr == '\t' || *pInputStr == '\n')
    {
        pInputStr++;
		// 跳过开头空白部分
		// 比如: " \t\n   Wow,YinHongai is amazing."  ==>  "Wow,YinHongai is amazing."
    }

    while(*pInputStr)
    {
		// 遇到非空格、制表符、换行、逗号、句号的其他字符 进入分支
		// "Wow,YinHongai is amazing."
		//  ^
        if(*pInputStr != ' ' && *pInputStr != '\t' && *pInputStr != '\n' && *pInputStr != ',' && *pInputStr != '.')
        {
			// 将wordBuffer这块512字节内存全部填充0 接着pWordBuf指向wordBuffer首地址
            memset(wordBuffer, 0, WORD_BUFFER_SIZE);
            char *pWordBuf = wordBuffer;

			// 程序跑呀跑 遇到了空格、制表符、换行、逗号或句号 停下来
			// "Wow,YinHongai is amazing."
			//     ^
            while(*pInputStr && *pInputStr != ' ' && *pInputStr != '\t' && *pInputStr != '\n' && *pInputStr != ',' && *pInputStr != '.')
            {
                // 将*pInputStr这个字符拷贝到pWordBuf内存
                *pWordBuf = *pInputStr;
                pWordBuf++;
                pInputStr++;
            }

            *pWordBuf = '\0';
			// pWordBuf: "Wow\0" 调用appendWordToWordsList把Wow放进链表
            appendWordToWordsList(wordBuffer, wordList);
        } else {
            pInputStr++;
        }
    }

	// 归还512字节内存
    free(wordBuffer);
}

// ↓找出重复频率最高单词的节点 然后从链表中删掉
// @wordList: 存储最终结果的链表
// @highestAppearTimesWordNode: 将最高节点复制到这块内存
void dumpWordOfHighestAppearTimesAndDeleteFromList(_Inout_ struct list_head *wordList,
        _Out_ WORD_INFO_NODE *highestAppearTimesWordNode)
{
	// 记录当前遇到的最高重复次数
    unsigned maxWordAppearTimes = 0;
    struct list_head *pTemp, *pHighestTag = NULL;
    WORD_INFO_NODE *pHighestNode = NULL;

	// 找呀找...
    list_for_each(pTemp, wordList)
    {
        WORD_INFO_NODE *pNode = list_entry(pTemp, WORD_INFO_NODE, internalList);

        if(pNode->wordAppearTimes > maxWordAppearTimes)
        {
			// 找到一个重复次数比maxWordAppearTimes高的 记下来
            maxWordAppearTimes = pNode->wordAppearTimes;
            pHighestTag = pTemp;
            pHighestNode = pNode;
        }
    }

	// 复制最高节点至highestAppearTimesWordNode
    memcpy(highestAppearTimesWordNode, pHighestNode, sizeof(WORD_INFO_NODE));
	// 删除最高节点
    list_del(pHighestTag);
}

// ↓输出结果
// @wordList: 存储最终结果的链表
void printWordsList(_In_ struct list_head *wordList)
{
	// 老师要求去除虚词 这里定义一张虚词表
	// 如需新增 在数组加上 再将functionWordsTableSize改为数组大小
    const unsigned functionWordsTableSize = 6;
    const char* functionWordsTable[] = {
        "the", "a", "an", "of", "at", "in"
    };
	// 统计虚词出现次数
    unsigned functionWordsCount = 0;

	// 老师要求按频率从高到低排序输出
	// 我的思路:  每次输出频率最大节点 并删掉它
	//            当链表节点一个不剩 输出完毕
    puts("WORD        \t\t\tTIMES");
	// 链表空了没?
    while(false == list_empty(wordList))
    {
		// 调用dumpWordOfHighestAppearTimesAndDeleteFromList找到最高节点并删除 在那之前拷一份到临时节点以便输出
        WORD_INFO_NODE *pHighestNode = malloc(sizeof(WORD_INFO_NODE));
        dumpWordOfHighestAppearTimesAndDeleteFromList(wordList, pHighestNode);

		// 是不是虚词?
        bool isFunctionWord = false;
        for(int i=0; i<functionWordsTableSize; i++)
        {
            if(strcmp(functionWordsTable[i],pHighestNode->wordCharactors) == 0)
            {
				// 是!
                isFunctionWord = true;
                functionWordsCount++;
            }
        }

		// 不是!
        if(!isFunctionWord)
        {
			// 输出这个节点
            printf("%-12s\t\t\t%d\n", pHighestNode->wordCharactors, pHighestNode->wordAppearTimes);
        }

		// 释放临时节点
        free(pHighestNode);
    }

    if(functionWordsCount)
    {
		// 虚词数>0 输出
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
*    Hi 小仙女 程序入口点在这儿~ ↓
***/

int main()
{
    char *inputBuffer = malloc(INPUT_BUFFER_SIZE); // 在内存堆分配一块1MB(1024字节*1024)的蛋糕 用于接收输入字符
	
	LIST_HEAD(wordList);	// 等价于-> struct list_head wordList; 新建一个list_head结构体实例 命名wordList
							// 结构体内有2个指针变量: *prev、*next 对他们进行赋值 指向自己的地址
							// 变成了一个双向循环链表

    fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin);	// 从标准输入输出流stdin 读取不超过1MB个字符 放进第一行的蛋糕
    
	// ↓函数负责解析输入 将结果放进链表wordList
	// @inputBuffer: 刚才输入的字符串首地址
	// @wordList: 单词链表地址
	parseInputString(inputBuffer, &wordList);
    
	// ↓现在链表里有每个单词 以及出现次数 我们把结果输出来
	// @wordList: 单词链表地址
	printWordsList(&wordList);

    free(inputBuffer); // 把1MB大蛋糕还给系统
    return 0;
}

// 编译器: gcc 7.3.0
// 用法:
//		1. ./程序名 <cr> 输入字符串 <cr>
//		2. ./程序名 < 文本文件 <cr>
//		3. cat 文本文件 | ./程序名 <cr>

/***
*	测试:
*	/home/snow/CLionProjects/WordsCount/cmake-build-debug/WordsCount
*	YinHongai is the most perfect and beautiful and beautiful and gorgeous and gorgeous and gorgeous and clever and clever and clever and clever girl in the world. <回车>
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