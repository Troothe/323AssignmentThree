
//CPSC 323 - Project 2 (Due 4/15)

//Include statements
#include<iostream>
#include<fstream>
#include <string>
#include<stack>
#include<sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
using namespace std;

//Declare some global variables
string temp, getlexime, seperator, num, operat;		
int startState = 0;
int state = startState;
char c1;
ofstream outputFile("output.txt");
int col = 0;
bool answer;
int linenumber = 0;
int declarationhappening = 0;
bool syntaxIsCorrect = true;
string id;
vector <string> storage;
vector <string> errorMessage;
string val;
int memoryloc = 5000;
string lasttype = "";
vector<vector<string>> symbol_table;

int fsmTable[8][7] = { {1, 2, 7, 3, 4, 5, 6},       //0=initail state
						 {1, 1, 7, 7, 7, 7, 6},     //1=identifier/keyword
						 {7, 2, 2, 7, 7, 7, 6},     //2=number
						 {7, 7, 7, 3, 7, 7, 6},     //3=operator
						 {7, 7, 7, 7, 4, 7, 6},     //4=seperator
						 {5 ,5, 5, 5, 5, 7, 5},     //5=comment
						 {7, 7, 7, 7, 7, 7, 7},     //6=newline/space/tab
						 {0, 0, 0, 0, 0, 0, 0} };   //7=final stage/token found/going back to initial

//List of Non-terminals
string Nonterminal[19] = { "<Statement List>", "<Statement>","<Assign>","<If Statement>","<ElsePart>","<While Statement>","<Declaration>","<Compound Statement>","<Expression>","<Expression Prime>","<Term>","<Term Prime>","<Factor>","<Condition>","<Operator>","<DataType>","<MoreIds>","<MoreStatement>","<Id>" };
//List of terminals
string terminals[31] = { "id","if","while","int","float","bool","{","(","integer","real","+","-","*","/","true","false","==","<>","<",">","<=",">=",",",";","else",")","}","$","=","endif","endwhile" };
//Dividing rules
string rules[38] = { "<Assign>", "<If Statement>", "<While Statement>", "<Declaration>", "<Compound Statement>", "<Id>:=:<Expression>:;", "<Term>:<Expression Prime>", "+:<Term>:<Expression Prime>",
				  "-:<Term>:<Expression Prime>", "<Factor>:<Term Prime>", "*:<Factor>:<Term Prime>", "/:<Factor>:<Term Prime>", "id", "(:<Expression>:)", "integer", "real",
				  "if:(:<Condition>:):<Statement>:<ElsePart>:endif", "else:<Statement>", "true", "false", "<Expression>:<Operator>:<Expression>", "==", "<>", "<", ">", "<=", ">=",
				  "while:(:<Condition>:):<Statement>:endwhile", "<DataType>:<Id>:<MoreIds>:;", ",:<Id>:<MoreIds>", "int", "float", "bool", "{:<Statement>:<MoreStatement>:}", "<Statement>:<MoreStatement>",
				  "Epsilon", "id", "<MoreStatement>"
};

