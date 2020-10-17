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

var toTreat: [4] string
var toTreatCpt: u32

//#[swag.printbc]
#compiler(CompilerMsgMask.SemanticFunc)
{
    itf := @compiler()
    msg := itf.getMessage()
    if msg.type.kind != TypeInfoKind.Func return

    typeF := cast(const *TypeInfoFunc) msg.type
    visit i: typeF.attributes
    {
        if(i.name == "myAttr")
        {
            @assert(toTreatCpt != @countof(toTreat))
            toTreat[toTreatCpt] = typeF.name
            toTreatCpt += 1
        }
    }
}

#placeholder GG

#compiler(CompilerMsgMask.PassAfterSemantic)
{
    itf := @compiler()
    itf.compileString("func GG() => 1234")

    loop toTreatCpt
    {
        //@print(toTreat[@index])
        //@print("\n")
    }
}

#run
{
    //itf := @compiler()
    //itf.compileString("#run @print(\"toto\")")
    @assert(GG() == 1234)
}