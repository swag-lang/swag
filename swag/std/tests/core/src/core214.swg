#import "core"
using core, swag

#[attributeUsage(AttributeUsage.Function)]
attr extension()

#[extension]
func thisIs();

var g: Array'typeinfo

#compiler(CompilerMsgMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    if(msg.type.kind != TypeInfoKind.Func) return

    type := cast(const* TypeInfoFunc) msg.type

    visit i: type.attributes
    {
        if(i.name == "__core214.extension")
        {
            console.print("%\n", type)
        }
    }
}
