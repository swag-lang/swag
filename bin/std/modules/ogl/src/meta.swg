using Core, Swag, Core.StrConv

#[AttrUsage(AttributeUsage.Function)]
attr Extension()

struct OneFunc
{
    type:     typeinfo
    name:     string
}

#[Compiler]
var g_Functions: Array'OneFunc

#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    let msg = itf.getMessage()
    if !Reflection.hasAttribute(msg.type, Extension) do
        return
    g_Functions.add({msg.type, msg.name})
}

#[Compiler]
func getSig(typeFunc: const *Swag.TypeInfoFunc, namedArgs: bool)->String
{
    var result: retval = "("

    foreach p, i in typeFunc.parameters
    {
        if i != 0 do
            result += ","
        if namedArgs do
            result += Format.toString("p%: ", i)
        if p.pointedType.flags & .Nullable do
            result += "#null "
        result += p.pointedType.name
    }

    result += ")"

    if typeFunc.returnType != void
    {
        result += "->"
        result += typeFunc.returnType.name
    }

    return result
}

#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    builderInit.appendString("public func glInitExtensions()\n{\n")

    foreach e in g_Functions
    {
        let typeFunc = cast(const *TypeInfoFunc) e.type

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: func", e.name)
        builderVars.appendString(getSig(typeFunc, false))
        builderVars.appendString("\n")

        // Make a wrapper function
        builderVars.appendFormat("public func %", e.name)
        builderVars.appendString(getSig(typeFunc, true))
        builderVars.appendString("\n{\n")
        builderVars.appendFormat("\treturn ext_%(", e.name)
        foreach p, i in typeFunc.parameters
        {
            if i != 0 do
                builderVars.appendString(",")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString(");\n}\n\n")

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("\text_% = cast(func%) getExtensionAddress(@dataof(\"%\"))\n", e.name, getSig(typeFunc, false), e.name)
    }

    let itf = @compiler
    var str = builderVars.toString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n")
    str = builderInit.toString()
    itf.compileString(str.toString())
}
