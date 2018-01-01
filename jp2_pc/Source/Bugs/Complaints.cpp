//
// Deficiency:  Return convention for small classes is inconsistent with documentation,
// and is inefficient.
//
// The help topic "Argument Passing and Naming Conventions" states:
//
//		All arguments are widened to 32 bits when they are passed. Return values are also 
//		widened to 32 bits and returned in the EAX register, except for 8-byte structures, 
//		which are returned in the EDX:EAX register pair. Larger structures are returned in 
//		the EAX register as pointers to hidden return structures. Parameters are pushed onto 
//		the stack from right to left.
//
// This is incorrect.  Class return values always seem to be returned as a pointer to a
// hidden return structure, even when they are 4 bytes in size.
//
// This is very inefficient, especially when trying to code small numerical types like
// fixed point.  And the lack of documentation correctness makes it hazardous to try to code
// assembly functions.
//

//
// Feature request: more explicit error messages.
//
// I'm not asking for much.  But it would help *IMMENSELY* if when the compiler detects an
// ambiguous call to overloaded functions, it would *PRINT OUT WHICH FUNCTIONS IT'S TALKING ABOUT*,
// in fully qualified prototype form.  You just don't know how much time that would save.  
// With operator functions, it's *impossible* to search through all of them and find out which
// ones the compiler is talking about.  Even with named functions, it's difficult if one has many
// overloaded versions of a function, and one is trying to read the compiler's mind and figure out
// what made it think the call was ambiguous.  Please just *SAY* which functions are ambiguous.
//
// You don't even need to print out *all* the ambiguous functions.  Any two will do, and will 
// let me eliminate ambiguities one at a time.
//
