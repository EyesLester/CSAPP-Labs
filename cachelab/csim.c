#include "cachelab.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

extern char* optarg;
extern int optind;
extern int opterr;
extern int optopt;
int getopt(int argc, char * const argv[], const char *optstring);

int s = 0;
int S = 0;
int E = 0;
int b = 0;
int t = 0;
typedef struct {
    bool validBit;
    uint64_t tagBits;
    int hitTime;
}CacheLine;
void cacheSimulator(CacheLine **cache, FILE * fp, bool verbose);

int main(int argc, char **argv)
{
    char helpinfo[] =   "Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
                        "Options:\n"
                        "  -h         Print this help message.\n"
                        "  -v         Optional verbose flag.\n"
                        "  -s <num>   Number of set index bits.\n"
                        "  -E <num>   Number of lines per set.\n"
                        "  -b <num>   Number of block offset bits.\n"
                        "  -t <file>  Trace file.\n";
    char tracefile[100];
    bool verbose = false;
    CacheLine **cache; // 行是set，列是line
    FILE *fp = NULL;
    int optc;
    /*----------处理命令行参数----------*/
    while ((optc = getopt(argc, argv, "hvs:E:b:t:")) != -1) {   
        switch (optc) { 
            case 'h':
                puts(helpinfo);
                exit(0);
                break;
            case 'v':
                verbose = true;
                break;
            case 's':
                sscanf(optarg, "%d", &s);
                S = 1 << s;
                break;
            case 'E':
                sscanf(optarg, "%d", &E);    
                break;
            case 'b':
                sscanf(optarg, "%d", &b);
                break;
            case 't':
                strncpy(tracefile, optarg, 100);
                break;
            default:
                break;  
        }  
    }
    t = 64 - b - s;
    /*----------为cache分配内存----------*/
    cache = (CacheLine **)malloc(sizeof(CacheLine *) * S);
    for (int i = 0; i < S; i++) {
        cache[i] = (CacheLine *)malloc(sizeof(CacheLine) * E);
        for (int j = 0; j < E; j++) {
            cache[i][j].validBit = false;
        }
    }

    /*----------打开文件----------*/
    fp = fopen(tracefile, "r");

    /*----------分析文件内容----------*/
    cacheSimulator(cache, fp, verbose);

    /*----------关闭文件----------*/
    fclose(fp);

    /*----------释放cache的内存----------*/   
    for (int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);

    return 0;
}

void cacheSimulator(CacheLine **cache, FILE *fp, bool verbose){
    int num_hit = 0;
    int num_miss = 0;
    int num_eviction = 0;
    char str[100];
    char op;
    uint64_t address;
    int size;
    uint64_t tag;
    uint64_t setID;
    int time = 0;
    bool flag = false;
    CacheLine *lineLRU = NULL;
    while (fgets(str, 100, fp) != NULL) {
        sscanf(str, " %c %"PRIx64",%d", &op, &address, &size);
        tag = address >> (b + s);
        setID = (address << t) >> (t + b);
        flag = false;
        // printf("%"PRIu64" %"PRIu64"\n", tag, setID);
        switch (op) {
            case 'L': // Load
            case 'S': // Store
                for (int i = 0; i < E; i++){
                    CacheLine *line = &cache[setID][i];
                    if (line->validBit && line->tagBits == tag) {
                        if(verbose) printf("%c %"PRIx64",%d hit\n", op, address, size);
                        num_hit++;
                        line->hitTime = time;
                        flag = true;
                        break;
                    }
                }
                if (flag) break;
                lineLRU = NULL;
                for (int i = 0; i < E; i++){
                    CacheLine *line = &cache[setID][i];
                    if (!line->validBit) {
                        if(verbose) printf("%c %"PRIx64",%d miss\n", op, address, size);
                        num_miss++;
                        line->validBit = true;
                        line->tagBits = tag;
                        line->hitTime = time;
                        flag = true;
                        break;
                    } 
                    if (lineLRU) {
                        if (line->hitTime < lineLRU->hitTime) {
                            lineLRU = line;
                        }
                    } else {
                        lineLRU = line;
                    }
                }
                if (flag) break;
                if(verbose) printf("%c %"PRIx64",%d miss eviction\n", op, address, size);
                num_miss++;
                num_eviction++;
                lineLRU->tagBits = tag;
                lineLRU->hitTime = time;
                break;
            case 'M':
                for (int i = 0; i < E; i++){
                    CacheLine *line = &cache[setID][i];
                    if (line->validBit && line->tagBits == tag) {
                        if(verbose) printf("%c %"PRIx64",%d hit hit\n", op, address, size);
                        num_hit += 2;
                        line->hitTime = time;
                        flag = true;
                        break;
                    } 
                }
                if (flag) break;
                lineLRU = NULL;
                for (int i = 0; i < E; i++){
                    CacheLine *line = &cache[setID][i];
                    if (!line->validBit) {
                        if(verbose) printf("%c %"PRIx64",%d miss hit\n", op, address, size);
                        num_miss++;
                        num_hit++;
                        line->validBit = true;
                        line->tagBits = tag;
                        line->hitTime = time;
                        flag = true;
                        break;
                    } 
                    if (lineLRU) {
                        if (line->hitTime < lineLRU->hitTime){
                            lineLRU = line;
                        }
                    } else {
                        lineLRU = line;
                    }
                }
                if (flag) break;
                if(verbose) printf("%c %"PRIx64",%d miss eviction hit\n", op, address, size);
                num_miss++;
                num_eviction++;
                num_hit++;
                lineLRU->tagBits = tag;
                lineLRU->hitTime = time;
                break;
            default:
                break;
        }
        time++;
    }
    printSummary(num_hit, num_miss, num_eviction);
}