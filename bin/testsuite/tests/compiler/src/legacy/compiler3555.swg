#global testerror "Err0107"
using Swag

#[AttrUsage(AttributeUsage.Struct | AttributeUsage.Gen)]
attr GenStuff(v: string)

#[GenStuff("opEquals")]
struct A {}

impl A
{
    mtd opDrop() {}
}

var G = 0
#message(Swag.CompilerMsgMask.AttributeGen)
{
    let itf = @compiler
    var msg = itf.getMessage()
    if msg.type.kind != .Struct do
        return

    var typeS = cast(const *TypeInfoStruct) msg.type
    if @countof(typeS.attributes) != 1 do
        return
    if typeS.attributes[0].name != "__compiler3555.GenStuff" do
        return
    itf.compileString("impl A {\n mtd opDrop() {\nG += 1\n}\n}")
}

//#[PrintBc]
#test
{
    var x: A
    @assert(G == 0)
}
