using Swag

struct Entity
{
    position: [3] f32
}

struct Monster
{
    gg: bool
    using entity: *Entity
}

func set(entity: Entity)
{
    @assert(entity.position[0] == 666)
}

//#[Swag.printbc]
#test
{
    var e: Entity
    var v: Monster
    v.entity = &e

    e.position[0] = 666
    set(v)
}