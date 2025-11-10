using Swag

var cpt = 0's64

#[Compiler]
func testInline(type: const *TypeInfoFunc)
{
    foreach i in type.attributes
    {
        if i.type == Swag.Inline
        {
            cpt += 1
            //@print(type.name)
            //@print("\n")
        }
    }
}

//#[Swag.PrintBc]
#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    let msg = itf.getMessage()
    @assert(msg != null)
    if (msg.type.kind != TypeInfoKind.Func) do
        return

    testInline(cast(const *TypeInfoFunc) msg.type)
    cpt += 1
}
