/**
 * @brief
 * 
 * @param
 * @param
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_DIVIDING_LINE "-------------------\n"
#define GAME_HEADER "date                letters           score  pangrams  #words    4  5  6  7  8\n"
#define GAME_HEADER_DIVIDING_LINE "------------------------------------------------------------------------------\n"
#define LETTER_HEADER "letter   #center   avg center   #other   avg other\n"
#define LETTER_HEADER_DIVIDING_LINE "--------------------------------------------------\n"
#define WORD_HEADER "word   count    a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z\n"
#define WORD_HEADER_DIVIDING_LINE "--------------------------------------------------------------------------------------------\n"
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
    int wordNum;
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

typedef struct {
    char word[9];
    int centerLetter[ALPHABET_SIZE];
    int count;
} Word;

int statsOpt = 0;
int gameNum = 0;
int wordNumSum = 0;
const char *argGame = "--game_stats";
const char *argLetter = "--letter_stats";
const char *argWord = "--word_stats";
char gameDataLine[GAME_DATA_LINE_LEN];
Game gameStats[DAYS_OF_A_YEAR];
char gameStatsLine[GAME_STATS_LINE_LEN];
Letter letterStats[ALPHABET_SIZE];

/**
 * @brief 解析命令行参数
 */
void parseArgs(int argc, char **argv);

/**
 * @brief 处理业务
 * 
 * @param inFile 输入文件名
 * @param outFile 输出文件名
 */
void handler(const char *inFile, const char *outFile);

/**
 * @brief 实现游戏统计
 * 
 * 打开输入和输出文件
 * 获取游戏数据并进行统计，对统计结果进行排序，将游戏结果格式化输出
 * 关闭输入和输出文件
 * 
 * @param inFile 输入文件名
 * @param outFile 输出文件名
 */
void gameStatistics(const char *inFile, const char *outFile);

/**
 * @brief 实现字母统计
 * 
 * 打开输入和输出文件
 * 统计字母信息，计算平均分数，将结果按字母顺序格式化输出到文件中
 * 关闭输入和输出文件
 * 
 * @param inFile 输入文件名
 * @param outFile 输出文件名
 */
void letterStatistics(const char *inFile, const char *outFile);

/**
 * @brief 实现单词统计
 * 
 * 打开输入和输出文件
 * 获取单词信息，对单词出现次数和对应中心字母出现次数进行统计
 * 将统计结果排序并格式化输出到文件中
 * 关闭输入和输出文件
 * 
 * @param inFile 输入文件名
 * @param outFile 输出文件名
 */
void wordStatistics(const char *inFile, const char *outFile);

/**
 * @brief 获取游戏日期和字母列表信息
 * 
 * 若为游戏统计，则读取日期和字母列表，将日期按年月日拆分，存入对应游戏场次的结构体中
 * 若为字母统计或单词统计，则读取字母列表，存入对应游戏场次的结构体中
 * 
 * @param fpIn 输入文件
 * @param gameRef 当前游戏场次索引
 */
void getGameInfo(FILE *fpIn, int gameRef);

/**
 * @brief 从输入文件读取一行，并进行解析
 * 
 * @param fpIn 输入文件
 */
void dataLineHandler(FILE *fpIn);

/**
 * @brief 将格式化的字符串形式的字母列表拆分为字母字符
 * 
 * @param gameLettersFormatted 格式化的字符串形式的字母列表
 * @param gameLetters 用于存储提取出的字母的字符数组
 */
void extractGameLetters(char *gameLettersFormatted, char *gameLetters);

/**
 * @brief 判断单词是否为"pangram"情况，即单词是否使用了字母列表中所有字母
 * 
 * @param word 当前判定的单词
 * @param gameLetters 包含字母列表的字符数组
 * 
 * @return 若为pangram则返回1, 若不是pangram则返回0
 */
int isPangram(char *word, char *gameLetters);

/**
 * @brief 将每场游戏按总分数从大到小排序
 * 
 * @param gameStats 存储游戏信息的结构体
 * @param high 数组索引，初始值为数组第一个元素的索引
 * @param low 数组索引，初始值为数组最后一个元素的索引
 */
void sortGameData(Game *gameStats, int high, int low);

/**
 * @brief 将数组拆分，并分别排序
 * 
 * @param gameStats 存储游戏信息的结构体
 * @param high 数组索引，初始值为数组第一个元素的索引
 * @param low 数组索引，初始值为数组最后一个元素的索引
 * 
 * @return 拆分数组的元素的索引
 */
int sortSplit(Game *gameStats, int high, int low);

