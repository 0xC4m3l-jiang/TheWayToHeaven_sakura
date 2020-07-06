/*
 *  The scanner definition for COOL.
 */

  /*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don not remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

/* to assemble string constants */
char string_buf[MAX_STR_CONST]; 
char *string_buf_ptr;
extern int curr_lineno;
extern int verbose_flag;
extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here 
 */
/* Judge whether brackets match*/
int comment_depth = 0;
int string_len;
void setErrMsg(char* msg);
bool strTooLong();
int max_strlen_err();

%}
/* Activity status */
%x COMMENT
%x S_LINE_COMMENT
%x STRING
/* Define names for regular expressions here.*/

/**/
WHITESPACE      [\ \f\r\t\v]
NUM [0-9]
ALPHANUMERIC [a-zA-Z0-9]
/* Starts with an uppercase character */
TYPE [A-Z]{ALPHANUMERIC}*  
/* Starts with a lowercase character*/
OBJECTID [a-z]{ALPHANUMERIC}*
DARROW  =>
LE      <=
ASSIGN  <-

/* Match string */
CLASS       (?i:class)
ELSE        (?i:else)
FI          (?i:fi)
IF          (?i:if)
IN          (?i:in)
INHERITS    (?i:inherits)
LET         (?i:let)
LOOP        (?i:loop)
POOL        (?i:pool)
THEN        (?i:then)
WHILE       (?i:while)
CASE        (?i:case)
NEW         (?i:new)
ISVOID      (?i:isvoid)
OF          (?i:of)
NOT         (?i:not)

INT_CONST   {NUM}+


%%

 /* the Comments 
  * -------------------------------------*/
  /* eat "("  COMMENT start */
"(*"                {
                        comment_depth++;
                        BEGIN(COMMENT);
                    }
  /* continue eat "(*" comment_depth ++*/
<COMMENT>"(*"       {
                        comment_depth++;
                    }
  /* if comment do nothing */
<COMMENT>.          {}
  /* if \n the line ++*/
<COMMENT>\n         {   curr_lineno++;}
  /* if not recvived */
<COMMENT><<EOF>>    {
                        setErrMsg("EOF in comment");
                        BEGIN(0);
                        return ERROR;
                    }
  /* if start recv is "*)" it's wrong */
"*)"                {
                        setErrMsg("Unmatched *)");
                        BEGIN(0);
                        return ERROR;
                    }
  /* cool notes */
"--"                {    BEGIN(S_LINE_COMMENT); }
<S_LINE_COMMENT>.   {}
<S_LINE_COMMENT>\n  {
                          curr_lineno++;
                          BEGIN(0);
                    }
"false"             {
                          cool_yylval.boolean=false;
                          return BOOL_CONST;
                    }
"true"              {
                          cool_yylval.boolean = true;
                          return BOOL_CONST;
                    }

  /* if is white space do nothing */
{WHITESPACE} {}

 /* The multiple-character operators.*/
{NUM}               {
                          cool_yylval.symbol = inttable.add_string(yytext);
                          return INT_CONST;
                    }
                    
"=>"        { return DARROW; }
"=<"        { return LE; }
"<-"        { return ASSIGN; }
"<"         { return '<'; }
"@"         { return '@'; }
"~"         { return '~'; }
"="         { return '='; }
"."         { return '.'; }
"-"         { return '-'; }
","         { return ','; }
"+"         { return '+'; }
"*"         { return '*'; }
"/"         { return '/'; }
"}"         { return '}'; }
"{"         { return '{'; }
")"         { return ')'; }
"("         { return '('; }
":"         { return ':'; }
";"         { return ';'; }
 
 /* keyword */
{CLASS}     { return CLASS; }
{ELSE}      { return ELSE; }
{FI}        { return FI; }
{IF}        { return IF; }
{IN}        { return IN; }
{INHERITS}  { return INHERITS; }    
{LET}       { return LET; } 
{LOOP}      { return LOOP; }    
{POOL}      { return POOL; }
{THEN}      { return THEN; }
{WHILE}     { return WHILE; }
{CASE}      { return CASE; }
{NEW}       { return NEW; }
{OF}        { return OF; }
{NOT}       { return NOT; }
{ISVOID}    { return ISVOID; }


{TTYPE}     {
              cool_yylval.symbol = stringtable.add_string(yytext);
            }
{OBJECTID}  {
              cool_yylval.symbol = stringtable.add_string(yytext);
            }

 /* if recv \" , string is strat */
 \"         {
              BEGIN(STRING);
              string_len = 0;
            }
<STRING>\" {
              cool_yylval.symbol = stringtable.add_string(string_buf);
              string_buf[0] = '\0';
              BEGIN(0);
              return (STR_CONST);
            }
<STRING><<EOF>> {
                    cool_yylval.error_msg = "EOF in string constant";
                    return ERROR;
                }
<STRING>\0  {
              setErrMsg("String contains null character");
              string_buf[0] = '\0';
              return ERROR;            
            }
<STRING>\n    {
                setErrMsg("Unterminated string constant.");
                string_buf[0] = '\0';
                curr_lineno++;
                BEGIN(0);
                return ERROR;
              }   
<STRING>\\\n { 
                if(strTooLong()) return max_strlen_err();
                curr_lineno++; 
                string_len++;
                strcat(string_buf, '\n');
              }
<STRING>\\t     {
                    if (strTooLong()) { return max_strlen_err(); }
                    string_len++;
                    strcat(string_buf, '\t');
                }
<STRING>\\b     {
                    if (strTooLong()) { return max_strlen_err(); }
                    string_len++;
                    strcat(string_buf, '\b');
	            }
<STRING>\\f     {
                    if (strTooLong()) { return max_strlen_err(); }
                    string_len++;
                    strcat(string_buf, '\f');
	            }  
<STRING>.       {
                    if (strTooLong()) { return max_strlen_err(); }
                    string_len++;
                    strcat(string_buf, yytext);
	            }



%%

void setErrMsg(char* msg) {
    cool_yylval.error_msg = msg;
}int max_strlen_err() { 
    BEGIN(INITIAL);
    cool_yylval.error_msg = "String constant too long";
    return ERROR;
}
bool strTooLong() {
	if (string_len + 1 >= MAX_STR_CONST) {
		BEGIN(STRING_ERR);
        return true;
    }
    return false;
}
int max_strlen_err() { 
    BEGIN(INITIAL);
    cool_yylval.error_msg = "String constant too long";
    return ERROR;
}