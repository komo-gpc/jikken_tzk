#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char outtoken[200][10];  //トークン出力用配列
int tnum;

char token[20];
char res[13][20]={"begin","end","if","then","while","do",
				  "return","function","var","const","odd",
				  "write","writeln"};
char sign[17][10]={"+","-","*","/","(",")","=","<",">",
			      "<>","<=",">=",",",".",";",":="};
FILE *fp;
//関数宣言
void nexttoken(int);	//次のトークンを読み込む関数
int ch(char[]);	//引数の文字列との比較　同じ:1　違う:0
int ttype();	//トークンのタイプを返す 1:予約語 2:記号
				//						3:名前   4:整数
void exit_func();	//構文解析エラー　強制終了
void backtoken();	//現在のトークンの番号を1つ前に戻す

void block(), varDecl(), constDecl(), funcDecl(),
	 statement(), condition(), expression(), term(),
	 factor();              //構文規則を指定
void get_token();
int check(char);

int main(int argc, char **argv)
{
	get_token();  //1週目で行ったトークン分割プログラム
	tnum=0;
	block();
	nexttoken(1);
	if(ch(".")) printf("Complete\n");
	else printf("Syntax Error\n");

	return 0;
}

void block()
{
	int c;
	c=0;
	nexttoken(0);
	while(ch("const")||ch("var")||ch("function"))
	{
		c++;
		if(ch("const"))
			constDecl();
		else if(ch("var"))
			varDecl();
		else if(ch("function"))	
			funcDecl();
	}
	if(!c) backtoken();
	statement();
}

void varDecl()
{
	nexttoken(1);
	if(ttype()!=3) exit_func("verdecl");
	nexttoken(1);
	while(ch(","))
	{
		nexttoken(1);
		if(ttype()==3) nexttoken(1);
		else exit_func("verdecl");
	}
	if(ch(";")){}
	else exit_func("verdecl");
}

void funcDecl()
{
	nexttoken(3);
	if(ttype()!=3)
		exit_func("funcdecl"); 
	nexttoken(3);
	if(!ch("("))
		exit_func("funcdecl");
	nexttoken(3);
	if(ttype()!=3) 
			exit_func("funcdecl"); 
	nexttoken(3);
	while(ch(","))
	{
		nexttoken(3);
		if(ttype()==3)
			nexttoken(3);
		else exit_func("funcdecl");
	}
	if(ch(")"))
		block();
	else exit_func("funcdec3");
	nexttoken(3);
	if(ch(";")) 
		nexttoken(3);
	else exit_func("funcdec4");
}

void constDecl()
{
	do{
		nexttoken(2);
		if(ttype()!=3)
			exit_func("constdecl");
		nexttoken(2);
		if(!ch("="))
			exit_func("constdecl");
		nexttoken(2);
		if(ttype()!=4)	
			exit_func("constdecl");
		nexttoken(2);
	}while(ch(","));
	if(ch(";"))
		nexttoken(2);
	else exit_func("constdecl");
}

void statement()
{
	nexttoken(4);
	/*ident*/
	if(ttype()==3)
	{
		nexttoken(4);
		if(ch(":=")==0)
			exit_func("Syntax Error");
		expression();
	}else if(ch("begin"))
	{
		statement();
		nexttoken(4);
		while(ch(";")==1)
		{
			statement();
			nexttoken(4);
		}
		if(ch("end")==0)
			exit_func("Syntax Error");
	}else if(ch("if"))
	{
		condition();
		nexttoken(4);
		if(ch("then")==0)
			exit_func("Syntax Error");
		statement();
	}else if(ch("while"))
	{
		condition();
		nexttoken(4);
		if(ch("do")==0)
			exit_func("Syntax Error");
		statement();
	}else if(ch("return"))
	{
		expression();
	}else if(ch("write"))
	{
		expression();
	}else if(ch("writeln"))
	{
	}else{   //何もせずにスルー 
		backtoken();
	}
}

void condition()
{
	nexttoken(5);
	if(ch("odd")==1)
		expression();
	else
		backtoken();
	expression();
	nexttoken(5);
	if(ch("=")==1 || ch("<>")==1 || ch("<")==1 || ch(">")==1 || ch("<=")==1 || ch(">=")==1)
	{
		expression();
	}else
	{
		exit_func("Syntax Error");
	}
}

void expression()
{
	nexttoken(6);
	if(ch("+")==1 || ch("-")==1)
	{}else backtoken();
	term();
	while(1)
	{
		nexttoken(6);
		if(ch("+")==1 || ch("-")==1)
		{
			term();
		}else
		{
			backtoken();
			break;
		}
	}
}

void term()
{
	factor();
	while(1)
	{
		nexttoken(7);
		if(ch("*")==1 || ch("/")==1)
		{
			factor();
		}else
		{
			backtoken();
			break;
		}
	}
}

