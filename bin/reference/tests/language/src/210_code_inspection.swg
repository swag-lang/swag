/**
`#message` is a special function that will be called by the compiler when something specific occurs during the build. The parameter of `#message` is a mask that tells the compiler when to call the function.
*/

/**
With the `Swag.CompilerMsgMask.SemFunctions` flag, for example, `#message` will be called each time a function of the module **has been typed**. You can then use `getMessage()` in the `@compiler()` interface to retrieve some informations about the reason of the call.
*/
#message(Swag.CompilerMsgMask.SemFunctions)
{
    // Get the interface to communicate with the compiler
    let itf = @compiler()

    // Get the current message
    var msg = itf.getMessage()

    // We know that the type in the message is a function because of the '#message' mask.
    // So we can safely cast.
    var typeFunc = cast(const *Swag.TypeInfoFunc) msg.type

    // The message name, for `Swag.CompilerMsgMask.SemFunctions`, is the name of the
    // function being compiled.
    var nameFunc = msg.name

    // As an example, we count that name if it starts with "XX".
    // But we could use custom function attributes instead...
    if @countof(nameFunc) > 2 and nameFunc[0] == "X"'u8 and nameFunc[1] == "X"'u8
        G += 1
}

var G = 0

func XXTestFunc1() {}
func XXTestFunc2() {}
func XXTestFunc3() {}

/**
The compiler will call the following function after the semantic pass. So after **all the functions** of the module have been parsed.
*/
#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    // We should have found 3 functions starting with "XX"
    @assert(G == 3)
}

/**
This will be called for every global variables of the module.
*/
#message(Swag.CompilerMsgMask.SemGlobals)
{
    let itf = @compiler()
    var msg = itf.getMessage()
}

/**
This will be called for every global types of the module (structs, enums, interfaces...).
*/
#message(Swag.CompilerMsgMask.SemTypes)
{
    let itf = @compiler()
    var msg = itf.getMessage()
}