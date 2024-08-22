/**
# `#message` Function
`#message` is a special function that will be called by the compiler when certain events occur during the build process. The parameter of `#message` is a mask that tells the compiler when to trigger the function. This allows you to hook into specific compilation stages and perform custom actions or checks.
*/

/**
# Function Message Mask
For example, with the `Swag.CompilerMsgMask.SemFunctions` flag, `#message` will be called each time a function in the module **has been typed**. You can use `getMessage()` in the `@compiler()` interface to retrieve information about the event that triggered the call.
*/
#message(Swag.CompilerMsgMask.SemFunctions)
{
    // Get the interface to communicate with the compiler
    let itf = @compiler()

    // Get the current message
    let msg = itf.getMessage()

    // Since the mask is `Swag.CompilerMsgMask.SemFunctions`, we know that the type
    // in the message is a function. We can safely cast it.
    let typeFunc = cast(const *Swag.TypeInfoFunc) msg.type

    // The message name for `Swag.CompilerMsgMask.SemFunctions` is the name of the
    // function being compiled.
    let nameFunc = msg.name

    // Example: Count functions starting with "XX"
    if @countof(nameFunc) > 2 and nameFunc[0] == `X` and nameFunc[1] == `X`:
        G += 1
}

// Global variable to count functions starting with "XX"
var G = 0

// Example functions to demonstrate `#message` functionality
func XXTestFunc1() {}
func XXTestFunc2() {}
func XXTestFunc3() {}

/**
# Semantic Pass Completion
The compiler will call the following function after the semantic pass, which occurs after **all functions** in the module have been parsed. This is useful for performing checks or actions once the entire module has been processed.
*/
#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    // Ensure that exactly 3 functions starting with "XX" were found
    @assert(G == 3)
}

/**
# Global Variables Message Mask
This `#message` will be called for every global variable in the module. This allows you to perform actions or checks on each global variable.
*/
#message(Swag.CompilerMsgMask.SemGlobals)
{
    let itf = @compiler()
    var msg = itf.getMessage()
    // You can process the message as needed
}

/**
# Global Types Message Mask
This `#message` will be called for every global type in the module, such as structs, enums, and interfaces. You can use this to analyze or modify global types during compilation.
*/
#message(Swag.CompilerMsgMask.SemTypes)
{
    let itf = @compiler()
    var msg = itf.getMessage()
    // You can process the message as needed
}
