typealias F = func(s32)->bool

struct A
{
    b:     *B
    f:     F
}

#[Swag.Optim("", false)]
#test
{
    var a: A
    var b: B
}

struct B
{
    a: *A
}
