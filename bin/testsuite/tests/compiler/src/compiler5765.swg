#global testerror "San0006"

#[Swag.Safety("sanity", true)]
func setErr1(err: any)
{
    let cxt = @getcontext()
    var type = cast(const *Swag.TypeInfoStruct) @kindof(err)
    type = null
    let ptr  = cxt.errorAllocator.alloc(cast(u64) type.sizeof)
}