alias F = func(s32)->bool

struct A
{
    b: *B
    f: F
}

#[Swag.Optimize("", false)]
#test
{
    var a: A
    var b: B
    //@print("toto")
}

struct B
{
    a: *A
}
