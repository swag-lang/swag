//#[Swag.PrintBc]
#[Swag.CanOverflow(true)]
func transform(buf: const [*] u8)
{
    var m: [64] u32

    var i = 0'u32
    while i < 16 do
        i += 1

    while i < 64
    {
        m[i] = m[i - 2]
        i += 1
    }
}
