#include"main.h"



/*
��ʼ��ҳ�棺
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
��ʼ��һ��Ͱ��
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
Ͱ���غ�����
    ��ָ����Ͱ��ָ����ҳ�������
������ָ��Ͱ��λ�ã�Ͱ�ţ�ҳ���
*/
void bucket_load(int position,int bid,int pid)
{
    bucket[position].bucket_id = bid;
    bucket[position].page_id = pid;
    bucket[position].head = (char*)page[pid].pointer;
    bucket[position].position = position;
    bucket[position].tail = (int*)page[pid].pointer+PAGE_SIZE/INT-1;
    
    /*����������¼Ͱ��ȣ�Ԫ�����������λ�����ڴ��е�ƫ��ֵ*/
    *(bucket[position].tail) = 1;
    *(bucket[position].tail-1) = 0;
    *(bucket[position].tail-2) = 0;
    
    page[pid].current_size = PAGE_SIZE-3*INT;
    page[pid].bucket_id = bid;
    page[pid].used = true;
}


/*
����Ͱ�������ֵ��
    ��ָ��Ͱ����������ø�Ͱ��Ӧ������ֵ
    ����������ֵ
������Ͱλ��*/
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
���Ͱʣ��ռ䣺
    ���Ͱʣ��ռ��С�����Ƿ��ܹ���������Ԫ��
������Ͱλ��
*/
int get_size_from_bucket(int position)
{
    int offset = *(bucket[position].tail-1);
    int num = *(bucket[position].tail-2);
    return PAGE_SIZE-offset-(num*2+3)*INT;
}

/*
���Ͱ�ţ�
    ���������еó�ָ��Ԫ����������Ӧ��Ͱ��
������Ԫ��
*/
int get_bucketid(Info tempi)
{
    int key,noffset;
    
    //�������ֵ 
    if(exten_model==LEAST)
        key = get_indexid_least(get_search_key(tempi),-1);
    else
        key = get_indexid_most(get_search_key(tempi),-1);
    noffset = key/(PAGE_SIZE/INT);
    
    //�ж��ڴ����������Ƿ� ��ָ��������ֵ 
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
�����������
    ���ظ���Ԫ���������
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
    //ת����ʮ���� 
    for(j=0;j<i;j++)
        res +=(int)( (key[j]-48)*pow(10,double(i-1-j)) );
    return res;
}

/*
����������
    ���ڲ��Һ����ǰ������ 
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
���ŵݹ麯����
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

