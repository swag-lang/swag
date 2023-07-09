#[Swag.Inline]
func HIWORD(l: u64) => cast(s32) cast,bit(s16) ((l >> 16) & 0xffff)

#[Swag.Optim("", false)]
#test
{
    var x = HIWORD(0xFFEC002B)
    @assert(x == -20)
}