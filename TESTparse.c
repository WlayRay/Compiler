//语法、语义分析及代码生成
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#define maxvartablep 500//定义符号表的容量
int TESTparse();
int program();
int compound_stat();
int statement();
int expression_stat();
int expression();
int bool_expr();
int log_expr();
int additive_expr();
int term();
int factor();
int if_stat();
int while_stat();
int for_stat();
int do_stat();
int write_stat();
int read_stat();
int declaration_stat();
int declaration_list();
int statement_list();
int compound_stat();
int name_def(char* name);
char token[20], token1[40];//token保存单词符号，token1保存单词值
extern char Scanout[300], Codeout[300]; //保存词法分析输出文件名
FILE* fp, * fout; //用于指向输入输出文件的指针
struct {//定义符号表结构
	char name[8];
	int address;
}vartable[maxvartablep];//改符号表最多容纳maxvartablep个记录
int vartablep = 0, labelp = 0, datap = 0;
/*
vartablep指出符号表的最后一个记录的下一个位置，即第一个空白记录位置。每当有一个记录加入符号表，该值加1。
datap为TEST虚拟机的数据区指针。TEST语言只有整型，目标机又是虚拟机，所以，每声明一个变量，datap加1，即增加一个存储整型的单元。
labelp是程序的标号，用于标记代码的位置，以便汇编指令跳转到某行代码。
*/

//插入符号表动作name-def（ID.name, int）的程序如下：
int name_def(char* name)
{
	int i, es = 0;
	if (vartablep >= maxvartablep) return(21);
	for (i = vartablep - 1; i >= 0; i--)//查符号表
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			es = 22;//22表示变量重复声明
			break;
		}
	}
	if (es > 0)   return(es);
	strcpy_s(vartable[vartablep].name, sizeof(vartable[0].name), name);
	vartable[vartablep].address = datap;
	datap++;//分配一个单元，数据区指针加1
	vartablep++;
	return(es);
}
//查询符号表返回地址
int lookup(char* name, int* paddress)
{
	int i, es = 0;
	for (i = 0; i < vartablep; i++)
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			*paddress = vartable[i].address;
			return(es);
		}
	}
	es = 23;//变量没有声明
	return(es);
}

//语法、语义分析及代码生成程序
int TESTparse()
{
	int es = 0;
	if (fopen_s(&fp, Scanout, "r") != 0)
	{
		printf("\n打开%s错误!\n", Scanout);
		es = 10;
		return(es);
	}
	printf("请输入目标文件名（包括路径）：");
	scanf_s("%s", Codeout, (unsigned int)sizeof(Codeout));
	if (fopen_s(&fout, Codeout, "w") != 0)
	{
		printf("\n创建%s错误!\n", Codeout);
		es = 10;
		return(es);
	}
	if (es == 0) es = program();
	printf("==语法、语义分析及代码生成程序结果==\n");
	switch (es)
	{
	case 0: printf("语法、语义分析成功并抽象机汇编生成代码!\n"); break;
	case 10: printf("打开文件 %s失败!\n", Scanout); break;
	case 1: printf("缺少{!\n"); break;
	case 2: printf("缺少}!\n"); break;
	case 3: printf("缺少标识符!\n"); break;
	case 4: printf("少分号!\n"); break;
	case 5: printf("缺少(!\n"); break;
	case 6: printf("缺少)!\n"); break;
	case 7: printf("缺少操作数!\n"); break;
	case 21: printf("符号表溢出!\n"); break;
	case 22: printf("变量重复定义!\n"); break;
	case 23: printf("变量未声明!\n"); break;

	}
	fclose(fp);
	fclose(fout);
	return(es);
}

