#import "core"
using core, swag, strconv

#[attributeUsage(AttributeUsage.Function)]
attr extension()

struct OneFunc
{
    type: typeinfo
    name: string
}

#[compiler]
private var g: Array'OneFunc

#compiler(CompilerMsgMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    if(!reflection.hasAttribute(msg.type, "ogl.extension")) return
    g.add(@{msg.type, msg.name})
}

#compiler(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    builderInit.appendString("public func glInitExtensions()\n{\n");

    visit e: g
    {
        typeFunc := cast(const *TypeInfoFunc) e.type

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: func%\n", e.name, typeFunc.flatName)

        // Make a wrapper function
        builderVars.appendFormat("public func %(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%: %", i, p.pointedType.flatName)
        }

        builderVars.appendFormat(")->%\n{\n", typeFunc.returnType.flatName)
        builderVars.appendFormat("\treturn ext_%(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString(");\n}\n\n")

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("\text_% = @mkforeign(func%, getExtensionAddress(@dataof(\"%\")))\n", e.name, typeFunc.flatName, e.name);
    }

    itf := @compiler()
    str := builderVars.moveToString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.moveToString()
    itf.compileString(str.toString())
}
