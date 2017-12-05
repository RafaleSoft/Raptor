// Micro lex description file
//
// Commands definitions

// Basic.lex must be included before this file
//	is parsed. Otherwise, forward declarations
//	must be done

atom:clear=clear
atom:select=select
atom:scale=scale
atom:translate=translate
atom:translateabsolute=translate_absolute
atom:visible=visible
atom:rotation=rotation
atom:help=help

sequence:help=<help>
sequence:clear=<clear>
sequence:selection=<select><string>
sequence:scaleobject=<scale><inparam><float><sep><float><sep><float><outparam>
sequence:translateobject=<translate><inparam><float><sep><float><sep><float><outparam>
sequence:translateobjectabsolute=<translateabsolute><inparam><float><sep><float><sep><float><outparam>
sequence:visibleobject=<visible><inparam>{boolean}<outparam>
sequence:rotateobject=<rotation><inparam><float><sep><float><sep><float><sep><float><outparam>
alternate:command={help}{clear}{selection}{scaleobject}{translateobject}{visibleobject}{rotateobject}{translateobjectabsolute}

