#[Swag.Inline]
func HIWORD(l: u64) => cast(s32) cast,bit(s16) ((l >>,over 16) & 0xffff)

#[Swag.Optim("", false)]
#test
{
    x := HIWORD(0xFFEC002B)
    @assert(x == -20)
}