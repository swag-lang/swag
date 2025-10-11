/**
# `#message` Function

The `#message` function in Swag is a special hook that gets invoked by the compiler
when specific events occur during the build process. This function allows you to
intercept certain stages of compilation and execute custom actions or checks.
The parameter of `#message` is a mask that specifies the compilation stage or
event that should trigger the function. By utilizing these hooks, developers
can gain deeper insights into the compilation process and perform additional
processing when necessary.
*/

/**
# Function Message Mask

For instance, when you use the `Swag.CompilerMsgMask.SemFunctions` flag, the `#message`
function will be called each time a function within the module has been successfully
typed. This means the function has been fully analyzed and its type information is
available. Within the `@compiler` interface, you can use the `getMessage()` method
to retrieve details about the event that triggered the call, such as the function's name
and type information.
*/
#message(Swag.CompilerMsgMask.SemFunctions)
{
    // Obtain the compiler interface to interact with the compilation process
    let itf = @compiler

    // Retrieve the current compilation message
    let msg = itf.getMessage()

    // Given that the mask is `Swag.CompilerMsgMask.SemFunctions`, the message
    // pertains to a function, allowing us to safely cast the type information.
    let typeFunc = cast(const *Swag.TypeInfoFunc) msg.type

    // The message name, in this case, corresponds to the function's name being compiled.
    let nameFunc = msg.name

    // Example: Count functions whose names start with "XX"
    if @countof(nameFunc) > 2 and nameFunc[0] == 'X' and nameFunc[1] == 'X' do
        G += 1
}

// Global variable to count the number of functions starting with "XX"
var G = 0

// Example functions to demonstrate the functionality of the `#message` hook
func XXTestFunc1() {}
func XXTestFunc2() {}
func XXTestFunc3() {}

/**
# Semantic Pass Completion

The compiler will invoke the following `#message` function after the semantic pass
has completed. The semantic pass occurs after all functions within the module have
been parsed and typed. This stage is ideal for performing final checks or actions
that need to consider the entire module's content.
*/
#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    // Verify that exactly 3 functions starting with "XX" were found during the compilation
    @assert(G == 3)
}

/**
# Global Variables Message Mask

The `#message` function can also be triggered for each global variable in the module
by using the `Swag.CompilerMsgMask.SemGlobals` flag. This allows you to process or
validate each global variable as it is encountered by the compiler during the
semantic analysis phase.
*/
#message(Swag.CompilerMsgMask.SemGlobals)
{
    // Retrieve the compiler interface
    let itf = @compiler

    // Get the current message, which contains details about the global variable
    var msg = itf.getMessage()

    // Process the message as needed, for example, by analyzing the global variable's properties
}

/**
# Global Types Message Mask

Similarly, the `Swag.CompilerMsgMask.SemTypes` flag triggers the `#message` function
for each global type in the module, such as structs, enums, and interfaces.
This allows you to inspect, analyze, or modify global types during compilation.
*/
#message(Swag.CompilerMsgMask.SemTypes)
{
    // Access the compiler interface
    let itf = @compiler

    // Retrieve the current message, which contains information about the global type
    var msg = itf.getMessage()

    // Process the message as required, which could include analyzing type attributes or properties
}
