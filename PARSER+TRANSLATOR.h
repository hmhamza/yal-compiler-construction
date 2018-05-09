#include "LEX.h"

using namespace std;

void BD();
void E();
int AV();
string FC(string);

string current;
string lexeme;

int curr1=0,spaces=0,tmpCount=0,funcIndex,funcRel,nop;
bool funcFlag,isFuncDef=false;

ofstream fout_P("Parse Tree.txt");

int n=0,rel=0;
string temp;

vector<string> ALL_EMITS;
vector<int> toBeRemovedEntriesFmSymblTbl;

enum OPERATION { ADDITION=1,
	SUBTRACTION=2,
	MULTIPLICATION=3,
	DIVISION=4,
	GOTO=5,
	IN=6,
	OUT=7,
	EQUAL_TO=8,
	NOT_EQUAL_TO=9,
	GREATER_THAN=10,
	LESS_THAN=11,
	GREATER_THAN_OR_EQUAL_TO=12,
	LESS_THAN_OR_EQUAL_TO=13,
	ASSIGNMENT=14,
	CALL=15,
	ARG=16,
    RET=17};

struct OPinfo{
	OPERATION OPcode;
	int Operand1;
	string O1_Type;
	int Operand2;
	string O2_Type;
	int Target;

	int size;
};

vector <OPinfo>QUAD;

int getSymbolTableIndex(string lexeme){

	for(int i=0;i<SYMBOL_TABLE.size();i++){
		if(SYMBOL_TABLE.at(i).ID==lexeme)
			return i;
	}

	return 0;
}

int getRelAddFrmSymblTbl(string lexeme){

	if(isFuncDef){

		for(int i=0;i<SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.size();i++)
			if(SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.at(i).ID==lexeme){
				return SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.at(i).relAdd;
			}
	}

	else{

		for(int i=0;i<SYMBOL_TABLE.size();i++){
			if(SYMBOL_TABLE.at(i).ID==lexeme)
				return SYMBOL_TABLE.at(i).relAdd;
		}

	}
	return -1;
}

string getTypeFrmSymblTbl(int relAdd){

	for (int i = 0; i<SYMBOL_TABLE.size(); i++){
		if (SYMBOL_TABLE.at(i).relAdd == relAdd)
			return SYMBOL_TABLE.at(i).Type;
	}

	return 0;
}

string newTmp(){
	string ret;
	ret.clear();
	ret.append("t");

	char tn[10];
	itoa(tmpCount,tn,10);
	ret.append(tn);
	tmpCount++;

	if(isFuncDef){
		int lexemeIndex=getSymbolTableIndex(lexeme);

		multiLevelSymbolTableData temp;
		temp.ID=ret;
		temp.Type="TMP";
		temp.relAdd=funcRel;
		SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.push_back(temp);
		funcRel+=4;

		st++;
	}
	else{
		symbolTableData sd;
		sd.ID=ret;
		sd.Type="TMP";
		sd.relAdd=rel;
		SYMBOL_TABLE.push_back(sd);
		st++;

		rel+=4;
	}

	return ret;

}

void EMIT(string str="",int p=-1){

	char cn[10];
	itoa(n,cn,10);

	ALL_EMITS.push_back("");
	ALL_EMITS.at(n).append(cn);
	ALL_EMITS.at(n).append(") ");
	if(str=="")		
		ALL_EMITS.at(n).append(temp);

	else{
		if(p!=-1)
			str.append(itoa(p,cn,10));

		ALL_EMITS.at(n).append(str);
	}

	n++;
}

void BACKPATCH(int d){

	char cn[10];
	itoa(n,cn,10);

	ALL_EMITS.at(d).append(cn);

}

void printSpaces(){

	fout_P<<"\n";

	for(int i=0;i<spaces-1;i++)
		fout_P<<"|   ";

	if(spaces!=0)
		fout_P<<"|---";

	spaces++;
}

void ERROR(){

	fout_P<<"Invalid String\n";
}

string nextToken(){

	if(++curr1<TOKENS.size()){
		lexeme=LEXEMES.at(curr1);
		return TOKENS.at(curr1);
	}
	else
		return "FINISH";
}

