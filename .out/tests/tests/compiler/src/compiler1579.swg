using Swag

#[attributeMulti]
attr NC(x: string = "totoAAA");

#[NC, NC("XXX")]
func toto()
{

}

#test
{
    ptr := cast(const *TypeInfoFunc) @typeof(&toto)
    @assert(@countof(ptr.attributes) == 2)

    @assert(@countof(ptr.attributes[0].params) == 1)
    @assert(cast(string) ptr.attributes[0].params[0].value == "totoAAA")

    @assert(@countof(ptr.attributes[1].params) == 1)
    @assert(cast(string) ptr.attributes[1].params[0].value == "XXX")
}