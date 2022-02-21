#include"Lexer.h"
using namespace std;

Lexer::Lexer(string form)
{
	length = form.length();
	pointer = 0;
	for (int i = 0; i < length; i++)
	{
		formula[i] = form[i];
	}
	formula[length] = '\0';
}

int Lexer::GetWord(word &w)
{
	switch (formula[pointer++])
	{
	case '!': {
		w.character = "!";
		w.typenum = $Neg;
		return OK;
		break; 
	}
	case '-': {
		if (formula[pointer] == '>')
		{
			pointer++;
			w.character = "->";
			w.typenum = $Impli;
			return OK;
		}
		else
			return ERROR;
		break; 
	}
	case '<': {
		if (formula[pointer] == '-'&&formula[pointer + 1] == '>')
		{
			pointer += 2;
			w.character = "<->";
			w.typenum = $Equ;
			return OK;
		}
		else if (formula[pointer] == '>')
		{
			pointer++;
			w.character = "<>";
			w.typenum = $Eve;
			return OK;
		}
		else
			return ERROR;
		break; 
	}
	case '&': {
		if (formula[pointer] == '&')
		{
			pointer++;
			w.character = "&&";
			w.typenum = $And;
			return OK;
		}
		else
			return ERROR;
		break; 
	}
	case '|': {
		if (formula[pointer] == '|')
		{
			pointer++;
			w.character = "||";
			w.typenum = $Or;
			return OK;
		}
		else
			return ERROR;
		break; 
	}
	case '[': {
		if (formula[pointer] == ']')
		{
			pointer++;
			w.character = "[]";
			w.typenum = $Alw;
			return OK;
		}
		else
			return ERROR;
		break; 
	}
	case 'U': {
		w.character = "U";
		w.typenum = $Until;
		return OK;
		break; 
	}
	case 'V': {
		w.character = "V";
		w.typenum = $Rele;
		return OK;
		break; 
	}
	case 'X': {
		w.character = "X";
		w.typenum = $Next;
		break; 
	}
	case '(': {
		w.character = "(";
		w.typenum = $Lpar;
		return OK;
		break;
	}
	case ')': {
		w.character = ")";
		w.typenum = $Rpar;
		return OK;
		break;
	}
	case '\0':{
		return $End;
	}
	case '{': {
		char p[MAXFORMLEN];
		int i = 0;
		while (formula[pointer-1]!='}')
		{
			p[i++] = formula[pointer - 1];
			pointer++;
		}
		p[i++] = formula[pointer-1];
		p[i] = '\0';
		w.character = p;
		w.typenum = $ID;
		return OK;
	}
	case '#': {
		return $End;
		break;
	}
	default: {
		/*char p[10];
		int i = 0;
		while (formula[pointer]!='\0' && formula[pointer] != '!' && formula[pointer] != '-'
			&& formula[pointer] != '<' && formula[pointer] != '&' && formula[pointer] != '|'
			&& formula[pointer] != '[' && formula[pointer] != 'U' && formula[pointer] != 'R'
			&& formula[pointer] != 'X' && formula[pointer] != '(' && formula[pointer] != ')')
		{
			p[i++] = formula[pointer-1];
			pointer++;
		}
		p[i++] = formula[pointer - 1];
		p[i] = '\0';
		w.character = p;
		w.typenum = $ID;
		return OK;
		break;*/
	}
	}
}
