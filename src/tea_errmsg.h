/*
** tea_errmsg.h
** VM error messages
*/

/* This file may be included multiple times with different ERRDEF macros */

ERRDEF(MEM, "Not enough memory")

ERRDEF(STROV, "String length overflow")

ERRDEF(TOSTR,  TEA_QL("tostring") " must return a string")
ERRDEF(STKOV, "Stack overflow")
ERRDEF(CALL, TEA_QS " is not callable")

ERRDEF(NOARGS, "Expected 0 arguments but got %d")
ERRDEF(ARGS, "Expected %d arguments, but got %d")
ERRDEF(OPTARGS, "Expected at most %d arguments, but got %d")
ERRDEF(NOVAL, "Expected value")
ERRDEF(BADTYPE, "Expected %s, got %s")

ERRDEF(PATH, "Unable to resolve path " TEA_QS)
ERRDEF(NOMOD, "Unknown module " TEA_QS)
ERRDEF(NOPATH, "Could not resolve path " TEA_QS)
ERRDEF(READPATH, "Could not read " TEA_QS)

/* String buffer errors */
ERRDEF(BUFFER_SELF, "Cannot put buffer into itself")

/* VM errors */
ERRDEF(METHOD, "Undefined method " TEA_QS)
ERRDEF(OBJMETHOD, "Only objects have methods, " TEA_QS " given")
ERRDEF(MODVAR, "Undefined variable " TEA_QS " in " TEA_QS " module")
ERRDEF(NOSTATIC, TEA_QS " is not static. Only static methods can be invoked directly from a class")
ERRDEF(NOMETHOD, TEA_QS " has no method " TEA_QS)
ERRDEF(NUMSLICE, "Slice index must be a number")
ERRDEF(SLICE, TEA_QS " is not sliceable")
ERRDEF(SUBSCR, TEA_QS " is not subscriptable")
ERRDEF(INSTSUBSCR, TEA_QS " instance is not subscriptable")
ERRDEF(NUMRANGE, "Range index must be a number")
ERRDEF(IDXRANGE, "Range index out of bounds")
ERRDEF(NUMLIST, "List index must be a number")
ERRDEF(IDXLIST, "List index out of bounds")
ERRDEF(MAPKEY, "Key does not exist within map")
ERRDEF(NUMSTR, "String index must be a number, got " TEA_QS)
ERRDEF(IDXSTR, "String index out of bounds")
ERRDEF(SETSUBSCR, TEA_QS " does not support item assignment")
ERRDEF(OBJATTR, "Only objects have properties")
ERRDEF(INSTATTR, TEA_QS " instance has no property: " TEA_QS)
ERRDEF(CLSATTR, TEA_QS " class has no property: " TEA_QS)
ERRDEF(MODATTR, TEA_QS " module has no property: " TEA_QS)
ERRDEF(MAPATTR, "Map has no property: " TEA_QS)
ERRDEF(NOATTR, TEA_QS " has no property " TEA_QS)
ERRDEF(SETATTR, "Cannot set property on type " TEA_QS)
ERRDEF(UNOP, "Attempt to use " TEA_QS " unary operator with " TEA_QS)
ERRDEF(BIOP, "Attempt to use " TEA_QS " operator with " TEA_QS " and " TEA_QS)
ERRDEF(VAR, "Undefined variable " TEA_QS)
ERRDEF(RANGE, "Range operands must be numbers")
ERRDEF(UNPACK, "Can only unpack lists")
ERRDEF(MAXUNPACK, "Too many values to unpack")
ERRDEF(MINUNPACK, "Not enough values to unpack")
ERRDEF(SUPER, "Superclass must be a class")
ERRDEF(IS, "Right operand must be a class")
ERRDEF(ITER, TEA_QS " is not iterable")
ERRDEF(BUILTINSELF, "Cannot inherit from built-in " TEA_QS)
ERRDEF(SELF, "A class can't inherit from itself")
ERRDEF(EXTMETHOD, "Cannot assign extension method to " TEA_QS)
ERRDEF(VARMOD, TEA_QS " variable can't be found in module " TEA_QS)

