#include"main.h"


/*
全局化：
    将页面和桶都初始化
    用一个页面来存放当前使用的索引
    用一个页面用来当做读取数据的缓冲页
    刚刚开始的时候全局深度是1，有两个桶存在
无参数
*/
void globallize()
{
    int i;
    for(i=0;i<PAGE_COUNT;i++)
    {
        initpage(i);        //初始化所有内存页面 
        if(i<PAGE_COUNT-2)
            init_bucket(i); //初始化所有桶 
    }
    
    index = (int*)page[0].pointer;//第一个页是缓冲页 
    index[0] = 0;                 //刚刚开始索引表只有两个索引 
    index[1] = 1;                 //占用两个整形位置 
    page[0].current_size  = page[0].current_size-INT*2;
    page[0].used = true;
    index_size = 1;               //这个时候索引大小事小于一个页的 
    index_offset = 0;
    global_depth = 1;
    
    bucket_load(0,0,2);
    bucket_load(1,1,3);
    bucket_number = 1;
    
    Clock = 2;
    
    read_buffer = (char*)page[1].pointer;
    page[1].used = true;
}

/*
把数据从磁盘读入内存：
     每次从磁盘中读取一个页大小数据到缓冲区
     然后对缓冲区中的每个元组建立索引
无参数
*/
void read_inof()
{
    FILE *readp = fopen(TABLE,"rb");
    Info tempi;
    long read_offset = 0;//刚开始的时候从页面顶部开始读取 
    int length,i;
    while( !feof(readp) )
    {
        fseek(readp,read_offset,SEEK_CUR);
        
        /*每次读取大小比页面小1个字节，用来存放换页标志*/
        fread(read_buffer,PAGE_SIZE-1,1,readp);
        
        io++;
        read_buffer[PAGE_SIZE-1] = '\0';
        i = length = 0;
                                                                         //for(int t=0;t<200;t++) printf("%c",read_buffer[t]);
        /*
          当没有遇到换页标志的时候继续读取， 
          每次遇到回车表示一个元组的结束； 
          每读取到一个元组就为其建立索引； 
          最后一个元组如果没有完全读入，将进入
          下一轮读取中来完全读取 
        */
        while(read_buffer[i]!='\0')
        {
            if(read_buffer[i]=='\n'/*||read_buffer[i]=='\r'*/)
            {
                tempi.length = length/*+1*/;                                    //printf("%d\n",length);
                make_index(tempi);
                length = 0;
                //每次建立好索引之后都需要清空缓冲元组 
                memset(tempi.info,0,sizeof(tempi.length)/*512*/);
            }
            else
            {                                                               //if(read_buffer[i]!='\r')printf("%c",read_buffer[i]);
                tempi.info[length] = read_buffer[i];                        //printf("%c",read_buffer[i]);/**/
                length++;
            }
            i++;
        } 
        read_offset = -length;                                              //printf("%s\n",tempi.info);
    }
    fclose(readp);
}


/*
建立索引：
    把每个元组计算得到搜索键，再转换成索引值
    根据索引值得到相应的桶号，进行插入
参数：需要建立索引的元组 
*/
void  make_index(Info tempi)
{
    int bid = get_bucketid(tempi);
    int i;
    //查找当前内存中是否存在所需要的桶 
    for(i=0;i<PAGE_COUNT-2;i++)
    {
        if(bucket[i].bucket_id == bid)
        {
            check_put(tempi,i);
            return ;
        }
    }
    
    /*如果当前没有需要的桶在内存中，就需要使用空白页，
      或者替换页来存放所需要的桶
      在替换页的时候要将当前桶中的数据写入磁盘中*/
    int position,npid = get_blank_page(position);//得到或者页面替换 
    
    FILE *fptr = fopen(BUCKET,"rb+");
    bucket_load(position,bid,npid);
    fseek(fptr,bid*PAGE_SIZE,SEEK_SET);
    fread(bucket[position].head,PAGE_SIZE,1,fptr);
    io++;
    page[npid].current_size = get_size_from_bucket(position);
    check_put(tempi,position);
    fclose(fptr);
}

