#include <stdio.h>
#include <ctype.h>
#include <String.h>
#include <stdlib.h>

int alpha(char ch)
{
	if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	return 1;
	return 0;
}

int digit(char ch)
{
	if (ch - '0' >= 0 && ch - '0' <= 9)
	return 1;
	return 0;
}

//下面定义保留，为简化程序，使用字符指针数组保存所有保留字。
//如果想增加保留字，可继续添加，并修改保留字数目
#define keywordSum 35
const char keyword[keywordSum][20] = { "if", "else", "for", "while", "do", "int", "read", "write", "char", "real", "bool", "break", "true", "false", "switch", "case", "repeat", "until" };
char singleword[50] = "+-*(){};,:'.%";
//下面定义双分界符的首字符
char doubleword[10] = "><=!&/|";
extern char Scanin[300], Scanout[300]; //用于接收输入输出文件名,在TEST_main.c中定义
extern FILE* fin, * fout; //用于指向输入输出文件的指针,在TEST_main.c中定义
int TESTscan()//词法分析函数
{
	char ch;//ch为每次读入的字符，token用于保存识别出的单词
	char* token = (char*)malloc(80 * sizeof(char));
	int es = 0;
	int j;
	int n; //es错误代码，0表示没有错误。j,n为临时变量，控制组合单词时的下标等
	printf("请输入源程序文件名（包括路径）：");
	scanf_s("%s", Scanin, (unsigned int)sizeof(Scanin) + 1);
	printf("请输入词法分析输出文件名（包括路径）：");
	scanf_s("%s", Scanout, (unsigned int)sizeof(Scanout) + 1);
	if ((fopen_s(&fin, Scanin, "r")) != 0) //判断输入文件名是否正确
	{
		printf("\n打开词法分析输入文件出错!\n");
		return(1);//输入文件出错返回错误代码1
	}
	if ((fopen_s(&fout, Scanout, "w")) != 0) //判断输出文件名是否正确
	{
		printf("\n创建词法分析输出文件出错!\n");
		return(2); //输出文件出错返回错误代码2
	}
	ch = fgetc(fin);
	while (ch != EOF)
	{
		while (ch == ' ' || ch == '\n' || ch == '\t') ch = fgetc(fin);
		if (ch == EOF) break;
		if (alpha(ch))
		{
			token[0] = ch; j = 1;
			ch = fgetc(fin);
			if (ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			else
			{
				while (isalnum(ch))  //如果是字母数字则组合标识符；如果不是则标识符组合结束
				{
					token[j++] = ch;  //组合的标识符保存在token中
					ch = fgetc(fin);  //读下一个字符
				}
				token[j] = '\0';  //标识符组合结束	 
				//查保留字
				n = 0;
				while ((n < keywordSum) && strcmp(token, keyword[n])) n++;
				if (n >= keywordSum)  //不是保留字，输出标识符
					fprintf(fout, "%s\t%s\n", "ID", token);  //输出标识符符号
				else
					//是保留字，输出保留字
					fprintf(fout, "%s\t%s\n", token, token);  //输出保留字符号
			}
		}
		else if (digit(ch))//数字处理
		{
			token[0] = ch; j = 1;
			ch = fgetc(fin);  //读下一个字符
			if (!alpha(ch) && !isalnum(ch) && ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			while (digit(ch))  //如果是数字则组合整数；如果不是则整数组合结束
			{
				token[j++] = ch;  //组合整数保存在token中
				ch = fgetc(fin);  //读下一个字符
			}
			if (ch == '.') {
				token[j++] = ch;
				ch = fgetc(fin);
				if (!digit(ch)) {
					token[0] = ch; token[1] = '\0';
					ch = fgetc(fin);  //读下一个符号以便识别下一个单词
					es = 3;  //设置错误代码
					fprintf(fout, "%s\t%s\n", "ERROR", token);  //输出错误符号
				}
				else {
					while (digit(ch))  //如果是数字则组合整数；如果不是则整数组合结束
					{
						token[j++] = ch;  //组合整数保存在token中
						ch = fgetc(fin);  //读下一个字符
					}
					if (ch == 'E' || ch == 'e') {
						token[j++] = ch;
						ch = fgetc(fin);
						if (!digit(ch) && (ch != '+' && ch != '-')) {
							token[0] = ch; token[1] = '\0';
							ch = fgetc(fin);  //读下一个符号以便识别下一个单词
							es = 3;  //设置错误代码
							fprintf(fout, "%s\t%s\n", "ERROR", token);  //输出错误符号
						}
						else
						{
							if (digit(ch)) {
								while (digit(ch))  //如果是数字则组合整数；如果不是则整数组合结束
								{
									token[j++] = ch;  //组合整数保存在token中
									ch = fgetc(fin);//读下一个字符
								}
							}
							else if (ch == '+' || ch == '-') {
								token[j++] = ch;  //组合整数保存在token中
								ch = fgetc(fin);
								if (!digit(ch)) {
									token[0] = ch; token[1] = '\0';
									ch = fgetc(fin);  //读下一个符号以便识别下一个单词
									es = 3;  //设置错误代码
									fprintf(fout, "%s\t%s\n", "ERROR", token);  //输出错误符号
								}
								else {
									while (digit(ch))  //如果是数字则组合整数；如果不是则整数组合结束
									{
										token[j++] = ch;  //组合整数保存在token中
										ch = fgetc(fin);  //读下一个字符
									}
								}
							}
						}
						token[j] = '\0';
					}
					else
						token[j] = '\0';
					fprintf(fout, "%s\t%s\n", "RE", token);
				}
			}
			else {
				token[j] = '\0';  //整数组合结束		 
				fprintf(fout, "%s\t%s\n", "NUM", token);  //输出整数符号
			}
		}
		else if (strchr(singleword, ch) != NULL)  //单分符处理
		{
			token[0] = ch;
			ch = fgetc(fin);//读下一个符号以便识别下一个单词
			if (ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			else if ((token[0] == '+' || token[0] == '-') && ch == token[0]) {
				token[1] = token[0];
				token[2] = '\0';  //组合双分界符结束 
				ch = fgetc(fin);//读下一个符号以便识别下一个单词
				fprintf(fout, "%s\t%s\n", token, token);  //输出双分界符符号
			}
			else {
				token[1] = '\0';
				fprintf(fout, "%s\t%s\n", token, token);  //输出单分界符符号	
			}
		}
		else if (strchr(doubleword, ch) != NULL)  // 双分界符处理
		{
			token[0] = ch;
			ch = fgetc(fin);  // 读下一个字符判断是否为双分界符
			if (!alpha(ch) && !isalnum(ch) && ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			if ((token[0] == '&' || token[0] == '/' || token[0] == '|') && ch == token[0]) {
				token[1] = token[0];
				token[2] = '\0';  // 组合双分界符结束
				ch = fgetc(fin); // 读下一个符号以便识别下一个单词
			}
			else if (ch == '=' && (token[0] == '>' || token[0] == '<' || token[0] == '=' || token[0] == '!')) {
				// 如果是=，组合双分界符
				token[1] = ch;
				token[2] = '\0';  // 组合双分界符结束
				ch = fgetc(fin);  // 读下一个符号以便识别下一个单词
			}
			else {
				token[1] = '\0';
			}
			fprintf(fout, "%s\t%s\n", token, token);  // 输出单或双分界符符号
		}
		else if (ch == '/')  //注释处理
		{
			ch = fgetc(fin);  //读下一个字符
			if (ch == '*')  //如果是*，则开始处理注释
			{
				char ch1;
				ch1 = fgetc(fin);  //读下一个字符
				do
				{
					ch = ch1; ch1 = fgetc(fin);
				}  //删除注释
				while ((ch != '*' || ch1 != '/') && ch1 != EOF);  //直到遇到注释结束符*/或文件尾
				ch = fgetc(fin);//读下一个符号以便识别下一个单词
			}
			else  //不是*则处理单分界符/
			{
				token[0] = '/'; token[1] = '\0';
				fprintf(fout, "%s\t%s\n", token, token);  //输出单分界符/
			}
		}
		else if (ch == '#')  //注释处理
		{
			char temp;
			temp = ch;
			ch = fgetc(fin);  //读下一个字符
			if (ch == '#')  //如果是#，则开始处理注释
			{
				char ch1;
				ch1 = fgetc(fin);  //读下一个字符
				do
				{
					ch = ch1; ch1 = fgetc(fin);
				}  //删除注释
				while ((ch != '\n') && ch1 != EOF);  //直到遇到换行或文件尾
				ch = fgetc(fin);//读下一个符号以便识别下一个单词
			}
			else//错误处理
			{
				token[0] = temp; token[1] = '\0';
				es = 3;  //设置错误代码
				fprintf(fout, "%s\t%s\n", "ERROR", token);  //输出错误符号
			}
		}
		else//错误处理
		{
			token[0] = ch; token[1] = '\0';
			ch = fgetc(fin);  //读下一个符号以便识别下一个单词
			es = 3;  //设置错误代码
			fprintf(fout, "%s\t%s\n", "ERROR", token);  //输出错误符号
		}
	}
	fclose(fin);//关闭输入输出文件
	fclose(fout);
	return(es);  //返回主程序
}