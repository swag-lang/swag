#import "core"
using core, swag, strconv

#[attributeUsage(AttributeUsage.Function)]
attr extension()

#[extension]
{
    func extension1();
    func extension2();
}

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
    if(msg.type.kind != TypeInfoKind.Func) return
    if(!reflection.hasAttribute(msg.type, "__core214.extension")) return

    g.add(@{msg.type, msg.name})
}

#compiler(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderFct: StringBuilder

    builderFct.appendString("func printAll() {\n");

    visit t: g
    {
        builderVars.appendFormat("var gen_%: s32\n", t.name)
        builderVars.appendFormat("func %(){}\n", t.name)

        builderFct.appendFormat("gen_% = 555\n", t.name)
        //builderFct.appendFormat(@"console.print("gen %%", gen_%);"@, t.name);
        builderFct.appendEOL()
    }

    builderFct.appendString("}\n");

    itf := @compiler()

    strVars := builderVars.moveToString()
    //console.print("%\n", strVars)
    itf.compileString(strVars.toString())

    strFunc := builderFct.moveToString()
    //console.print("%\n", strFunc)
    itf.compileString(strFunc.toString())
}

#placeholder printAll
#test
{
    printAll()
    @assert(gen_extension1 == 555)
    @assert(gen_extension2 == 555)
}