ERRDEF(ASSERT, "Assertion failed")
ERRDEF(OPEN, "Unable to open file " TEA_QS)
ERRDEF(DUMP, "Unable to dump given function")
ERRDEF(STRFMT, "Invalid option " TEA_QS " to " TEA_QL("format"))

/* Lexer/parser errors */
ERRDEF(XNUMBER, "Malformed number")
ERRDEF(XUND, "Cannot have consecutive " TEA_QL("_") " in number literal")
ERRDEF(XLUND, "Cannot have leading " TEA_QL("_") " in number literal")
ERRDEF(XHEX, "Invalid hex number")
ERRDEF(XSCI, "Unterminated scientific notation")
ERRDEF(XSFMT, "String interpolation too deep")
ERRDEF(XSTR, "Unterminated string")
ERRDEF(XHESC, "Incomplete hex escape sequence")
ERRDEF(XUESC, "Incomplete unicode escape sequence")
ERRDEF(XESC, "Invalid escape character")
ERRDEF(XLCOM, "Unterminated block comment")
ERRDEF(XCHAR, "Unexpected character")
ERRDEF(XLOOP, "Loop body too big")
ERRDEF(XKCONST, "Too many constants in one chunk")
ERRDEF(XJUMP, "Too much code to jump over")
ERRDEF(XUPVAL, "Too many closure variables in function")
ERRDEF(XLOCALS, "Too many local variables in function")
ERRDEF(XARGS, "Can't have more than 255 arguments")
ERRDEF(XVCONST, "Cannot assign to a const variable")
ERRDEF(XSUPERO, "Can't use " TEA_QL("super") " outside of a class")
ERRDEF(XSUPERK, "Can't use " TEA_QL("super") " in a class with no superclass")
ERRDEF(XTHISO, "Can't use " TEA_QL("this") " outside of a class")
ERRDEF(XTHISM, "Can't use " TEA_QL("this") " inside a static method")
ERRDEF(XSTATIC, "Can't use " TEA_QL("static") " outside of a class")
ERRDEF(XASSIGN, "Invalid assignment target")
ERRDEF(XEXPR, "Expected expression")
ERRDEF(XDUPARGS, "Duplicate parameter name in function declaration")
ERRDEF(XSPREADARGS, "Spread parameter must be last in the parameter list")
ERRDEF(XSPREADOPT, "Spread parameter cannot have an optional value")
ERRDEF(XOPT, "Cannot have non-optional parameter after optional")
ERRDEF(XMAXARGS, "Cannot have more than 255 parameters")
ERRDEF(XMETHOD, "Invalid method name")
ERRDEF(XSINGLEREST, "Cannot rest single variable")
ERRDEF(XVALASSIGN, "Not enough values to assign to")
ERRDEF(XBREAK, "Cannot use 'break' outside of a loop")
ERRDEF(XCONTINUE, "Cannot use 'continue' outside of a loop")
ERRDEF(XCASE, "Unexpected case after default")
ERRDEF(XRET, "Can't return from top-level code")
ERRDEF(XINIT, "Can't return a value from init")
ERRDEF(XTOKEN, "Expected " TEA_QS)
ERRDEF(XDOTS, "Multiple " TEA_QL("..."))
ERRDEF(XSWITCH, "Switch statement can not have more than 256 case blocks")
ERRDEF(XVARS, "Cannot have more than 255 variables")
ERRDEF(XVASSIGN, "Not enough values to assign to")

#undef ERRDEF

/* Detecting unused error messages:
   awk -F, '/^ERRDEF/ { gsub(/ERRDEF./, ""); printf "grep -q TEA_ERR_%s *.[ch] || echo %s\n", $1, $1}' tea_errmsg.h | sh
*/