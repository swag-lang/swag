struct S_AA3273
{
}

enum E_AA3273
{
}

var G: s32

#message(Swag.CompilerMsgMask.SemTypes)
{
    itf := @compiler()
    msg := itf.getMessage()
    @assert(msg.type != null)
    if msg.type.flatName == "S_AA3273" G += 1
    if msg.type.flatName == "E_AA3273" G += 1
}

#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    @assert(G == 2)
}