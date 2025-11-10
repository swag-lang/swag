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
    mtd toto() {}
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
impl IEvent for Evt3
{
}

func can(toType, fromType: const *Swag.TypeInfoStruct)->bool
{
    if @typecmp(toType, fromType, .Strict) do
        return true

    foreach ft in fromType.usingFields
    {
        if @typecmp(toType, cast(const *Swag.TypeInfoStruct) ft.pointedType, .Strict)
        {
            //@print(ft.offset, " ")
            return true
        }
    }

    return false
}

func toto(v: IEvent)
{
    @assert(can(Evt1, @kindof(v)) == false)
    @assert(can(Evt2, @kindof(v)) == true)
    @assert(can(Evt3, @kindof(v)) == true)
}

#test
{
    var b: Evt3{x: 10}
    toto(b)
}
