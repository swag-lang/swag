struct A
{
    x:   f32
}

struct B
{
    a:   A //= {1}
}

struct C
{
    b:   B = {{1}}
}

#test
{
    var v: C
    @assert(v.b.a.x == 1.0)
    //var v: B
    //@print(v.a.x)
}