//语法规则 ：program::={<declaration_list><statement_list>}
//这里强调TEST源程序必须是声明语句序列在前，语句序列在后，如果打乱该次序，则程序将出现异常
int program()
{
	int es = 0, i;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "{"))//判断是否'{'
	{
		es = 1;
		return(es);
	}
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = declaration_list();
	if (es > 0) return(es);
	printf("     符号表\n");
	printf("     名字       地址\n");
	for (i = 0; i < vartablep; i++)
		printf("      %s         %d\n", vartable[i].name, vartable[i].address);
	if (strcmp(token, "int") != 0) //这里处理第一个非声明语句的第一个单词，将其正常输出，以保证在输出符号表时不拆开显示第一个非声明语句的各单词
		printf("%s %s\n", token, token1);
	es = statement_list();
	if (es > 0) return(es);
	if (strcmp(token, "}"))//判断是否'}'
	{
		es = 2;
		return(es);
	}
	fprintf(fout, "        STOP\n");//产生停止指令
	return(es);
}

//语法规则 ：<declaration_list>::=
//<declaration_list><declaration_stat>|<declaration_stat>
//改成<declaration_list>::={<declaration_stat>}
int declaration_list()
{
	int es = 0;
	while (strcmp(token, "int") == 0 || strcmp(token, "bool") == 0)
	{
		es = declaration_stat();
		if (es > 0) return(es);
	}
	return(es);
}

//语法规则：<declaration_stat>::=int ID{, ID}|bool ID{, ID};
//翻译模式：<声明语句>::= int ID {name-def（ID.name, int/bool）};
int declaration_stat()
{
	int es = 0;
	do
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		if (strcmp(token, "ID"))	es = 3;  //不是标识符

		es = name_def(token1);//插入符号表
		if (es > 0) return(es);

		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);

	} while (strcmp(token, ",") == 0);
	if (strcmp(token, ";"))	es = 4;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	if (strcmp(token, "int") == 0 || strcmp(token, "bool") == 0) //如果输入单词是int或bool就打印（说明下一个语句还是声明语句），非int不打印（下一个语句是非声明语句），保证在输出符号表时不拆开显示第一个非声明语句的各单词
		printf("%s %s\n", token, token1);
	return(es);
}

//语法规则 ：<statement_list>::=<statement_list><statement>|<statement>
//改成<statement_list>::={<statement>}
int statement_list()
{
	int es = 0;
	while (strcmp(token, "}"))
	{
		es = statement();
		if (es > 0) return(es);
	}
	return(es);
}

//语法规则 ：<statement>::= <if_stat>|<while_stat>|<for_stat>
//                   |<compound_stat> |<expression_stat>

int statement()
{
	int es = 0;
	if (es == 0 && strcmp(token, "if") == 0) es = if_stat();//<IF语句>
	if (es == 0 && strcmp(token, "do") == 0) es = do_stat();//<do语句>
	if (es == 0 && strcmp(token, "while") == 0) es = while_stat();//<while语句>
	if (es == 0 && strcmp(token, "for") == 0) es = for_stat();//<for语句>
	//可在此处添加do语句调用
	if (es == 0 && strcmp(token, "read") == 0) es = read_stat();//<read语句>
	if (es == 0 && strcmp(token, "write") == 0) es = write_stat();//<write语句>
	if (es == 0 && strcmp(token, "{") == 0) es = compound_stat();//<复合语句>
	if (es == 0 && (strcmp(token, "ID") == 0 || strcmp(token, "NUM") == 0 || strcmp(token, "(") == 0)) es = expression_stat();//<表达式语句>
	return(es);
}

//语法规则：<if_stat>::= if (<expr>) <statement > [else < statement >]
/*
翻译模式：
<if语句>::= if ( <表达式> ){print(BRF label1)}  <语句> {print(BR label2)} {print(label1:)} [else <语句> ] {print(label2:)}
*/
int if_stat() {
	int es = 0, label1, label2;  //if
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "(")) 	return(es = 5);  //少左括号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ")"))  return(es = 6); //少右括号
	label1 = labelp++;//用label1记住条件为假时要转向的标号
	fprintf(fout, "        BRF LABEL%d\n", label1);//输出假转移指令
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);
	label2 = labelp++;//用label2记住要转向的标号
	fprintf(fout, "        BR LABEL%d\n", label2);//输出无条件转移指令
	fprintf(fout, "LABEL%d:\n", label1);//设置label1记住的标号
	if (strcmp(token, "else") == 0)//else部分处理
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = statement();
		if (es > 0) return(es);
	}
	fprintf(fout, "LABEL%d:\n", label2);//设置label2记住的标号
	return(es);
}

