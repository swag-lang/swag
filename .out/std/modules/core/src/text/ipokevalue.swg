#global public

namespace Parse
{
    public interface IPokeValue
    {
        poke: func(self, string)->string throw;
    }
}

impl Parse.IPokeValue for Drawing.Color
{
    func poke(using self, buf: string)->string throw
    {
        var eat: uint
        one := tokenize.eatSpaces(buf)
        if @countof(one) == 0 throw "invalid"

        if Latin1.isDigit(buf[0])
        {
            (a, eat) = try Parse.getU8(one)
            one = tokenize.eatCount(one, eat)

            one = tokenize.eatSpaces(one)
            (r, eat) = try Parse.getU8(one)
            one = tokenize.eatCount(one, eat)

            one = tokenize.eatSpaces(one)
            (g, eat) = try Parse.getU8(one)
            one = tokenize.eatCount(one, eat)

            one = tokenize.eatSpaces(one)
            (b, eat) = try Parse.getU8(one)
            one = tokenize.eatCount(one, eat)
        }
        else
        {
            id := try Parse.getIdentifier(one)
            one = tokenize.eatCount(one, @countof(id))
            self.argb = cast(u32) Argb.fromName(id)
        }

        return one
    }
}