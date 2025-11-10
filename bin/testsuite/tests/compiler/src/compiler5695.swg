struct Evt
{
    x:   s32
}

struct Evt1
{
    using base: Evt
}

struct Evt2
{
    using base: Evt
}

struct Evt3
{
    using base: Evt2
}

interface IEvent
{
}
impl IEvent for Evt
{
}
impl IEvent for Evt1
{
}
impl IEvent for Evt2
{
}

func can(toType, fromType: const *Swag.TypeInfoStruct)->bool
{
    if @typecmp(toType, fromType, .Strict) do
        return true

    foreach ft in toType.fields
    {
        if ft.flags & .HasUsing and can(cast(const *Swag.TypeInfoStruct) ft.pointedType, fromType) do
            return true
    }

    return false
}

func toto(v: IEvent)
{
    @assert(can(Evt1, @kindof(v)) == true)
    @assert(can(Evt2, @kindof(v)) == false)
}

#test
{
    var a: Evt
    var b: Evt1{x: 10}
    toto(b)
}
