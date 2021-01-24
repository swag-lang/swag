interface Error
{
    toString: func(self)->string;
}

struct MyError
{
}

//#[swag.printbc]
func toto()->Error
{
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
    @assert(v.toString() == "XX")
}