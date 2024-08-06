#global testerror "Err0678"

struct(T) AA
{
    x: T
}

impl AA
{
    #[Swag.NotGeneric]
    func create(size: T)->AA'u8
    {
        var result: retval
        result.x = size
        return result
    }
}
