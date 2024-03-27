using Swag

struct Entity
{
    position: [3] f32
}

struct Monster
{
    using entity: *Entity
}

func set(entity: *Entity)
{
    entity.position[0] = 666
}

//#[Swag.PrintBc]
#test
{
    var e: Entity
    var v: Monster
    v.entity = &e

    set(&v)
    @assert(e.position[0] == 666)
    @assert(v.position[0] == 666)
}