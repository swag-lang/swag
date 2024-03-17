#global skip
namespace A
{
    typealias T = s32
}

#[Swag.Using(A)]
struct B
{
    x: {a: T}
}

/*
#[Swag.Using(A)]
func toto()->{a: T}
{
    return {}
}
*/