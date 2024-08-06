#global testerror "Err0592"

private struct Bot
{
    val: [2] s32 = { Swag.S32.Max }
}

/*#test
{
    a = Vector{1, 1}
    var b = Vector{2, 5}
    var c = a - b
    @assert(c.x == -1)
    @assert(c.y == -4)
}*/

/*#test
(
    var a = Vector{1, 1}
)*/

/*
#test
{
    struct Vector{a, b: s32;}
    var a = Vector{1, 1}
    var b = Vector{2, 5}
    var c = a -- b
    @assert(c.x == -1)
    @assert(c.y == -4)
}*/