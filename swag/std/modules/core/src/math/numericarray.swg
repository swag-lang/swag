#global export
#global namespace math
using swag

struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
}