#global export
#global namespace Slice

// Transform the content of a slice with a given lambda
#[Swag.Inline]
func(T) modifyInPlace(values: [..] T, cb: func(*T))
{
    loop i: values
        cb(&values[i])
}

// Reverse the content of a slice
func(T) reverse(values: [..] T)
{
    var i = 0'u64
    var j = @countof(values) - 1
    var d = @dataof(values)
    while i < j
    {
        swap(d + i, d + j)
        i += 1
        j -= 1
    }
}