using Core, Core.Serialization

#test
{
    struct Toto
    {
        toto: s32
    }

    const str = """
       a {{"toto" : 666}}
    """

    var rea: Decoder'Read.JSon
    let v    = catch rea.readAll'Toto(str)
    defer Memory.delete(v)
    @assert(@err() == Errors.SyntaxError)
}
