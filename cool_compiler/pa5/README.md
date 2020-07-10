## 分析

P5 实现 将我们语义分析后的代码结构，功能等转化为，电脑能够识别等汇编语言，从而实现cool的编译，实现在电脑上运行（汇编语言）。 用到 mips32 汇编 [学习地址]([https://onetale.xyz/2020/01/18/1_mips32%e6%9e%b6%e6%9e%84%e4%b8%8e%e6%8c%87%e4%bb%a4%e9%9b%86%e5%88%9d%e6%ad%a5/](https://onetale.xyz/2020/01/18/1_mips32架构与指令集初步/))

### 代码文件功能

- `cgen.h` 定义代码转换时用到的 函数，全局参数  一共五个 class 类型分别保存 `CgenClassTable` 用于符号类型（symbol）`CgenNode` 用于类类型的继承等（class）`BoolConst` bool类型用到的函数。
- `cool-tree.handcode.h` 宏定义头文件，定义最基础的，会用到的函数，类类型，全局函数，yylineno全局变量等。
- `cgen.cc` 关键代码文件。用于实现cool 语言转化为汇编指令，（机器码）二进制文件，从而能在电脑环境下运行。

- `emit.h` 寄存器名称和地址以字符串形式传递。可以通过这个文件查看对应的寄存器的名字等

  `Emit.h`

寄存器关系 [链接](https://blog.csdn.net/gujing001/article/details/8476685)

```c++
///////////////////////////////////////////////////////////////////////
//
//  Assembly Code Naming Conventions:
//
//     Dispatch table            <classname>_dispTab
//     Method entry point        <classname>.<method>
//     Class init code           <classname>_init
//     Abort method entry        <classname>.<method>.Abort
//     Prototype object          <classname>_protObj
//     Integer constant          int_const<Symbol>
//     String constant           str_const<Symbol>
//
///////////////////////////////////////////////////////////////////////

#include "stringtab.h"

#define MAXINT  100000000    
#define WORD_SIZE    4
#define LOG_WORD_SIZE 2     // for logical shifts

// Global names
#define CLASSNAMETAB         "class_nameTab"
#define CLASSOBJTAB          "class_objTab"
#define INTTAG               "_int_tag"
#define BOOLTAG              "_bool_tag"
#define STRINGTAG            "_string_tag"
#define HEAP_START           "heap_start"

// Naming conventions
#define DISPTAB_SUFFIX       "_dispTab"
#define METHOD_SEP           "."
#define CLASSINIT_SUFFIX     "_init"
#define PROTOBJ_SUFFIX       "_protObj"
#define OBJECTPROTOBJ        "Object"PROTOBJ_SUFFIX
#define INTCONST_PREFIX      "int_const"
#define STRCONST_PREFIX      "str_const"
#define BOOLCONST_PREFIX     "bool_const"


#define EMPTYSLOT            0
#define LABEL                ":\n"

#define STRINGNAME (char *) "String"
#define INTNAME    (char *) "Int"
#define BOOLNAME   (char *) "Bool"
#define MAINNAME   (char *) "Main"

//
// information about object headers
//
#define DEFAULT_OBJFIELDS 3
#define TAG_OFFSET 0
#define SIZE_OFFSET 1
#define DISPTABLE_OFFSET 2

#define STRING_SLOTS      1
#define INT_SLOTS         1
#define BOOL_SLOTS        1

#define GLOBAL        "\t.globl\t"
#define ALIGN         "\t.align\t2\n"
#define WORD          "\t.word\t"

//
// register names
//
#define ZERO "$zero"		// Zero register 
#define ACC  "$a0"		// Accumulator 
#define A1   "$a1"		// For arguments to prim funcs 
#define SELF "$s0"		// Ptr to self (callee saves) 
#define T1   "$t1"		// Temporary 1 
#define T2   "$t2"		// Temporary 2 
#define T3   "$t3"		// Temporary 3 
#define SP   "$sp"		// Stack pointer 
#define FP   "$fp"		// Frame pointer 
#define RA   "$ra"		// Return address 

//
// Opcodes
//
#define JALR  "\tjalr\t"  
#define JAL   "\tjal\t"                 
#define RET   "\tjr\t"RA"\t"

#define SW    "\tsw\t"
#define LW    "\tlw\t"
#define LI    "\tli\t"
#define LA    "\tla\t"

#define MOVE  "\tmove\t"
#define NEG   "\tneg\t"
#define ADD   "\tadd\t"
#define ADDI  "\taddi\t"
#define ADDU  "\taddu\t"
#define ADDIU "\taddiu\t"
#define DIV   "\tdiv\t"
#define MUL   "\tmul\t"
#define SUB   "\tsub\t"
#define SLL   "\tsll\t"
#define BEQZ  "\tbeqz\t"
#define BRANCH   "\tb\t"
#define BEQ      "\tbeq\t"
#define BNE      "\tbne\t"
#define BLEQ     "\tble\t"
#define BLT      "\tblt\t"
#define BGT      "\tbgt\t"

```



### 代码分析

我们的 `string` , `ing` , `bool`  这两个常数类型，被保存在自定义的表中 `stringtable` `inttable` 表中，而bool类型只有两个值不会生成表



将此`代码`和`代码生成器树的类列表`传递给`“CgenClassTable”`的构造函数。

```cc
void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}
```

在cage.cc 文件中已经定义好了基础的 函数。

定义 push 栈，当有寄存器被push 进栈时。栈向低地址放心增长 `emit_addiu(SP,SP,-4,str);` (-4大小)

```c++
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}
```

将int的整数值保存在寄存器，将寄存器的值复制给int整数部分

```c++
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }
```



我们要启动 `.data` 段，`.text` 段，并且声明他们的全局名称，并将初始引用记录下来。

---

初始化`stringtable` 和 `inttable` （初始 “”，0）然后将扫描得到的 string int bool 分别保存到对应的table

```c++
void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);
  code_bools(boolclasstag);
}
```



对我们定义的类的 基础属性进行一个记录。名字，类型。以此记录下来。

让我们去记录 这个类 拥有的方法，方法名字，方法属性等，（先扫描记录父类中拥有的方法函数，然后记录下自身定义的心的方法函数）要注意是否 `override` 还要记录起大小偏移。

对应类里面根据名字生成 新的 树 `symboletable`

```c++
void CgenClassTable::code_dispatchTab()
{
	for (List<CgenNode> *l = nds; l; l = l->tl())	{
		CgenNodeP node = l->hd();
		dispatch_list[node->get_name()] = std::vector<Symbol>();
		dispatch_table[node->get_name()] 
					= std::map<Symbol, Class_Offset>();
					//= std::map<Symbol, std::map<Symbol, int> >();
		add_method(node, node->get_name());
	}
	for (List<CgenNode> *l = nds; l; l = l->tl())	{
		CgenNodeP node = l->hd();
		Symbol name = node->get_name();
		str << name << DISPTAB_SUFFIX << LABEL;
		std::vector<Symbol> *vec = &(dispatch_list[name]);
		std::vector<Symbol>::iterator itr;
#if 0
		for (size_t index = 0; index != (*vec).size(); ++index) {
			Symbol m_name = (*vec)[index]; // the method name
			std::map <Symbol, int> *sym_off = &dispatch_table[name][m_name];
			for (std::map<Symbol, int>::iterator it = (*sym_off).begin();
				it != (*sym_off).end(); ++it) {
				if (it->second == (int)index) {
		
					str << WORD << (it->first) << METHOD_SEP << m_name <<endl; 
				}
			}
		}
#endif
		for (itr = (*vec).begin(); itr != (*vec).end(); ++itr) {	
			Symbol cla = dispatch_table[name][*itr].cla;
			str << WORD << cla << METHOD_SEP << (*itr) <<endl; 
		}

	}
}
```



对于每个 类的基本属性，在每个表中，用名字作为下标记录，不同属性的表都会用 name作为下标来保存值。

`cgenNode_table[name] = node;`  `attr_list[name]= std::vector<Symbol>();` 等。



转换class method 的时候

1. 循环遍历 nds 表 排除`if (name == Object || name == IO || name == Str) continue;`  
2. 当得到一个对应的方法等时候。会将对应的参数放入栈中。然后会call 这个方法类，在方法执行完返回时，会 pop出栈，恢复栈平衡

```c++
for (int i = fs->first(); fs->more(i); i = fs->next(i)) {
			method_class *method = dynamic_cast<method_class*>(fs->nth(i));
			if (method == NULL) continue;
			// the  all the parameters and store them in the stack
			Formals fs = method->formals;
			method_args.clear();
			for (int i = fs->first(); fs->more(i); i = fs->next(i)) {
				method_args.push_back(fs->nth(i)->get_name());
			}
	 
			str << node->get_name() << METHOD_SEP << method->name << LABEL;
			// same as class init
			emit_addiu(SP, SP, -12, str);
			emit_store(FP, 3, SP, str);
			emit_store(SELF, 2, SP, str);
			emit_store(RA, 1, SP, str); 
			emit_addiu(FP, SP, 4, str); 
			// notice, this is where self is set
			// when dispatch a method , ACC must set to the object address
			emit_move(SELF, ACC, str);  
			// before dispatch other code , s0 is set to self
			method->expr->code(str, cur_class);
			
			// after the method call 
			// pop the stack	
			emit_load(FP, 3, SP, str);  // pop the stack 
			emit_load(SELF, 2, SP, str);
			emit_load(RA, 1, SP, str);
			emit_addiu(SP, SP, 12 + method->formals->len() * 4, str);
			emit_return(str);
		}
```



然后初始化 `CgenClassTable`

```c++
CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
   stringclasstag = 0 /* Change to your String class tag here */;
   intclasstag =    0 /* Change to your Int class tag here */;
   boolclasstag =   0 /* Change to your Bool class tag here */;

   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();

   code();
   exitscope();
}
```

定义最基础的类 `Object类` `IO类` 然后定义 `int类` `bool类` `str类`

```c++
Object类没有父类。 其方法是
        	 cool_abort（）：对象中止程序
         	type_name（）：Str返回类名的字符串表示形式
        	 copy（）：SELF_TYPE返回对象的副本

IO类继承自Object。 其方法是
         out_string（Str）：SELF_TYPE将字符串写入输出
         out_int（Int）：SELF_TYPE“一个int”“”
         in_string（）：Str从输入中读取一个字符串
         in_int（）：Int“一个int”“”

Str类具有许多插槽和操作：
       	val					???
        str_field	字符串本身
        length（）：字符串的整数长度
        concat（arg：Str）：Str字符串连接
        substr（arg：Int，arg2：Int）：Str子字符串
```



然后重新生成类的 继承树，  设置继承树中类的 父类，和子类关系。将合法类添加到 类列表和符号表 。

最后 实现不同类型的 code函数，编译不同的函数功能。

