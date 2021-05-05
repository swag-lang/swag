#global public

namespace parse
{
    public interface IPokeValue
    {
        poke: func(self, string)->string throw;
    }
}

namespace drawing
{
    impl parse.IPokeValue for Color
    {
        func poke(using self, buf: string)->string throw
        {
            var eat: uint

            one := tokenize.eatSpaces(buf)
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

            return one
        }
    }
}