void match(string token){


	if(token==current){
		printSpaces();
		spaces--;
		fout_P<<token;
		if(lexeme!="NULL")
			fout_P<<" ( "<<lexeme<<" )";

		//      FOR TRANSLATOR    //
		if(lexeme!="NULL"){
			if(lexeme=="<-")
				temp.append("=");
			else if(lexeme=="=<")
				temp.append("<=");
			else if(lexeme=="=>")
				temp.append(">=");
			else if(lexeme=="=")
				temp.append("==");
			else if(lexeme=="=/")
				temp.append("!=");
			else
				temp.append(lexeme);

		}
		else{
			if(token=="print")
				temp.append("out");
			else if(token=="while")
				temp.append("if");
			else if(token=="return")
				temp.append("ret");
			else
				temp.append(token);
		}
		temp.append(" ");
		/////////////////////////////////////////

		current=nextToken();
	}
	else
		ERROR();

}

bool B(){
	printSpaces();
	fout_P<<"B";

	if(current=="["){
		match("[");
		match("]");
		
		return true;
	}
	else{
		;									//NULL

		return false;
	}


	spaces--;


}

void T1(string &type,int &size){
	printSpaces();
	fout_P<<"T1";


	if(current=="integer"){
		match("integer");
		type="INT";
		//size=4;
		size=1;
	}

	else if(current=="character"){
		match("character");
		type="CHR";
		size=1;
	}
	else 
		ERROR();


	spaces--;

}

void T2(){
	printSpaces();
	fout_P<<"T2";

	string ts;int ti;
	if(current=="integer" ||current=="character")
		T1(ts,ti);
	else if(current=="void")
		match("void");
	else 
		ERROR();


	spaces--;

}

void A2(){
	printSpaces();
	fout_P<<"A2";

	string ts;int ti;

	if(current==","){
		nop++;

		match(","); 
		T1(ts,ti);

		multiLevelSymbolTableData temp;
		temp.Type=ts;
		temp.relAdd=funcRel;

		if(B())
			funcRel+=50;
		else
			funcRel+=ti;

		temp.ID=lexeme;
		SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.push_back(temp);

		toBeRemovedEntriesFmSymblTbl.push_back(getSymbolTableIndex(temp.ID));

		match("ID");
		A2(); 
	}
	else 
		;						//NULL


	spaces--;

}

void A1(){
	printSpaces();
	fout_P<<"A1";

	string ts;int ti;		

	if(current!=")"){
		nop++;

		T1(ts,ti); 
		
		multiLevelSymbolTableData temp;
		temp.Type=ts;
		temp.relAdd=funcRel;

		if(B())
			funcRel+=50;
		else
			funcRel+=ti;

		temp.ID=lexeme;
		SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.push_back(temp);

		toBeRemovedEntriesFmSymblTbl.push_back(getSymbolTableIndex(temp.ID));

		match("ID"); 
		A2(); 
	}
	else
		;						//NULL


	spaces--;
}

void FD(){
	printSpaces();
	fout_P<<"FD";

	isFuncDef=true;funcRel=0;

	match("function"); 
A:

	funcIndex=getSymbolTableIndex(lexeme);
	SYMBOL_TABLE.at(funcIndex).Type="FUN";
	SYMBOL_TABLE.at(funcIndex).relAdd=-1; 
	SYMBOL_TABLE.at(funcIndex).FN.start=n; 		

	match("ID"); 

	nop=0;										//Initializing no of parameters count to 0

	match("("); 

	A1(); 

	match(")");
	
	SYMBOL_TABLE.at(funcIndex).FN.no_of_params=nop; 		

	match("returns");
	T2(); 
	match("begin"); 

	//int end=n;
	BD();
	//end=n-end;
	//SYMBOL_TABLE.at(funcIndex).FN.end=end;
	SYMBOL_TABLE.at(funcIndex).FN.no_of_local_vars=SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.size()-nop;
	SYMBOL_TABLE.at(funcIndex).FN.total_size=SYMBOL_TABLE.at(funcIndex).FN.no_of_local_vars+SYMBOL_TABLE.at(funcIndex).FN.no_of_params;
	SYMBOL_TABLE.at(funcIndex).FN.end=n-1; 		

	match("end");

	isFuncDef=false;

	rel=rel+SYMBOL_TABLE.at(funcIndex).FN.no_of_local_vars+SYMBOL_TABLE.at(funcIndex).FN.no_of_params;

	spaces--;
}

