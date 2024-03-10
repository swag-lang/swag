using Core, Swag, Core.StrConv

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
    let itf = @compiler()
    let msg = itf.getMessage()
    if !Reflection.hasAttribute(msg.type, Extension) do
        return
    g_Functions.add({msg.type, msg.name})
}

#placeholder glInitExtensions
#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    builderInit.appendString("public func glInitExtensions()\n{\n");

    visit e: g_Functions
    {
        let typeFunc = cast(const *TypeInfoFunc) e.type

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: %\n", e.name, typeFunc.name)

        // Make a wrapper function
        builderVars.appendFormat("public func %(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 do builderVars.appendString(", ")
            builderVars.appendFormat("p%: %", i, p.pointedType.name)
        }

        if typeFunc.returnType == void do
            builderVars.appendFormat(")\n{\n")
        else do
            builderVars.appendFormat(")->%\n{\n", typeFunc.returnType.name)
        builderVars.appendFormat("\treturn ext_%(", e.name)
        visit p, i: typeFunc.parameters
        {
            if i != 0 do builderVars.appendString(", ")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString(");\n}\n\n")

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n", e.name, typeFunc.name, e.name);
    }

    let itf = @compiler()
    var str = builderVars.toString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.toString()
    itf.compileString(str.toString())
}
