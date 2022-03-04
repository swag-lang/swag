#global skip
using Swag

#[AttrUsage(AttributeUsage.Struct|AttributeUsage.KindMsgGen)]
attr GenStuff1(v: string);

#[GenStuff1("opEquals")]
struct A
{
}

impl A
{
    mtd toto() {}
}

#message(Swag.CompilerMsgMask.AttributeGen)
{
}