/*
元组放入桶前的判断：
    判断桶大小是否足够存放，是否需要分裂；
    如果分裂是否需要索引表扩展*/
void check_put(Info tempi,int position)
{
    /*每个元组需要两个位置来记录起始位置和元组长度*/
    if(tempi.length+INT*2>page[bucket[position].page_id].current_size)
    {
        //局部深度与全局深度相同就需要索引扩展 
        if(*(bucket[position].tail)==global_depth)
        {
            //当索引的大小超过一个页面大小就需要在页面间扩展
            //这个时候需要保存当前索引页面的信息 
            if((int)pow(2,(double)(global_depth+1))>PAGE_SIZE/INT)
            {
                if(exten_model==LEAST)
                    exten_index_page_least();
                else
                    exten_index_page_most();
            }
            else
            {
                if(exten_model==LEAST)
                    exten_index_in_least();
                else
                    exten_index_in_most();
            }
        }
        if(exten_model==LEAST)
            bucket_split_least(position);
        else
            bucket_split_most(position);
        make_index(tempi);
        return ;
    }
    put_into_bucket(tempi,position);
}

/*
元组入桶：
    把元组直接放进桶内
参数：元组，对应桶的位置*/
void put_into_bucket(Info tempi,int position)
{
    int p,offset,num,i;
    if(position!=-1)
        p = position;
    else
    {
        for(i=0;i<PAGE_COUNT-2;i++)
            if(bucket[i].bucket_id==get_bucketid(tempi))
            {
                p = i;
                break;
            }
    }
    
    offset = *(bucket[p].tail-1);
    num = *(bucket[p].tail-2);
    for(i=0;i<tempi.length;i++)
        bucket[position].head[offset+i] = tempi.info[i];
    *(bucket[p].tail-1) += tempi.length;
    *(bucket[p].tail-2) +=1;
    
    /*对于桶内的每个元组，都有特定的长度的和在桶
      内的偏移位置*/
    *(bucket[p].tail-3-num*2) = tempi.length;
    *(bucket[p].tail-4-num*2) = offset;
    page[bucket[p].page_id].current_size -= (tempi.length+INT*2);
    page[bucket[p].page_id].refered = true;
}


/*
申请空白页面：
    返回第一个空白页面，或者使用时钟算法找到
    并初始化一个可用页面
参数：空白参数，记录页面对应磁盘中的位置*/
int get_blank_page(int &position)
{
    int i,j;
    for(i=2;i<PAGE_COUNT;i++)
    {
        if(page[i].used==false)
        {
            for(j=0;j<PAGE_COUNT-2;j++)
            {
                if(bucket[j].bucket_id==-1)
                {
                    position = j;
                    return i;
                }
            }
        }
    }
    while(page[Clock].refered==true||page[Clock].locked==true)
    {
        //循环时钟，直到找到页面 
        
        if(Clock==PAGE_COUNT-1)
            Clock = 2;
        else 
            Clock++;
        page[Clock].refered = false;
    }
    
    //如果内存中有空闲的桶与空闲页面对应，则直接返回桶号 
    for(i=0;i<PAGE_COUNT-2;i++)
    {
        if(bucket[i].page_id==Clock)
        {
            position = i;
            break;
        }
    }
    
    FILE *fptr = fopen(BUCKET,"rb+");
    fseek(fptr,page[Clock].bucket_id*PAGE_SIZE,SEEK_SET);
    fwrite(bucket[position].head,PAGE_SIZE,1,fptr);
    io++;
    fclose(fptr);
    //if(page[bucket[position].page_id].pointer)
    reset(position);
    return Clock;
}

/*
复位一个页面：
    为指定页面和对应的桶进行初始化
*/
void reset(int position)
{
    int pid = bucket[position].page_id;
    free(page[pid].pointer);
    bucket[position].bucket_id = -1;
    bucket[position].head = NULL;
    bucket[position].page_id = -1;
    bucket[position].position = -1;
    bucket[position].tail = NULL;
    
    page[pid].bucket_id = -1;
    page[pid].current_size = PAGE_SIZE;
    page[pid].pointer = malloc(PAGE_SIZE);
    page[pid].refered = false;
    page[pid].used = false;
}


