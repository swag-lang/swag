using Swag

struct Entity
{
    position: [3] f32
}

struct Phys
{
    speed: [3] f32
}

struct Monster
{
    using entity:   *Entity
    using phys:     Phys
}

func set(phys: Phys)
{
    using phys
    @assert(speed[0] == 666)
}

//#[Swag.PrintBc]
#test
{
    var e: Entity
    var v: Monster
    v.entity = &e

    v.position[1] = 666
    v.speed[0] = 666
    set(&v)
}