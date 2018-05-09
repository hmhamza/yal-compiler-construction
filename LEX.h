#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<fstream>
#include <algorithm>

using namespace std;

int start,state;
int init=0,curr;
vector <string> KEYWORDS;
char *File;

struct LexemeData{
	int index;
	string lexeme;
};

vector <string> TOKENS;
vector <string> LEXEMES;

struct multiLevelSymbolTableData{
	string ID;
	string Type;
	int relAdd;
};

struct FUNC_DATA{
	int start;
	int end;
	int no_of_params;
	int no_of_local_vars;
	int total_size;

	vector<multiLevelSymbolTableData> MULTILEVEL_SYMBOL_TABLE;
};

struct symbolTableData{
	string ID;
	string Type;
	int relAdd;
	
	FUNC_DATA FN;
};

vector<symbolTableData> SYMBOL_TABLE;				//ID, Type, RelativeAddress
int st=0;

int addToSymbolTable(string id){
	
	for(int i=0;i<st;i++)
		if(SYMBOL_TABLE.at(i).ID==id)
			return i;
	
	symbolTableData temp;
	temp.ID=id;
	temp.Type="-1";
	temp.relAdd=-1;
	SYMBOL_TABLE.push_back(temp);

	return st++;
}

bool isLetter(char ch){
	if((ch>='A'&&ch<='Z') || (ch>='a'&&ch<='z'))
		return true;

	return false;

}

bool isDigit(char ch){
	if(ch>='0'&&ch<='9')
		return true;

	return false;

}

bool Failure(){

	switch(start){

	case 101:
		start=201;
		break;

	case 201:
		start=301;
		break;

	case 301:
		start=401;
		break;

	case 401:
		start=501;
		break;

	case 501:
		start=-1;
		state=start;
		curr=init;
		return false;

	}

	state=start;
	curr=init;
	return true;

}

string GetToken(string s){

	if(s==";" || s=="[" || s=="]" || s=="(" || s==")" || s==",")
		return "NULL";
	

	switch(start){

	case 101:
		for(int i=0;i<(signed)KEYWORDS.size();i++)
			if(KEYWORDS.at(i)==s)
				return "NULL";		//return "KW";

		return "ID";


	case 201:
		if(state==209)
			return "EQ";
		return "RO";

	case 301:
		if(state==311)
			return "CMMNT";
		
		return "NULL";							//For both, Assignment Operators + Comment
	case 401:
		return "NUM";

	case 501:
		return "CHAR";

	case -1:
		return "Invalid";

	default:
		return "Invalid";
	}
}

bool Run_DFAS(ifstream& fin){

	char ch;
	//bool isFirstLetter=true;
	fin.seekg(curr,ios::beg);

	//while(!fin.eof()){
	while(true){

		if(!fin.eof()){
			fin.get(ch);
			curr++;
		}
		else
			ch='#';

		//if(isFirstLetter&&(ch==';'||ch==','||ch=='['||ch==']'||ch=='('||ch==')')){
		if(state==101 && (ch==';'||ch==','||ch=='['||ch==']'||ch=='('||ch==')')){
			start=501;
			return false;
		}

		//isFirstLetter=false;

		switch(state){

		case 101:
			if(isLetter(ch)){
				state=102;
				break;
			}
			else 
				return false;

		case 102:
			if(isLetter(ch) ||isDigit(ch))
				state=102;
			else
				state=103;
			break;

		case 103:
			curr-=3;
			return true;



			/**********      RELATIONAL OPERATORS + ASSIGNMENT OPERATOR      **********/

		case 201:
			if(ch=='=')
				state=202;
			else if(ch=='<')
				state=207;
			else if(ch=='>')
				state=208;
			else 
				return false;

			break;

		case 202:

			if(ch=='/')
				state=204;
			else if(ch=='<')
				state=205;
			else if(ch=='>')
				state=206;
			else
				state=203;
			break;

		case 203:
			curr-=3;
			return true;

		case 204:
			curr-=2;
			return true;

		case 205:
			curr-=2;
			return true;

		case 206:
			curr-=2;
			return true;

		case 207:
			if(ch=='-')
				state=209;
			else
				state=210;
			break;

		case 208:
			curr-=2;
			return true;

		case 209:
			curr-=2;
			return true;

		case 210:
			curr-=3;
			return true;





			/**********      ARITHMATIC OPERATORS  +  COMMENTS     **********/

		case 301:
			if(ch=='+')
				state=302;
			else if(ch=='-')
				state=303;
			else if(ch=='/')
				state=304;
			else if(ch=='*')
				state=305;
			else
				return false;

			break;

		case 302:
			curr-=2;
			return true;

		case 303:
			curr-=2;
			return true;

		case 304:
			curr-=2;
			return true;

		case 305:
			if(ch=='*')
				state=307;
			else
				state=306;

			break;

		case 306:
			curr-=3;
			return true;

		case 307:
			if(ch=='*'){
				state=308;
				break;
			}
			else
				return false;

		case 308:
			if(ch=='*')
				state=309;
			else
				state=308;
			break;

		case 309:
			if(ch=='*')
				state=310;
			else
				state=308;
			break;

		case 310:
			if(ch=='*')
				state=311;
			else
				state=308;
			break;

		case 311:
			curr-=2;
			return true;




			/**********      INTEGER LITERALS      **********/

		case 401:
			if(isDigit(ch)){
				state=402;
				break;
			}
			else
				return false;

		case 402:
			if(isDigit(ch))
				state=402;
			else 
				state=403;

			break;

		case 403:
			curr-=3;
			return true;





			/**********      CHARACTER LITERALS      **********/

		case 501:
			if(ch=='\''){
				state=502;
				break;
			}
			else
				return false;

		case 502:
			state=503;
				break;
			/*if(isLetter(ch)){
				state=503;
				break;
			}
			else 
				return false;*/

		case 503:
			if(ch=='\''){
				state=504;
				break;
			}
			else
				return false;

		case 504:
			curr-=2;
			return true;

		default:
			curr-=2;
			return true;

		}
	}

}

