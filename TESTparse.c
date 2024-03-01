//�﷨�������������������
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#define maxvartablep 500//������ű������
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
char token[20], token1[40];//token���浥�ʷ��ţ�token1���浥��ֵ
extern char Scanout[300], Codeout[300]; //����ʷ���������ļ���
FILE* fp, * fout; //����ָ����������ļ���ָ��
struct {//������ű�ṹ
	char name[8];
	int address;
}vartable[maxvartablep];//�ķ��ű��������maxvartablep����¼
int vartablep = 0, labelp = 0, datap = 0;
/*
vartablepָ�����ű�����һ����¼����һ��λ�ã�����һ���հ׼�¼λ�á�ÿ����һ����¼������ű���ֵ��1��
datapΪTEST�������������ָ�롣TEST����ֻ�����ͣ�Ŀ�����������������ԣ�ÿ����һ��������datap��1��������һ���洢���͵ĵ�Ԫ��
labelp�ǳ���ı�ţ����ڱ�Ǵ����λ�ã��Ա���ָ����ת��ĳ�д��롣
*/

//������ű���name-def��ID.name, int���ĳ������£�
int name_def(char* name)
{
	int i, es = 0;
	if (vartablep >= maxvartablep) return(21);
	for (i = vartablep - 1; i >= 0; i--)//����ű�
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			es = 22;//22��ʾ�����ظ�����
			break;
		}
	}
	if (es > 0)   return(es);
	strcpy_s(vartable[vartablep].name, sizeof(vartable[0].name), name);
	vartable[vartablep].address = datap;
	datap++;//����һ����Ԫ��������ָ���1
	vartablep++;
	return(es);
}
//��ѯ���ű��ص�ַ
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
	es = 23;//����û������
	return(es);
}

//�﷨������������������ɳ���
int TESTparse()
{
	int es = 0;
	if (fopen_s(&fp, Scanout, "r") != 0)
	{
		printf("\n��%s����!\n", Scanout);
		es = 10;
		return(es);
	}
	printf("������Ŀ���ļ���������·������");
	scanf_s("%s", Codeout, (unsigned int)sizeof(Codeout));
	if (fopen_s(&fout, Codeout, "w") != 0)
	{
		printf("\n����%s����!\n", Codeout);
		es = 10;
		return(es);
	}
	if (es == 0) es = program();
	printf("==�﷨������������������ɳ�����==\n");
	switch (es)
	{
	case 0: printf("�﷨����������ɹ��������������ɴ���!\n"); break;
	case 10: printf("���ļ� %sʧ��!\n", Scanout); break;
	case 1: printf("ȱ��{!\n"); break;
	case 2: printf("ȱ��}!\n"); break;
	case 3: printf("ȱ�ٱ�ʶ��!\n"); break;
	case 4: printf("�ٷֺ�!\n"); break;
	case 5: printf("ȱ��(!\n"); break;
	case 6: printf("ȱ��)!\n"); break;
	case 7: printf("ȱ�ٲ�����!\n"); break;
	case 21: printf("���ű����!\n"); break;
	case 22: printf("�����ظ�����!\n"); break;
	case 23: printf("����δ����!\n"); break;

	}
	fclose(fp);
	fclose(fout);
	return(es);
}

//�﷨���� ��program::={<declaration_list><statement_list>}
//����ǿ��TESTԴ����������������������ǰ����������ں�������Ҹô�������򽫳����쳣
int program()
{
	int es = 0, i;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "{"))//�ж��Ƿ�'{'
	{
		es = 1;
		return(es);
	}
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = declaration_list();
	if (es > 0) return(es);
	printf("     ���ű�\n");
	printf("     ����       ��ַ\n");
	for (i = 0; i < vartablep; i++)
		printf("      %s         %d\n", vartable[i].name, vartable[i].address);
	if (strcmp(token, "int") != 0) //���ﴦ���һ�����������ĵ�һ�����ʣ���������������Ա�֤��������ű�ʱ������ʾ��һ�����������ĸ�����
		printf("%s %s\n", token, token1);
	es = statement_list();
	if (es > 0) return(es);
	if (strcmp(token, "}"))//�ж��Ƿ�'}'
	{
		es = 2;
		return(es);
	}
	fprintf(fout, "        STOP\n");//����ָֹͣ��
	return(es);
}

