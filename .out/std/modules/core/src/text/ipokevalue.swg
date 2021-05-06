#global public

namespace parse
{
    public interface IPokeValue
    {
        poke: func(self, string)->string throw;
    }
}

impl parse.IPokeValue for drawing.Color
{
    func poke(using self, buf: string)->string throw
    {
        var eat: uint
        one := tokenize.eatSpaces(buf)
        if @countof(one) == 0 throw "invalid"

        if latin1.isDigit(buf[0])
        {
            (a, eat) = try parse.getU8(one)
            one = tokenize.eatCount(one, eat)

            one = tokenize.eatSpaces(one)
            (r, eat) = try parse.getU8(one)
            one = tokenize.eatCount(one, eat)

            one = tokenize.eatSpaces(one)
            (g, eat) = try parse.getU8(one)
            one = tokenize.eatCount(one, eat)

            one = tokenize.eatSpaces(one)
            (b, eat) = try parse.getU8(one)
            one = tokenize.eatCount(one, eat)
        }
        else
        {
            id := try parse.getIdentifier(one)
            one = tokenize.eatCount(one, @countof(id))
            self.argb = cast(u32) Argb.fromName(id)
        }

        return one
    }
}