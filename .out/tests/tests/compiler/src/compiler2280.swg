using Swag

struct Entity
{
    position: f32
}

struct Monster
{
    using entity: *Entity
}

//#[Swag.printbc]
#test
{
    var e: Entity
    var v: Monster
    v.entity = &e

    v.position = 666
    @assert(v.position == 666)
    @assert(v.entity.position == 666)
}