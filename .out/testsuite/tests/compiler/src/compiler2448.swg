interface Error
{
    toString: func(self)->string;
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
    func toString(using self) => "XX"
}

var err: MyError

//#[Swag.PrintBc]
#test
{
    v := toto()
    @assert(v.toString() == "XX")
}