/**
 * @brief 将游戏统计信息格式化输出
 * 
 * 若某项为0则不显示
 * 
 * @param fpOut 输出文件
 */
void outputFormattedGameStats(FILE *fpOut);

/**
 * @brief 统计字母作为中心字母和其他字母出现在字母列表中的次数，并计算对应平均分数
 */
void letterStatsHandler();

/**
 * @brief 将字母统计信息格式化输出
 * 
 * 若某项为0则不显示，从未出现过的字母不显示
 * 
 * @param fpOut 输出文件
 */
void outputFormattedLetterStats(FILE *fpOut);

/**
 * @brief 统计所有游戏场次出现的单词的总数，包括重复出现的单词
 * 
 * @param fpIn 输入文件
 */
void wordCounter(FILE *fpIn);

/**
 * @brief 对存储单词统计信息的结构体进行初始化
 * 
 * @param wordStats 存储单词统计信息的结构体数组
 */
void wordStatsInit(Word *wordStats);


/**
 * @brief 遍历所有单词，统计单词出现次数，和对应的中心字母出现次数
 * 
 * @param fpIn 输入文件
 * @param wordStats 存储单词统计信息的结构体数组
 */
void wordStatsHandler(FILE *fpIn, Word *wordStats);

/**
 * @brief 将单词统计信息格式化输出，若某项为0则不输出
 * 
 * @param fpOut 输出文件
 * @param wordStats 存储单词统计信息的结构体数组
 */
void outputFormattedWordStats(FILE *fpOut, Word *wordStats);

/**
 * @brief 对单词出现次数进行排序时提供给qsort()的比较函数
 * 
 * @param a 指向用于比较的单词结构体的指针
 * @param b 指向用于比较的单词结构体的指针
 */
int wordCountCompare(Word *a, Word *b);

/**
 * @brief 对单词字符串进行排序时提供给qsort()的比较函数
 * 
 * @param a 指向用于比较的单词结构体的指针
 * @param b 指向用于比较的单词结构体的指针
 */
int wordStrCompare(Word *a, Word *b);


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

    dataLineHandler(fpIn);
    sortGameData(gameStats, 0, gameNum - 1);
    outputFormattedGameStats(fpOut);

    fclose(fpIn);
    fclose(fpOut);
}

void letterStatistics(const char *inFile, const char *outFile) {
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
    fputs(LETTER_HEADER_DIVIDING_LINE, fpOut);

    for(int i=0; i<ALPHABET_SIZE; i++)
        letterStats[i].letter = 'a' + i;
    
    dataLineHandler(fpIn);
    letterStatsHandler();
    outputFormattedLetterStats(fpOut);

    fclose(fpIn);
    fclose(fpOut);
}

void wordStatistics(const char *inFile, const char *outFile){
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
    fputs(WORD_HEADER, fpOut);
    fputs(WORD_HEADER_DIVIDING_LINE, fpOut);

    wordCounter(fpIn); //记录每场游戏的单词数,相加获得总单词数，创建单词结构体
    fseek(fpIn, 0, SEEK_SET);
    Word wordStats[wordNumSum];
    wordStatsInit(wordStats); //初始化单词统计结构体
    wordStatsHandler(fpIn, wordStats);

    qsort(wordStats, wordNumSum, sizeof(Word), wordStrCompare);
    qsort(wordStats, wordNumSum, sizeof(Word), wordCountCompare);
    outputFormattedWordStats(fpOut, wordStats);

    fclose(fpIn);
    fclose(fpOut);
}

void dataLineHandler(FILE *fpIn) {
    int gameRef = -1;
    char gameLetters[GAME_LETTERS_NUM+1];
    while(fgets(gameDataLine, sizeof(gameDataLine), fpIn)!=NULL) {
        //若读到分割线，则往下读取字母，跳过下一个分割线
        if(!strcmp(gameDataLine, DATA_DIVIDING_LINE)) {
            gameRef++;
            memset(gameLetters, '\0', sizeof(gameLetters));
            getGameInfo(fpIn ,gameRef);
            extractGameLetters(gameStats[gameRef].gameLettersFormatted, gameLetters);
            strcpy(gameStats[gameRef].extractedLettersList, gameLetters);
        } else {
            //读单词，并解析 
            ++wordNumSum;
            ++gameStats[gameRef].wordNum;
            gameDataLine[strlen(gameDataLine)-1] = '\0'; //去掉换行符            
            int pangram = 0;
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
                    gameStats[gameRef].scoreSum += (SCORE_SEVEN + (pangram ? SCORE_BONUS : 0));
                    break;
                case 8:
                    ++gameStats[gameRef].eightNum;
                    gameStats[gameRef].scoreSum += (SCORE_EIGHT + (pangram ? SCORE_BONUS : 0));
                    break;
                default:
                    break;
            }
              
        }
    }
    gameNum = gameRef + 1;
}

