#include"steps.cpp"
#include"bace_functions.cpp"
#include"hash_extend.cpp"
#include"main.h"

int main(int argc,char **argv/**/)
{
    exten_model = MOST;
    /*char **argv; argv[1] = "D:\\datab";*/
    strcpy(BUCKET,argv[1]);
    strcat(BUCKET,"\\hashbucket.out");
    strcpy(INDEX,argv[1]);
    strcat(INDEX,"\\hashindex.out");
    strcpy(IN,argv[1]);
    strcat(IN,"\\testinput.in");
    strcpy(OUT,argv[1]);
    strcat(OUT,"\\testoutput.out");
    strcpy(TABLE,argv[1]);
    strcat(TABLE,"\\lineitem.tbl");
    
    if(exten_model==MOST)
    {
        strcpy(TEMP,argv[1]);
        strcat(TEMP,"\\indextemp.out");
    }
    
    
    FILE *cfptr = fopen(BUCKET,"w+");
    fclose(cfptr);
    cfptr = fopen(INDEX,"w+");
    fclose(cfptr);
    cfptr = fopen(OUT,"w+");
    fclose(cfptr);
    if(exten_model==MOST)
    {
        cfptr = fopen(TEMP,"w+");
        fclose(cfptr);
    }
    
    printf("页面数目是：%d\t",PAGE_COUNT);
    if(exten_model==LEAST)
        printf("低位扩展\n");
    else 
        printf("高位扩展\n");
        
    io = 0;
    globallize();
    printf("Loading....\n");
    start = clock();
    read_inof();
    end = clock();
    printf("索引建立完成，哈希桶数量：%d，I/O次数：%d，耗时：%.4lf秒\n",bucket_number,io,(end-start)/1000.0);
    start = clock();
    io = 0;
    search();
    end = clock();
    //printf("d\n",index[0]);/**/
    printf("查询结束，共用时%.4lf秒，I/O:%d次。\n",(end-start)/1000.0,io);
    del_all_page();
    if(exten_model==MOST)
        remove(TEMP);
        
    system("pause");
    return 0;
    
}

