using Swag

var cpt = 0's64

#[compiler]
func testInline(type: const* TypeInfoFunc)
{
    visit i: type.attributes
    {
        if(i.name == "Swag.inline")
        {
            cpt += 1
            //@print(type.name)
            //@print("\n")
        }
    }
}

//#[Swag.PrintBc]
#compiler(CompilerMsgMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    @assert(msg != null)
    if(msg.type.kind != TypeInfoKind.Func) return

    testInline(cast(const* TypeInfoFunc) msg.type)
    cpt += 1
}