void wordCounter(FILE *fpIn) {
    while(fgets(gameDataLine, sizeof(gameDataLine), fpIn)!=NULL) {
        //若读到分割线，则往下读到下一个分割线
        if(!strcmp(gameDataLine, DATA_DIVIDING_LINE)) {
            ++gameNum;
            fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到日期行
            fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到字母列表
            fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到分割线
        } else
            ++wordNumSum;          
    }
}

void getGameInfo(FILE *fpIn, int gameRef) {
    fgets(gameDataLine, sizeof(gameDataLine), fpIn); //读到日期
    if(statsOpt == OPT_GAME){
        strcpy(gameStats[gameRef].gameDateMonth, strtok(gameDataLine, " ,\n"));
        strcpy(gameStats[gameRef].gameDateDay, strtok(NULL, " ,\n"));
        strcpy(gameStats[gameRef].gameDateYear, strtok(NULL, " ,\n"));
    }
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
                gameStats[i].wordNum, (!strcmp(fourNumStr, "0")) ? " " : fourNumStr, (!strcmp(fiveNumStr, "0")) ? " " : fiveNumStr,
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

void wordStatsInit(Word *wordStats) {
    for(int i = 0; i < wordNumSum; i++) {
        wordStats[i].count = 0;
        for(int j = 0; j < ALPHABET_SIZE; j++)
            wordStats[i].centerLetter[j] = 0;
    }
}

void wordStatsHandler(FILE *fpIn, Word *wordStats) {
    //从头遍历数据，读取格式化字母列表并提取字母，存入游戏统计结构体中。遍历单词
    //创建局部变量记录已遍历到的单词数量wordRef，即当前单词的索引
    //将当前单词与之前存入的单词对比，若有相同的，则之前的单词数量加一，将当前单词所在游戏的中心字母在之前单词的中心字母列表对应位置加一
    int gameRef = -1;
    int wordRef = -1;
    char gameLetters[GAME_LETTERS_NUM+1];
    while(fgets(gameDataLine, sizeof(gameDataLine), fpIn)!=NULL) {
        //若读到分割线，则往下读取字母，跳过下一个分割线
        if(!strcmp(gameDataLine, DATA_DIVIDING_LINE)) {
            gameRef++;
            memset(gameLetters, '\0', sizeof(gameLetters));
            getGameInfo(fpIn ,gameRef);
            extractGameLetters(gameStats[gameRef].gameLettersFormatted, gameLetters);
            strcpy(gameStats[gameRef].extractedLettersList, gameLetters);
        } else {
            //读到单词，解析 
            ++wordRef;
            gameDataLine[strlen(gameDataLine)-1] = '\0'; //去掉换行符
            strcpy(wordStats[wordRef].word, gameDataLine);
            if(wordRef > 0) {
                int flag = 0;
                for(int i = 0; i <= (wordRef - 1); i++) {
                    if(strcmp(wordStats[i].word, wordStats[wordRef].word) == 0) {
                        ++(wordStats[i].count);
                        ++(wordStats[i].centerLetter[gameStats[gameRef].extractedLettersList[0]-'a']); //对应的中心字母数量加一
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0) {
                    ++(wordStats[wordRef].count);
                    ++(wordStats[wordRef].centerLetter[gameStats[gameRef].extractedLettersList[0]-'a']);
                }
            }               
        }   
    }
}

void outputFormattedWordStats(FILE *fpOut, Word *wordStats) {
    for(int i = 0; i < wordNumSum; i++) {
        if(wordStats[i].count != 0) {
            fprintf(fpOut, "%-8s%4d  ", wordStats[i].word, wordStats[i].count);
            for(int j = 0; j < ALPHABET_SIZE; j++) {
                if(wordStats[i].centerLetter[j] != 0)
                    fprintf(fpOut, "%3d", wordStats[i].centerLetter[j]);
                else
                    fprintf(fpOut, "   ");
            }
            fprintf(fpOut, "\n");
        }
    }
}

int wordCountCompare(Word *a, Word *b){
    if(a->count > b->count)
        return -1;
    if(a->count < b->count)
        return 1;
    if(a -> count == b->count)
        return 0;
}

int wordStrCompare(Word *a, Word *b) {
    return strcmp(a->word, b->word);
}