//<do_stat>::=do <statement> while (<expression>)
int do_stat() {
	int es = 0, label1, label2;
	label1 = labelp++;
	fprintf(fout, "LABEL%d:\n", label1);//设置label1标号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);

	if (strcmp(token, "while") == 0) {
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		if (strcmp(token, "("))  return(es = 5);  //少左括号
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = expression();
		if (es > 0) return(es);
		if (strcmp(token, ")"))  return(es = 6); //少右括号
		/*es = while_stat();*/
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		if (strcmp(token, ";")) 	return(es = 4);  //少分号
	}
	label2 = labelp++;
	fprintf(fout, "        BRF LABEL%d\n", label2);//输出假转移指令
	fprintf(fout, "        BR LABEL%d\n", label1);//输出无条件转移指令
	fprintf(fout, "LABEL%d:\n", label2);//设置label2标号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1)); //读取分号 
	printf("%s %s\n", token, token1);	
	return(es);
}

//语法规则：<while_stat>::= while (<expr >) < statement >
//翻译模式：<while语句>::= while {print(label1:) (<表达式 >) {print(BRF label2)} <语句>{print(BR label2)} {print(label2:)
int while_stat()
{
	int es = 0, label1, label2;
	label1 = labelp++;
	fprintf(fout, "LABEL%d:\n", label1);//设置label1标号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))  return(es = 5);  //少左括号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ")"))  return(es = 6); //少右括号
	label2 = labelp++;
	fprintf(fout, "        BRF LABEL%d\n", label2);//输出假转移指令
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);
	fprintf(fout, "        BR LABEL%d\n", label1);//输出无条件转移指令
	fprintf(fout, "LABEL%d:\n", label2);//设置label2标号
	return(es);
}

//语法规则：<for_stat>::= for(<expr>,<expr>,<expr>)<statement>
/*
翻译模式：
<for语句>::= for( <表达式> ; {print(label1:)<表达式> {print(BRF label2)}  {print(BR label3)}; {print(label4:)} <表达式> ){print(BR label1)} {print(label3:)<语句>{print(BR label4)}  {print(label2:)
*/
int for_stat()
{
	int es = 0, label1, label2, label3, label4;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))  return(es = 5);  //少左括号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ";")) 	return(es = 4);  //少分号
	label1 = labelp++;
	fprintf(fout, "LABEL%d:\n", label1);//设置label1标号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	label2 = labelp++;
	fprintf(fout, "        BRF LABEL%d\n", label2);//输出假条件转移指令
	label3 = labelp++;
	fprintf(fout, "        BR LABEL%d\n", label3);//输出无条件转移指令
	if (strcmp(token, ";"))  return(es = 4);  //少分号
	label4 = labelp++;
	fprintf(fout, "LABEL%d:\n", label4);//设置label4标号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	fprintf(fout, "        BR LABEL%d\n", label1);//输出无条件转移指令
	if (strcmp(token, ")"))  return(es = 6); //少右括号
	fprintf(fout, "LABEL%d:\n", label3);//设置label3标号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);
	fprintf(fout, "        BR LABEL%d\n", label4);//输出无条件转移指令
	fprintf(fout, "LABEL%d:\n", label2);//设置label2标号
	return(es);

}

//语法规则：<write_stat>::=write <expression>;
//翻译模式：<write_语句>::= write <表达式> {print(OUT)} ;

int write_stat()
{
	int es = 0;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)return(es);
	if (strcmp(token, ";"))  return(es = 4);  //少分号
	fprintf(fout, "        OUT\n");//输出指令
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	return(es);
}

//语法规则：<read_stat>::=read ID;
//翻译模式：<read_语句>::=read ID {ID.d = Look(ID.name)} {print(IN)} {print(STI ID.d)};

