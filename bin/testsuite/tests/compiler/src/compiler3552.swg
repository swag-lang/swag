using Swag

#[AttrUsage(AttributeUsage.Struct | AttributeUsage.Gen)]
attr GenStuff(v: string)

#[GenStuff("opEquals")]
struct A {}

var G = 0
#message(Swag.CompilerMsgMask.AttributeGen)
{
    let itf = @compiler
    let msg = itf.getMessage()
    if msg.type.kind != .Struct do
        return

    let typeS = cast(const *TypeInfoStruct) msg.type
    if @countof(typeS.attributes) != 1 do
        return
    if typeS.attributes[0].type != #typeof(GenStuff) do
        return
    itf.compileString("impl A {\n mtd opDrop() {\nG += 1\n}\n}")
}

//#[PrintBc]
#test
{
    {
        var x: A
        @assert(G == 0)
    }

    let t = #typeof(A)
    @assert(t.opDrop != null)
    @assert(G == 1)
}
