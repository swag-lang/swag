#global skip
*namespace A
{
    typealias T = s32
}

#[Swag.Using(A)]
struct B
{
    //t: T
    x: {a: T}
}