int read_stat()
{
	int es = 0, address;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "ID"))  return(es = 3);  //少标识符
	es = lookup(token1, &address);
	if (es > 0) return(es);
	fprintf(fout, "        IN   \n");//输入指令
	fprintf(fout, "        STI   %d\n", address);//指令
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, ";"))  return(es = 4);  //少分号
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	return(es);
}

//语法规则：<compound_stat>::={<statement_list>} //此处的花括号为单词
int compound_stat() {   //复合语句函数
	int es = 0;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement_list();
	if (strcmp(token, "}"))  return(es = 2);  //少}
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	return(es);
}

//语法规则：<expression_stat>::=<expression>;|;
int expression_stat()
{
	int es = 0;
	if (strcmp(token, ";") == 0)
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		return(es);
	}
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ";") == 0)
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		return(es);
	}
	else
	{
		es = 4;
		return(es);//少分号
	}
}
//语法规则：<表达式>::=ID=<布尔表达式> | <布尔表达式>
//翻译模式：<表达式>::=ID {Look(ID.name)} {ASSIGN}=<布尔表达式>{print(STO ID.d)} | <布尔表达式>
//<expr>::=ID=<log_expr> | <log_expr>
int expression()
{
	int es = 0, fileadd;
	char token2[20], token3[40];
	if (strcmp(token, "ID") == 0)
	{
		fileadd = ftell(fp);   //记住当前文件位置
		fscanf_s(fp, "%s %s\n", token2, (unsigned int)sizeof(token2), token3, (unsigned int)sizeof(token3));
		if (strcmp(token2, "=") == 0)	//在原文要加上这一行
			printf("%s %s\n", token2, token3);//这里能保证非=不重复输出，但能正常输出=
		if (strcmp(token2, "=") == 0)  //'='
		{
			int address;
			es = lookup(token1, &address);
			if (es > 0) return(es);
			fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
			printf("%s %s\n", token, token1);
			es = log_expr();
			if (es > 0) return(es);
			fprintf(fout, "        STO %d\n", address);
		}
		else
		{
			fseek(fp, fileadd, 0); //若非'='则文件指针回到'='前的标识符
			//printf("%s %s\n", token, token1);
			es = log_expr();
			if (es > 0) return(es);
		}
	}
	else es = log_expr();

	return(es);
}


//语法规则：<bool_expr>::=<additive_expr>
//       		    |< additive_expr >(>|<|>=|<=|==|!=)< additive_expr >
/*
翻译模式：
<布尔表达式>::=<算术表达式> |
| <算术表达式> > <算术表达式> {print(GT)}
| <算术表达式> < <算术表达式> {print(LES)}
| <算术表达式> >=<算术表达式> {print(GE)}
| <算术表达式> <= <算术表达式> {print(LE)}
| <算术表达式> == <算术表达式> {print(EQ)}
| <算术表达式> !=<算术表达式>  {print(NOTEQ)}

*/
int bool_expr()
{
	int es = 0;
	es = additive_expr();
	if (es > 0) return(es);
	if (strcmp(token, ">") == 0 || strcmp(token, ">=") == 0
		|| strcmp(token, "<") == 0 || strcmp(token, "<=") == 0
		|| strcmp(token, "==") == 0 || strcmp(token, "!=") == 0)
	{
		char token2[20];
		strcpy_s(token2, sizeof(token2), token);
		//保存运算符
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = additive_expr();
		if (es > 0) return(es);
		if (strcmp(token2, ">") == 0) fprintf(fout, "        GT\n");
		if (strcmp(token2, ">=") == 0) fprintf(fout, "        GE\n");
		if (strcmp(token2, "<") == 0) fprintf(fout, "        LES\n");
		if (strcmp(token2, "<=") == 0) fprintf(fout, "        LE\n");
		if (strcmp(token2, "==") == 0) fprintf(fout, "        EQ\n");
		if (strcmp(token2, "!=") == 0) fprintf(fout, "        NOTEQ\n");
	}
	return(es);
}

