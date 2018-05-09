#include"PARSER+TRANSLATOR.h"
#include<stack>


#define DATA_SEGMENT_SIZE 100000
string DS[DATA_SEGMENT_SIZE];


bool find_Main(int &strt,int &end,int &smblTbl){

	int i;
	for(i=0;i<SYMBOL_TABLE.size();i++){
		if(SYMBOL_TABLE.at(i).Type=="FUN" && SYMBOL_TABLE.at(i).ID=="main"){
			strt=SYMBOL_TABLE.at(i).FN.start;
			end=SYMBOL_TABLE.at(i).FN.end;
			smblTbl=i;
			return true;
		}

	}

	if(i==SYMBOL_TABLE.size()){
		cout<<"\n\"main\" not found\n\n";
		return false;
	}

}

void run_VM(){

	int strt,end,smblTbl;
	if(!find_Main(strt,end,smblTbl))
		return;

	int func_size,arg_cnt=0;
	stack<int> myStack;

	int curr_add=0;
	int next_add=curr_add+SYMBOL_TABLE.at(smblTbl).FN.total_size;

	string *curr_seg=DS+curr_add;
	
	int PC=strt;
	while(true){
	
		if(PC>end){

			if(myStack.empty())
				break;
			else{
				myStack.pop();

				curr_add=myStack.top();
				myStack.pop();

				curr_seg=DS+curr_add;

				end=myStack.top();
				myStack.pop();

				PC=myStack.top();
				myStack.pop();

				PC++;
				continue;
			}

		}
		string o1,o2;
		OPinfo currOP=QUAD.at(PC);

		if (currOP.O1_Type == "ID")
			o1 = curr_seg[currOP.Operand1];
		else
			o1 = currOP.O1_Type;

		if (currOP.O2_Type == "ID")
			o2 = curr_seg[currOP.Operand2];

		else
			o2 = currOP.O2_Type;

		switch (currOP.OPcode){

		case ADDITION:

			if (currOP.Target != -1)
				curr_seg[currOP.Target] = to_string(stoi (o1,nullptr,10) + stoi (o2,nullptr,10));
			  
			break;

		case SUBTRACTION:

			if (currOP.Target != -1)
				curr_seg[currOP.Target] = to_string(stoi (o1,nullptr,10) - stoi (o2,nullptr,10));
			break;

		case MULTIPLICATION:

			if (currOP.Target != -1)
				curr_seg[currOP.Target] = to_string(stoi (o1,nullptr,10) * stoi (o2,nullptr,10));
			break;

		case DIVISION:

			if (currOP.Target != -1)
				curr_seg[currOP.Target] = to_string(stoi (o1,nullptr,10) / stoi (o2,nullptr,10));
			break;

		case GOTO:
			PC = currOP.Operand1 - 1;
			break;

		case IN:
			cin >> curr_seg[currOP.Operand1];
			break;

		case OUT:
			cout << curr_seg[currOP.Operand1] << endl;
			break;

		case EQUAL_TO:
			
			if (stoi(o1,nullptr,10) == stoi(o2,nullptr,10))
				PC = currOP.Target - 1;

			break;

		case NOT_EQUAL_TO:
		
			if (stoi(o1,nullptr,10) != stoi(o2,nullptr,10))
				PC = currOP.Target - 1;

			break;

		case GREATER_THAN:
			
			if (stoi(o1,nullptr,10) > stoi(o2,nullptr,10))
				PC = currOP.Target - 1;

			break;

		case LESS_THAN:
			
			if (stoi(o1,nullptr,10) < stoi(o2,nullptr,10))
				PC = currOP.Target - 1;
			
			break;

		case GREATER_THAN_OR_EQUAL_TO:
		
			if (stoi(o1,nullptr,10) >= stoi(o2,nullptr,10))
				PC = currOP.Target - 1;

			break;

		case LESS_THAN_OR_EQUAL_TO:
		
			if (stoi(o1,nullptr,10) <= stoi(o2,nullptr,10))
				PC = currOP.Target - 1;

			break;

		case ASSIGNMENT:
						
			if(currOP.Operand2!=-1)
				curr_seg[currOP.Operand1]=curr_seg[currOP.Operand2];
			else
				curr_seg[currOP.Operand1]= currOP.O2_Type;
			break;

		case CALL:
			arg_cnt=0;
			
			myStack.push(PC);
			myStack.push(end);
			myStack.push(curr_add);
			myStack.push(currOP.Target);


			PC=SYMBOL_TABLE.at(getSymbolTableIndex(currOP.O1_Type)).FN.start-1;
			end=SYMBOL_TABLE.at(getSymbolTableIndex(currOP.O1_Type)).FN.end;

			curr_add=curr_add+next_add;

			curr_seg=DS+curr_add;

			next_add=next_add+SYMBOL_TABLE.at(getSymbolTableIndex(currOP.O1_Type)).FN.total_size;


			break;

		case ARG:
			if(currOP.Operand1!=-1)
				curr_seg[next_add+arg_cnt]=curr_seg[currOP.Operand1];
			else
				curr_seg[next_add+arg_cnt]=currOP.O1_Type;

			arg_cnt++;
			break;

		case RET:

			int r=myStack.top();

			if(r!=-1){
				myStack.pop();

				if(currOP.Operand1!=-1)
					DS[myStack.top()+r]=curr_seg[currOP.Operand1];
				else
					DS[myStack.top()+r]=currOP.O1_Type;

				myStack.push(r);
			}
			break;

		}
		PC++;
	}
}