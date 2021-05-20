#[Swag.inline]
public func toto2589()
{
    x := 0
    switch x
    {
    case 0, 1:      @print("0")
    case 4, 5, 6:   @print("0")
    case 7:         @print("0")
    default:
        {
            @assert(false)
            @assert(false)
        }
    }
}