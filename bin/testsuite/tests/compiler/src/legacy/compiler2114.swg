using Swag

#[AttrUsage(AttributeUsage.Function)]
public attr myAttr()

#[myAttr]
func toto() {}

#[myAttr]
func titi() {}

var toTreat:    [4] string
var toTreatCpt: u32

//#[Swag.PrintBc]
#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    let msg = itf.getMessage()
    if msg.type.kind != TypeInfoKind.Func do
        return

    let typeF = cast(const *TypeInfoFunc) msg.type
    foreach i in typeF.attributes
    {
        if i.type == #typeof(myAttr)
        {
            @assert(toTreatCpt != @countof(toTreat))
            toTreat[toTreatCpt] = typeF.fullname
            toTreatCpt += 1
        }
    }
}

#message(CompilerMsgMask.PassAfterSemantic)
{
    let itf = @compiler
    itf.compileString("func GG() => 1234")

    for toTreatCpt
    {
        //@print(toTreat[@index])
        //@print("\n")
    }
}

#run
{
    //let itf = @compiler
    //itf.compileString("#run @print(\"toto\")")
    //@assert(GG() == 1234)
}
