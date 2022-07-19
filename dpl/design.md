# The Datapack Language Design

## Compilation stages
- [ ] Preprocessor (*)
- [ ] Lexer (Tokenizer) (*)
- [ ] Builder
- [ ] Generator

## Preprocessor
This step parses preprocessor definitions

## Lexer
This will tokenizer the preprocessed source code

## Builder
This step will build the AST

## Generator
This step converts the AST into .mcfunction files

## Example
Preprocessor input:
```
%define PP_DEF 5
%define PP_FLAG
$global_var = 15;

function start()
{
	print("Hello World!");
	# This is a comment
}

function foo(bar)
{
	print("Nice: %", $bar);
	raw
	{
		say hi
	}
}

function tick()
{
	print("TICK!");
	$baz = 2137;
%ifdef PP_FLAG
	$baz += PP_DEF;
%endif
	foo($baz);
}
```

## Possible Lexer tokens

```
VariableName (string)
NumericLiteral (int)
TextLiteral (string)

Function (string)
FunctionCall (string)

ArgsStart
ArgsEnd
BlockStart
BlockEnd

Comment (string)

OpEquals
OpAdd
OpSubtract
OpDivide
OpMultiply
OpModulo
OpLess
OpNot
OpMore
```
