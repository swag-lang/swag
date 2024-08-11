#global testerror "Err0660"

struct(T) AA
{
    x: T
}

impl AA
{
    #[Swag.NotGeneric]
    func(T) create(size: u8)->AA'u8
    {
        var result: retval
        result.x = size
        return result
    }
}