//�﷨���� ��<declaration_list>::=
//<declaration_list><declaration_stat>|<declaration_stat>
//�ĳ�<declaration_list>::={<declaration_stat>}
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

//�﷨����<declaration_stat>::=int ID{, ID}|bool ID{, ID};
//����ģʽ��<�������>::= int ID {name-def��ID.name, int/bool��};
int declaration_stat()
{
	int es = 0;
	do
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		if (strcmp(token, "ID"))	es = 3;  //���Ǳ�ʶ��

		es = name_def(token1);//������ű�
		if (es > 0) return(es);

		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);

	} while (strcmp(token, ",") == 0);
	if (strcmp(token, ";"))	es = 4;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	if (strcmp(token, "int") == 0 || strcmp(token, "bool") == 0) //������뵥����int��bool�ʹ�ӡ��˵����һ����仹��������䣩����int����ӡ����һ������Ƿ�������䣩����֤��������ű�ʱ������ʾ��һ�����������ĸ�����
		printf("%s %s\n", token, token1);
	return(es);
}

//�﷨���� ��<statement_list>::=<statement_list><statement>|<statement>
//�ĳ�<statement_list>::={<statement>}
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

//�﷨���� ��<statement>::= <if_stat>|<while_stat>|<for_stat>
//                   |<compound_stat> |<expression_stat>

int statement()
{
	int es = 0;
	if (es == 0 && strcmp(token, "if") == 0) es = if_stat();//<IF���>
	if (es == 0 && strcmp(token, "do") == 0) es = do_stat();//<do���>
	if (es == 0 && strcmp(token, "while") == 0) es = while_stat();//<while���>
	if (es == 0 && strcmp(token, "for") == 0) es = for_stat();//<for���>
	//���ڴ˴����do������
	if (es == 0 && strcmp(token, "read") == 0) es = read_stat();//<read���>
	if (es == 0 && strcmp(token, "write") == 0) es = write_stat();//<write���>
	if (es == 0 && strcmp(token, "{") == 0) es = compound_stat();//<�������>
	if (es == 0 && (strcmp(token, "ID") == 0 || strcmp(token, "NUM") == 0 || strcmp(token, "(") == 0)) es = expression_stat();//<���ʽ���>
	return(es);
}

//�﷨����<if_stat>::= if (<expr>) <statement > [else < statement >]
/*
����ģʽ��
<if���>::= if ( <���ʽ> ){print(BRF label1)}  <���> {print(BR label2)} {print(label1:)} [else <���> ] {print(label2:)}
*/
int if_stat() {
	int es = 0, label1, label2;  //if
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "(")) 	return(es = 5);  //��������
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ")"))  return(es = 6); //��������
	label1 = labelp++;//��label1��ס����Ϊ��ʱҪת��ı��
	fprintf(fout, "        BRF LABEL%d\n", label1);//�����ת��ָ��
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);
	label2 = labelp++;//��label2��סҪת��ı��
	fprintf(fout, "        BR LABEL%d\n", label2);//���������ת��ָ��
	fprintf(fout, "LABEL%d:\n", label1);//����label1��ס�ı��
	if (strcmp(token, "else") == 0)//else���ִ���
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = statement();
		if (es > 0) return(es);
	}
	fprintf(fout, "LABEL%d:\n", label2);//����label2��ס�ı��
	return(es);
}

