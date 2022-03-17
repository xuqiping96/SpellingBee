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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_DIVIDING_LINE "-------------------\n"
#define GAME_HEADER "date                letters           score  pangrams  #words    4  5  6  7  8\n"
#define GAME_HEADER_DIVIDING_LINE "------------------------------------------------------------------------------\n"
#define LETTER_HEADER "letter   #center   avg center   #other   avg other\n"
#define LETTER_HEADER_DIVING_LINE "--------------------------------------------------\n"
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
#define SCORE_FOUR 1
#define SCORE_FIVE 5
#define SCORE_SIX 6
#define SCORE_SEVEN 7
#define SCORE_EIGHT 8
#define SCORE_BONUS 7
#define GAME_LETTERS_NUM 7
#define ALPHABET_SIZE 26

typedef struct {
    char gameDateMonth[MAX_MONTH_LEN+1];
    char gameDateDay[MAX_DAY_LEN+1];
    char gameDateYear[MAX_YEAR_LEN+1];
    char gameLettersFormatted[LETTERS_LIST_LEN+1];
    char extractedLettersList[GAME_LETTERS_NUM+1];
    int scoreSum;
    int wordsNum;
    int pangramsNum;
    int fourNum;
    int fiveNum;
    int sixNum;
    int sevenNum;
    int eightNum;
} Game;

typedef struct {
    char letter;
    int centerNum;
    int otherNum;
    float centerScoreSum;
    float otherScoreSum;
    float centerScoreAvg;
    float otherScoreAvg;
} Letter;

int statsOpt = 0;
int gameNum = 0;
int wordNum = 0;
const char *argGame = "--game_stats";
const char *argLetter = "--letter_stats";
const char *argWord = "--word_stats";
char gameDataLine[GAME_DATA_LINE_LEN];
Game gameStats[DAYS_OF_A_YEAR];
//char gameStatsLine[GAME_STATS_LINE_LEN];
Letter letterStats[ALPHABET_SIZE];

void parseArgs(int argc, char **argv);

void handler(const char *inFile, const char *outFile);

void gameStatistics(const char *inFile, const char *outFile);

void letterStatistics(const char *inFile, const char *outFile);

void wordStatistics(const char *inFile, const char *outFile);

void getGameInfo(FILE *fpIn, int gameRef);

void dataLineHandler(FILE *fpIn);

void extractGameLetters(char *gameLettersFormatted, char *gameLetters);

int isPangram(char *word, char *gameLetters);

void sortGameData(Game *gameStats, int high, int low);

int sortSplit(Game *gameStats, int high, int low);

void outputFormattedGameStats(FILE *fpOut);

void letterStatsHandler();

void outputFormattedLetterStats(FILE *fpOut);


int main(int argc, char **argv) {
    parseArgs(argc, argv);

    handler(argv[1], argv[2]);

    return 0;
}