int NI(){
	printSpaces();
	fout_P<<"NI";

	int nir=-1;
	if(current=="NUM"){
		nir=stoi (lexeme,nullptr,10);
		match("NUM");
	}
	else if(current=="ID")
		match("ID");
	else
		ERROR();	

	return nir;

	spaces--;							
}

int D(){
	printSpaces();
	fout_P<<"D";

	int Dr=-1;
	if(current=="["){
		match("["); 
		Dr=NI();  
		match("]");
	}
	else
		;								//NULL

	return Dr;

	spaces--;
}

int VAL(string &type){
	printSpaces();
	fout_P << "VAL";

	int rAdd = -1;
	if (current == "ID"){
		rAdd = getRelAddFrmSymblTbl(lexeme);
		match("ID");
		D();
		type = "ID";
	}
	else if (current == "NUM"){
		type = lexeme;
		match("NUM");
		//type = "NUM";
	}
	else if (current == "CHAR"){
		type = lexeme;
		match("CHAR");
		//type = "CHAR";
	}

	else
		ERROR();

	return rAdd;

	spaces--;
}

int AV(){
	printSpaces();
	fout_P<<"AV";

	int avr=-1;
	if(current=="["){
		match("[");

		avr=stoi (lexeme,nullptr,10);

		match("NUM");
		match("]"); 
	}
	else if(current=="EQ"){
		match("EQ");
		string type;
		VAL(type);
	}
	else
		;						//NULL

	return avr;

	spaces--;
}

void M(string type,int size){
	printSpaces();
	fout_P<<"M";

	int vdr;

	if(current==","){
		match(",");
		if(isFuncDef){
			multiLevelSymbolTableData temp;

			int lexemeIndex=getSymbolTableIndex(lexeme);

			temp.ID=SYMBOL_TABLE.at(lexemeIndex).ID;
			temp.Type=type;
			temp.relAdd=funcRel;
			SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.push_back(temp);
			funcRel+=size;

			toBeRemovedEntriesFmSymblTbl.push_back(lexemeIndex);
			//SYMBOL_TABLE.erase(SYMBOL_TABLE.begin()+lexemeIndex);

			match("ID");

			vdr=AV();

			if(vdr!=-1)
				funcRel=funcRel+vdr-1;
		}
		else{
			int lexemeIndex=getSymbolTableIndex(lexeme);

			SYMBOL_TABLE.at(lexemeIndex).Type=type;
			SYMBOL_TABLE.at(lexemeIndex).relAdd=rel; 
			rel+=size;

			match("ID");

			vdr=AV();

			if(vdr!=-1)
				rel=rel+vdr-1;
		}

	 
		M(type,size);
	}
	else
		;						//NULL


	spaces--;
}

void VD(){
	printSpaces();
	fout_P<<"VD";

	string type;int size,vdr;
	T1(type,size); 
A:
	if(isFuncDef){
		int lexemeIndex=getSymbolTableIndex(lexeme);

		multiLevelSymbolTableData temp;
		temp.ID=SYMBOL_TABLE.at(lexemeIndex).ID;
		temp.Type=type;
		temp.relAdd=funcRel;
		SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.push_back(temp);
		funcRel+=size;

		toBeRemovedEntriesFmSymblTbl.push_back(lexemeIndex);
		//SYMBOL_TABLE.erase(SYMBOL_TABLE.begin()+lexemeIndex);

		match("ID");

		vdr=AV();

		if(vdr!=-1)
			funcRel=funcRel+vdr-1;

	}
	else{
		int lexemeIndex=getSymbolTableIndex(lexeme);

		SYMBOL_TABLE.at(lexemeIndex).Type=type;
		SYMBOL_TABLE.at(lexemeIndex).relAdd=rel; 
		rel+=size;

		match("ID");

		vdr=AV();

		if(vdr!=-1)
			rel=rel+vdr-1;
	}


	  
	M(type,size);
	match(";");


	spaces--;
}

void OP(){

	printSpaces();
	fout_P<<"OP";


	if(current=="+")
		match("+");
	else if(current=="-")
		match("-");
	else if(current=="*")
		match("*");
	else if(current=="/")
		match("/");
	else
		ERROR();


	spaces--;
}

