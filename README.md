An implementation of BASIC \
\
Made for hackClub terra 26/27 \

### How to use: \
 \
Type in an unsigned int to write to a line number. \
The interpreter is case-insensitive for keywords but not variable names. \
Use CTRL+C to cancel the running program at any time \
 \
### HYA CLOVER!!! <3 Hope you're reading this \
 \
### Supported functions \
 \
#### Operators \
"," Seperate arguments for functions \
"()" Everyone knows what parentheses do \
"+" Add stuff. Literally anything (that is a value) \
"-" Subtract. Numbers only \
"*" Multiply. Numbers only \
"/" Divide. Numbers only \
"^" Powers. Numbers only \
"<" Less than \
">" Greater than \
"<=" Less than or equal \
">=" Greater than or equal \
"OR" Please note that all boolean operations are bitwise and that true is -1 and false is 0 \
"AND" \
"NOT" \
 \
#### Keywords \
"REM" Use at start of line \
"LIST" List program. Provide an int to only see that line \
"RUN" Run program \
"LET" Define variable, optional \
"DIM" Declare variable, necessary \
"PRINT" Print anything \
"INPUT" Provide a string and a variable "NO COMMA" to get user input \
"GOTO" Goto line 10, 20 or heck even 100 just don't go to france \
"GOSUB" Go subroutine (What did you expect???) \
"RETURN" No more sub \
"END" Never use this. BASIC FOREVER \
"IF" Branching is for losers \
"ELSE" Branching for huge losers \
"THEN" Going to need this to branch \
 \
#### Functions \
"SIN" Pretty acurate name for trigonometry \
"COS" Why not? \
"TAN" Why? \
"ASIN" It's a-sin! \
"ACOS" Uhhhh \
"ATAN" Sure. \
"ABS" A true C programmer's most distant dream \
"LEFT" BIDEN BLAST \
"RIGHT" Lost the plot \
"MID" is a solid choice \
"STR" Alternatively use ""+x to save literally nothing \
"VAL" Write your own parser. Mine sucks \
 \
### Example program \
I coul've written a loading function and maybe I will someday but to increas accuracy I've decided to make you all type these in by hand. Have fun and NO TYPOS!!! \
 \
#### Poor man's calculator \
 \
1 dim a \
2 dim b \
3 dim res \
4 gosub 100 \
5 print "RES: " + res \
6 end REM HYPOCRITE! \
 \
100 input "A: " a \
101 input "B: " b \
102 a = val(a) \
103 b = val(b) \
104 res = a + b \
105 return \

