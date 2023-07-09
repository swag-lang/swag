var   G1_3274 = 0
const G2_3274 = 666

var G: s32

#message(Swag.CompilerMsgMask.SemGlobals)
{
    var itf = @compiler()
    var msg = itf.getMessage()
    @assert(msg.type != null)
    if msg.name == "G1_3274" G += 1
    if msg.name == "G2_3274" G += 1
    //@print(msg.name, "\n")
}

#message(Swag.CompilerMsgMask.PassAfterSemantic)
{
    @assert(G == 2)
}