string Y(){
	printSpaces();
	fout_P<<"Y";

	string n="-1";
	if(current=="NUM"){
A12:
		n=lexeme;
		match("NUM");

		//type="NUM";


	}
	else if(current=="ID"){
A13:
		n=lexeme;
		string funcLex=lexeme;
		match("ID");
		D(); 
		if(current=="("){
			funcFlag=false;
			n=FC(funcLex);

		}
	}
	else if(current=="CHAR"){
A14:
		n=lexeme;
		match("CHAR");

		//type="CHAR";
	}
	else
		ERROR();


	spaces--;	

	return n;
}

string Z(string i){
	printSpaces();
	fout_P<<"Z";

	string s;
	if(current=="*"){
		match("*");
		string tn=Y();
A11:
		string i1=newTmp();
		temp.clear();
		temp.append(i1);
		temp.append("=");
		temp.append(i);
		temp.append("*");
		temp.append(tn);
		EMIT(temp);

		OPinfo o;										//for Quadruple
		o.OPcode=MULTIPLICATION;						//for Quadruple
		o.Operand1=getRelAddFrmSymblTbl(i);				//for Quadruple
		o.O1_Type="ID";

		if(o.Operand1==-1)
			o.O1_Type=i;

		o.Operand2=getRelAddFrmSymblTbl(tn);			//for Quadruple
		o.O2_Type="ID";

		if(o.Operand2==-1)
			o.O2_Type=tn;

		o.Target=getRelAddFrmSymblTbl(i1);				//for Quadruple
		QUAD.push_back(o);								//for Quadruple

		////////////////////////

		string s1=Z(i1);
A12:
		s=s1;
		///////////////////////

	}
	else if(current=="/"){
		match("/");
		string tn=Y();
A13:
		string i1=newTmp();
		temp.clear();
		temp.append(i1);
		temp.append("=");
		temp.append(i);
		temp.append("/");
		temp.append(tn);
		EMIT(temp);

		OPinfo o;										//for Quadruple
		o.OPcode=DIVISION;								//for Quadruple

		o.Operand1=getRelAddFrmSymblTbl(i);				//for Quadruple
		o.O1_Type="ID";

		if(o.Operand1==-1)
			o.O1_Type=i;

		o.Operand2=getRelAddFrmSymblTbl(tn);			//for Quadruple
		o.O2_Type="ID";

		if(o.Operand2==-1)
			o.O2_Type=tn;

		o.Target=getRelAddFrmSymblTbl(i1);				//for Quadruple
		QUAD.push_back(o);								//for Quadruple

		////////////////////////

		string s1=Z(i1);
A14:
		s=s1;
		///////////////////////

	}
	else{
		;
A15:
		s=i;
		///////////////////////
	}
	spaces--;

	return s;
}

string T(){
	printSpaces();
	fout_P<<"T";

A9:	string i=Y();

A10:	string n=Z(i);


	spaces--;

	return n;
}

string Q(string i){
	printSpaces();
	fout_P<<"Q";

	string s;
	if(current=="+"){
		match("+");
		string tn=T();
A4:
		string i1=newTmp();
		temp.clear();
		temp.append(i1);
		temp.append("=");
		temp.append(i);
		temp.append("+");
		temp.append(tn);
		EMIT(temp);

		OPinfo o;										//for Quadruple
		o.OPcode=ADDITION;								//for Quadruple
		o.Operand1=getRelAddFrmSymblTbl(i);				//for Quadruple
		o.O1_Type="ID";

		if(o.Operand1==-1)
			o.O1_Type=i;

		o.Operand2=getRelAddFrmSymblTbl(tn);			//for Quadruple
		o.O2_Type="ID";

		if(o.Operand2==-1)
			o.O2_Type=tn;

		o.Target=getRelAddFrmSymblTbl(i1);				//for Quadruple
		QUAD.push_back(o);								//for Quadruple

		////////////////////////

		string s1=Q(i1);
A5:
		s=s1;
		///////////////////////

	}
	else if(current=="-"){
		match("-");
		string tn=T();
A6:
		string i1=newTmp();
		temp.clear();
		temp.append(i1);
		temp.append("=");
		temp.append(i);
		temp.append("-");
		temp.append(tn);
		EMIT(temp);

		OPinfo o;										//for Quadruple
		o.OPcode=SUBTRACTION;							//for Quadruple
		
		o.Operand1=getRelAddFrmSymblTbl(i);				//for Quadruple
		o.O1_Type="ID";

		if(o.Operand1==-1)
			o.O1_Type=i;

		o.Operand2=getRelAddFrmSymblTbl(tn);			//for Quadruple
		o.O2_Type="ID";

		if(o.Operand2==-1)
			o.O2_Type=tn;

		o.Target=getRelAddFrmSymblTbl(i1);				//for Quadruple
		QUAD.push_back(o);								//for Quadruple

		////////////////////////

		string s1=Q(i1);
A7:
		s=s1;
		///////////////////////

	}
	else{
		;
A8:
		s=i;
		///////////////////////
	}
	spaces--;

	return s;
}

