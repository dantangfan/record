#include"main.h"



/*
初始化页面：
*/
void initpage(int pageid)
{
    page[pageid].bucket_id = -1;
    page[pageid].current_size = PAGE_SIZE;
    page[pageid].pointer = malloc(PAGE_SIZE);
    page[pageid].refered = false;
    page[pageid].used = false;
    page[pageid].locked = false;
}


/*
初始化一个桶：
*/
void init_bucket(int position)
{
    bucket[position].bucket_id = -1;
    bucket[position].head = NULL;
    bucket[position].tail = NULL;
    bucket[position].page_id = -1;
    bucket[position].position = position;
}


/*
桶加载函数：
    把指定的桶和指定的页面相关联
参数：指定桶的位置，桶号，页面号
*/
void bucket_load(int position,int bid,int pid)
{
    bucket[position].bucket_id = bid;
    bucket[position].page_id = pid;
    bucket[position].head = (char*)page[pid].pointer;
    bucket[position].position = position;
    bucket[position].tail = (int*)page[pid].pointer+PAGE_SIZE/INT-1;
    
    /*三个参数记录桶深度，元组个数，空闲位置在内存中的偏移值*/
    *(bucket[position].tail) = 1;
    *(bucket[position].tail-1) = 0;
    *(bucket[position].tail-2) = 0;
    
    page[pid].current_size = PAGE_SIZE-3*INT;
    page[pid].bucket_id = bid;
    page[pid].used = true;
}


/*
根据桶获得索引值；
    从指定桶的搜索键获得该桶对应的索引值
    并返回索引值
参数：桶位置*/
int get_indexid_from_bucket(int position)
{
    int i=0;
    Info temp;
    while(bucket[position].head[i]!='|')
    {
        temp.info[i] = bucket[position].head[i];
        i++;
    }
    temp.info[i] = '|';
    if(exten_model==LEAST)
        return get_indexid_least(get_search_key(temp),*bucket[position].tail);
    else
        return get_indexid_most(get_search_key(temp),*bucket[position].tail);
}   

/*
获得桶剩余空间：
    获得桶剩余空间大小，看是否能够继续插入元组
参数：桶位置
*/
int get_size_from_bucket(int position)
{
    int offset = *(bucket[position].tail-1);
    int num = *(bucket[position].tail-2);
    return PAGE_SIZE-offset-(num*2+3)*INT;
}

/*
获得桶号：
    从索引表中得出指定元组搜索键对应的桶号
参数：元组
*/
int get_bucketid(Info tempi)
{
    int key,noffset;
    
    //获得索引值 
    if(exten_model==LEAST)
        key = get_indexid_least(get_search_key(tempi),-1);
    else
        key = get_indexid_most(get_search_key(tempi),-1);
    noffset = key/(PAGE_SIZE/INT);
    
    //判断内存中索引表是否 有指定的索引值 
    if(index_offset!=noffset)
    {
        FILE *fptr = fopen(INDEX,"rb+");
        fseek(fptr,index_offset*PAGE_SIZE,SEEK_SET);
        fwrite(index,PAGE_SIZE,1,fptr);
        io++;
        fseek(fptr,noffset*PAGE_SIZE,SEEK_SET);
        fread(index,PAGE_SIZE,1,fptr);
        io++;
        index_offset = noffset;
        fclose(fptr);
    }
    return index[key-index_offset*(PAGE_SIZE/INT)];
}

/*
获得搜索键：
    返回给定元组的搜索键
*/
int get_search_key(Info tempi)
{
    char key[32];
    int i=0,j;
    int res=0;
    while(tempi.info[i]!='|')
    {
        key[i] = tempi.info[i];
        i++;
    }
    //转换成十进制 
    for(j=0;j<i;j++)
        res +=(int)( (key[j]-48)*pow(10,double(i-1-j)) );
    return res;
}

/*
获得排序键：
    用于查找后输出前的排序 
*/
int get_sort_key(Info tempi)
{
    char key[32];
    int i=0,j=0,res=0;
    while(tempi.info[i]!='|')
        i++;
    i++;
    while(tempi.info[i]!='|')
    {
        key[j] = tempi.info[i];
        j++;
        i++;
    }
    for(i=0;i<j;i++)
        res += (int)( ( key[i]-48)*pow(10,double(j-i-1)) );
    return res;
    
}

/*
快排递归函数：
*/
void quick_sort_help(int *a,Info *b, int begin,int end)
{
    int i=begin;
    int j=end;
    int p = a[(i+j)/2];
    while(i<j)
    {
        for(;(i<end)&&(a[i]<p);i++);
        for(;(j>begin)&&(a[j]>p);j--);
        if(i<=j)
        {
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            Info t = b[i];
            b[i] = b[j];
            b[j] = t;
            j--;
            i++;
        }
    }
    if(i<end)
        quick_sort_help(a,b,i,end);
    if(j>begin)
        quick_sort_help(a,b,begin,j);
}

