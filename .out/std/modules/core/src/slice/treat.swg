#global export
#global namespace slice

// Transform the content of a slice with a given lambda
#[Swag.inline]
func(T) modifyInPlace(values: [..] T, cb: func(*T))
{
    loop i: values
        cb(&values[i])
}
