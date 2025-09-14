using Core, Swag, Core.StrConv

#[AttrUsage(AttributeUsage.Function)]
attr Extension()

#[Extension, Swag.PlaceHolder]
{
    func extension1();
    func extension2();
}

struct OneFunc
{
    type:     typeinfo
    name:     string
}

var g: Array'OneFunc

#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    let msg = itf.getMessage()
    if msg.type.kind != TypeInfoKind.Func:
        return
    if !Reflection.hasAttribute(msg.type, Extension):
        return

    g.add({msg.type, msg.name})
}

#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderFct:  StringBuilder

    builderFct.appendString("func printAll() {\n")

    foreach t in g
    {
        builderVars.appendFormat("var gen_%: s32\n", t.name)
        builderVars.appendFormat("func %(){}\n", t.name)

        builderFct.appendFormat("gen_% = 555\n", t.name)
        //builderFct.appendFormat(@"Console.print("gen %%", gen_%);"@, t.name);
        builderFct.appendEOL()
    }

    builderFct.appendString("}\n")

    let itf = @compiler

    let strVars = builderVars.toString()
    //Console.print("%\n", strVars)
    itf.compileString(strVars.toString())

    let strFunc = builderFct.toString()
    //Console.print("%\n", strFunc)
    itf.compileString(strFunc.toString())
}

#test
{
    printAll()
    @assert(gen_extension1 == 555)
    @assert(gen_extension2 == 555)
}
