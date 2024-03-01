#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
int TESTmachine();

int es = 0, i, k = 0, codecount, stack[1000], stacktop = 0;
char Codein[300], code[1000][20], data[1000];
int label[100];
char lno[4];
int TESTmachine()
{
  FILE *fin;
  printf("������Ŀ���ļ���(����·��):");
  scanf_s("%s", Codein, (unsigned)_countof(Codein));

  if (fopen_s(&fin, Codein, "r") != 0) {
      // ��ȡ������Ϣ����ӡ
      char errMsg[100];
      strerror_s(errMsg, sizeof(errMsg), errno);
      printf("\n�޷����ļ� %s��������Ϣ��%s\n", Codein, errMsg);

      // ���ش������ִ�������������߼�
      return 1;
  }

  codecount=0;
  i= fscanf_s(fin, "%s", &code[codecount], (unsigned)_countof(code));
  while(!feof(fin))
  {
    i=(int)strlen(code[codecount])-1;
    if(code[codecount][i]==':')
     {
       i=i-5;
       strncpy_s(lno, i, &code[codecount][5], _TRUNCATE);
       lno[i]='\0';
       label[atoi(lno)]=codecount;
       code[codecount][0]=':';
       code[codecount][1]='\0';
       strcat_s(code[codecount], sizeof(code[codecount]), lno);
       k++;
     }
     codecount++;
     i= fscanf_s(fin, "%s", &code[codecount], (unsigned)_countof(code));
  }

  fclose(fin);

  for (int i = 0; i < codecount; ++i)
  {
      int len = (int)(strlen(code[i]));

      if ((len > 1) && (code[i][1] == 'A'))
      {
          char lno[256];  // ����ʵ��������������С
          strncpy_s(lno, sizeof(lno), &code[i][5], len - 5);
          lno[len - 5] = '\0';

          int labelIndex = atoi(lno);
          snprintf(code[i], sizeof(code[i]), "%d", label[labelIndex]);
      }
  }

  i=0;
  while(i<codecount)     //ִ��ÿ��ָ��
  {
    if(strcmp(code[i],"LOAD")==0)    //LOAD D  ��D�е����ݼ��ص�������ջ
     {
        i++;
        stack[stacktop]=data[atoi(code[i])];
        stacktop++;
     }

    if(strcmp(code[i],"LOADI")==0)   //LOADI a ��aѹ�������ջ
     {
        i++;
        stack[stacktop]=atoi(code[i]);
        stacktop++;
     }

    if (strcmp(code[i], "INC") == 0) // INC D����D�е����ݼ�1
    {
        i++;
        stack[stacktop] = data[atoi(code[i])] + 1;
        stacktop++;
        data[atoi(code[i])] = stack[stacktop - 1];
        stacktop--;
    }

    //STD D��������ջջ����Ԫ����D����ջ����Ԫ���ݱ��ֲ���
    if(strcmp(code[i],"STO")==0)
     {
        i++;
        data[atoi(code[i])]=stack[stacktop-1];
     }

    //STI D��������ջջ����Ԫ����D����ջ����Ԫ���ݳ�ջ
    if(strcmp(code[i],"STI")==0)
     {
        i++;
        data[atoi(code[i])]=stack[stacktop-1];
        stacktop--;
     }

    //ADD ��ջ����Ԫ���ջ����Ԫ��ջ����ӣ�������ջ��
    if(strcmp(code[i],"ADD")==0)
     {
        stack[stacktop-2]=stack[stacktop-2]+stack[stacktop-1];
        stacktop--;
     }

    //SUB ����ջ����Ԫ��ȥջ����Ԫ����ջ��������ջ��
    if(strcmp(code[i],"SUB")==0)
     {

        stack[stacktop-2]=stack[stacktop-2]-stack[stacktop-1];
        stacktop--;
     }

    //MULT ��ջ����Ԫ���ջ����Ԫ��ջ����ˣ�������ջ��
    if(strcmp(code[i],"MULT")==0)
     {

        stack[stacktop-2]=stack[stacktop-2]*stack[stacktop-1];
        stacktop--;
     }

    //DIV ��ջ����Ԫ���ջ����Ԫ��ջ�������������ջ��
    if(strcmp(code[i],"DIV")==0)
     {

        stack[stacktop-2]=stack[stacktop-2]/stack[stacktop-1];
        stacktop--;
     }

    //BR lab ������ת�Ƶ�lab
    if(strcmp(code[i],"BR")==0)
     {
        i++;
        i=atoi(code[i]);
     }

    //BRF lab ���ջ����Ԫ�߼�ֵ����Ϊ�٣�0����ת�Ƶ�lab
    if(strcmp(code[i],"BRF")==0)
     {
        i++;
        if(stack[stacktop-1]==0)  i=atoi(code[i]);
     }

    //EQ ��ջ������Ԫ�����ڱȽϣ�����������٣�1��0������ջ��
    if(strcmp(code[i],"EQ")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]==stack[stacktop-1];
       stacktop--;
     }

    //NOTEQ ��ջ������Ԫ�������ڱȽϣ�����������٣�1��0������ջ��
    if(strcmp(code[i],"NOTEQ")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]!=stack[stacktop-1];
       stacktop--;
     }

    //GT ��ջ����Ԫ����ջ����Ԫ����ջ����1��������0
    if(strcmp(code[i],"GT")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]>stack[stacktop-1];
       stacktop--;
     }

    //LES ��ջ����ԪС��ջ����Ԫ����ջ����1��������0
    if(strcmp(code[i],"LES")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]<stack[stacktop-1];
       stacktop--;
     }

    //GE ��ջ����Ԫ���ڵ���ջ����Ԫ����ջ����1��������0
    if(strcmp(code[i],"GE")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]>=stack[stacktop-1];
       stacktop--;
     }

    //LE ��ջ����ԪС�ڵ���ջ����Ԫ����ջ����1��������0
    if(strcmp(code[i],"LE")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]<=stack[stacktop-1];
       stacktop--;
     }

    //AND ��ջ������Ԫ���߼������㣬����������٣�1��0������ջ��
    if(strcmp(code[i],"AND")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]&&stack[stacktop-1];
       stacktop--;
     }

    //OR ��ջ������Ԫ���߼������㣬����������٣�1��0������ջ��
    if(strcmp(code[i],"OR")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]||stack[stacktop-1];
       stacktop--;
     }

    //NOT ��ջ�����߼�ֵȡ��
    if(strcmp(code[i],"NOT")==0)
     {
       stack[stacktop-1]=!stack[stacktop-1];
     }

    //IN �ӱ�׼�����豸�����̣�����һ���������ݣ�����ջ
    if(strcmp(code[i],"IN")==0)
     {
       //i++;
       printf("���������ݣ�");
       //scanf("%d",&data[atoi(code[i])]);
       scanf_s("%d",&stack[stacktop]);
       stacktop++;
     }

    //OUT ��ջ����Ԫ���ݳ�ջ�����������׼����豸�ϣ���ʾ����
    if(strcmp(code[i],"OUT")==0)
     {
       printf("�������%d\n",stack[stacktop-1]);
       stacktop--;
     }

    //STOP ִֹͣ��
    if(strcmp(code[i],"STOP")==0)
     {
        printf("The test program is over!");
        return(es);
        break;
     }
     i++;
  }
  printf("The test program is over!");
  return(es);
}
