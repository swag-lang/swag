#import "core"
using core, swag, strconv

struct OneFunc
{
    type: typeinfo
    name: string
}

var g: Array'OneFunc

#compiler(CompilerMsgMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    if(!reflection.hasAttribute(msg.type, "extension")) return
    g.add(@{msg.type, msg.name})
}

// For each extension, we will generate :
//
// global :
// var ext_Function = func(...)
// public func Function(...) { ext_Function(...) }
//
// in initExtensions() :
// ext_Function = @mkforeign(func(...), getExtensionAddress("Function"))

#compiler(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    builderInit.appendString("func initExtensions() {\n");

    visit e: g
    {
        typeFunc := cast(const *TypeInfoFunc) e.type
        typeFunc = cast(const *TypeInfoFunc) typeFunc.rawType

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: func%\n", e.name, typeFunc.name)

        // Make a wrapper function
        builderVars.appendFormat("public func %(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 builderVars.appendString(",")
            builderVars.appendFormat("p%: %", i, p.pointedType)
        }

        builderVars.appendString(") {\n")
        builderVars.appendFormat("ext_%(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 builderVars.appendString(",")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString("); }\n")

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("ext_% = @mkforeign(func%, getExtensionAddress(@dataof(\"%\")))\n", e.name, typeFunc.rawType.name, e.name);
    }

    itf := @compiler()
    str := builderVars.moveToString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.moveToString()
    itf.compileString(str.toString())
}

#placeholder initExtensions
#init
{
    initExtensions()
}