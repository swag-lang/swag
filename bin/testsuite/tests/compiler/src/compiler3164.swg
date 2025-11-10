//#[Swag.PrintBc]
#test
{
    var az: [128] u8
    az[0] = 't''u8
    az[1] = 'o''u8
    az[2] = 't''u8
    az[3] = 'o''u8
    az[4] = 0
    @assert(cast(cstring) az == "toto")
    @assert(cast(cstring) az != "toto1")
}
