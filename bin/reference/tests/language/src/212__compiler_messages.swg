/**
`#message` is a special function that will be called by the compiler. The parameter is a mask that tells the compiler when to call the function.
*/

/**
Here the function will be called each time a function of the module has been typed.
*/
#message(Swag.CompilerMsgMask.SemFunctions)
{
    itf := @compiler()          // Get the interface to communicate with the compiler
    msg := itf.getMessage()     // Get the current message

    // We know the type is a function because of the '#message' mask
    typeFunc := cast(const *Swag.TypeInfoFunc) msg.type
    nameFunc := msg.name

    // For example, we register the function name if it starts with "XX".
    // But we could use custom function attributes instead...
    if @countof(nameFunc) > 2 and nameFunc[0] == "X"'u8 and nameFunc[1] == "X"'u8
        G += 1
}

/**
The compiler will call the following function after the semantic pass.
So after all the functions of the module have been parsed.
*/
#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    // We should have found 3 functions starting with "XX"
    @assert(G == 3)
}

var G = 0

func XXTestFunc1() {}
func XXTestFunc2() {}
func XXTestFunc3() {}

/**
This will be called for every global variables of the module.
*/
#message(Swag.CompilerMsgMask.SemGlobals)
{
    itf := @compiler()
    msg := itf.getMessage()
}

/**
This will be called for every global types of the module (structs, enums, interfaces...).
*/
#message(Swag.CompilerMsgMask.SemTypes)
{
    itf := @compiler()
    msg := itf.getMessage()
}