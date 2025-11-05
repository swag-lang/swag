struct S_AA3273 {}

enum E_AA3273
{
    Z
}

var G: s32

#message(Swag.CompilerMsgMask.SemTypes)
{
    let itf = @compiler
    let msg = itf.getMessage()
    @assert(msg.type != null)
    if msg.type.name == "S_AA3273" do
        G += 1
    if msg.type.name == "E_AA3273" do
        G += 1
}

#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    @assert(G == 2)
}
