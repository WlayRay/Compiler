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
  printf("请输入目标文件名(包括路径):");
  scanf_s("%s", Codein, (unsigned)_countof(Codein));

  if (fopen_s(&fin, Codein, "r") != 0) {
      // 获取错误消息并打印
      char errMsg[100];
      strerror_s(errMsg, sizeof(errMsg), errno);
      printf("\n无法打开文件 %s。错误消息：%s\n", Codein, errMsg);

      // 返回错误码或执行其他错误处理逻辑
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
          char lno[256];  // 根据实际情况调整数组大小
          strncpy_s(lno, sizeof(lno), &code[i][5], len - 5);
          lno[len - 5] = '\0';

          int labelIndex = atoi(lno);
          snprintf(code[i], sizeof(code[i]), "%d", label[labelIndex]);
      }
  }

  i=0;
  while(i<codecount)     //执行每条指令
  {
    if(strcmp(code[i],"LOAD")==0)    //LOAD D  将D中的内容加载到操作数栈
     {
        i++;
        stack[stacktop]=data[atoi(code[i])];
        stacktop++;
     }

    if(strcmp(code[i],"LOADI")==0)   //LOADI a 将a压入操作数栈
     {
        i++;
        stack[stacktop]=atoi(code[i]);
        stacktop++;
     }

    if (strcmp(code[i], "INC") == 0) // INC D：将D中的内容加1
    {
        i++;
        stack[stacktop] = data[atoi(code[i])] + 1;
        stacktop++;
        data[atoi(code[i])] = stack[stacktop - 1];
        stacktop--;
    }

    //STD D将操作数栈栈顶单元存入D，且栈顶单元内容保持不变
    if(strcmp(code[i],"STO")==0)
     {
        i++;
        data[atoi(code[i])]=stack[stacktop-1];
     }

    //STI D将操作数栈栈顶单元存入D，且栈顶单元内容出栈
    if(strcmp(code[i],"STI")==0)
     {
        i++;
        data[atoi(code[i])]=stack[stacktop-1];
        stacktop--;
     }

    //ADD 将栈顶单元与次栈顶单元出栈并相加，和置于栈顶
    if(strcmp(code[i],"ADD")==0)
     {
        stack[stacktop-2]=stack[stacktop-2]+stack[stacktop-1];
        stacktop--;
     }

    //SUB 将次栈顶单元减去栈顶单元并出栈，差置于栈顶
    if(strcmp(code[i],"SUB")==0)
     {

        stack[stacktop-2]=stack[stacktop-2]-stack[stacktop-1];
        stacktop--;
     }

    //MULT 将栈顶单元与次栈顶单元出栈并相乘，积置于栈顶
    if(strcmp(code[i],"MULT")==0)
     {

        stack[stacktop-2]=stack[stacktop-2]*stack[stacktop-1];
        stacktop--;
     }

    //DIV 将栈顶单元与次栈顶单元出栈并相除，商置于栈顶
    if(strcmp(code[i],"DIV")==0)
     {

        stack[stacktop-2]=stack[stacktop-2]/stack[stacktop-1];
        stacktop--;
     }

    //BR lab 无条件转移到lab
    if(strcmp(code[i],"BR")==0)
     {
        i++;
        i=atoi(code[i]);
     }

    //BRF lab 检查栈顶单元逻辑值，若为假（0）则转移到lab
    if(strcmp(code[i],"BRF")==0)
     {
        i++;
        if(stack[stacktop-1]==0)  i=atoi(code[i]);
     }

    //EQ 将栈顶两单元做等于比较，并将结果真或假（1或0）置于栈顶
    if(strcmp(code[i],"EQ")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]==stack[stacktop-1];
       stacktop--;
     }

    //NOTEQ 将栈顶两单元做不等于比较，并将结果真或假（1或0）置于栈顶
    if(strcmp(code[i],"NOTEQ")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]!=stack[stacktop-1];
       stacktop--;
     }

    //GT 次栈顶单元大于栈顶单元，则栈顶置1，否则置0
    if(strcmp(code[i],"GT")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]>stack[stacktop-1];
       stacktop--;
     }

    //LES 次栈顶单元小于栈顶单元，则栈顶置1，否则置0
    if(strcmp(code[i],"LES")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]<stack[stacktop-1];
       stacktop--;
     }

    //GE 次栈顶单元大于等于栈顶单元，则栈顶置1，否则置0
    if(strcmp(code[i],"GE")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]>=stack[stacktop-1];
       stacktop--;
     }

    //LE 次栈顶单元小于等于栈顶单元，则栈顶置1，否则置0
    if(strcmp(code[i],"LE")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]<=stack[stacktop-1];
       stacktop--;
     }

    //AND 将栈顶两单元做逻辑与运算，并将结果真或假（1或0）置于栈顶
    if(strcmp(code[i],"AND")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]&&stack[stacktop-1];
       stacktop--;
     }

    //OR 将栈顶两单元做逻辑或运算，并将结果真或假（1或0）置于栈顶
    if(strcmp(code[i],"OR")==0)
     {
       stack[stacktop-2]=stack[stacktop-2]||stack[stacktop-1];
       stacktop--;
     }

    //NOT 将栈顶的逻辑值取反
    if(strcmp(code[i],"NOT")==0)
     {
       stack[stacktop-1]=!stack[stacktop-1];
     }

    //IN 从标准输入设备（键盘）读入一个整型数据，并入栈
    if(strcmp(code[i],"IN")==0)
     {
       //i++;
       printf("请输入数据：");
       //scanf("%d",&data[atoi(code[i])]);
       scanf_s("%d",&stack[stacktop]);
       stacktop++;
     }

    //OUT 将栈顶单元内容出栈，并输出到标准输出设备上（显示器）
    if(strcmp(code[i],"OUT")==0)
     {
       printf("程序输出%d\n",stack[stacktop-1]);
       stacktop--;
     }

    //STOP 停止执行
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
