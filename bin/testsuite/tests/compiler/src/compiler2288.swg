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

func set1(monster: Monster)
{
    set(monster)
}

//#[Swag.PrintBc]
#test
{
    var e: Entity
    var v: Monster
    v.entity = &e

    using v
    using entity
    position[0] = 666
    @assert(e.position[0] == 666)
}