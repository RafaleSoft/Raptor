// Micro lex description file
//
// Atoms definitions

atom:false=false
atom:true=true
atom:eq===
atom:ge=\>=
atom:le=\<=
atom:gt=\>
atom:lt=\<
atom:neq=!=
atom:not=!

// Rules definitions

forward:boolexpr

alternate:predicate=<false><true>
alternate:boolop=<eq><ge><le><gt><lt><neq>
sequence:bbinexpr={boolexpr}{boolop}{boolexpr}
sequence:bunexpr=<not>{boolexpr}
sequence:bpexpr=<in>{boolexpr}<out>

alternate:boolexpr={predicate}{bunexpr}{bpexpr}{bbinexpr}

// Main rule
//sequence:main={boolexpr}