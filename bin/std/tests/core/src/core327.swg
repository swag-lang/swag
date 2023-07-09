using Swag, Core, StrConv

#[AttrUsage(AttributeUsage.Struct|AttributeUsage.Gen)]
attr GenOpEquals();

#[GenOpEquals]
struct A
{
    a: String
    b: f32
}

#message(Swag.CompilerMsgMask.AttributeGen)
{
    var itf = @compiler()
    var msg = itf.getMessage()

    if msg.type.kind != .Struct
        return
    if !Reflection.hasAttribute(msg.type, GenOpEquals)
        return

    var types = cast(const *TypeInfoStruct) msg.type

    var sb: StringBuilder
    sb.appendFormat("impl % {\n", msg.type.name)
    sb.appendString("mtd opEquals(other: Self)->bool {\n")
    sb.appendString("\treturn ")

    visit f: types.fields
    {
        if @index
            sb.appendString(" and ")
        sb.appendFormat("% == other.%", f.name, f.name)
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