#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>
#include<math.h>
#include<time.h>


#ifndef FILE1_H
#define FILE1_H

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++宏定义++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define PAGE_COUNT 128      //一共有128个页 
//#define PAGE_COUNT   8    //一共有8个页 
#define PAGE_SIZE 1024*8    //每个页面大小bit 
#define INT sizeof(int)     //一个整形的大小 
#define CHAR sizeof(char)   //一个 字符的大小 
#define MOST 1              //高位扩展 
#define LEAST 0             //低位扩展 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++结构定义++++++++++++++++++++++++++*/ 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*内存页*/
typedef struct
{
    void *pointer;          //页指向内存空间 
    int current_size;       //单个页面当前剩余的大小 
    int bucket_id;          //页面对应的桶号，-1代表没有桶 
    bool used;              //记录页面是否已被使用 
    bool refered;           //记录页面是否被访问过，在时钟算法中使用 
    bool locked;            //记录页面是否已被锁定 ，在时钟算法中防止被时钟置换 
}Page; 


/*桶*/
typedef struct
{
    int bucket_id;          //桶号 
    int page_id;            //桶对应的内存页面，-1表示没有在内存中 
    char *head;             //指针用于指向桶在磁盘中的开始 
    int *tail;              //指针用于指向桶在磁盘中的最后 
    int position;           //用于表示在磁盘中的位置（文件中） 
}Bucket;


/*元组*/
typedef struct
{
    char info[512];         //记录一个元组的全部信息 
    int length;             //单个元组的长度 
}Info;


/*预留一个页存放索引，一个留作置换*/
Page page[PAGE_COUNT];      //页面数量 
Bucket bucket[PAGE_COUNT-2];//桶的数量 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++变量定义+++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
int bucket_number;          //当前桶的数量 

double start;               //记录查询开始时间 
double end;                 //记录查询结束时间 
int global_depth;           //全局深度 
int exten_model;            //扩展方式，高位或者低位 
char* read_buffer;          //记录数据文件的指针 
int io;                     //记录I/O次数 
int *index;                 //索引指针 
int index_size;             //当前索引大小（占据页面个数） 
int index_offset;           //在磁盘中的索引相对于整个索引文件的偏移值 
int Clock;                  //时钟算法指针 
/*下面变量用于记录文件路径和名称*/
char INDEX[100];
char BUCKET[100];
char IN[100];
char OUT[100];
char TABLE[100];
char TEMP[100];


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++函数定义++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
void globallize();                     //全局化 
void read_inof();                      //从磁盘中读取数据文件 
void make_index(Info);                 //给读取出来的元组做索引 
void check_put(Info,int);              //把元组放入桶之前的条件判断 
void put_into_bucket(Info,int);        //把元组放入桶中 
void del_all_page();                   //释放内存 
void search();                         //查询 
void match_and_output(int , int );     //输出结果到磁盘中 
int get_blank_page(int &);             //申请一个页面，返回页面号 
void reset(int);                       //重置一个页面，返回初始状态
void quick_sort(int *,Info *,int);     //把查询得到的结果排序 


void initpage(int);                    //初始化一个页面 
void init_bucket(int);                 //初始化一个桶 
void bucket_load(int,int,int);         //把桶加载到指定页面 
int get_indexid_from_bucket(int);      //从桶获得相应的索引值 
int get_size_from_bucket(int);         //从桶获得剩余空间 
int get_bucketid(Info);                //计算元组应该放入的桶号 
int get_search_key(Info);              //从元组中计算搜索键 
int get_sort_key(Info);                //从元组中计算排序键/////////// 
void quick_sort_help(int*,Info*,int,int);//快排递归函数 



/*----------------------高位扩展--------------------------*/

void exten_index_in_most();             //索引表从高位扩展时，在单个页面内扩展 
void exten_index_page_most();           //索引表从高位扩展时，在页面之间扩展 
void bucket_split_most(int);            //高位扩展时的桶分裂 
int get_indexid_most(int , int );       //获得元组对应的索引值 
int binary_to_digit(int , int );        //二进制和十进制的转换 
/*---------------------低位扩展-----------------------------*/
void exten_index_in_least();
void exten_index_page_least();
void bucket_split_least(int);
int get_indexid_least(int , int );

#endif