void factor()
{
	nexttoken(8);
	if(ttype()==4)
	{}
	else if(ttype()==3)
	{
		nexttoken(8);
		if(ch("(")==1)
		{
			nexttoken(8);
			if(ch(")")==0)
			{
				backtoken();
				expression();
				while(1)
				{
					nexttoken(8);
					if(ch(",")==1)
						expression();
					else if(ch(")"))
					{
						break;
					}
					else exit_func("Syntax Error");
				}
			}else if(ch(")")==1){}
		}else
		{
			backtoken();
		}
	}else if(ch("(")==1)
	{
		expression();
		nexttoken(8);
		if(ch(")")==0)
			exit_func("Syntax Error");
		else {};
	}else exit_func("Syntax Error");
}

void nexttoken(int s)
{
	char state[30];
	switch(s){
		/*block*/
		case 0: sprintf(state,"block");
				break;
		/*constDecl*/
		case 1: sprintf(state, "constDecl");
				break;
		case 2: sprintf(state, "verDecl");
				break;
		case 3: sprintf(state, "funcDecl");
				break;
		case 4: sprintf(state, "statement");
				break;
		case 5: sprintf(state, "condition");
				break;
		case 6: sprintf(state, "expression");
				break;
		case 7: sprintf(state, "term");
				break;
		case 8: sprintf(state, "factor");
				break;
	}
	strcpy(token,outtoken[tnum]);
	tnum++;
	printf("%-13s : [%s]\n",state,token);
}

/*現在のトークンと代入された文字列とを比較し、同じなら1,違うなら0を返す*/
int ch(char str[20])
{
	if(strcmp(token,str)==0)
		return 1;
	else return 0;
}


int ttype()	//トークンのタイプを返す
{
	int i,c;
	//予約語
	for(i=0;i<13;i++)
	{
		c=strcmp(token,res[i]);
		if(c==0)
		{
			return 1;
			break;
		}
	}
	//記号
	for(i=0;i<16;i++){
		c=strcmp(token,sign[i]);
		if(c==0){
			return 2;
			break;
		}
	}
	//整数
	if(token[0] >='0' && token[0]<='9')
		return 4;
	else
		//名前　ident
		return 3;
}

void backtoken() //トークン番号を1つ前に戻す
{
	tnum--;
}

void exit_func(char str[20])  //文法規則に合わないため終了
{
	printf("Syntax Error Near the Word \"%s\"\n",token);
	printf("%s\n",str);
	exit(1);  //強制終了
}


void get_token()
{
	fp = fopen("program1.txt","r");
	if(fp==NULL)
	{
		printf("file open error\n");
	}
	
	
	char c;
	//char token[256][256];
	int num,i,j;
	//動的確保がしたい？
	num = i = 0;
	
	while(1)
	{
		c=fgetc(fp);
		j=check(c);
		//空白等
		if(j==0)
		{
			if(i!=0)
			{
				outtoken[num][i]='\0';
				i=0;
				num++;
			}
		}
		//記号
		else if(j==1)
		{
			if(i!=0)
			{
				i=0;
				num++;
			}
			outtoken[num][i]=c;
			i=0;
			num++;
		}
		//二文字記号
		else if(j==2)
		{
			if(i!=0)
			{
				i=0;
				num++;
			}
			outtoken[num][i]=c;
			c=fgetc(fp);
			outtoken[num][i+1]=c;
			num++;
			i=0;
		}
		//数字
		else if(j==3)
		{
			outtoken[num][i]=c;
			i++;
		}
		//文字
		else if(j==4)
		{
			if(outtoken[num][0]>='0' && outtoken[num][0]<='9')
			{
				num++;
				i=0;
			}
			outtoken[num][i]=c;
			i++;
		}
		else if(j==5)
			break;
	}
	fclose(fp);
}

int check(char c)
{
	//空白,改行,タブ return 0
	if(c==' ' || c=='\n' || c=='\t')
		return 0;
	//記号(通常) return 1
	else if(c=='+' || c=='-' || c=='*' || c=='/' || c=='(' || c==')' || c=='=' || c==',' || c=='.' || c==';')
		return 1;
	//記号(例外) return 2
	else if(c=='<')
	{
		c=fgetc(fp);
		if(c!='>' && c!='=')
		{
			fseek(fp,-1,1);
			return 1;
		}
		else
		{
			fseek(fp,-1,1);
			return 2;
		}
	}else if(c=='>' || c==':')
	{
		c=fgetc(fp);
		if(c!='=')
		{
			fseek(fp,-1,1);
			return 1;
		}
		else
		{
			fseek(fp,-1,1);
			return 2;
		}
	}
	//数字 return 3
	else if(c>='0' && c<='9')
		return 3;
	//文字 return 4
	else if(c>='a' && c<='z')
		return 4;
	//エラー
	else if(c==EOF)
		return 5;
	else return 6;
}

