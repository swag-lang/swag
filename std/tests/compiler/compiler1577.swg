using swag

#[attributeMulti]
attr NC();

#[NC, NC]
func toto()
{

}

#test
{
    ptr := cast(const *TypeInfoFunc) @typeof(&toto)
    @assert(@countof(ptr.attributes) == 2)
    @assert(ptr.attributes[0].name == "__compiler1577.NC")
    @assert(ptr.attributes[1].name == "__compiler1577.NC")
}