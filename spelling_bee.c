/*
    解析参数
    1.游戏统计部分
        声明有366个元素的结构体数组，每一场比赛定义一个结构体
        定义字符串常量：分割线，游戏统计表头
        从文件读取字符串
        解析字符串
    2.字母统计部分
    3.单词统计部分

*/


//头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//定义常量
#define DATA_DIVIDING_LINE "-------------------\n"
#define GAME_HEADER "date                letters           score  pangrams  #words    4  5  6  7  8\n"
#define GAME_HEADER_DIVIDING_LINE "------------------------------------------------------------------------------\n"
#define OPT_GAME 1
#define OPT_LETTER 2
#define OPT_WORD 3
#define GAME_DATA_LINE_LEN 21
#define GAME_STATS_LINE_LEN 80
#define DAYS_OF_A_YEAR 366
#define MAX_MONTH_LEN 9
#define MAX_DAY_LEN 2
#define MAX_YEAR_LEN 4
#define LETTERS_LIST_LEN 17
#define SCORE_FOUR 4
#define SCORE_FIVE 5
#define SCORE_SIX 6
#define SCORE_SEVEN 7
#define SCORE_EIGHT 8
#define SCORE_BONUS 7
#define GAME_LETTERS_NUM 7

typedef struct {
    char gameDateMonth[MAX_MONTH_LEN+1];
    char gameDateDay[MAX_DAY_LEN+1];
    char gameDateYear[MAX_YEAR_LEN+1];
    char gameLettersFormatted[LETTERS_LIST_LEN+1];
    int scoreSum;
    int wordsNum;
    int pangramsNum;
    int fourNum;
    int fiveNum;
    int sixNum;
    int sevenNum;
    int eightNum;
} Game;

int statsOpt = 0;
const char *argGame = "--game_stats";
const char *argLetter = "--letter_stats";
const char *argWord = "--word_stats";
char gameDataLine[GAME_DATA_LINE_LEN];
Game gameStats[DAYS_OF_A_YEAR];
char gameStatsLine[GAME_STATS_LINE_LEN];


//函数原型声明
void parseArgs(int argc, char **argv);

void handler(const char *inFile, const char *outFile);

void gameStatistics(const char *inFile, const char *outFile);

void letterStatistics(const char *inFile, const char *outFile);

void wordStatistics(const char *inFile, const char *outFile);

void getGameInfo(FILE *fpIn, int gameRef);

void dataLineHandler(FILE *fpIn);

char *extractGameLetters(char *gameLettersFormatted);

int isPangram(char *word, char *gameLetters);


//主函数入口
int main(int argc, char **argv) {
    parseArgs(argc, argv);

    handler(argv[1], argv[2]);

    return 0;
}



