//#global skip
namespace A
{
    alias T = s32
}

#[Swag.Using(A)]
struct B
{
    t:   T
    //x: {a: T}
}
