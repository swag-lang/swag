using Swag

#[AttrMulti]
attr NC()

#[NC, NC]
func toto() {}

#test
{
    let ptr = cast(const *TypeInfoFunc) #typeof(toto)
    @assert(@countof(ptr.attributes) == 2)
    @assert(ptr.attributes[0].type == #typeof(NC))
    @assert(ptr.attributes[1].type == #typeof(NC))
}
