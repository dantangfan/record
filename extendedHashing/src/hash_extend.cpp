#include"main.h"

//-----------------------
/*
索引表单个页内扩展：
    单个页内扩展直接将索引表翻倍
    在把扩展出来的表指向相应桶
*/
void exten_index_in_least()
{
    for(int i =0;i<pow(2,(double)(global_depth));i++)
        index[(int)(i+pow(2,(double)global_depth))] = index[i];
    global_depth++;
}

/*
索引表页间扩展：
    首先要将当前索引存入文件中，在把扩展出来的索引指向
    相应的桶
*/
void exten_index_page_least()
{
    int i;
    FILE *fptr = fopen(INDEX,"rb+");
    fseek(fptr,index_offset*PAGE_SIZE,SEEK_SET);
    fwrite(index,PAGE_SIZE,1,fptr);
    io++;
    for(i=0;i<index_size;i++)
    {
        fseek(fptr,i*PAGE_SIZE,SEEK_SET);
        fread(index,PAGE_SIZE,1,fptr);
        io++;
        fseek(fptr,(index_size+i)*PAGE_SIZE,SEEK_SET);
        fwrite(index,PAGE_SIZE,1,fptr);
        io++;
    }
    index_size *=2;
    index_offset = i-1;
    global_depth++;
    fclose(fptr);
}

/*
桶分裂：
    分裂指定的桶，将原来桶中的元组按照局部深度重新放入
    分裂号的两个桶中
*/
void bucket_split_least(int position)
{
    int fpid,spid,fsnum,ssnum;
    //申请两个页面用于桶分裂过程 
    page[bucket[position].page_id].locked = true;            //给原桶占据的页面加锁，防止被时钟页面置换算法换出
    fpid = get_blank_page(fsnum);
    bucket_load(fsnum,bucket[position].bucket_id,fpid);
    page[fpid].locked = true;                                //第一个申请成功的页面同样加锁，防止换出
    spid = get_blank_page(ssnum);
    bucket_load(ssnum,++bucket_number,spid);
    bucket[position].bucket_id = -2;                        //将桶号赋予其中一个新桶后，撤销原桶的桶号，
    page[bucket[position].page_id].locked = false;           
    page[fpid].locked = false;
    
    int local_depth =*(bucket[position].tail)+1;
    ( *(bucket[fsnum].tail) ) = ( *(bucket[ssnum].tail) ) = local_depth;
    
    int niid = get_indexid_from_bucket(position)+(int)pow(2,(double)(local_depth-1)),exten_indexid;
    int i,j;
    /*分裂过程，其中一个新桶拥有原桶的桶号，另一个桶根据桶的数量获得新的桶号
	  而在索引表扩展的过程中，仅对桶号进行复制，所以新桶号出现后，要将对应的索引值更改，指向新桶
	  故以下循环实现这个功能*/
    for(i=0;i<(int)pow(2,(double)(global_depth-local_depth));i++)
    {
        exten_indexid = niid+binary_to_digit(i,local_depth);//全局深度与局部深度不相同时需要对从桶获得的索引值进行扩展（即补充高位的所有可能性）
        int noffset = exten_indexid/(PAGE_SIZE/INT);
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
        index[exten_indexid-index_offset*(PAGE_SIZE/INT)] = bucket[ssnum].bucket_id;
    }
    //桶内元组重新分配 
    Info temp;
    for(i=0;i<*(bucket[position].tail-2);i++)
    {
        temp.length = *(bucket[position].tail-3-i*2);
        for(j=0;j<temp.length;j++)
            temp.info[j] = bucket[position].head[*(bucket[position].tail-4-i*2)+j];
        if(get_indexid_least(get_search_key(temp),local_depth)==niid)
            put_into_bucket(temp,bucket[ssnum].position);
        else
            put_into_bucket(temp,bucket[fsnum].position);
    }
    reset(position);//分配完后，撤销原桶，即初始化原桶占据的页面信息
}

/*
获得索引值：
    把元组搜索键从十进制转化成二进制 
    提取相应深度的位数，在转化成十进制
    获得索引值
参数：元组搜索值，局部深度 
*/
int get_indexid_least(int key,int depth)
{
    char temp1[32],temp2[32];
    int i=0,j=0,indexid=0,d;
    if(key!=1&&key!=0)
    {
        while(key!=1&&key!=0)
        {
            temp1[i] = key%2;
            key = key/2;
            i++;
        }
        temp1[i] = key;
    }
    else
        temp1[i] = key;
    
    if(depth!=-1)
        d = depth;
    else
        d = global_depth;
    
    if(i+1>d)
        i = d-1;
    for(j=0;j<=i;j++)
        temp2[j] = temp1[i-j];
    for(j=0;j<=i;j++)
        indexid +=temp2[j]*((int)pow(2,double(i-j)));
    return indexid;
}

/*
扩展索引辅助：
    扩展后的索引表可能有多个索引指向同一个桶的现象
    这里就要让每个索引正确的指向桶，并返回扩展后的索引值 
参数：目标补充数目，局部深度 
*/ 
int binary_to_digit(int target,int local_depth)
{
    char temp[32];
    int i=0,j,res=0;
    if(target!=1&&target!=0)
    {
        while(target!=1&&target!=0)
        {
            temp[i] = target%2;
            target = target/2;
            i++;
        }
        temp[i] = target;
    }
    else
        temp[i] = target;
    
    for(j=0;j<=i;j++)
       res +=temp[j]*(int)(pow(2,double(local_depth+j)));
    return res;
}



