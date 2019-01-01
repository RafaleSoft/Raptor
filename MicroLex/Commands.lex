// Micro lex description file
//
// Commands definitions

// Basic.lex must be included before this file
//	is parsed. Otherwise, forward declarations
//	must be done

atom:select=select
atom:scale=scale

sequence:selection=<select><string>
sequence:scaleobject=<scale><inparam><float><sep><float><sep><float><outparam>
alternate:command={selection}{scaleobject}

