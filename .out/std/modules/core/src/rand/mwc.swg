#global namespace random

public struct Mwc
{
    x: 		u32 = 1234567
    carry: 	u32 = 1234567
}

impl Mwc
{
    const mult: u32 = 2083801278
    const mhi = (mult & 0xFFFF0000) >> 16
    const mlo = mult & 0xFFFF

    public func seed(using self, value: u32)
    {
        x = value
        carry = 1234567
    }

    public func next(using self)->u32
    {
        xhi := x >>,safe 16
        xlo := x & 0xFFFF

        chi := carry >>,safe 16
        clo := carry & 0xFFFF

        x = (xlo * mlo) + clo
        axc := (xlo * mhi) + (xhi * mlo) + chi + (x >>,safe 16)
        carry = (xhi * mhi) + (axc >>,safe 16)

        x = (x & 0xFFFF) + (axc <<,safe 16)
        return x
    }
}
