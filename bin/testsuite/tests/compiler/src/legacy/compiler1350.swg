const CONST = true

interface TOTO
{
    #if CONST
    {
        func print(v: *TOTO, x: bool);
    }
}