string G(){
	printSpaces();
	fout_P<<"G";

A2:	string i=T();

A3:	string n=Q(i);

	spaces--;

	return n;
}

void VAO (){
	printSpaces();
	fout_P<<"VAO";

	string currtemp=temp,idLex=temp;
	idLex.erase(idLex.size()-1);

	int dr=D(); 
	match("EQ");

	string n=G();

	currtemp.append("=");
	currtemp.append(n);
	EMIT(currtemp);

	OPinfo o;											//for Quadruple
	o.OPcode=ASSIGNMENT;								//for Quadruple
	o.Operand1=getRelAddFrmSymblTbl(idLex);				//for Quadruple

	if(dr!=-1)
		o.Operand1+=dr;

	o.Operand2=getRelAddFrmSymblTbl(n);					//for Quadruple

	if(o.Operand2==-1)
		o.O2_Type=n;

	o.Target=-1;										//for Quadruple
	QUAD.push_back(o);									//for Quadruple


	match(";");


	spaces--;
}

void W(){
	printSpaces();
	fout_P<<"W";

	int t,f;
	temp.clear();

	OPinfo o;											//for Quadruple

	match("while");
	string type;
	o.Operand1=VAL(type);
	o.O1_Type = type;

	if(lexeme==">")								//for Quadruple
		o.OPcode=GREATER_THAN;							//for Quadruple
	else if(lexeme=="<")							//for Quadruple
		o.OPcode=LESS_THAN;							//for Quadruple
	else if(lexeme=="=>")						//for Quadruple
		o.OPcode=GREATER_THAN_OR_EQUAL_TO;				//for Quadruple
	else if(lexeme=="=<")						//for Quadruple
		o.OPcode=LESS_THAN_OR_EQUAL_TO;					//for Quadruple
	else if(lexeme=="=")							//for Quadruple
		o.OPcode=EQUAL_TO;								//for Quadruple
	else if(lexeme=="/=")						//for Quadruple
		o.OPcode=NOT_EQUAL_TO;							//for Quadruple

	match("RO");  
	o.Operand2 = VAL(type);
	o.O2_Type = type;

	temp.append("goto ");

	//*****************************************//
A1:
	EMIT();							
	t=n-1;

	QUAD.push_back(o);								//for Quadruple

	EMIT("goto ");

	f=n-1;

	o.OPcode=GOTO;									//for Quadruple
	o.Operand1=-1;									//for Quadruple
	o.Operand2=-1;									//for Quadruple
	o.Target=-1;									//for Quadruple
	QUAD.push_back(o);								//for Quadruple

	//*****************************************//

	match("begin");

	BACKPATCH(t);

	QUAD.at(t).Target=n;							//for Quadruple

	BD();

	EMIT("goto ",t);

	o.OPcode=GOTO;									//for Quadruple
	o.Operand1=t;									//for Quadruple
	o.Operand2=-1;									//for Quadruple
	o.Target=-1;									//for Quadruple
	QUAD.push_back(o);								//for Quadruple

	BACKPATCH(f);

	QUAD.at(f).Operand1=n;							//for Quadruple

	match("end");


	spaces--;
}

