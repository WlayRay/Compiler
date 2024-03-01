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

//���涨�屣����Ϊ�򻯳���ʹ���ַ�ָ�����鱣�����б����֡�
//��������ӱ����֣��ɼ�����ӣ����޸ı�������Ŀ
#define keywordSum 35
const char keyword[keywordSum][20] = { "if", "else", "for", "while", "do", "int", "read", "write", "char", "real", "bool", "break", "true", "false", "switch", "case", "repeat", "until" };
char singleword[50] = "+-*(){};,:'.%";
//���涨��˫�ֽ�������ַ�
char doubleword[10] = "><=!&/|";
extern char Scanin[300], Scanout[300]; //���ڽ�����������ļ���,��TEST_main.c�ж���
extern FILE* fin, * fout; //����ָ����������ļ���ָ��,��TEST_main.c�ж���
int TESTscan()//�ʷ���������
{
	char ch;//chΪÿ�ζ�����ַ���token���ڱ���ʶ����ĵ���
	char* token = (char*)malloc(80 * sizeof(char));
	int es = 0;
	int j;
	int n; //es������룬0��ʾû�д���j,nΪ��ʱ������������ϵ���ʱ���±��
	printf("������Դ�����ļ���������·������");
	scanf_s("%s", Scanin, (unsigned int)sizeof(Scanin) + 1);
	printf("������ʷ���������ļ���������·������");
	scanf_s("%s", Scanout, (unsigned int)sizeof(Scanout) + 1);
	if ((fopen_s(&fin, Scanin, "r")) != 0) //�ж������ļ����Ƿ���ȷ
	{
		printf("\n�򿪴ʷ����������ļ�����!\n");
		return(1);//�����ļ������ش������1
	}
	if ((fopen_s(&fout, Scanout, "w")) != 0) //�ж�����ļ����Ƿ���ȷ
	{
		printf("\n�����ʷ���������ļ�����!\n");
		return(2); //����ļ������ش������2
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
				while (isalnum(ch))  //�������ĸ��������ϱ�ʶ��������������ʶ����Ͻ���
				{
					token[j++] = ch;  //��ϵı�ʶ��������token��
					ch = fgetc(fin);  //����һ���ַ�
				}
				token[j] = '\0';  //��ʶ����Ͻ���	 
				//�鱣����
				n = 0;
				while ((n < keywordSum) && strcmp(token, keyword[n])) n++;
				if (n >= keywordSum)  //���Ǳ����֣������ʶ��
					fprintf(fout, "%s\t%s\n", "ID", token);  //�����ʶ������
				else
					//�Ǳ����֣����������
					fprintf(fout, "%s\t%s\n", token, token);  //��������ַ���
			}
		}
		else if (digit(ch))//���ִ���
		{
			token[0] = ch; j = 1;
			ch = fgetc(fin);  //����һ���ַ�
			if (!alpha(ch) && !isalnum(ch) && ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			while (digit(ch))  //�����������������������������������Ͻ���
			{
				token[j++] = ch;  //�������������token��
				ch = fgetc(fin);  //����һ���ַ�
			}
			if (ch == '.') {
				token[j++] = ch;
				ch = fgetc(fin);
				if (!digit(ch)) {
					token[0] = ch; token[1] = '\0';
					ch = fgetc(fin);  //����һ�������Ա�ʶ����һ������
					es = 3;  //���ô������
					fprintf(fout, "%s\t%s\n", "ERROR", token);  //����������
				}
				else {
					while (digit(ch))  //�����������������������������������Ͻ���
					{
						token[j++] = ch;  //�������������token��
						ch = fgetc(fin);  //����һ���ַ�
					}
					if (ch == 'E' || ch == 'e') {
						token[j++] = ch;
						ch = fgetc(fin);
						if (!digit(ch) && (ch != '+' && ch != '-')) {
							token[0] = ch; token[1] = '\0';
							ch = fgetc(fin);  //����һ�������Ա�ʶ����һ������
							es = 3;  //���ô������
							fprintf(fout, "%s\t%s\n", "ERROR", token);  //����������
						}
						else
						{
							if (digit(ch)) {
								while (digit(ch))  //�����������������������������������Ͻ���
								{
									token[j++] = ch;  //�������������token��
									ch = fgetc(fin);//����һ���ַ�
								}
							}
							else if (ch == '+' || ch == '-') {
								token[j++] = ch;  //�������������token��
								ch = fgetc(fin);
								if (!digit(ch)) {
									token[0] = ch; token[1] = '\0';
									ch = fgetc(fin);  //����һ�������Ա�ʶ����һ������
									es = 3;  //���ô������
									fprintf(fout, "%s\t%s\n", "ERROR", token);  //����������
								}
								else {
									while (digit(ch))  //�����������������������������������Ͻ���
									{
										token[j++] = ch;  //�������������token��
										ch = fgetc(fin);  //����һ���ַ�
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
				token[j] = '\0';  //������Ͻ���		 
				fprintf(fout, "%s\t%s\n", "NUM", token);  //�����������
			}
		}
		else if (strchr(singleword, ch) != NULL)  //���ַ�����
		{
			token[0] = ch;
			ch = fgetc(fin);//����һ�������Ա�ʶ����һ������
			if (ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			else if ((token[0] == '+' || token[0] == '-') && ch == token[0]) {
				token[1] = token[0];
				token[2] = '\0';  //���˫�ֽ������ 
				ch = fgetc(fin);//����һ�������Ա�ʶ����һ������
				fprintf(fout, "%s\t%s\n", token, token);  //���˫�ֽ������
			}
			else {
				token[1] = '\0';
				fprintf(fout, "%s\t%s\n", token, token);  //������ֽ������	
			}
		}
		else if (strchr(doubleword, ch) != NULL)  // ˫�ֽ������
		{
			token[0] = ch;
			ch = fgetc(fin);  // ����һ���ַ��ж��Ƿ�Ϊ˫�ֽ��
			if (!alpha(ch) && !isalnum(ch) && ch == '\'')
			{
				fprintf(fout, "%s\t%s\n", "CH", token);
			}
			if ((token[0] == '&' || token[0] == '/' || token[0] == '|') && ch == token[0]) {
				token[1] = token[0];
				token[2] = '\0';  // ���˫�ֽ������
				ch = fgetc(fin); // ����һ�������Ա�ʶ����һ������
			}
			else if (ch == '=' && (token[0] == '>' || token[0] == '<' || token[0] == '=' || token[0] == '!')) {
				// �����=�����˫�ֽ��
				token[1] = ch;
				token[2] = '\0';  // ���˫�ֽ������
				ch = fgetc(fin);  // ����һ�������Ա�ʶ����һ������
			}
			else {
				token[1] = '\0';
			}
			fprintf(fout, "%s\t%s\n", token, token);  // �������˫�ֽ������
		}
		else if (ch == '/')  //ע�ʹ���
		{
			ch = fgetc(fin);  //����һ���ַ�
			if (ch == '*')  //�����*����ʼ����ע��
			{
				char ch1;
				ch1 = fgetc(fin);  //����һ���ַ�
				do
				{
					ch = ch1; ch1 = fgetc(fin);
				}  //ɾ��ע��
				while ((ch != '*' || ch1 != '/') && ch1 != EOF);  //ֱ������ע�ͽ�����*/���ļ�β
				ch = fgetc(fin);//����һ�������Ա�ʶ����һ������
			}
			else  //����*�����ֽ��/
			{
				token[0] = '/'; token[1] = '\0';
				fprintf(fout, "%s\t%s\n", token, token);  //������ֽ��/
			}
		}
		else if (ch == '#')  //ע�ʹ���
		{
			char temp;
			temp = ch;
			ch = fgetc(fin);  //����һ���ַ�
			if (ch == '#')  //�����#����ʼ����ע��
			{
				char ch1;
				ch1 = fgetc(fin);  //����һ���ַ�
				do
				{
					ch = ch1; ch1 = fgetc(fin);
				}  //ɾ��ע��
				while ((ch != '\n') && ch1 != EOF);  //ֱ���������л��ļ�β
				ch = fgetc(fin);//����һ�������Ա�ʶ����һ������
			}
			else//������
			{
				token[0] = temp; token[1] = '\0';
				es = 3;  //���ô������
				fprintf(fout, "%s\t%s\n", "ERROR", token);  //����������
			}
		}
		else//������
		{
			token[0] = ch; token[1] = '\0';
			ch = fgetc(fin);  //����һ�������Ա�ʶ����һ������
			es = 3;  //���ô������
			fprintf(fout, "%s\t%s\n", "ERROR", token);  //����������
		}
	}
	fclose(fin);//�ر���������ļ�
	fclose(fout);
	return(es);  //����������
}