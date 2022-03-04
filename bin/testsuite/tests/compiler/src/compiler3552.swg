using Swag

#[AttrUsage(AttributeUsage.Struct|AttributeUsage.KindMsgGen)]
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
    //@print(msg.type.name)

    /*typeS := cast(const *TypeInfoStruct) msg.type
    visit f: typeS.attributes
    {
        //@print(f.name)
        visit p: f.params
        {
            //@print(p.name)
            if @kindof(p.value) == string
            {
                @print(cast(string) p.value)
            }
        }
    }*/

    itf.compileString("impl A {\n mtd opDrop() {\nG += 1\n}\n}")
}

//#[PrintBc]
#test
{
    {
        var x: A
        @assert(G == 0)
    }

    @assert(G == 1)
}