//Table used for syntax_analysis
int syntaxTable[19][31] = { {37,37,37,37,37,37,37,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {0,1,2,3,3,3,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,17,-1,-1,-1,-1,35,-1},
							 {-1,-1,27,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,28,28,28,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,-1,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {6,-1,-1,-1,-1,-1,-1,6,6,6,-1,-1,-1,-1,6,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,7,8,-1,-1,-1,-1,35,35,35,35,35,35,-1,35,-1,35,-1,-1,-1,-1,-1},
							 {9,-1,-1,-1,-1,-1,-1,9,9,9,-1,-1,-1,-1,9,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,35,35,10,11,-1,-1,35,35,35,35,35,35,-1,35,-1,35,-1,-1,-1,-1,-1},
							 {12,-1,-1,-1,-1,-1,-1,13,14,15,-1,-1,-1,-1,18,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {20,-1,-1,-1,-1,-1,-1,20,20,20,-1,-1,-1,-1,18,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,21,22,23,24,25,26,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,30,31,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
							 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,29,35,-1,-1,-1,-1,-1,-1,-1},
							 {34,34,34,34,34,34,34,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,35,35,-1,-1,-1},
							 {36,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

void printSymbolTable(vector<vector<string>> symbol_table, ifstream& file)
{

	const char separator = ' ';
	const int nameWidth = 12;
	cout << endl << "************************************************" << endl;
	cout << left << setw(nameWidth) << setfill(separator) << "Identifier";
	cout << left << setw(nameWidth) << setfill(separator) << "Line #";
	cout << left << setw(nameWidth) << setfill(separator) << "Memory";
	cout << left << setw(nameWidth) << setfill(separator) << "Type";
	cout << endl << "************************************************" << endl;

	outputFile << "\n************************************************\n";
	outputFile << left << setw(nameWidth) << setfill(separator) << "Identifier";
	outputFile << left << setw(nameWidth) << setfill(separator) << "Line #";
	outputFile << left << setw(nameWidth) << setfill(separator) << "Memory";
	outputFile << left << setw(nameWidth) << setfill(separator) << "Type";
	outputFile << "\n" << "************************************************" << "\n";


	for (int y = 0; y < symbol_table.size(); y++) {
		for (int z = 0; z < 4; z++) {
			cout << left << setw(nameWidth) << setfill(separator) << symbol_table.at(y).at(z);
			outputFile << left << setw(nameWidth) << setfill(separator) << symbol_table.at(y).at(z);
			
		}
		cout << endl;
		outputFile << "\n";
	}
}

//Add ID to our vector
void addIdToVector(string s)
{
	vector<string>::iterator it = find(storage.begin(), storage.end(), s);
	if (it == storage.end()) {
		storage.push_back(s);
	}
}

//Remove semicolon from the string
string removeSemicolon(string str)
{
	string result;
	int a = str.size();
	for (int i = 0; i < a; i++) {
		if (str[i] != ':') {
			result += str[i];
		}
	}
	return result;
}

//Reverse the values on the table
void reverseString(string x, stack<string>& stack1)
{
	string data;
	size_t it;
	stringstream linestream(x);
	it = x.find(":");
	if (it == -1) {
		getline(linestream, data);
		if (data != "Epsilon") {
			stack1.push(data);
		}
		return;
	}
	else {
		getline(linestream, data, char(58));
		x.erase(0, it + 1);
		reverseString(x, stack1);
		stack1.push(data);
	}
}

//Function for ID check
bool checkIfIdExists(string y)
{
	vector<string>::iterator it = find(storage.begin(), storage.end(), y);

	if (it != storage.end()) {
		return true;
	}
	else {
		return false;
	}

}

//Traverse terminal array, return index if found
int checkIfTermIsTerminal(string stacktop, int findit)
{
	for (size_t n = 0; n < sizeof(terminals) / sizeof(terminals[0]); n++) {
		if (terminals[n] == stacktop) {
			findit = n;
			return findit;
		}
	}
	return findit;
}

//Check if top of stack is non-terminal
int checkIfTermIsNotTerminal(string stacktop, int findit)
{
	for (size_t n = 0; n < sizeof(Nonterminal) / sizeof(Nonterminal[0]); n++) {
		if (Nonterminal[n] == stacktop) {
			findit = n;
			return findit;
		}
	}
	return findit;
}

bool inSymbolTable(string identifier)
{
	for (int y = 0; y < symbol_table.size(); y++) {
		if (symbol_table.at(y).at(0) == identifier) {
			return true;
		}
	}
	return false;
}

//Functions to show the output as token and lexeme
void compareKeywordIdentifier(string x, int linenumber) {
	if (x == "true" || x == "false") {
		val = x;
		cout << "\nToken: boolean value:           Lexeme: " << x << endl;
		outputFile << "\nToken: boolean value:        Lexeme:    " << x << endl;
		return;
	}
	//Keywords list/array
	string keywords[20] = { "int", "float", "bool", "if", "else", "then", "endif", "while", "endwhile", "do", "doend",
	"for", "forend", "input", "output", "and", "or", "function" , "return", "include" };

	string keyword = "";
	//Find the value is keyword or not, show token/lexeme if needed
	for (int i = 0; i < 20; i++) {
		if (x == keywords[i]) {
			keyword = x;
			cout << "\nToken: Keyword:           Lexeme: " << keyword << endl;
			outputFile << "\nToken: Keyword:        Lexeme:    " << keyword << endl;
			val = keyword;
		}
		if (x == "int" || x == "float" || x == "bool") {
			lasttype = x;
		}
	}
	//If the value is not keyword then it is identifier, show token and lexeme values
	if (keyword == "") {
		id = x;
		cout << "\nToken: Identifier:         Lexeme: " << id << endl;
		outputFile << "\nToken: Identifier:      Lexeme:   " << id << endl;

		if (lasttype != "") {

			if (inSymbolTable(id)) {
				errorMessage.push_back((id + " has already been declared (line " + to_string(linenumber) + ")"));
			}
			else {
				symbol_table.push_back({ id, to_string(linenumber), to_string(memoryloc++), lasttype });
			}
		}
		

		if (declarationhappening == 1) {
			addIdToVector(id);
		}
		val = "id";
	}
}

//Find row for syntax analysis that is related to the non-terminal array
int findNotTerminalRow(string stacktopval, int row1)
{
	for (size_t k = 0; k < sizeof(Nonterminal) / sizeof(Nonterminal[0]); k++) {
		if (Nonterminal[k] == stacktopval) {
			row1 = k;
			return row1;
		}
	}
}

//Finds the col for syntax analysis
int findTerminalCol(string term, int col1)
{
	for (size_t z = 0; z < sizeof(terminals) / sizeof(terminals[0]); z++) {
		if (terminals[z] == term) {
			col1 = z;
			return col1;
		}
	}
}

//Responsible for finding errors in the syntax
void findError(stack<string>& stack1, int line)
{
	int d = -1;
	while (checkIfTermIsTerminal(stack1.top(), d) == -1) {
		int find = -1;
		string top = stack1.top();
		if (checkIfTermIsNotTerminal(top, find) != -1) {
			stack1.pop();
		}
	}
	errorMessage.push_back(("Missing " + stack1.top() + " in line " + to_string(line)));
}

//Find the rule number from the table.  Will determine if it should be parsed or not
bool getTableValWorkOnStack(stack<string>& stack1, int row1, int col1, int line)
{
	string pushval;
	if (syntaxTable[row1][col1] != -1) {
		stack1.pop();
		pushval = rules[syntaxTable[row1][col1]];
		cout << Nonterminal[row1] << "->" << removeSemicolon(pushval) << endl;
		outputFile << Nonterminal[row1] << "->" << removeSemicolon(pushval) << endl;

		reverseString(pushval, stack1);
		return true;
	}
	else {
		cout << "is error:" << endl;
		outputFile << "is error:" << endl;
		findError(stack1, line);
		return false;
	}
}

//Find the number, seperate the number as int or float and show the token as real and Integer and show its value related to token
void Number(string z)
{
	int found = 0;
	for (int i = 0; i < z.size(); i++) {
		if (z[i] == '.') {
			found = 1;
		}
	}
	if (found == 1) {
		cout << "\nToken: Real   :           Lexeme:  " << z << endl;
		outputFile << "\nToken: Real   :         Lexeme:   " << z << endl;
		val = "real";
	}
	else {
		cout << "\nToken: Integer :          Lexeme: " << z << endl;
		outputFile << "\nToken: Integer :       Lexeme:    " << z << endl;
		val = "integer";
	}
}

//Find the operator, ans show the token as operator and show its lexeme vlaue
void Operator(string a) {
	cout << "\nToken: Operator:          Lexeme: " << a << endl;
	outputFile << "\nToken: Operator:        Lexeme:   " << a << endl;
	val = a;
}

//Find the separator, ans show the token as seperator and show its lexeme value
void seperate(string y)
{
	if (y == ";") {
		lasttype = "";
	}

	cout << "\nToken: seperator :        Lexeme: " << y << endl;
	outputFile << "\nToken: seperator :       Lexeme:  " << y << endl;
	val = y;


}

//Function to show the output and store the value in string temp, until the final stage
int stateOutput(int& state, char c, int linenumber) {
	switch (state) {
		//Initial states
	case 0:
		break;
	case 1:
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			temp += c;
			state = fsmTable[state][col];
			break;
		}
		if (temp != "") {
			if (c >= '0' && c <= '9') {
				temp += c;
				state = fsmTable[state][col];
				break;
			}
			getlexime = temp;
			temp = "";
			compareKeywordIdentifier(getlexime, linenumber);
			state = 1;
			break;

		}
	case 2://numbers
		if (c >= '0' && c <= '9') {
			temp += c;
			state = fsmTable[state][col];
			break;
		}
		else {
			if (temp != "") {
				if (c == '.') {
					temp += c;
					state = fsmTable[state][col];
					break;
				}
				num = temp;
				temp = "";
				Number(num);
				state = 2;
				break;
			}
		}
	case 3: //operators
		if (c == '=' || c == '+' || c == '-' || c == '>' || c == '<' || c == '%' || c == '*' || c == '/') {
			temp += c;
			state = fsmTable[state][col];
			break;
		}
		else {
			if (temp != "") {
				operat = temp;
				temp = "";
				Operator(operat);
			}
			state = 3;
			break;
		}
	case 4: //separator
		if ((c == '\' ' || c == '(' || c == '{' || c == '[' || c == ',' || c == ':' || c == ';' || c == ')' || c == '}' || c == ']' || c == '#') && temp == "") {
			temp += c;
			state = fsmTable[state][col];
			break;
		} else {
			if (temp != "") {
				seperator = temp;
				temp = "";
				seperate(seperator);

				if ((c == '\' ' || c == '(' || c == '{' || c == '[' || c == ',' || c == ':' || c == ';' || c == ')' || c == '}' || c == ']' || c == '#') && temp == "") {
					temp += c;
					state = fsmTable[state][col];
					break;
				}
			}
			state = 4;
			break;
		}
	case 5:
		if (c == '!') {
			if (temp == "") {
				temp += c;
				state = 5;
				break;
			}if (temp != "") {
				temp = "";
				state = fsmTable[state][col];
				break;
			}
		}
		if (temp[0] == '!') {
			state = fsmTable[state][col];
			break;
		}
	case 6:
		if (c == char(10) || c == '\0') {
			state = fsmTable[state][col];
			break;
		}

		if (c == char(32)) {
			state = fsmTable[state][col];
			break;
		}

		//Tab check
		if (c == char(9)) {
			state = fsmTable[state][col];
			break;
		}
	case 7: //Shouldn't get here
		temp = "";
		state = fsmTable[state][col];
		break;
	default:
		state = 0;
		break;
	}
	return state;
}

//Main lexer function
string lexer(int& state, char c1, int linenumber) {

checkAgain:
	if (c1 == '$' || temp == "$") {
		if (temp != "") {
			if (temp != "$") {
				state = stateOutput(state, col, linenumber);
				temp = "$";
			} else {
				val = "$";
				return val;
			}
		} else {
			temp = "";
			val = "$";
		}

	}
	if (state == 7) {
		state = fsmTable[state][col];
	}

	if ((c1 >= 'a' && c1 <= 'z') || (c1 >= 'A' && c1 <= 'Z')) {
		col = 0;
		if (state == 0) {
			state = fsmTable[state][col];
		}
		state = stateOutput(state, c1, linenumber);
		state = fsmTable[state][col];

		if (state == 7)	{
			goto checkAgain;
		}
	}

	if (c1 >= '0' && c1 <= '9') {
		col = 1;
		if (state == 0) {
			state = fsmTable[state][col];
		}

		state = stateOutput(state, c1, linenumber);
		state = fsmTable[state][col];
		if (state == 7) {
			goto checkAgain;
		}
	}

	if (c1 == '.') {
		col = 2;
		if (state == 0) {
			state = fsmTable[state][col];
		}
		state = fsmTable[state][col];
		state = stateOutput(state, c1, linenumber);
		if (state == 7) {
			goto checkAgain;
		}
	}
	if (c1 == '=' || c1 == '+' || c1 == '-' || c1 == '>' || c1 == '<' || c1 == '%' || c1 == '*' || c1 == '/') {
		col = 3;
		if (state == 0) {
			state = fsmTable[state][col];
		}
		state = stateOutput(state, c1, linenumber);
		state = fsmTable[state][col];
		if (state == 7) {
			goto checkAgain;
		}
	}

	if (c1 == '\' ' || c1 == '(' || c1 == '{' || c1 == '[' || c1 == ',' || c1 == ':' || c1 == ';' || c1 == ')' || c1 == '}' || c1 == ']' || c1 == '#') {
		col = 4;
		if (state == 0) {
			state = fsmTable[state][col];
		}
		state = stateOutput(state, c1, linenumber);

		state = fsmTable[state][col];
		if (state == 7) {
			goto checkAgain;
		}
	}
	//checking the input (!) if so then col =5, state according to current state
	if (c1 == '!') {
		col = 5;
		if (state == 0){
			state = fsmTable[state][col];
			state = stateOutput(state, c1, linenumber);
		} else if (state != 0) {
			state = stateOutput(state, c1, linenumber);
			state = fsmTable[state][col];
		}

		if (state == 7) {
			goto checkAgain;
		}
	}
	if (c1 == char(10) || c1 == char(32) || c1 == char(9) || c1 == '\0') {
		col = 6;
		if (state == 0) {
			state = fsmTable[state][col];
		}
		state = stateOutput(state, c1, linenumber);
		state = fsmTable[state][col]; 
		state = stateOutput(state, c1, linenumber);

	}

	return val;
}


//Syntax analysis line by line
bool syntaxAnalysis(ifstream& file, stack<string>stack1) {
	stack1.push(Nonterminal[0]);
	string line;
	string wholefile = "";
	string stacktopval;
	int findit = -1;
	int col1 = -1;
	int row1 = -1;
	while (!file.eof()) {
		getline(file, line);
		wholefile = wholefile + char(10) + line;
	}
	wholefile = wholefile + '$';

	string term, pushval;
	for (int i = 0; i <= wholefile.size(); i++) {
		c1 = wholefile[i];
		if (c1 == char(10)) {
			linenumber += 1;
			if (declarationhappening == 1) {
				declarationhappening = 0;
			}
		}
		term = lexer(state, c1, linenumber);
		if (term != "") {
			while (!stack1.empty()) {
				stacktopval = stack1.top();
				if (stacktopval == "<Declaration>") {
					declarationhappening = 1;
				}

				int findit = -1;
				int col1 = -1;
				int row1 = -1;
				findit = checkIfTermIsTerminal(stacktopval, findit);
				if (findit != -1) {
					if (stacktopval == term) {
						if (term == "id" & (declarationhappening == 0)) {
							if (!checkIfIdExists(id)) {
								errorMessage.push_back(("Variable " + id + " used in line " + to_string(linenumber) + " is not declared ."));
								syntaxIsCorrect = false;
							}
						}
						stack1.pop();
						val = "";
						break;
					} else {
						errorMessage.push_back(("In line " + to_string(linenumber) + " lexeme " + term + " is incorrect."));
						syntaxIsCorrect = false;
						break;
					}
				} else {
					row1 = findNotTerminalRow(stacktopval, row1);
					col1 = findTerminalCol(term, col1);
					if (!getTableValWorkOnStack(stack1, row1, col1, linenumber)) {
						syntaxIsCorrect = false;
						break;

					}
				}
			}
		}

	}

	return syntaxIsCorrect;
}
//Main function

int main()
{

	ifstream file("input.txt");   //input file
	//Make sure our input file was found, end the program if it wasn't
	if (!file) {
		cout << "Unable to open input file.\n";
		return 0;
	}
	//If the file was found, loop through it
	while (file) {

		//If we get to the end of the file, exit the while loop
		if (file.eof()) { break; }

		stack <string> stack1;
		stack1.push("$");
		val = "";
		answer = syntaxAnalysis(file, stack1);

		if (errorMessage.size() > 0) {
			for (int i = 0; i < errorMessage.size(); i++) {
				cout << " ERROR " << (i + 1) << " : " << errorMessage.at(i) << endl;
				outputFile << " ERROR " << (i + 1) << " : " << errorMessage.at(i) << endl;
			}
		}
		else {
			cout << "Symbol table: " << endl;

		}

		printSymbolTable(symbol_table, file);

		file.close();     //Input file close
		outputFile.close();  //Output file close

		return 0;

	}
}
