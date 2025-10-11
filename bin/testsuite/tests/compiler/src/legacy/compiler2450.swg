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
    let v = toto()
    //@print(#typeof(v).name)
    //@print(@kindof(v).name)
    @assert(@kindof(v) == MyError)
    //@assert(v.toString() == "XX")
}