/*
查询函数：
    读取搜索键，通过哈希索引找到相应的桶，
    把桶读入内存中 ，并找出该桶的内存位置 
*/
void search()
{
    int flag=0;
    FILE *sptr = fopen(IN,"rb+");
    int time,skey,bid,noffset,indexid;
    fscanf(sptr,"%d",&time);
    
    while(time--)
    {
        fscanf(sptr,"%d",&skey);
        int flag=0;
        //得到搜索键 对应的索引值 
        if(exten_model==MOST)
        {
            indexid = get_indexid_most(skey,-1);
            noffset = get_indexid_most(skey,-1)/(PAGE_SIZE/INT);
        }
        else
        {
            indexid = get_indexid_least(skey,-1);
            noffset = get_indexid_least(skey,-1)/(PAGE_SIZE/INT);
        }
        
        /*判断内存中的索引表是否有需要的索引值，如果没有
          就要从磁盘中读取*/
        if(index_offset!=noffset)
        {
            FILE *fptr = fopen(INDEX,"r+");////////////////////////////
            fseek(fptr,index_offset*PAGE_SIZE,SEEK_SET);
            fwrite(index,PAGE_SIZE,1,fptr);
            io++;
            fseek(fptr,noffset*PAGE_SIZE,SEEK_SET);
            fread(index,PAGE_SIZE,1,fptr);
            io++;
            index_offset = noffset;
            fclose(fptr);
        }
        
        bid = index[indexid-index_offset*(PAGE_SIZE/INT)];
        
        for(int i=0;i<PAGE_COUNT-2;i++)
        {
            if(bucket[i].bucket_id==bid)
            {
                match_and_output(skey,i);
                flag = 1;
                break;
            }
        }
        if(flag==0)
        {
            int position,npid = get_blank_page(position);
            FILE *fptr = fopen(BUCKET,"rb+");
            bucket_load(position,bid,npid);
            fseek(fptr,bid*PAGE_SIZE,SEEK_SET);
            fread(bucket[position].head,PAGE_SIZE,1,fptr);
            io++;
            page[npid].current_size = get_size_from_bucket(position);
        
            match_and_output(skey,position);
            fclose(fptr);
        }
    }
    fclose(sptr);
}

/*
输出函数：
    在桶内检索搜索键，找到匹配的就放入缓冲区中
    进行排序输出，最后写入输出文件。
参数：搜索键，对应桶的位置 
*/
void match_and_output(int skey,int position)
{
    Info temp;
    int i,j,count=0,pks[100];
    Info inofset[100];
    FILE *fptr = fopen(OUT,"rb+");
    for(i=0;i<*(bucket[position].tail-2);i++)
    {
        temp.length = *(bucket[position].tail-3-i*2);
        for(j=0;j<temp.length;j++)
            temp.info[j] = bucket[position].head[*(bucket[position].tail-4-i*2)+j];
        if(get_search_key(temp)==skey)
        {
            pks[count] = get_sort_key(temp);
            inofset[count] = temp;
            count++;
        }
    }
    
    quick_sort(pks,inofset,count);
    for(i=0;i<count;i++)
    {
        fseek(fptr,0,SEEK_END);
        fwrite(inofset[i].info,inofset[i].length,1,fptr);
        io++;
        fprintf(fptr,"\r\n");
    }
    fseek(fptr,0,SEEK_END);
    fprintf(fptr,"-1\r\n");
    fclose(fptr);
}


/*
快排函数：
    进入快排递归的输入函数
参数：排序键数组，元组数目，长度
*/
void quick_sort(int *a,Info *b,int length)
{
    quick_sort_help(a,b,0,length-1);
}


/*
删除页：
    程序结束，删除所有页，释放内存
*/
void del_all_page()
{
    for(int i=0;i<PAGE_COUNT;i++)
        free(page[i].pointer);
}