void F(){
	printSpaces();
	fout_P<<"F";

	int t,f,x;
	temp.clear();

	OPinfo o;											//for Quadruple

	match("if"); 
	string type;
	o.Operand1=VAL(type);
	o.O1_Type = type;


	if(lexeme==">")								//for Quadruple
		o.OPcode=GREATER_THAN;							//for Quadruple
	else if(lexeme=="<")							//for Quadruple
		o.OPcode=LESS_THAN;								//for Quadruple
	else if(lexeme=="=>")						//for Quadruple
		o.OPcode=GREATER_THAN_OR_EQUAL_TO;				//for Quadruple
	else if(lexeme=="=<")						//for Quadruple
		o.OPcode=LESS_THAN_OR_EQUAL_TO;					//for Quadruple
	else if(lexeme=="=")							//for Quadruple
		o.OPcode=EQUAL_TO;								//for Quadruple
	else if(lexeme=="=/")						//for Quadruple
		o.OPcode=NOT_EQUAL_TO;							//for Quadruple



	match("RO"); 
	o.Operand2 = VAL(type);
	o.O2_Type = type;

	temp.append(" goto ");

A1:
	EMIT();							
	t=n-1;

	QUAD.push_back(o);								//for Quadruple

	EMIT("goto ");
	f=n-1;

	o.OPcode=GOTO;									//for Quadruple
	o.Operand1=-1;									//for Quadruple
	o.Operand2=-1;									//for Quadruple
	o.Target=-1;									//for Quadruple
	QUAD.push_back(o);								//for Quadruple

	match("begin");
	BACKPATCH(t);

	QUAD.at(t).Target=n;							//for Quadruple

	temp.clear();
	BD();
	EMIT("goto ");
	x=n-1;

	o.OPcode=GOTO;									//for Quadruple
	o.Operand1=-1;									//for Quadruple
	o.Operand2=-1;									//for Quadruple
	o.Target=-1;									//for Quadruple
	QUAD.push_back(o);								//for Quadruple


	match("end");
	BACKPATCH(f);

	QUAD.at(f).Operand1=n;							//for Quadruple

	E();
	BACKPATCH(x);

	QUAD.at(x).Operand1=n;							//for Quadruple


	spaces--;
}

void E(){
	printSpaces();
	fout_P<<"E";

	if(current=="else"){
		match("else"); 
		if(current=="begin"){
			match("begin");
			BD(); 
			match("end");
		}
		else if(current=="if"){
			F();
		}
	}
	else 
		;									//NULL


	spaces--;
}

void R(){
	printSpaces();
	fout_P<<"R";

	temp.clear();

	OPinfo o;											//for Quadruple

	o.OPcode=RET;									//for Quadruple
	
	match("return");
	string type;
	
	EMIT();							

	o.Operand1=VAL(type);							//for Quadruple
	o.O1_Type=type;									//for Quadruple

	o.Operand2=-1;									//for Quadruple
	o.Target=-1;									//for Quadruple
	QUAD.push_back(o);								//for Quadruple

	match(";");


	spaces--;
}

void P(){
	printSpaces();
	fout_P<<"P";

	temp.clear();

	match("print");
	OPinfo o;											//for Quadruple
	o.OPcode=OUT;										//for Quadruple
	o.Operand1=getRelAddFrmSymblTbl(lexeme);		    //for Quadruple
	o.Operand2=-1;										//for Quadruple
	o.Target=-1;										//for Quadruple
	
	match("ID");						
	int pr=D(); 
	
	if(pr!=-1)
		o.Operand1+=pr;

	QUAD.push_back(o);									//for Quadruple

	EMIT();

	match(";");


	spaces--;
}

void I(){
	printSpaces();
	fout_P<<"I";

	temp.clear();


	match("input");

	OPinfo o;											//for Quadruple
	o.OPcode=IN;										//for Quadruple
	
	/*if(isFuncDef){

		for(int b=0;b<SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.size();b++)
			if(SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.at(b).ID==lexeme){
				o.Operand1=SYMBOL_TABLE.at(funcIndex).FN.MULTILEVEL_SYMBOL_TABLE.at(b).relAdd;
				break;
			}
	}
	else*/
		o.Operand1=getRelAddFrmSymblTbl(lexeme);		    //for Quadruple

	o.Operand2=-1;										//for Quadruple
	o.Target=-1;										//for Quadruple
	QUAD.push_back(o);									//for Quadruple

	match("ID"); 

	EMIT();


	match(";");


	spaces--;
}

