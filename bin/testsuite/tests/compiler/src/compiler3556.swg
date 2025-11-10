//#global skip
using Swag

#[AttrUsage(AttributeUsage.Struct | AttributeUsage.Gen)]
attr GenStuff()

#[GenStuff]
struct A {}

impl A
{
}

interface I
{
    func toto(me);
}

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

    itf.compileString("impl I for A {\n mtd impl toto() {\nG += 1\n}\n}")
}

//#[PrintBc]
#test
{
    var x: A
    let i  = cast(I) x
    i.toto()
    @assert(G == 1)
}
