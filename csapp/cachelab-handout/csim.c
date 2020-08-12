#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

typedef unsigned long int uint64_t;

typedef struct {
    int valid;
    int lru;
    uint64_t tag;
}cacheLine;

typedef cacheLine* cacheSet;
typedef cacheSet* Cache;

int verbose = 0; //verbose flag 
int s;  //number of set index bits 
int E;  //number of lines per set
int b;  //number of block bits
FILE* fp = NULL;
Cache cache;

int hits = 0;
int misses = 0;
int evictions = 0;

void parseArgument(int argc, char* argv[]);
int visitCache(uint64_t address);
int simulate();
void printUsage(char* argv[]) ;
int main(int argc, char* argv[])
{
    parseArgument(argc, argv);
    simulate();
    printSummary(hits, misses, evictions);
    return 0;
}

// 更具运行参数 设置程序的 状况。
// optarg——当前选项参数字串（如果有）
void parseArgument(int argc, char* argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch(opt)
        {
            case 'h':
                printUsage(argv);
                exit(1);
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);  // 索引数
                break;
            case 'E':
                E = atoi(optarg);  // 行数
                break;
            case 'b':
                b = atoi(optarg);  // 块偏移
                break;
            case 't':
                fp = fopen(optarg, "r");
                break;
            default:
                printUsage(argv);
                exit(1);
        }
    }
}

void printUsage(char* argv[]) {
	printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", *argv);
	puts("Options:");
	puts("  -h         Print this help message.");
	puts("  -v         Optional verbose flag.");
	puts("  -s <num>   Number of set index bits.");
	puts("  -E <num>   Number of lines per set.");
	puts("  -b <num>   Number of block offset bits.");
	puts("  -t <file>  Trace file.");
	puts("\nExamples:");
	printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", *argv);
	printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", *argv);
	exit(0);
}

int simulate()
{
    // S = 2 ^ s
    int S = pow(2, s);
    cache = (Cache)malloc(sizeof(cacheSet) * S); // 创建缓存的结构
    if (cache == NULL) return -1;
    
    for (int i = 0; i < S; i++)
    {
        cache[i] = (cacheSet)calloc(E, sizeof(cacheLine));
        if (cache[i] == NULL) return -1;
    }

    char buf[20];
    char operation;
    uint64_t address;
    int size;

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        int ret;

        if (buf[0] == 'I') //ignore instruction cache accesses
        {
            continue;
        }
        else
        {
            sscanf(buf, " %c %lx,%d", &operation, &address, &size);
            // L：Load，数据载入，可能发生1次miss
            // S：Store，可能发生1次miss
            // M：store后再load，两次访存。1 miss & 1 hit + 可能eviction
            switch (operation)
            {
                case 'S':
                    ret = visitCache(address);
                    break;
                case 'L':
                    ret = visitCache(address);
                    break;
                case 'M':
                    ret = visitCache(address);
                    hits++;
                    break;
            }

            if (verbose)
            {
                switch(ret)
                {
                    case 0:
                        printf("%c %lx,%d hit\n", operation, address, size);
                        break;
                    case 1:
                        printf("%c %lx,%d miss\n", operation, address, size);
                        break;
                    case 2:
                        printf("%c %lx,%d miss eviction\n", operation, address, size);
                        break;
                }
            }
        }
    }
    
    for (int i = 0; i < S; i++)
        free(cache[i]);
    free(cache);
    fclose(fp);
    return 0;
}

/*return value
      0             cache hit
      1             cache miss
      2             cache miss, eviction
*/
// 访问 cache 判断数据 是否被命中(hit)  不在就是 (miss)。  将这个 Miss 的数据在 主存总找到对应的块，然后放到 cache 空闲行
// 主存 > cache 所以 cache 内容会发生淘汰
int visitCache(uint64_t address)
{
    // 得到 tag 和对应的 偏移
    uint64_t tag = address >> (s + b);      // tag = addr >>b>>s
    unsigned int setIndex = address >> b & ((1 << s) - 1);

    int evict = 0;
    int empty = -1;
    // 对应的 cacheset
    cacheSet cacheset = cache[setIndex];
    // 循环去找到对应的 tag
    for (int i = 0; i < E; i++)
    {
        if (cacheset[i].valid)
        {
            if (cacheset[i].tag == tag)
            {
                hits++;
                cacheset[i].lru = 1;
                return 0;
            }
            // 使用数 ++
            cacheset[i].lru++;
            if (cacheset[evict].lru <= cacheset[i].lru) 
            {
                evict = i;
            }
        }
        else
        {
            empty = i;
        }
    }

    //cache miss
    misses++;
    // 如果是 -1 说明有移除。
    if (empty != -1)
    {
        cacheset[empty].valid = 1;
        cacheset[empty].tag = tag;
        cacheset[empty].lru = 1;
        // 1             cache miss
        return 1;
    }
    else
    {
        cacheset[evict].tag = tag;
        cacheset[evict].lru = 1;
        evictions++;
        // 2             cache miss, eviction
        return 2;
    }
}