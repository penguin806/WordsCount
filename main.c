//
// Created by snow on Jun 13th, 2019.
// https://github.com/penguin806/WordsCount.git
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head);	\
       pos = pos->next)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

typedef struct node{
	char *pattern;
	int count;
	struct list_head myList;
} LIST_NODE;






void extractWord(char *inputString, const int inputBufferSize, struct list_head *wordList);
void outputResult(struct list_head *list, FILE *outputFileHandle);

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

	extractWord(inputBuffer, inputBufferSize, &wordList);
	outputResult(&wordList, pOutputFile);

	free(inputBuffer);
	return 0;
}

int compareWords_IgnoreCase(char *wordA, char *wordB)
{
	char *pA = wordA;
	char *pB = wordB;

	while (*pA && *pB)
	{
		if(tolower(*pA) != tolower(*pB))
		{
			return 0;
		}
		pA++;
		pB++;
	}

	if(*pA || *pB)
	{
		return 0;
	} else {
		return 1;
	}
}

int isInList(char *word, struct list_head *list)
{
	struct list_head *item;
	list_for_each(item, list)
	{
		LIST_NODE *pNode = list_entry(item, LIST_NODE, myList);
		if(compareWords_IgnoreCase(word, pNode->pattern))
		{
			pNode->count++;
			return 1;
		}
	}

	return 0;
}

void addWord(char *word, struct list_head *list)
{
	int check = isInList(word, list);
	if(1 == check)
	{
		return;
	}

	char *buffer = malloc(strlen(word) + 1);
	strcpy(buffer, word);

	LIST_NODE *pWordInfo;
	pWordInfo = malloc(sizeof(LIST_NODE));
	pWordInfo->pattern = buffer;
	pWordInfo->count = 1;
	INIT_LIST_HEAD(&pWordInfo->myList);
	list_add_tail(&pWordInfo->myList, list);
}

void extractWord(char *inputString, const int inputBufferSize, struct list_head *wordList)
{
	if(inputBufferSize <= 0)
	{
		return;
	}
	char *pInputStr = inputString;
	char *space = malloc(inputBufferSize);

	while(*pInputStr && pInputStr < inputString + inputBufferSize)
	{
		// if the character is NOT 'space' or 'tab' or 'newLine' or any 'punctuationCharacter'
		if(!isspace(*pInputStr) && !ispunct(*pInputStr))
		{
			memset(space, 0, inputBufferSize);
			char *pWordBuf = space;
			while(*pInputStr && pInputStr < inputString + inputBufferSize && !isspace(*pInputStr) && !ispunct(*pInputStr))
			{
				*pWordBuf = *pInputStr;
				pWordBuf++;
				pInputStr++;
			}

			*pWordBuf = '\0';
			addWord(space, wordList);
		} else {
			pInputStr++;
		}
	}

	free(space);
}

void dumpWord(struct list_head *list, LIST_NODE *node)
{
	unsigned highest = 0;
	struct list_head *pTemp, *pHighestTag = NULL;
	LIST_NODE *pHighestNode = NULL;

	list_for_each(pTemp, list)
	{
		LIST_NODE *pNode = list_entry(pTemp, LIST_NODE, myList);

		if(pNode->count > highest)
		{
			highest = pNode->count;
			pHighestTag = pTemp;
			pHighestNode = pNode;
		}
	}

	memcpy(node, pHighestNode, sizeof(LIST_NODE));
	list_del(pHighestTag);
}

void outputResult(struct list_head *list, FILE *outputFileHandle)
{
	const unsigned functionWordsTableSize = 6;
	const char* functionWordsTable[] = {
			"the", "a", "an", "of", "at", "in"
	};
	unsigned functionWordsCount = 0;

	fputs("PATTERN             \t\t\tCOUNT\n", outputFileHandle);
	while(0 == list_empty(list))
	{
		LIST_NODE *pHighestNode = malloc(sizeof(LIST_NODE));
		dumpWord(list, pHighestNode);

		int isFunctionWord = 0;
		for(int i=0; i<functionWordsTableSize; i++)
		{
			if(compareWords_IgnoreCase((char *) functionWordsTable[i], pHighestNode->pattern) )
			{
				isFunctionWord = 1;
				functionWordsCount++;
			}
		}

		if(!isFunctionWord)
		{
			fprintf(outputFileHandle, "%-20s\t\t\t%d\n", pHighestNode->pattern, pHighestNode->count);
		}

		free(pHighestNode);
	}
}