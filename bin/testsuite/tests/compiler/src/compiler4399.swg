using Swag

// Returns true if the given slice contains the 'value'
#[Swag.ConstExpr]
func(T, V) contains(values: const [..] T, value: V)->bool
{
    foreach v in values
    {
        if v == value:
            return true
    }

    return false
}

//#[Swag.PrintBc]
//#[Swag.PrintAsm]
#test
{
    var arr = [1, 2, 4]
    let b = contains(arr[..], 1's32)
    @assert(b)
}
