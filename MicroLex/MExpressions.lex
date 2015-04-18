// Micro lex description file
//
// Atoms definitions

atom:integer=[-]?[0123456789]+
atom:float=<integer>[.][0123456789]*
atom:identifier=[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*

atom:in=(
atom:out=)
atom:mult=[*]
atom:add=[+]

// comments
atom:linecomment=[/][/][^\$]*
comment:linecomment


// Type mapping
map:integer=int
map:float=float
map:identifier=CString


// Rules definitions
forward:mult_expression
forward:add_expression
forward:expression

sequence:parent_expression=<in>{expression}<out>
alternate:literal=<integer><float><identifier>
alternate:primary_expression={literal}{parent_expression}
sequence:mult_expression_loop={primary_expression}<mult>{mult_expression}
alternate:mult_expression={primary_expression}{mult_expression_loop}
sequence:add_expression_loop={mult_expression}<add>{add_expression}
alternate:add_expression={mult_expression}{add_expression_loop}
sequence:expression={add_expression}

// Main rule
sequence:main={expression}