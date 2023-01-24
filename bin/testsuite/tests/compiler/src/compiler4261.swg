#[Swag.Inline]
func HIWORD(l: u64) => cast(s32) bitcast(s16) ((l >>,safe 16) & 0xffff)

#[Swag.Optim("", false)]
#test
{
    x := HIWORD(0xFFEC002B)
    @assert(x == -20)
}