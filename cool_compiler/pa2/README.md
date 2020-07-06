## 分析

全局字符指针yytext中提供与该匹配相对应的文本(称为token)，并在全局int变量yyleng中提供长度。

然后执行与匹配模式(pattern)相对应的操作(action)，然后扫描剩余的输入寻找下一个匹配。



%x 定义的区域可以在规则部分 用  BEGIN() 进入。

进入后 正则匹配 对呀 <> 前缀的环境下进行

```c++
%x STRING
%%
xxx  BEGIN(STRING)

<STRING>正则 {操作}
%%
自定义函数
```



### 定义我们自己设置的函数。

```c#
int comment_depth = 0;
int string_len;
void setErrMsg(char* msg);
bool strTooLong();
int max_strlen_err();

%%
正则
%%
(自定义函数)
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
```

上面的定义 定义在 %{    xxxx %}.  xxx里面



### 然后是定义扫描的 作用域名字

``` C#
%x COMMENT
%x S_LINE_COMMENT
%x STRING
```

为了保证 代码 安装类型去进行 扫描 并判断基础语法的闭合等是否正确



### 定义不同类型的 正则匹配逻辑

这里定义 匹配到后应该返回什么

```c#
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
```



### 扫描的正则匹配

1. 需要判断（） 前后是否闭合
2. 当然匹配到 前括号 于是开始扫描匹配 括号内到能容 括号个数用 `comment_depth` 变量来统计
3. 匹配内容时还要统计 是否换行等用 `curr_lineno` 变量来记录
4. 如果匹配到的 `)` 后 `comment_depth` == 0 这时代码正常 否者 存储错误



1. 然后匹配运算符，类型名字，`class, if ,else` 等
2. 染匹配 `string` 的数据并保存，切判断 `""` 是否闭合