void P2(int &c){
	printSpaces();
	fout_P<<"P2";

	if(current==","){
		match(",");

		string n=G();
A:
		c++;
		temp.clear();
		temp.append("arg ");
		temp.append(n);
		EMIT();

		OPinfo o;											//for Quadruple
		o.OPcode=ARG;										//for Quadruple
		o.Operand1=getRelAddFrmSymblTbl(n);					//for Quadruple

		if(o.Operand1==-1)
			o.O1_Type=n;

		o.Operand2=-1;										//for Quadruple
		o.Target=-1;										//for Quadruple
		QUAD.push_back(o);									//for Quadruple

		P2(c);
	}
	else 
		;


	spaces--;
}

void P1(int &c){
	printSpaces();
	fout_P<<"P1";

	if(current!=")"){
		string n=G();
A:
		c++;
		temp.clear();
		temp.append("arg ");
		temp.append(n);
		EMIT();

		OPinfo o;											//for Quadruple
		o.OPcode=ARG;										//for Quadruple
		o.Operand1=getRelAddFrmSymblTbl(n);					//for Quadruple
		if(o.Operand1==-1)
			o.O1_Type=n;

		o.Operand2=-1;										//for Quadruple
		o.Target=-1;										//for Quadruple
		QUAD.push_back(o);									//for Quadruple

		P2(c);
	}
	else 
		;


	spaces--;
}

string FC (string idLex){
	printSpaces();
	fout_P<<"FC";

	int c=0;

	match("(");
	P1(c);
	match(")");

	if(funcFlag)
		match(";");

	string n=newTmp();
	temp.clear();
	temp.append(n);
	temp.append(" = call ");
	temp.append(idLex);
	temp.append(", ");

	char cc[5];
	itoa(c,cc,10);
	temp.append(cc);

	EMIT();

	OPinfo o;											//for Quadruple
	o.OPcode=CALL;										//for Quadruple
	//o.Operand1= getRelAddFrmSymblTbl(idLex);			//for Quadruple
	o.O1_Type= idLex;									//for Quadruple
	o.Operand2=c;										//for Quadruple
	o.Target=getRelAddFrmSymblTbl(n);					//for Quadruple
	QUAD.push_back(o);									//for Quadruple

	spaces--;

	return n;
}

void BD(){
	printSpaces();
	fout_P<<"BD";


	if(current=="while"){
		W(); 
		BD();
	}
	else if(current=="if"){
		F(); 
		BD();
	}
	else if(current=="print"){
		P(); 
		BD();
	}
	else if(current=="input"){
		I(); 
		BD();
	}
	else if(current=="return"){
		R();
	}
	else if(current=="character" || current=="integer"){
		VD();
		BD();
	}
	else if(current=="ID"){
		temp.clear();
		string funcLex=lexeme;
		match("ID");

		if(current=="("){
			funcFlag=true;
			FC(funcLex);
			BD();
		}
		else {
			VAO();
			BD();
		}

	}

	else
		;							//NULL


	spaces--;

}

void S(){

	printSpaces();
	fout_P<<"S";

	if(current=="function"){
		FD();

		if(current!="FINISH")
			S();
	}
	else if(current=="integer"||current=="character"){
		VD();
		if(current!="FINISH")
			S();
	}
	else if(current=="FINISH")
		;
	else
		ERROR();

	spaces--;
}

void run_PARSER(){
	//POPULATE();
	current=TOKENS.at(curr1); lexeme=LEXEMES.at(curr1);
	S();

	while(!toBeRemovedEntriesFmSymblTbl.empty()){

		int i=toBeRemovedEntriesFmSymblTbl.at(toBeRemovedEntriesFmSymblTbl.size()-1);

		SYMBOL_TABLE.erase(SYMBOL_TABLE.begin()+i);
		toBeRemovedEntriesFmSymblTbl.erase(toBeRemovedEntriesFmSymblTbl.begin()+toBeRemovedEntriesFmSymblTbl.size()-1);

	}

	fout_P.close();
}

void run_TRANSLATOR(){

	ofstream fout_T("Translator Output.txt");

	for(int i=0;i<n;i++){
		//cout<<ALL_EMITS.at(i)<<endl;
		fout_T<<ALL_EMITS.at(i)<<endl;
	}

	fout_T.close();


}

