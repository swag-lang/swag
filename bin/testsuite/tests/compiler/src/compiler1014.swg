#global skip // no more relative pointers (18 july 2021)
using Swag

func toto(x: s32, y: bool)->s32 => x

//#[Swag.PrintBc]
#test
{
	ptr := cast(const *TypeInfoFunc) @typeof(toto)
	@assert(@typeof(ptr.returnType) == @typeof(const *~64 TypeInfo))
    @assert(ptr.returnType == s32)
}
