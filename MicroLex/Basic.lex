// Micro lex description file
//
// Atoms definitions

atom:integer=[-]?[0123456789]+
atom:float=<integer>[.][0123456789]*
atom:identifier=[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*
atom:string=["][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*["]

atom:linecomment=[/][/][^\$]*

atom:inbloc={
atom:outbloc=}
atom:inparam=(
atom:outparam=)
atom:sep=,
atom:set==
atom:end=;

map:integer=int
map:float=float
map:string=string
map:identifier=string

comment:linecomment