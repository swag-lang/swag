//#global skip
//#global testerror "San0002"
using Swag

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
func free(buffer: ^void, size: u64, allocator: IAllocator = null, hint: string = null, loc = #callerlocation)
{
    var req: AllocatorRequest
    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)
}