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

#compiler(CompilerMsgMask.PassAfterSemantic)
{
    itf := @compiler()

    var builderVars: StringBuilder
    var builderInit: StringBuilder
    var str: String

    builderInit.appendString("func initExtensions() {\n");

    visit e: g
    {
        builderVars.appendFormat("var ext_%: const *void\n", e.name)
        builderInit.appendFormat("ext_% = getExtensionAddress(@dataof(\"%\"))\n", e.name, e.name);
    }

    str = builderVars.moveToString()
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