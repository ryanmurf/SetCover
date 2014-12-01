/*
 * SetCover.c
 *
 *  Created on: Nov 29, 2014
 *      Author: Ryan Murphy
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

//buffer used by input statements
char inbuf[1024];
char _file[1024];

typedef struct Set Set;

struct Set /* the state of the game at each possible move */
{
	int nGlobalSetSize;
	int nSubSets;
	int* originalOrder;
	int* nSubSetSizes;
	int** subsets;
	int** subSetsSizesSum;
};

typedef struct Solution Solution;
struct Solution
{
	int nSolutionSize;
	int* subSets;
	int* boolIncluded;
};

Set* set;
Solution* solution;
Solution* bestSolution;

bool GetALine(FILE *f, char buf[]);
void readGameFile(const char *s);
void init_args(int argc, char **argv);
void echoInit();
void printSubSet(int nSubSetIndex);
void printSolution(Solution* solution);
bool checkSolution(Solution* solution);
bool containsSubSet(Solution* solution, int subSetIndex);
void addSubSet(Solution* solution, int subSetIndex);
void removeSubSet(Solution* solution, int subSetIndex);
void backTrack(Solution* solution);
void backTrack3(Solution* solution, int last);
void backTrack4(Solution* solution, int last, int sum);
void createSolutionStruct();
void copySolutionToBest();
void sortSubSets();
void greedy();
int numberOfUncoveredElements(int subSetIndex);

void intHandler(int dummy) {
	printSolution(bestSolution);
    exit(0);
}

int main(int argc, char *argv[]) {

	signal(SIGINT, intHandler);

	init_args(argc, argv);
	readGameFile(_file);
	sortSubSets();
	//echoInit();
	createSolutionStruct();

	greedy();

	backTrack4(solution, 0, 0);

	printSolution(bestSolution);
	printf("\n");

	return 0;
}

void greedy() {
	int i;
	int temp;
	bestSolution->nSolutionSize = 0;
	addSubSet(bestSolution, 0);
	int addIndex = 0;
	int addNumber = 0;

	while(!checkSolution(bestSolution)) {
		addIndex = 0;
		addNumber = 0;
		for(i=0; i<set->nSubSets; i++) {
			if(!containsSubSet(bestSolution, i)) {
				temp = numberOfUncoveredElements(i);
				if(temp > addNumber) {
					addNumber = temp;
					addIndex = i;
				}
			}
		}
		addSubSet(bestSolution, addIndex);
	}
	//printSolution(bestSolution);
}

int numberOfUncoveredElements(int subSetIndex) {
	int i;
	int count=0;

	for(i=0; i<set->nSubSetSizes[subSetIndex]; i++) {
		if(!bestSolution->boolIncluded[set->subsets[subSetIndex][i] - 1])
			count++;
	}

	return count;
}

//This sorts the subsets by size
void sortSubSets() {
	int i,j;
	int tempInt;
	int* tempIntP;
	for(i=0; i<set->nSubSets;++i) {
		for(j=i+1; j<set->nSubSets; ++j) {
			if(set->nSubSetSizes[i] < set->nSubSetSizes[j]) {
				tempInt = set->nSubSetSizes[i];
				tempIntP = set->subsets[i];
				set->subsets[i] = set->subsets[j];
				set->subsets[j] = tempIntP;
				set->nSubSetSizes[i] = set->nSubSetSizes[j];
				set->nSubSetSizes[j] = tempInt;
				tempInt = set->originalOrder[i];
				set->originalOrder[i] = set->originalOrder[j];
				set->originalOrder[j] = tempInt;
			}
		}
	}
	//Get Memory for sizes table
	set->subSetsSizesSum = (int **) malloc(sizeof(int *) * set->nSubSets);
	for(i=0; i<set->nSubSets; i++) {
		set->subSetsSizesSum[i] = (int *) malloc(sizeof(int) * set->nSubSets);
		memset(set->subSetsSizesSum[i], 0, set->nSubSets);
	}

	for(i=0; i<set->nSubSets; i++) {
		tempInt = 0;
		for(j=i; j<set->nSubSets; j++) {
			tempInt += set->nSubSetSizes[j];
			set->subSetsSizesSum[i][j-i] = tempInt;
		}
	}

	//Check
	/*for(i=0; i<set->nSubSets; i++) {
		for(j=0; j<set->nSubSets; j++) {
			printf("%u ", set->subSetsSizesSum[i][j]);
		}
		printf("\n");
	}*/
}

