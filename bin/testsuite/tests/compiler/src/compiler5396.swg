#global skip
struct B
{
    x: s32 = 6
}

struct A
{
    ptr: [1] *B
}

impl A
{
    mtd toto()->&*B
    {
        return &ptr[0]
    }
}

#[Swag.Optim("", false)]
#[Swag.PrintBc]
#test
{
    var b: B
    var a: A
    a.ptr[0] = &b

    a.toto().x = 4
}