using swag

#[attributeMulti]
attr NC(x: string = "totoAAA");

#[NC]
func toto()
{

}

#test
{
    ptr := cast(const *TypeInfoFunc) @typeof(&toto)   
    @assert(@countof(ptr.attributes) == 1)
    @assert(@countof(ptr.attributes[0].params) == 1)
    @assert(cast(string) ptr.attributes[0].params[0].value == "totoAAA")
}