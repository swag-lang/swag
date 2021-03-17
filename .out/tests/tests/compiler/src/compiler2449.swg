interface Error
{
    toString: func(self)->string;
}

struct MyError
{
    x, y, z = 3.14
}

//#[swag.printbc]
func toto()->Error
{
    err.x = 666
    return err
}

impl Error for MyError
{
    func toString(using self) => "XX"
}

var err: MyError

//#[swag.printbc]
#test
{
    v := toto()
    vv := cast(*MyError) @dataof(v)
    @assert(vv.x == 666)
    @assert(vv.z == 3.14)
    //@print(@typeof(v).name)
    //@print(@kindof(v).name)
    //@assert(v.toString() == "XX")
}