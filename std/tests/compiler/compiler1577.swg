using swag

attr NC();

#[NC]
func toto()
{

}

#test
{
    ptr := cast(const *TypeInfoFunc) @typeof(&toto)   
    @assert(@countof(ptr.attributes) == 1)
    @assert(ptr.attributes[0].name == "compiler1577.NC")
}