//<log_expr>::=true | false 
//| <bool_expr> && <bool_expr>
//| <bool_expr> || <bool_expr>
//| !<bool_expr>
int log_expr() {
	int es = 0;
	if (strcmp(token, "true") == 0 || strcmp(token, "false") == 0) {
		if (strcmp(token, "true") == 0) {
			fprintf(fout, "        LOADI %d\n", 1);
		}
		else if (strcmp(token, "false") == 0) {
			fprintf(fout, "        LOADI %d\n", 0);
		}
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = 0;
	}
	else if (strcmp(token, "!") == 0) {
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = bool_expr();
		fprintf(fout, "        NOT \n");
	}
	else {
		es = bool_expr();
	}
	if (strcmp(token, "&&") == 0 || strcmp(token, "||") == 0) {
		char token2[20];
		strcpy_s(token2, sizeof(token2), token);
		//保存运算符
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = bool_expr();
		if (strcmp(token2, "&&") == 0) {
			fprintf(fout, "        AND \n");
		}
		else if (strcmp(token2, "||") == 0) {
			fprintf(fout, "        OR \n");
		}
	}
	return es;
}

//语法规则：<additive_expr>::=<term>{(+|-)< term >} 
//翻译模式：<算术表达式>::=<项>{(+<项> {print(ADD)} | -<项> {print(SUB)} ) }

int additive_expr()
{
	int es = 0;
	es = term();
	if (es > 0) return(es);
	while (strcmp(token, "+") == 0 || strcmp(token, "-") == 0)
	{
		char token2[20];
		strcpy_s(token2, sizeof(token2), token);
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = term();
		if (es > 0) return(es);
		if (strcmp(token2, "+") == 0) fprintf(fout, "        ADD\n");
		if (strcmp(token2, "-") == 0) fprintf(fout, "        SUB\n");
	}
	return(es);
}

//语法规则：< term >::=<factor>{(*| /)< factor >} 
//翻译模式：<项>::=<因子>{(*<因子> {print(MULT)} | / <因子> {print(DIV)} ) }
int term()
{
	int es = 0;
	es = factor();
	if (es > 0) return(es);
	while (strcmp(token, "*") == 0 || strcmp(token, "/") == 0)
	{
		char token2[20];
		strcpy_s(token2, sizeof(token2), token);
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = factor();
		if (es > 0) return(es);
		if (strcmp(token2, "*") == 0) fprintf(fout, "        MULT\n");
		if (strcmp(token2, "/") == 0) fprintf(fout, "        DIV\n");
	}
	return(es);
}

//语法规则：< factor >::=(<additive_expr>)| ID|NUM
//翻译模式：<因子>::=(<算术表达式>) | ID {ID.d = Look(ID.name)} {print(LOAD ID.d)} | NUM {print(LOADI NUM.val)}

int factor()
{
	int es = 0;

	if (strcmp(token, "(") == 0)
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = expression();
		if (es > 0) return(es);
		if (strcmp(token, ")"))  return(es = 6); //少右括号
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
	}
	else
	{

		if (strcmp(token, "ID") == 0)
		{
			int flag = 0; //判断是否是自增表达式
			int address;
			es = lookup(token1, &address);//查符号表，获取变量地址
			if (es > 0) return(es);//变量没声明
			fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
			printf("%s %s\n", token, token1);
			if (strcmp(token, "++") == 0) {  // 如果是自增表达式
				flag = 1;
				fprintf(fout, "        INC %d\n", address);
				fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
				printf("%s %s\n", token, token1);
			}
			if (flag == 0) {
				fprintf(fout, "        LOAD %d\n", address);
			}
			return(es);
		}
		else if (strcmp(token, "NUM") == 0 || strcmp(token, "true") == 0 || strcmp(token, "false") == 0)
			{
					if (strcmp(token, "true") == 0) {
						fprintf(fout, "        LOADI %d\n", 1);
					}
					else if (strcmp(token, "false") == 0) {
						fprintf(fout, "        LOADI %d\n", 0);
					}
					else{
						fprintf(fout, "        LOADI %s\n", token1);
					}
					fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
					printf("%s %s\n", token, token1);
					return(es);
			}
			else
			{
				es = 7;//缺少操作数
				return(es);
			}
	}
	return(es);
}