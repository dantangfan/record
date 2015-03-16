#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>
#include<math.h>
#include<time.h>


#ifndef FILE1_H
#define FILE1_H

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++�궨��++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define PAGE_COUNT 128      //һ����128��ҳ 
//#define PAGE_COUNT   8    //һ����8��ҳ 
#define PAGE_SIZE 1024*8    //ÿ��ҳ���Сbit 
#define INT sizeof(int)     //һ�����εĴ�С 
#define CHAR sizeof(char)   //һ�� �ַ��Ĵ�С 
#define MOST 1              //��λ��չ 
#define LEAST 0             //��λ��չ 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++�ṹ����++++++++++++++++++++++++++*/ 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*�ڴ�ҳ*/
typedef struct
{
    void *pointer;          //ҳָ���ڴ�ռ� 
    int current_size;       //����ҳ�浱ǰʣ��Ĵ�С 
    int bucket_id;          //ҳ���Ӧ��Ͱ�ţ�-1����û��Ͱ 
    bool used;              //��¼ҳ���Ƿ��ѱ�ʹ�� 
    bool refered;           //��¼ҳ���Ƿ񱻷��ʹ�����ʱ���㷨��ʹ�� 
    bool locked;            //��¼ҳ���Ƿ��ѱ����� ����ʱ���㷨�з�ֹ��ʱ���û� 
}Page; 


/*Ͱ*/
typedef struct
{
    int bucket_id;          //Ͱ�� 
    int page_id;            //Ͱ��Ӧ���ڴ�ҳ�棬-1��ʾû�����ڴ��� 
    char *head;             //ָ������ָ��Ͱ�ڴ����еĿ�ʼ 
    int *tail;              //ָ������ָ��Ͱ�ڴ����е���� 
    int position;           //���ڱ�ʾ�ڴ����е�λ�ã��ļ��У� 
}Bucket;


/*Ԫ��*/
typedef struct
{
    char info[512];         //��¼һ��Ԫ���ȫ����Ϣ 
    int length;             //����Ԫ��ĳ��� 
}Info;


/*Ԥ��һ��ҳ���������һ�������û�*/
Page page[PAGE_COUNT];      //ҳ������ 
Bucket bucket[PAGE_COUNT-2];//Ͱ������ 

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++��������+++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
int bucket_number;          //��ǰͰ������ 

double start;               //��¼��ѯ��ʼʱ�� 
double end;                 //��¼��ѯ����ʱ�� 
int global_depth;           //ȫ����� 
int exten_model;            //��չ��ʽ����λ���ߵ�λ 
char* read_buffer;          //��¼�����ļ���ָ�� 
int io;                     //��¼I/O���� 
int *index;                 //����ָ�� 
int index_size;             //��ǰ������С��ռ��ҳ������� 
int index_offset;           //�ڴ����е�������������������ļ���ƫ��ֵ 
int Clock;                  //ʱ���㷨ָ�� 
/*����������ڼ�¼�ļ�·��������*/
char INDEX[100];
char BUCKET[100];
char IN[100];
char OUT[100];
char TABLE[100];
char TEMP[100];


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++��������++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
void globallize();                     //ȫ�ֻ� 
void read_inof();                      //�Ӵ����ж�ȡ�����ļ� 
void make_index(Info);                 //����ȡ������Ԫ�������� 
void check_put(Info,int);              //��Ԫ�����Ͱ֮ǰ�������ж� 
void put_into_bucket(Info,int);        //��Ԫ�����Ͱ�� 
void del_all_page();                   //�ͷ��ڴ� 
void search();                         //��ѯ 
void match_and_output(int , int );     //�������������� 
int get_blank_page(int &);             //����һ��ҳ�棬����ҳ��� 
void reset(int);                       //����һ��ҳ�棬���س�ʼ״̬
void quick_sort(int *,Info *,int);     //�Ѳ�ѯ�õ��Ľ������ 


void initpage(int);                    //��ʼ��һ��ҳ�� 
void init_bucket(int);                 //��ʼ��һ��Ͱ 
void bucket_load(int,int,int);         //��Ͱ���ص�ָ��ҳ�� 
int get_indexid_from_bucket(int);      //��Ͱ�����Ӧ������ֵ 
int get_size_from_bucket(int);         //��Ͱ���ʣ��ռ� 
int get_bucketid(Info);                //����Ԫ��Ӧ�÷����Ͱ�� 
int get_search_key(Info);              //��Ԫ���м��������� 
int get_sort_key(Info);                //��Ԫ���м��������/////////// 
void quick_sort_help(int*,Info*,int,int);//���ŵݹ麯�� 



/*----------------------��λ��չ--------------------------*/

void exten_index_in_most();             //������Ӹ�λ��չʱ���ڵ���ҳ������չ 
void exten_index_page_most();           //������Ӹ�λ��չʱ����ҳ��֮����չ 
void bucket_split_most(int);            //��λ��չʱ��Ͱ���� 
int get_indexid_most(int , int );       //���Ԫ���Ӧ������ֵ 
int binary_to_digit(int , int );        //�����ƺ�ʮ���Ƶ�ת�� 
/*---------------------��λ��չ-----------------------------*/
void exten_index_in_least();
void exten_index_page_least();
void bucket_split_least(int);
int get_indexid_least(int , int );

#endif
