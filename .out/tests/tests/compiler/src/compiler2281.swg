using Swag

struct Entity
{
    position: [3] f32
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

    v.position[0] = 666
    @assert(v.position[0] == 666)
    @assert(v.entity.position[0] == 666)
}