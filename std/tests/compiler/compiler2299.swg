#module "compiler2299"
#unittest error

struct Range16
{
    lo, hi, stride: u16
}

const _White_Space: [] Range16 =
@[
    {0x0009, 0x000d, 1, 1},
    {0x0020, 0x0085, 101},
    {0x00a0, 0x1680, 5600},
    {0x2000, 0x200a, 1},
    {0x2028, 0x2029, 1},
    {0x202f, 0x205f, 48},
    {0x3000, 0x3000, 1},
]

#assert @sizeof(_White_Space) == 6 * 7