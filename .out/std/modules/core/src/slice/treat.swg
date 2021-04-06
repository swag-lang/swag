#global export
#global namespace slice

// Transform the content of a slice with a given lambda
#[swag.inline]
func(T) modifyInPlace(values: [..] T, cb: func(*T))
{
    loop i: values
        cb(&values[i])
}
