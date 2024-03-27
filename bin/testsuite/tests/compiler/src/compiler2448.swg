interface Error
{
    func toString(self)->string;
}

struct MyError
{
}

//#[Swag.PrintBc]
func toto()->Error
{
    return err
}

impl Error for MyError
{
    func impl toString(using self) => "XX"
}

var err: MyError

//#[Swag.PrintBc]
#test
{
    let v = toto()
    @assert(v.toString() == "XX")
}
