using swag

var cpt = 0's64

func testInline(type: const* TypeInfoFunc)
{
    visit i: type.attributes
    {
        if(i.name == "swag.inline")
        {
            cpt += 1
            //@print(type.name)
            //@print("\n")
        }
    }
}

//#[swag.printbc]
#compiler(swag.CompilerMsgKindMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    @assert(msg != null)

    if(msg.type.kind == TypeInfoKind.Func)
        testInline(cast(const* TypeInfoFunc) msg.type)

    cpt += 1
}
