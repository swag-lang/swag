using Swag, Core, Core.StrConv

#[AttrUsage(AttributeUsage.Struct | AttributeUsage.Gen)]
attr GenOpEquals()

#[GenOpEquals]
struct A
{
    a:     String
    b:     f32
}

#message(Swag.CompilerMsgMask.AttributeGen)
{
    let itf = @compiler
    let msg = itf.getMessage()

    if msg.type.kind != .Struct do
        return
    if !Reflection.hasAttribute(msg.type, GenOpEquals) do
        return

    let types = cast(const *TypeInfoStruct) msg.type

    var sb: StringBuilder
    sb.appendFormat("impl % {\n", msg.type.name)
    sb.appendFormat("mtd opEquals(other: %)->bool {\n", msg.type.name)
    sb.appendString("\treturn ")

    foreach f in types.fields
    {
        if @index do
            sb.appendString(" and ")
        sb.appendFormat("me.% == other.%", f.name, f.name)
    }

    sb.appendString("\n}\n")
    sb.appendString("}\n")

    itf.compileString(sb.toString())
    //@print("EXIT\n")
}

//#[PrintBc]
#test
{
    var x, y: A
    x.a = "toto"
    x.b = 1
    y.a = "toto"
    y.b = 1
    @assert(x == y)
}

#test
{
    var x, y: A
    x.a = "toto"
    x.b = 1
    y.a = "tata"
    y.b = 1
    @assert(x != y)
}