//void populate_SymbolTable(){
//
//	ofstream fout_ST("Symbol Table.txt");
//	fout_ST<<"ID,Type,Relative Address\n\n";
//
//	int L,maxL=-1;
//	for(int i=0;i<SYMBOL_TABLE.size();i++){
//		L=SYMBOL_TABLE.at(i).ID.length();
//		if(L>maxL)
//			maxL=SYMBOL_TABLE.at(i).ID.length();
//	}
//	
//	for(int i=0;i<SYMBOL_TABLE.size();i++){
//		fout_ST<<" "<<SYMBOL_TABLE.at(i).ID;
//		
//		L=SYMBOL_TABLE.at(i).ID.length();
//		for(int j=0;j<maxL-L;j++)
//			fout_ST<<" ";
//		
//		//fout_ST<<" | "<<SYMBOL_TABLE.at(i).Type<<" | "<<SYMBOL_TABLE.at(i).relAdd<<endl;
//		fout_ST<<" | "<<SYMBOL_TABLE.at(i).Type<<" | "<<SYMBOL_TABLE.at(i).relAdd;
//		
//		if(SYMBOL_TABLE.at(i).Type=="FUN")
//			fout_ST<<" | "<<SYMBOL_TABLE.at(i).FN.start<<" | "<<SYMBOL_TABLE.at(i).FN.end;
//		
//		fout_ST<<endl;
//
//	}
//	
//	fout_ST.close();
//
//}

void populate_SymbolTable(){

	ofstream fout_ST("Symbol Table.txt");
	fout_ST<<"ID,Type,Relative Address\n\n";

	int L,maxL=-1;
	for(int i=0;i<SYMBOL_TABLE.size();i++){
		L=SYMBOL_TABLE.at(i).ID.length();
		if(L>maxL)
			maxL=SYMBOL_TABLE.at(i).ID.length();
	}

	for(int i=0;i<SYMBOL_TABLE.size();i++){

		//if(SYMBOL_TABLE.at(i).Type!="FUN"){
		fout_ST<<" "<<SYMBOL_TABLE.at(i).ID;

		L=SYMBOL_TABLE.at(i).ID.length();
		for(int j=0;j<maxL-L;j++)
			fout_ST<<" ";

		fout_ST<<" | "<<SYMBOL_TABLE.at(i).Type;
		if(SYMBOL_TABLE.at(i).Type!="FUN")
			fout_ST<<" | "<<SYMBOL_TABLE.at(i).relAdd;
		fout_ST<<endl;

		//}
	}



	for(int i=0;i<SYMBOL_TABLE.size();i++){

		if(SYMBOL_TABLE.at(i).Type=="FUN"){
			fout_ST<<endl<<endl<<"\tID: "<<SYMBOL_TABLE.at(i).ID<<endl<<"\tStart: "<<SYMBOL_TABLE.at(i).FN.start<<endl<<"\tEnd: "<<SYMBOL_TABLE.at(i).FN.end<<endl<<"\tNo of Parameters: "<<SYMBOL_TABLE.at(i).FN.no_of_params<<endl<<"\tNo of Local Variables: "<<SYMBOL_TABLE.at(i).FN.no_of_local_vars<<endl<<endl;

			for(int j=0;j<SYMBOL_TABLE.at(i).FN.MULTILEVEL_SYMBOL_TABLE.size();j++)
				fout_ST<<SYMBOL_TABLE.at(i).FN.MULTILEVEL_SYMBOL_TABLE.at(j).ID<<", "<<SYMBOL_TABLE.at(i).FN.MULTILEVEL_SYMBOL_TABLE.at(j).Type<<", "<<SYMBOL_TABLE.at(i).FN.MULTILEVEL_SYMBOL_TABLE.at(j).relAdd<<endl;



		}
	}


	fout_ST.close();

}

void printQuadRuple(){



	for(int i=0;i<QUAD.size();i++){

		cout<<i<<")  "<<QUAD.at(i).OPcode<<"  "<<QUAD.at(i).Operand1<<"  ";

		if(QUAD.at(i).OPcode==ASSIGNMENT)
			cout<<QUAD.at(i).Operand2;

		else if(QUAD.at(i).OPcode!=GOTO && QUAD.at(i).OPcode!=IN && QUAD.at(i).OPcode!=OUT && QUAD.at(i).OPcode!=ARG)
			cout<<QUAD.at(i).Operand2<<"  "<<QUAD.at(i).Target;


		cout<<endl;
	}
}