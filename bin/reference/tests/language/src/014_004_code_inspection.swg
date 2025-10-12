/**
# '#message' Function

The '#message' function in Swag is a special compiler hook invoked when certain
build events occur. It allows custom actions or checks to run at specific
stages of compilation.

The function receives a mask defining which events trigger it, such as
function typing, global variable processing, or semantic pass completion.
*/

/**
# Function Message Mask

Using 'Swag.CompilerMsgMask.SemFunctions', '#message' runs whenever a function
in the module is fully typed (its type info is available). Inside the function,
'@compiler.getMessage()' retrieves information about the event.
*/
#message(Swag.CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    let msg = itf.getMessage()

    // Since the message is about a function, cast its type.
    let typeFunc = cast(const *Swag.TypeInfoFunc) msg.type
    let nameFunc = msg.name

    // Count functions starting with "XX"
    if @countof(nameFunc) > 2 and nameFunc[0] == 'X' and nameFunc[1] == 'X' do
        G += 1
}

// Global counter for functions starting with "XX"
var G = 0

func XXTestFunc1() {}
func XXTestFunc2() {}
func XXTestFunc3() {}

/**
# Semantic Pass Completion

After the semantic pass (once all functions are parsed and typed),
'#message(Swag.CompilerMsgMask.PassAfterSemantic)' is triggered.
Ideal for whole-module checks or summary actions.
*/
#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    @assert(G == 3)
}

/**
# Global Variables Message Mask

'Swag.CompilerMsgMask.SemGlobals' triggers for each global variable during
semantic analysis. This can be used to validate or analyze globals.
*/
#message(Swag.CompilerMsgMask.SemGlobals)
{
    let itf = @compiler
    var msg = itf.getMessage()

    // Example placeholder for analyzing global variable properties
}

/**
# Global Types Message Mask

'Swag.CompilerMsgMask.SemTypes' triggers for each global type (struct, enum,
interface) encountered by the compiler. This enables compile-time inspection
or analysis of type information.
*/
#message(Swag.CompilerMsgMask.SemTypes)
{
    let itf = @compiler
    var msg = itf.getMessage()

    // Example placeholder for analyzing global type attributes
}