void parseArgs(int argc, char **argv) {
    if(argc < 4) {
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
   FILE *fpIn, *fpOut;

   fpIn = fopen(inFile, "r");
   fpOut = fopen(outFile, "w");
    if(fpIn == NULL) {
        fprintf(stderr, "Failed to open file %s.", inFile);
        exit(EXIT_FAILURE);
    }
    if(fpOut == NULL) {
        fclose(fpIn);
        fprintf(stderr, "Failed to open file %s.", outFile);
        exit(EXIT_FAILURE);
    }
        
    //输出统计表头
    fputs(GAME_HEADER, fpOut);
    fputs(GAME_HEADER_DIVIDING_LINE, fpOut);

    //读取单词并统计信息，存入结构体中
    dataLineHandler(fpIn);

    //根据总分数对结构体排序
    sortGameData(gameStats, 0, gameNum - 1);

    //将排序后的结构体中的数据格式化为字符串并输出到文件中
    outputFormattedGameStats(fpOut);

    fclose(fpIn);
    fclose(fpOut);

}

void letterStatistics(const char *inFile, const char *outFile) {
    //创建全局变量结构体数组存储26个字母和字母出现次数，出现字母当天的总分数（作为中心字母和其他字母的总分数），和平均分数
    //根据每一天的字母列表和总分数进行统计
    //每天的字母列表第一个是中心字母，对剩下的字母遍历，统计数量，将当天的总分加到对应出现的字母的结构体中
    //循环结束后，计算平均分数

    FILE *fpIn, *fpOut;

    fpIn = fopen(inFile, "r");
    fpOut = fopen(outFile, "w");
    if(fpIn == NULL) {
        fprintf(stderr, "Failed to open file %s.", inFile);
        exit(EXIT_FAILURE);
    }
    if(fpOut == NULL) {
        fclose(fpIn);
        fprintf(stderr, "Failed to open file %s.", outFile);
        exit(EXIT_FAILURE);
    }
        
    //输出统计表头
    fputs(LETTER_HEADER, fpOut);
    fputs(LETTER_HEADER_DIVING_LINE, fpOut);

    for(int i=0; i<ALPHABET_SIZE; i++)
        letterStats[i].letter = 'a' + i;
    
    dataLineHandler(fpIn);
    //统计字母次数和平均分数
    letterStatsHandler();
    //将字母统计结果格式化输出到文件中
    outputFormattedLetterStats(fpOut);

    fclose(fpIn);
    fclose(fpOut);
}

void wordStatistics(const char *inFile, const char *outFile){
    printf("To do");
    //遍历单词，记录单词总数，创建对应大小的结构体，结构体记录一个单词出现的次数，和每次对应的中心字母
    //再次遍历单词，每读一个单词，与之前的单词比较，若相同，则之前的单词数量加一，当前的单词数量为0，当前单词的中心字母记录到之前单词的结构体中
    //记录中心字母方法：结构体中记录中心字母的是一个字符数组，下标为0-25, 代表'a'-'z',默认初始化为0,每出现一次中心字母，对应数组元素加一
    //统计完单词数量和中心字母次数，对结构体进行排序。先对单词出现次数从大到小排序，相同的出现次数，对单词进行从小到大排序
    //输出时，循环遍历结构体，先输出单词和数量（数量为0的不输出），再遍历结构体中的字符数组，元素为0的输出三个空格，若不为0则输出空格加整数
}

void dataLineHandler(FILE *fpIn) {
    int gameRef = -1;
    char gameLetters[GAME_LETTERS_NUM+1];
    while(fgets(gameDataLine, sizeof(gameDataLine), fpIn)!=NULL) {
        //若读到分割线，则往下读日期和字母信息
        if(!strcmp(gameDataLine, DATA_DIVIDING_LINE)) {
            gameRef++;
            memset(gameLetters, '\0', sizeof(gameLetters));
            getGameInfo(fpIn ,gameRef);
            extractGameLetters(gameStats[gameRef].gameLettersFormatted, gameLetters);
            strcpy(gameStats[gameRef].extractedLettersList, gameLetters);
        } else {
            //读单词，并解析
            int pangram = 0;
            
            ++wordNum; //统计单词总数，包括重复出现的单词
            ++gameStats[gameRef].wordsNum;
            gameDataLine[strlen(gameDataLine)-1] = '\0'; //去掉换行符            
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
                    gameStats[gameRef].scoreSum += (SCORE_SEVEN + (isPangram(gameDataLine, gameLetters) ? SCORE_BONUS : 0));
                    break;
                case 8:
                    ++gameStats[gameRef].eightNum;
                    gameStats[gameRef].scoreSum += (SCORE_EIGHT + (isPangram(gameDataLine, gameLetters) ? SCORE_BONUS : 0));
                    break;
                default:
                    break;
            }   
        }
    }
    gameNum = gameRef + 1;
}

void getGameInfo(FILE *fpIn, int gameRef) {
    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到日期
    strcpy(gameStats[gameRef].gameDateMonth, strtok(gameDataLine, " ,\n"));
    strcpy(gameStats[gameRef].gameDateDay, strtok(NULL, " ,\n"));
    strcpy(gameStats[gameRef].gameDateYear, strtok(NULL, " ,\n"));

    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到字母列表
    gameDataLine[LETTERS_LIST_LEN] = '\0'; //去掉换行符
    strcpy(gameStats[gameRef].gameLettersFormatted, gameDataLine);

    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到第二个分割线，并跳过
}

void extractGameLetters(char *gameLettersFormatted, char *gameLetters) {
    char temp[LETTERS_LIST_LEN+1];
    strcpy(temp, gameLettersFormatted);
    char *token = strtok(temp, "[] ");
    while(token != NULL) {
        strcat(gameLetters, token);
        token = strtok(NULL, "[] ");
    }
}

int isPangram(char *word, char *gameLetters) {
    int flag = 0;
    
    for(int i = 0; i < strlen(gameLetters); i++) {
        for(int j = 0; j < strlen(word); j++) {
            if(gameLetters[i] == word[j]) {
                if (gameLetters[i+1] == '\0')
                    flag = 1;
                break;
            }
            if(word[j+1] == '\0')
                return 0;
        }
        if(flag)
            return 1;
    }
    
}

