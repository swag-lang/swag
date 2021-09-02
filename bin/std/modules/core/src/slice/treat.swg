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
    i := 0'uint
    j := @countof(values) - 1
    d := @dataof(values)
    while i < j
    {
        swap(d + i, d + j)
        i += 1
        j -= 1
    }
}