//-------------

void exten_index_in_most()
{
    for(int i =0;i<(int)(pow(2,(double)global_depth));i++)
        index[(int)(pow(2,(double)(global_depth+1)))-1-2*i]
        = index[(int)(pow(2,(double)(global_depth+1)))-2-2*i]
        = index[(int)(pow(2,(double)(global_depth)))-1-i];
    global_depth++;
}


void exten_index_page_most()
{
    int i,j,k,position,tiid = get_blank_page(position);
    int *temp_index = (int *)page[tiid].pointer;
    page[tiid].used = true;
    FILE* iptr = fopen(INDEX,"rb+");
    FILE* tptr = fopen(TEMP,"rb+");
    
    fseek(iptr,index_offset*PAGE_SIZE,SEEK_SET);
    fwrite(index,PAGE_SIZE,1,iptr);
    io++;
    
    for(i=0;i<index_size;i++)
    {
        fseek(iptr,i*PAGE_SIZE,SEEK_SET);
        fread(index,PAGE_SIZE,1,iptr);
        io++;
        fseek(tptr,i*PAGE_SIZE,SEEK_SET);
        fwrite(index,PAGE_SIZE,1,tptr);
        io++;
    }
    
    for(i=0;i<index_size;i++)
    {
        fseek(tptr,i*PAGE_SIZE,SEEK_SET);
        fread(temp_index,PAGE_SIZE,1,tptr);
        io++;
        k = 0;
        
        for(j=0;j<(PAGE_SIZE/INT)/2;j++)
        {
            index[2*j] = index[2*j+1] = temp_index[k];
            k++;
        }
        fseek(iptr,2*i*PAGE_SIZE,SEEK_SET);
        fwrite(index,PAGE_SIZE,1,iptr);
        io++;
        for(j=0;j<(PAGE_SIZE/INT)/2;j++)
        {
            index[2*j] = index[2*j+1] = temp_index[k];
            k++;
        }
        fseek(iptr,(2*i+1)*PAGE_SIZE,SEEK_SET);
        fwrite(index,PAGE_SIZE,1,iptr);
        io++;
    }
    index_size *=2;
    index_offset = 2*i-1;
    
    fclose(iptr);
    fclose(tptr);
    int pid = bucket[position].page_id;
    free(page[pid].pointer);
    page[pid].bucket_id = -1;
    page[pid].current_size = PAGE_SIZE;
    page[pid].pointer = malloc(PAGE_SIZE);
    page[pid].refered = false;
    page[tiid].used = false;
    global_depth++;
}

void bucket_split_most(int position)
{
    int fpid,spid,fsnum,ssnum;
    page[bucket[position].page_id].locked = true;
    fpid = get_blank_page(fsnum);
    bucket_load(fsnum,bucket[position].bucket_id,fpid);
    page[fpid].locked = true;
    spid = get_blank_page(ssnum);
    bucket_load(ssnum,++bucket_number,spid);
    bucket[position].bucket_id = -2;
    page[bucket[position].page_id].locked = false;
    page[fpid].locked = false;
    
    int local_depth = (*(bucket[position].tail))+1;
    
    (*(bucket[fsnum].tail)) = (*(bucket[ssnum].tail)) = local_depth;
    
    int niid = 2*get_indexid_from_bucket(position)+1;
    int eiid = niid;
    int i,j;
    int FIRST = 2*get_indexid_from_bucket(position);
    for(i=0;i<global_depth-local_depth;i++)
        eiid *=2;
    
    for(i=0;i<(int)pow(2,(double)(global_depth-local_depth));i++)
    {
        eiid += i;
        int noffset = eiid/(PAGE_SIZE/INT);
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
        
        index[eiid-index_offset*(PAGE_SIZE/INT)] = bucket[ssnum].bucket_id;
        eiid = eiid -i;
    }
    
    Info temp;
    for(i=0;i<*(bucket[position].tail-2);i++)
    {
        temp.length = *(bucket[position].tail-3-i*2);
        for(j=0;j<temp.length;j++)
            temp.info[j] = bucket[position].head[*(bucket[position].tail-4-i*2)+j];
            
        if(get_indexid_most(get_search_key(temp),local_depth)==niid)
            put_into_bucket(temp,bucket[ssnum].position);
        else
        put_into_bucket(temp,bucket[fsnum].position);
    }
    
    reset(position);
}


int get_indexid_most(int key, int depth)
{
    char temp[32];
    int i=0,iid=0,d,length;
    if(key!=1&&key!=0)
    {
        while(key!=1&&key!=0)
        {
            temp[i] = key%2;
            key = key/2;
            i++;
        }
        temp[i] = key;
    }
    else
        temp[i] = key;
    if(depth!=-1)
        d = depth;
    else
        d = global_depth;
    length = i+1;
    if(d<length)
        length = d;
    for(i=0;i<length;i++)
        iid +=temp[i]*(int)(pow(2,double(length-i-1)));
    if(d>length)
        for(i=0;i<d-length;i++)
            iid *=2;
    return iid;
}

