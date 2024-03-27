//#global #[Swag.PrintBc]
//#global #[Swag.Safety("", false)]

var t: [] struct
{
    inT: const [..] rune
}
 = [{[1'rune, 2, 3, 4]}]

#test
{
    @assert(@countof(t[0].inT) == 4)
    @assert(t[0].inT[0] == 1)
    @assert(t[0].inT[1] == 2)
    @assert(t[0].inT[2] == 3)
    @assert(t[0].inT[3] == 4)
}
