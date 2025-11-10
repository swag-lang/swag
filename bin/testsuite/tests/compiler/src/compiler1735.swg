//#[Swag.PrintBc]
#test
{
    enum Enum { A, B, C }
    @assert(#typeof(Enum).rawType.fullname == "s32")
}