void createSolutionStruct() {
	int i;
	solution = malloc(sizeof(Solution));
	solution->subSets = malloc(sizeof(int) * (set->nSubSets));
	for(i=0; i<(set->nSubSets); i++)
		solution->subSets[i] = -1;
	solution->boolIncluded = malloc(sizeof(int) * set->nGlobalSetSize);
	memset(solution->boolIncluded, 0, set->nGlobalSetSize);

	bestSolution = malloc(sizeof(Solution));
	bestSolution->subSets = malloc(sizeof(int) * (set->nSubSets));
	for (i = 0; i < (set->nSubSets); i++)
		bestSolution->subSets[i] = -1;
	bestSolution->boolIncluded = malloc(sizeof(int) * set->nGlobalSetSize);
	memset(bestSolution->boolIncluded, 0, set->nGlobalSetSize);
	bestSolution->nSolutionSize = set->nSubSets-1;
}

void copySolutionToBest() {
	int i;
	bestSolution->nSolutionSize = solution->nSolutionSize;
	for(i=0;i<solution->nSolutionSize;i++)
		bestSolution->subSets[i] = solution->subSets[i];
}

int depth = 0;

void backTrack(Solution* solution) {
	int i;

	if(solution->nSolutionSize >= bestSolution->nSolutionSize)
		return;

	if(checkSolution(solution)) {
		if(solution->nSolutionSize < bestSolution->nSolutionSize)
			copySolutionToBest();
	}

	for(i=0; i<set->nSubSets; i++) {
		if(!containsSubSet(solution, i)) {
			//add subset to solution
			addSubSet(solution, i);
			//recure
			depth++;
			backTrack(solution);
			depth--;
			//remove solution
			removeSubSet(solution, i);
		}
	}
}

void backTrack2(Solution* solution) {
	int i;

	if(solution->nSolutionSize >= bestSolution->nSolutionSize)
		return;

	if(checkSolution(solution)) {
		if(solution->nSolutionSize < bestSolution->nSolutionSize)
			copySolutionToBest();
	}

	for(i=depth; i<set->nSubSets; i++) {
		//if(!containsSubSet(solution, i)) {
			//add subset to solution
			addSubSet(solution, i);
			//recure
			depth++;
			backTrack2(solution);
			depth--;
			//remove solution
			removeSubSet(solution, i);
		//}
	}
}

void backTrack3(Solution* solution, int last) {
	int i;

	if(solution->nSolutionSize >= bestSolution->nSolutionSize)
		return;

	if(checkSolution(solution)) {
		if(solution->nSolutionSize < bestSolution->nSolutionSize) {
			copySolutionToBest();
		}
		return;
	}

	for(i=last; i<set->nSubSets; i++) {
		//add subset to solution
		addSubSet(solution, i);
		//recure
		backTrack3(solution, i + 1);
		//remove solution
		removeSubSet(solution, i);
	}
}

void backTrack4(Solution* solution, int last, int sum) {
	int i;

	if(solution->nSolutionSize >= bestSolution->nSolutionSize)
		return;

	if(checkSolution(solution)) {
		if(solution->nSolutionSize < bestSolution->nSolutionSize) {
			copySolutionToBest();
			printf("New Solution Size : %u\n",bestSolution->nSolutionSize);
		}
		return;
	}

	for(i=last; i<set->nSubSets; i++) {
		if(sum+set->subSetsSizesSum[i][(bestSolution->nSolutionSize-1)-solution->nSolutionSize] < set->nGlobalSetSize)
			return;
		//add subset to solution
		addSubSet(solution, i);
		sum+=set->nSubSetSizes[i];
		//recure
		backTrack4(solution, i + 1, sum);
		sum-=set->nSubSetSizes[i];
		//remove solution
		removeSubSet(solution, i);
	}
}

void addSubSet(Solution* solution, int subSetIndex) {
	int i;
	solution->nSolutionSize++;
	solution->subSets[solution->nSolutionSize-1] = subSetIndex;

	for(i=0; i<set->nSubSetSizes[subSetIndex]; i++) {
		solution->boolIncluded[set->subsets[subSetIndex][i] - 1] += 1;
	}

	/*for(i=0; i<set->nSubSetSizes[subSetIndex]; i++) {
		printf("%u ",solution->boolIncluded[i]);
	}
	printf("\n");*/
}

void removeSubSet(Solution* solution, int subSetIndex) {
	int i;
	solution->subSets[solution->nSolutionSize-1] = -1;
	solution->nSolutionSize--;

	for(i=0; i<set->nSubSetSizes[subSetIndex]; i++) {
		solution->boolIncluded[set->subsets[subSetIndex][i] - 1] -= 1;
	}
}

bool containsSubSet(Solution* solution, int subSetIndex) {
	int i;

	for(i=0; i<solution->nSolutionSize; i++) {
		if(solution->subSets[i] == subSetIndex) {
			return true;
		}
	}

	return false;
}

bool checkSolution(Solution* solution) {
	int i;
	bool allDone = true;
	for(i=0; i<set->nGlobalSetSize; i++) {
		int boolInc = solution->boolIncluded[i];
		if(boolInc == 0) {
			return false;
		}
	}
	return allDone;
}

