using swag

#[attributeUsage(AttributeUsage.Function)]
public attr myAttr()

#[myAttr]
func toto()
{
}

#[myAttr]
func titi()
{
}

//#[swag.printbc]
#compiler(CompilerMsgKindMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    if msg.type.kind != TypeInfoKind.Func return

    typeF := cast(const *TypeInfoFunc) msg.type
    visit i: typeF.attributes
    {
        if(i.name == "myAttr")
        {
            @print(typeF.name)
            @print("\n")
        }
    }
}
