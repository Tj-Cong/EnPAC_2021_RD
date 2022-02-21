#pragma once
/*词法分析器*/
#include<iostream>
#include<string>
#include <gperftools/tcmalloc.h>

using namespace std;
#define ERROR 0
#define OK 1
#define $Neg 1     //! 非
#define $Impli 2   //-> 蕴含
#define $Equ 3    //<-> 等价
#define $And 4   //&& 合取
#define $Or 5     //|| 析取
#define $Alw 6    // [] G
#define $Eve 7    // <> F
#define $Until 8   // U
#define $Rele 9   // R
#define $Next 10  // X
#define $Lpar 11   // (
#define $Rpar 12  // )
#define $ID 13      //原子命题
#define $End 14
#define $root 15
#define MAXFORMLEN 20000

typedef struct Lexer_word
{
	string character;
	int typenum;
}word;

class Lexer    //词法分析器
{
private:
	char formula[MAXFORMLEN];
	int pointer;
	int length;
public:
	Lexer(string form);
	int GetWord(word &w);
};
