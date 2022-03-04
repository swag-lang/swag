using Swag

#[AttrUsage(AttributeUsage.Struct|AttributeUsage.Gen)]
attr GenStuff(v: string);

#[GenStuff("opEquals")]
struct A
{
}

var G = 0
#message(Swag.CompilerMsgMask.AttributeGen)
{
    itf := @compiler()
    msg := itf.getMessage()
    if msg.type.kind != .Struct return

    typeS := cast(const *TypeInfoStruct) msg.type
    if @countof(typeS.attributes) != 1 return
    if typeS.attributes[0].name != "__compiler3552.GenStuff" return
    itf.compileString("impl A {\n mtd opDrop() {\nG += 1\n}\n}")
}

//#[PrintBc]
#test
{
    {
        var x: A
        @assert(G == 0)
    }

    t := @typeof(A)
    @assert(t.opDrop != null)
    @assert(G == 1)
}