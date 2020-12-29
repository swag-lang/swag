#import "core"
using core, swag, strconv

#[attributeUsage(AttributeUsage.Function)]
attr extension()

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

    builderInit.appendString("public func glInitExtensions()\n{\n");

    visit e: g
    {
        typeFunc := cast(const *TypeInfoFunc) e.type
        typeFunc = autocast typeFunc.rawType

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: func%\n", e.name, typeFunc.name)

        // Make a wrapper function
        builderVars.appendFormat("public func %(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%: %", i, p.pointedType)
        }

        builderVars.appendString(")\n{\n")
        builderVars.appendFormat("\text_%(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString(");\n}\n\n")

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("\text_% = @mkforeign(func%, getExtensionAddress(@dataof(\"%\")))\n", e.name, typeFunc.rawType.name, e.name);
    }

    itf := @compiler()
    str := builderVars.moveToString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.moveToString()
    itf.compileString(str.toString())
}