//<do_stat>::=do <statement> while (<expression>)
int do_stat() {
	int es = 0, label1, label2;
	label1 = labelp++;
	fprintf(fout, "LABEL%d:\n", label1);//����label1���
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);

	if (strcmp(token, "while") == 0) {
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		if (strcmp(token, "("))  return(es = 5);  //��������
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = expression();
		if (es > 0) return(es);
		if (strcmp(token, ")"))  return(es = 6); //��������
		/*es = while_stat();*/
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		if (strcmp(token, ";")) 	return(es = 4);  //�ٷֺ�
	}
	label2 = labelp++;
	fprintf(fout, "        BRF LABEL%d\n", label2);//�����ת��ָ��
	fprintf(fout, "        BR LABEL%d\n", label1);//���������ת��ָ��
	fprintf(fout, "LABEL%d:\n", label2);//����label2���
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1)); //��ȡ�ֺ� 
	printf("%s %s\n", token, token1);	
	return(es);
}

//�﷨����<while_stat>::= while (<expr >) < statement >
//����ģʽ��<while���>::= while {print(label1:) (<���ʽ >) {print(BRF label2)} <���>{print(BR label2)} {print(label2:)
int while_stat()
{
	int es = 0, label1, label2;
	label1 = labelp++;
	fprintf(fout, "LABEL%d:\n", label1);//����label1���
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))  return(es = 5);  //��������
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ")"))  return(es = 6); //��������
	label2 = labelp++;
	fprintf(fout, "        BRF LABEL%d\n", label2);//�����ת��ָ��
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);
	fprintf(fout, "        BR LABEL%d\n", label1);//���������ת��ָ��
	fprintf(fout, "LABEL%d:\n", label2);//����label2���
	return(es);
}

//�﷨����<for_stat>::= for(<expr>,<expr>,<expr>)<statement>
/*
����ģʽ��
<for���>::= for( <���ʽ> ; {print(label1:)<���ʽ> {print(BRF label2)}  {print(BR label3)}; {print(label4:)} <���ʽ> ){print(BR label1)} {print(label3:)<���>{print(BR label4)}  {print(label2:)
*/
int for_stat()
{
	int es = 0, label1, label2, label3, label4;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "("))  return(es = 5);  //��������
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	if (strcmp(token, ";")) 	return(es = 4);  //�ٷֺ�
	label1 = labelp++;
	fprintf(fout, "LABEL%d:\n", label1);//����label1���
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	label2 = labelp++;
	fprintf(fout, "        BRF LABEL%d\n", label2);//���������ת��ָ��
	label3 = labelp++;
	fprintf(fout, "        BR LABEL%d\n", label3);//���������ת��ָ��
	if (strcmp(token, ";"))  return(es = 4);  //�ٷֺ�
	label4 = labelp++;
	fprintf(fout, "LABEL%d:\n", label4);//����label4���
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0) return(es);
	fprintf(fout, "        BR LABEL%d\n", label1);//���������ת��ָ��
	if (strcmp(token, ")"))  return(es = 6); //��������
	fprintf(fout, "LABEL%d:\n", label3);//����label3���
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement();
	if (es > 0) return(es);
	fprintf(fout, "        BR LABEL%d\n", label4);//���������ת��ָ��
	fprintf(fout, "LABEL%d:\n", label2);//����label2���
	return(es);

}

//�﷨����<write_stat>::=write <expression>;
//����ģʽ��<write_���>::= write <���ʽ> {print(OUT)} ;

int write_stat()
{
	int es = 0;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = expression();
	if (es > 0)return(es);
	if (strcmp(token, ";"))  return(es = 4);  //�ٷֺ�
	fprintf(fout, "        OUT\n");//���ָ��
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	return(es);
}

//�﷨����<read_stat>::=read ID;
//����ģʽ��<read_���>::=read ID {ID.d = Look(ID.name)} {print(IN)} {print(STI ID.d)};

int read_stat()
{
	int es = 0, address;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, "ID"))  return(es = 3);  //�ٱ�ʶ��
	es = lookup(token1, &address);
	if (es > 0) return(es);
	fprintf(fout, "        IN   \n");//����ָ��
	fprintf(fout, "        STI   %d\n", address);//ָ��
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	if (strcmp(token, ";"))  return(es = 4);  //�ٷֺ�
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	return(es);
}

