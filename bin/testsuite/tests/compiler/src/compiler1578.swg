using Swag

attr NC(x: string, y: s32)

#[NC("myStringValue", 666)]
func toto() {}

#test
{
    let ptr = cast(const *TypeInfoFunc) #typeof(toto)
    @assert(@countof(ptr.attributes) == 1)
    @assert(@countof(ptr.attributes[0].params) == 2)
    @assert(@kindof(ptr.attributes[0].params[0].value) == #typeof(string))
    @assert(@kindof(ptr.attributes[0].params[1].value) == #typeof(s32))
    @assert(cast(string) ptr.attributes[0].params[0].value == "myStringValue")
    @assert(cast(s32) ptr.attributes[0].params[1].value == 666)
}
