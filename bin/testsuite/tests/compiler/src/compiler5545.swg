#global testerror "Err0676"

#test
(
    struct Vector{a, b: s32;}
    var a = Vector{1, 1}
)

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