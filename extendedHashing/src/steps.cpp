#include"main.h"


/*
ȫ�ֻ���
    ��ҳ���Ͱ����ʼ��
    ��һ��ҳ������ŵ�ǰʹ�õ�����
    ��һ��ҳ������������ȡ���ݵĻ���ҳ
    �ոտ�ʼ��ʱ��ȫ�������1��������Ͱ����
�޲���
*/
void globallize()
{
    int i;
    for(i=0;i<PAGE_COUNT;i++)
    {
        initpage(i);        //��ʼ�������ڴ�ҳ�� 
        if(i<PAGE_COUNT-2)
            init_bucket(i); //��ʼ������Ͱ 
    }
    
    index = (int*)page[0].pointer;//��һ��ҳ�ǻ���ҳ 
    index[0] = 0;                 //�ոտ�ʼ������ֻ���������� 
    index[1] = 1;                 //ռ����������λ�� 
    page[0].current_size  = page[0].current_size-INT*2;
    page[0].used = true;
    index_size = 1;               //���ʱ��������С��С��һ��ҳ�� 
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
�����ݴӴ��̶����ڴ棺
     ÿ�δӴ����ж�ȡһ��ҳ��С���ݵ�������
     Ȼ��Ի������е�ÿ��Ԫ�齨������
�޲���
*/
void read_inof()
{
    FILE *readp = fopen(TABLE,"rb");
    Info tempi;
    long read_offset = 0;//�տ�ʼ��ʱ���ҳ�涥����ʼ��ȡ 
    int length,i;
    while( !feof(readp) )
    {
        fseek(readp,read_offset,SEEK_CUR);
        
        /*ÿ�ζ�ȡ��С��ҳ��С1���ֽڣ�������Ż�ҳ��־*/
        fread(read_buffer,PAGE_SIZE-1,1,readp);
        
        io++;
        read_buffer[PAGE_SIZE-1] = '\0';
        i = length = 0;
                                                                         //for(int t=0;t<200;t++) printf("%c",read_buffer[t]);
        /*
          ��û��������ҳ��־��ʱ�������ȡ�� 
          ÿ�������س���ʾһ��Ԫ��Ľ����� 
          ÿ��ȡ��һ��Ԫ���Ϊ�佨�������� 
          ���һ��Ԫ�����û����ȫ���룬������
          ��һ�ֶ�ȡ������ȫ��ȡ 
        */
        while(read_buffer[i]!='\0')
        {
            if(read_buffer[i]=='\n'/*||read_buffer[i]=='\r'*/)
            {
                tempi.length = length/*+1*/;                                    //printf("%d\n",length);
                make_index(tempi);
                length = 0;
                //ÿ�ν���������֮����Ҫ��ջ���Ԫ�� 
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
����������
    ��ÿ��Ԫ�����õ�����������ת��������ֵ
    ��������ֵ�õ���Ӧ��Ͱ�ţ����в���
��������Ҫ����������Ԫ�� 
*/
void  make_index(Info tempi)
{
    int bid = get_bucketid(tempi);
    int i;
    //���ҵ�ǰ�ڴ����Ƿ��������Ҫ��Ͱ 
    for(i=0;i<PAGE_COUNT-2;i++)
    {
        if(bucket[i].bucket_id == bid)
        {
            check_put(tempi,i);
            return ;
        }
    }
    
    /*�����ǰû����Ҫ��Ͱ���ڴ��У�����Ҫʹ�ÿհ�ҳ��
      �����滻ҳ���������Ҫ��Ͱ
      ���滻ҳ��ʱ��Ҫ����ǰͰ�е�����д�������*/
    int position,npid = get_blank_page(position);//�õ�����ҳ���滻 
    
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
Ԫ�����Ͱǰ���жϣ�
    �ж�Ͱ��С�Ƿ��㹻��ţ��Ƿ���Ҫ���ѣ�
    ��������Ƿ���Ҫ��������չ*/
void check_put(Info tempi,int position)
{
    /*ÿ��Ԫ����Ҫ����λ������¼��ʼλ�ú�Ԫ�鳤��*/
    if(tempi.length+INT*2>page[bucket[position].page_id].current_size)
    {
        //�ֲ������ȫ�������ͬ����Ҫ������չ 
        if(*(bucket[position].tail)==global_depth)
        {
            //�������Ĵ�С����һ��ҳ���С����Ҫ��ҳ�����չ
            //���ʱ����Ҫ���浱ǰ����ҳ�����Ϣ 
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
Ԫ����Ͱ��
    ��Ԫ��ֱ�ӷŽ�Ͱ��
������Ԫ�飬��ӦͰ��λ��*/
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
    
    /*����Ͱ�ڵ�ÿ��Ԫ�飬�����ض��ĳ��ȵĺ���Ͱ
      �ڵ�ƫ��λ��*/
    *(bucket[p].tail-3-num*2) = tempi.length;
    *(bucket[p].tail-4-num*2) = offset;
    page[bucket[p].page_id].current_size -= (tempi.length+INT*2);
    page[bucket[p].page_id].refered = true;
}


/*
����հ�ҳ�棺
    ���ص�һ���հ�ҳ�棬����ʹ��ʱ���㷨�ҵ�
    ����ʼ��һ������ҳ��
�������հײ�������¼ҳ���Ӧ�����е�λ��*/
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
        //ѭ��ʱ�ӣ�ֱ���ҵ�ҳ�� 
        
        if(Clock==PAGE_COUNT-1)
            Clock = 2;
        else 
            Clock++;
        page[Clock].refered = false;
    }
    
    //����ڴ����п��е�Ͱ�����ҳ���Ӧ����ֱ�ӷ���Ͱ�� 
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
��λһ��ҳ�棺
    Ϊָ��ҳ��Ͷ�Ӧ��Ͱ���г�ʼ��
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
��ѯ������
    ��ȡ��������ͨ����ϣ�����ҵ���Ӧ��Ͱ��
    ��Ͱ�����ڴ��� �����ҳ���Ͱ���ڴ�λ�� 
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
        //�õ������� ��Ӧ������ֵ 
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
        
        /*�ж��ڴ��е��������Ƿ�����Ҫ������ֵ�����û��
          ��Ҫ�Ӵ����ж�ȡ*/
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
���������
    ��Ͱ�ڼ������������ҵ�ƥ��ľͷ��뻺������
    ����������������д������ļ���
����������������ӦͰ��λ�� 
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
���ź�����
    ������ŵݹ�����뺯��
��������������飬Ԫ����Ŀ������
*/
void quick_sort(int *a,Info *b,int length)
{
    quick_sort_help(a,b,0,length-1);
}


/*
ɾ��ҳ��
    ���������ɾ������ҳ���ͷ��ڴ�
*/
void del_all_page()
{
    for(int i=0;i<PAGE_COUNT;i++)
        free(page[i].pointer);
}

