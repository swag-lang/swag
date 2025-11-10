struct A {}

struct B
{
    using base: A
}

#[Swag.Overload]
func delete(x: *A) {}
#[Swag.Overload]
func delete(x: *B) {}

#test
{
    var b: B
    delete(&b)
}
