using swag

var cpt = 0's64

func testInline(type: const* TypeInfoFunc)
{
    visit i: type.attributes
    {
        if(i.name == "swag.inline")
        {
            cpt += 1
            @print(type.name)
            @print("\n")
        }
    }
}

#compiler(swag.CompilerMsgKindMask.SemanticFunc)
{
    msg := cast(const *CompilerMessage) @msg()
    if(msg.type.kind == TypeInfoKind.FuncAttr)
        testInline(cast(const* TypeInfoFunc) msg.type)

    cpt += 1
}