void printSolution(Solution* solution) {
	int i;
	printf("(%u)\n",solution->nSolutionSize);

	for(i=0; i<solution->nSolutionSize; i++) {
		printf("(%u) ", set->originalOrder[solution->subSets[i]]);
		printSubSet(solution->subSets[i]);
	}
}

void echoInit() {
	int i;
	printf("Universal Set 1-%u\n",set->nGlobalSetSize);
	printf("Number of subsets %u\n",set->nSubSets);
	for(i=0; i<set->nSubSets; i++) {
		printSubSet(i);
	}
}

void printSubSet(int nSubSetIndex) {
	int j;
	for (j = 0; j < set->nSubSetSizes[nSubSetIndex]; j++) {
		printf("%u ", set->subsets[nSubSetIndex][j]);
	}
	printf("\n");
}

bool GetALine(FILE *f, char buf[]) {
	/* Read a line of possibly commented input from the file *f.*/
	char *p;
	bool not_eof = false;
	while ( fgets(buf, 1024, f) != NULL) {
		p=strchr(buf, (int) '\n');
		if ( p != NULL )
			*p = '\0';

		if (*buf != '\0') {
			not_eof = true;
			break;
		}
	}
	return (not_eof);
}

void readGameFile(const char *s) {
	FILE *f;
	int lineno=0, i=0;
	int subSetSize=0;
	int nCurrSubSet=0;
	char * pch;
	char buf[1024];

	if(s==NULL)
		exit(1);

	f = fopen(s, "r");
	if(f == NULL) {
		printf("Could not open file");
		exit(1);
	}

	set = malloc(sizeof(Set));

	while(GetALine(f, inbuf)) {

		switch(lineno) {
		case 0:
			set->nGlobalSetSize = atoi(inbuf);
			break;
		case 1:
			set->nSubSets = atoi(inbuf);
			set->nSubSetSizes = (int *) malloc(sizeof(int) * set->nSubSets);
			set->originalOrder = (int *) malloc(sizeof(int) * set->nSubSets);
			for(i=0; i<set->nSubSets; i++) {
				set->originalOrder[i] = i+1;
			}
			set->subsets = (int**) malloc(sizeof(int*) * set->nSubSets);
			break;
		default:
			subSetSize = 0;
			strcpy(buf, inbuf);

			pch = strtok(inbuf, " ");
			while (pch != NULL) {
				subSetSize++;
				pch = strtok(NULL, " ");
			}

			set->nSubSetSizes[nCurrSubSet] = subSetSize;
			set->subsets[nCurrSubSet] = (int *) malloc(sizeof(int) * subSetSize);

			i = 0;
			pch = strtok(buf, " ");
			while (pch != NULL) {
				set->subsets[nCurrSubSet][i] = atoi(pch);
				i++;
				pch = strtok(NULL, " ");
			}

			nCurrSubSet++;
		}

		lineno++;
	}
}

void init_args(int argc, char **argv) {

	char str[1024], *opts[] = { "-f" }; /* valid options */
	int valopts[] = { 1 }; /* indicates options with values */
	/* 0=none, 1=required, -1=optional */
	int i, /* looper through all cmdline arguments */
	a, /* current valid argument-value position */
	op, /* position number of found option */
	nopts = sizeof(opts) / sizeof(char *);

	a = 1;
	for (i = 1; i <= nopts; i++) {
		if (a >= argc)
			break;

		/* figure out which option by its position 0-(nopts-1) */
		for (op = 0; op < nopts; op++) {
			if (strncmp(opts[op], argv[a], 2) == 0)
				break; /* found it, move on */
		}
		if (op == nopts) {
			fprintf(stderr, "Invalid option %s\n", argv[a]);
			printf("%s", "Pancake requires an input file. Use -f filename.txt");
			exit(-1);
		}

		*str = '\0';
		/* extract value part of option-value pair */
		if (valopts[op]) {
			if ('\0' != argv[a][2]) { /* no space betw opt-value */
				strcpy(str, (argv[a] + 2));
			} else if ('-' != *argv[a + 1]) { /* space betw opt-value */
				strcpy(str, argv[++a]);
			} else if (0 < valopts[op]) { /* required opt-val not found */
				fprintf(stderr, "Incomplete option %s\n", opts[op]);
				printf("%s", "Pancake requires an input file. Use -f filename.txt");
				exit(-1);
			} /* opt-val not required */
		}

		/* tell us what to do here                   */
		/* set indicators/variables based on results */
		switch (op) {
		case 0: /* -d */
			strcpy(_file, str);
			break; /* -f */
		default:
			fprintf(stderr, "Programmer: bad option in main:init_args:switch");
		}

		a++; /* move to next valid arg-value position */

	} /* end for(i) */
}
