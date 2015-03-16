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
    
    printf("ҳ����Ŀ�ǣ�%d\t",PAGE_COUNT);
    if(exten_model==LEAST)
        printf("��λ��չ\n");
    else 
        printf("��λ��չ\n");
        
    io = 0;
    globallize();
    printf("Loading....\n");
    start = clock();
    read_inof();
    end = clock();
    printf("����������ɣ���ϣͰ������%d��I/O������%d����ʱ��%.4lf��\n",bucket_number,io,(end-start)/1000.0);
    start = clock();
    io = 0;
    search();
    end = clock();
    //printf("d\n",index[0]);/**/
    printf("��ѯ����������ʱ%.4lf�룬I/O:%d�Ρ�\n",(end-start)/1000.0,io);
    del_all_page();
    if(exten_model==MOST)
        remove(TEMP);
        
    system("pause");
    return 0;
    
}

