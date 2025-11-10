interface Error
{
    func toString(me)->string;
}

struct MyError { x, y, z = 3.14 }

//#[Swag.PrintBc]
func toto()->Error
{
    err.x = 666
    return err
}

impl Error for MyError
{
    func impl toString(me) => "XX"
}

var err: MyError

//#[Swag.PrintBc]
#test
{
    let v  = toto()
    let vv = cast(*MyError) @dataof(v)
    @assert(vv.x == 666)
    @assert(vv.z == 3.14)
    //@print(#typeof(v).name)
    //@print(@kindof(v).name)
    //@assert(v.toString() == "XX")
}