void POPULATE_KEYWORDS(){

	KEYWORDS.push_back("function");
	KEYWORDS.push_back("integer");
	KEYWORDS.push_back("character");
	KEYWORDS.push_back("void");
	KEYWORDS.push_back("returns");
	KEYWORDS.push_back("return");
	KEYWORDS.push_back("begin");
	KEYWORDS.push_back("end");
	KEYWORDS.push_back("while");
	KEYWORDS.push_back("if");
	KEYWORDS.push_back("else");
	KEYWORDS.push_back("print");
	KEYWORDS.push_back("input");

}

void REFRESH(ifstream& fin){

	int xn=0;
	char ch;

	fin.seekg(init,ios::beg);
	fin.get(ch);

	while(ch==' '||ch=='\t'||ch=='\n')
		fin.get(ch);
	

	init=(int)fin.tellg()+xn;

	init--;
	curr=init;

	start=101;
	state=start;

}

void OUTPUT(ofstream& fout){

	string token,lexeme;
	int j,k,xn=0;
	char ch;

	ifstream fin(File);
	lexeme.clear();
	fin.seekg(init,ios::beg);
	for(j=init,k=0;j<=curr;j++){
		fin.get(ch);
		if(ch!='\n')
			lexeme.insert(k++,1,ch);
		else
			xn++;
	}
	curr+=xn;
	if(lexeme.at(k-1)=='\n')
		token.erase(k-1,1);

	token=GetToken(lexeme);

	if(token=="CMMNT")					//Ignoring Comments in the Lex
		return;

	if(token=="NULL"){
		//cout<<"( "<<sub_str<<" , "<<lexeme<<" )\n\n";
		fout<<"("<<lexeme<<","<<token<<")\n";

		TOKENS.push_back(lexeme);
		//LexemeData nl;
		//nl.index=-1;
		//nl.lexeme=token;
		LEXEMES.push_back(token);
	}
	else{
		//cout<<"("<<lexeme<<", "<<sub_str<<" )\n\n";
		fout<<"("<<token<<","<<lexeme<<")\n";

		//LexemeData nl;
		//nl.index=-1;
		//nl.lexeme=lexeme;
		if(token=="ID")
			//nl.index=addToSymbolTable(lexeme);
			addToSymbolTable(lexeme);

		TOKENS.push_back(token);
		LEXEMES.push_back(lexeme);
	}
}

void run_LEX(char File[]){

	bool failResult;
	
	POPULATE_KEYWORDS();

	ifstream fin(File);
	ofstream fout3("Lex Output.txt");

	while(!fin.eof()){
		
		REFRESH(fin);

		failResult=true;
		while(failResult && !Run_DFAS(fin) )
			failResult=Failure();

		OUTPUT(fout3);

		init=curr+1;
		
	}
	
	fout3.close();

}