void sortGameData(Game *gameStats, int high, int low) {
    int middle;
    if(high >= low)
        return;
    middle = sortSplit(gameStats, high, low);
    sortGameData(gameStats, high, middle-1);
    sortGameData(gameStats, middle+1, low);
}

int sortSplit(Game *gameStats, int high, int low) {
    Game partElement = gameStats[high];

    for(;;) {
        while(high < low && gameStats[low].scoreSum <= partElement.scoreSum)
            low--;
        if(high >= low)
            break;
        gameStats[high++] = gameStats[low];

        while(high < low && gameStats[high].scoreSum >= partElement.scoreSum)
            high++;
        if(high >= low)
        break;
        gameStats[low--] = gameStats[high];
    }

    gameStats[low] = partElement;
    return low;
}

void outputFormattedGameStats(FILE *fpOut) {
    char pangramsNumStr[3];
    char fourNumStr[3];
    char fiveNumStr[3];
    char sixNumStr[3];
    char sevenNumStr[3];
    char eightNumStr[3];
    for(int i = 0; i < gameNum; i++) {  
        sprintf(pangramsNumStr, "%d", gameStats[i].pangramsNum);
        sprintf(fourNumStr, "%d", gameStats[i].fourNum);
        sprintf(fiveNumStr, "%d", gameStats[i].fiveNum);
        sprintf(sixNumStr, "%d", gameStats[i].sixNum);
        sprintf(sevenNumStr, "%d", gameStats[i].sevenNum);
        sprintf(eightNumStr, "%d", gameStats[i].eightNum);
        fprintf(fpOut, "%-10s%2s, %-6s%-17s%6d%10s%8d%5s%3s%3s%3s%3s\n",
                gameStats[i].gameDateMonth, gameStats[i].gameDateDay, gameStats[i].gameDateYear,
                gameStats[i].gameLettersFormatted, gameStats[i].scoreSum, (!strcmp(pangramsNumStr, "0")) ? " " : pangramsNumStr,
                gameStats[i].wordsNum, (!strcmp(fourNumStr, "0")) ? " " : fourNumStr, (!strcmp(fiveNumStr, "0")) ? " " : fiveNumStr,
                (!strcmp(sixNumStr, "0")) ? " " : sixNumStr, (!strcmp(sevenNumStr, "0")) ? " " : sevenNumStr,
                (!strcmp(eightNumStr, "0")) ? " " : eightNumStr);
    }
}

void letterStatsHandler() {
    int centerLetterRef;
    int otherLetterRef;
    //统计字母出现次数和总分数
    for(int i = 0; i < gameNum; i++) {
        centerLetterRef = gameStats[i].extractedLettersList[0] - 'a';
        ++letterStats[centerLetterRef].centerNum;
        letterStats[centerLetterRef].centerScoreSum += gameStats[i].scoreSum;
        for(int j=1; j<GAME_LETTERS_NUM; j++) {
            otherLetterRef = gameStats[i].extractedLettersList[j] - 'a';
            ++letterStats[otherLetterRef].otherNum;
            letterStats[otherLetterRef].otherScoreSum += gameStats[i].scoreSum;
        }
    }
    //计算平均分数
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        if(letterStats[i].centerNum != 0)
            letterStats[i].centerScoreAvg = letterStats[i].centerScoreSum / letterStats[i].centerNum;
        if(letterStats[i].otherNum != 0)
            letterStats[i].otherScoreAvg = letterStats[i].otherScoreSum / letterStats[i].otherNum;
    }
}

void outputFormattedLetterStats(FILE *fpOut) {
    char centerNumStr[4];
    char centerScoreAvgStr[6];
    char otherNumStr[4];
    char otherScoreAvgStr[6];
    for(int i =0; i < ALPHABET_SIZE; i++) {
        if(letterStats[i].centerNum || letterStats[i].otherNum) {
            sprintf(centerNumStr, "%d", letterStats[i].centerNum);
            sprintf(centerScoreAvgStr, "%.1f", letterStats[i].centerScoreAvg);
            sprintf(otherNumStr, "%d", letterStats[i].otherNum);
            sprintf(otherScoreAvgStr, "%.1f", letterStats[i].otherScoreAvg);
            fprintf(fpOut, "%-9c%7s%13s%9s%12s\n",
                    letterStats[i].letter,
                    !strcmp(centerNumStr, "0") ? " " : centerNumStr,
                    !strcmp(centerScoreAvgStr, "0.0") ? " " : centerScoreAvgStr,
                    !strcmp(otherNumStr, "0") ? " " : otherNumStr,
                    !strcmp(otherScoreAvgStr, "0.0") ? " " : otherScoreAvgStr);
        }
    }
}
