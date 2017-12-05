//#include Expressions.lex
//#include BExpressions.lex

//atom:integer=[0123456789]+
//atom:float=<integer>[.][0123456789]*

//atom:identifier=[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]+
//atom:inbloc={
//atom:outbloc=}
//atom:endstatement=;
atom:string=["][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*["]

//atom:typeint=int
//atom:typefloat=float

//alternate:type=<typeint><typefloat>
//sequence:valparameter={type}<identifier>
//sequence:pointerparameter={type}<mult><identifier>
//alternate:parameter={valparameter}{pointerparameter}{expr}

//sequence:function={type}<identifier><in>{parameter}<out><inbloc><outbloc>

//atom:constant=constant
//atom:const=const


alternate:word=<string>
//alternate:word=<integer><float>
//alternate:word=<identifier><typeint><typefloat>
//alternate:word=<constant><const>
sequence:words={word}{words}

sequence:main={words}