//�﷨����<compound_stat>::={<statement_list>} //�˴��Ļ�����Ϊ����
int compound_stat() {   //������亯��
	int es = 0;
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	es = statement_list();
	if (strcmp(token, "}"))  return(es = 2);  //��}
	fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
	printf("%s %s\n", token, token1);
	return(es);
}

//�﷨����<expression_stat>::=<expression>;|;
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
		return(es);//�ٷֺ�
	}
}
//�﷨����<���ʽ>::=ID=<�������ʽ> | <�������ʽ>
//����ģʽ��<���ʽ>::=ID {Look(ID.name)} {ASSIGN}=<�������ʽ>{print(STO ID.d)} | <�������ʽ>
//<expr>::=ID=<log_expr> | <log_expr>
int expression()
{
	int es = 0, fileadd;
	char token2[20], token3[40];
	if (strcmp(token, "ID") == 0)
	{
		fileadd = ftell(fp);   //��ס��ǰ�ļ�λ��
		fscanf_s(fp, "%s %s\n", token2, (unsigned int)sizeof(token2), token3, (unsigned int)sizeof(token3));
		if (strcmp(token2, "=") == 0)	//��ԭ��Ҫ������һ��
			printf("%s %s\n", token2, token3);//�����ܱ�֤��=���ظ�����������������=
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
			fseek(fp, fileadd, 0); //����'='���ļ�ָ��ص�'='ǰ�ı�ʶ��
			//printf("%s %s\n", token, token1);
			es = log_expr();
			if (es > 0) return(es);
		}
	}
	else es = log_expr();

	return(es);
}


//�﷨����<bool_expr>::=<additive_expr>
//       		    |< additive_expr >(>|<|>=|<=|==|!=)< additive_expr >
/*
����ģʽ��
<�������ʽ>::=<�������ʽ> |
| <�������ʽ> > <�������ʽ> {print(GT)}
| <�������ʽ> < <�������ʽ> {print(LES)}
| <�������ʽ> >=<�������ʽ> {print(GE)}
| <�������ʽ> <= <�������ʽ> {print(LE)}
| <�������ʽ> == <�������ʽ> {print(EQ)}
| <�������ʽ> !=<�������ʽ>  {print(NOTEQ)}

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
		//���������
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
		//���������
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

//�﷨����<additive_expr>::=<term>{(+|-)< term >} 
//����ģʽ��<�������ʽ>::=<��>{(+<��> {print(ADD)} | -<��> {print(SUB)} ) }

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

//�﷨����< term >::=<factor>{(*| /)< factor >} 
//����ģʽ��<��>::=<����>{(*<����> {print(MULT)} | / <����> {print(DIV)} ) }
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

//�﷨����< factor >::=(<additive_expr>)| ID|NUM
//����ģʽ��<����>::=(<�������ʽ>) | ID {ID.d = Look(ID.name)} {print(LOAD ID.d)} | NUM {print(LOADI NUM.val)}

int factor()
{
	int es = 0;

	if (strcmp(token, "(") == 0)
	{
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
		es = expression();
		if (es > 0) return(es);
		if (strcmp(token, ")"))  return(es = 6); //��������
		fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
		printf("%s %s\n", token, token1);
	}
	else
	{

		if (strcmp(token, "ID") == 0)
		{
			int flag = 0; //�ж��Ƿ����������ʽ
			int address;
			es = lookup(token1, &address);//����ű���ȡ������ַ
			if (es > 0) return(es);//����û����
			fscanf_s(fp, "%s %s\n", token, (unsigned int)sizeof(token), token1, (unsigned int)sizeof(token1));
			printf("%s %s\n", token, token1);
			if (strcmp(token, "++") == 0) {  // ������������ʽ
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
				es = 7;//ȱ�ٲ�����
				return(es);
			}
	}
	return(es);
}