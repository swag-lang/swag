
#unittest error

struct A
{
    y: s32
    x: string
}

//#[swag.printbc]
#test
{
    var x : A
    #[swag.safety(true)]
    ptr := x.x[1]
}