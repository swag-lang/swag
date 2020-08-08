using swag

var cpt = 0's64

func testInline(type: const* swag.TypeInfoFunc)
{
    visit i: type.attributes
    {
        if(i.name == "swag.inline")
        {
            @print(type.name)
            @print("\n")
        }
    }
}

#compiler(swag.CompilerMsgKindMask.SemanticFunc)
{
    msg := cast(const *swag.CompilerMessage) @msg()
    if(msg.type.kind == swag.TypeInfoKind.FuncAttr)
        testInline(cast(const* swag.TypeInfoFunc) msg.type)

    cpt += 1
}