//函数定义
void parseArgs(int argc, char **argv) {
    if(argc<4) {
        fprintf(stderr, "Usage: %s InputFile OutputFile --game_stats|--letter_stats|--word_stats", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(!strcmp(argv[3], argGame))
        statsOpt = OPT_GAME;
    else if(!strcmp(argv[3], argLetter))
        statsOpt = OPT_LETTER;
    else if(!strcmp(argv[3], argWord))
        statsOpt = OPT_WORD;
    else {
        fprintf(stderr, "Only %s, %s and %s are allowed", argGame, argLetter, argWord);
        exit(EXIT_FAILURE);
    }

}

void handler(const char *inFile, const char *outFile) {
    switch(statsOpt) {
        case OPT_GAME:
            gameStatistics(inFile, outFile);
            break;
        case OPT_LETTER:
            letterStatistics(inFile, outFile);
            break;
        case OPT_WORD:
            wordStatistics(inFile, outFile);
            break;
        default:
            break;
    }
}

void gameStatistics(const char *inFile, const char *outFile) {
    /*
        1.打开文件
        2.读取数据：遇到分割线则读取比赛信息，跳过下一个分割线，开始遍历单词，计算单词长度，统计单词个数，统计全字母单词，
        计算总分，各项结果保存进结构体中。
        3.将每一场比赛的统计信息格式化输出到目标文件中
    */

   FILE *fpIn, *fpOut;
   
    if((fpIn=fopen(inFile, "r"))==NULL) {
        fprintf(stderr, "Failed to open file %s.", inFile);
        exit(EXIT_FAILURE);
    }

    if((fpOut=fopen(outFile, "w"))==NULL) {
        fclose(fpIn);
        fprintf(stderr, "Failed to open file %s.", outFile);
        exit(EXIT_FAILURE);
    }
        
    //输出统计表头
    fputs(GAME_HEADER, fpOut);
    fputs(GAME_HEADER_DIVIDING_LINE, fpOut);

    //读取单词并统计信息，存入结构体中
    dataLineHandler(fpIn);

    //将结构体中的数据格式化为字符串并输出到文件中



    fclose(fpIn);
    fclose(fpOut);

}

void letterStatistics(const char *inFile, const char *outFile) {
    printf("To do");
}

void wordStatistics(const char *inFile, const char *outFile){
    printf("To do");
}

void dataLineHandler(FILE *fpIn) {
    int gameRef = -1;
    while(fgets(gameDataLine, sizeof(gameDataLine), fpIn)!=NULL) {
        //若读到分割线，则往下读日期和字母信息
        if(!strcmp(gameDataLine, DATA_DIVIDING_LINE)) {
            gameRef++;
            getGameInfo(fpIn ,gameRef);
        } else {
            //读单词，并解析
            int pangram = 0;
            ++gameStats[gameRef].wordsNum;
            gameDataLine[strlen(gameDataLine)-1] = '\0'; //去掉换行符
            char gameLetters[] = extractGameLetters(gameStats[gameRef].gameLettersFormatted);
            pangram = isPangram(gameDataLine, gameLetters);
            if(pangram)
                ++gameStats[gameRef].pangramsNum;
            switch(strlen(gameDataLine)) {
                case 4:
                    ++gameStats[gameRef].fourNum;
                    gameStats[gameRef].scoreSum += SCORE_FOUR;
                    break;
                case 5:
                    ++gameStats[gameRef].fiveNum;
                    gameStats[gameRef].scoreSum += SCORE_FIVE;
                    break;
                case 6:
                    ++gameStats[gameRef].sixNum;
                    gameStats[gameRef].scoreSum += SCORE_SIX;
                    break;
                case 7:
                    ++gameStats[gameRef].sevenNum;
                    gameStats[gameRef].scoreSum += (SCORE_SEVEN + pangram ? SCORE_BONUS : 0);
                    break;
                case 8:
                    ++gameStats[gameRef].eightNum;
                    gameStats[gameRef].scoreSum += (SCORE_EIGHT + pangram ? SCORE_BONUS : 0);
                    break;
                default:
                    break;
            }   
        }
    }
}

void getGameInfo(FILE *fpIn, int gameRef) {
    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到日期
    strcpy(gameStats[gameRef].gameDateMonth, strtok(gameDataLine, " ,\n"));
    strcpy(gameStats[gameRef].gameDateDay, strtok(NULL, " ,\n"));
    strcpy(gameStats[gameRef].gameDateYear, strtok(NULL, " ,\n"));
    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到字母列表
    strcpy(gameStats[gameRef].gameLettersFormatted, strtok(gameDataLine, "\n"));
    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到第二个分割线，并跳过
}

char *extractGameLetters(char *gameLettersFormatted) {
    char gameLetters[GAME_LETTERS_NUM+1] = "\0";
    char *token = strtok(gameLettersFormatted, "[] ");

    while(token!=NULL) {
        strcat(gameLetters, token);
        token = strtok(NULL, "[] ");
    }

    return gameLetters;
}

int isPangram(char *word, char *gameLetters) {
    int i = 0, j = 0;
    for(; i<strlen(gameLetters); i++) {
        for(; j<strlen(word); j++) {
            if(gameLetters[i]==word[j]) {
                j = 0;
                break;
            }
            if(gameLetters[j+1]=='\0')
                return 0;
        }

        return 1;
    }
}