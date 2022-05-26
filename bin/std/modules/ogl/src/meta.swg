using Core, Swag, StrConv

#[AttrUsage(AttributeUsage.Function)]
attr Extension()

struct OneFunc
{
    type: typeinfo
    name: string
}

#[Compiler]
var g_Functions: Array'OneFunc

#message(CompilerMsgMask.SemFunctions)
{
    itf := @compiler()
    msg := itf.getMessage()
    if !Reflection.hasAttribute(msg.type, @stringof(Extension))
        return
    g_Functions.add(@{msg.type, msg.name})
}

#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    builderInit.appendString("public func glInitExtensions()\n{\n");

    visit e: g_Functions
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

        if typeFunc.returnType == void
            builderVars.appendFormat(")\n{\n")
        else
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
    str := builderVars.toString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.toString()
    itf.compileString(str.toString())
}
