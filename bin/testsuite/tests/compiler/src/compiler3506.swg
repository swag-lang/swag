struct A
{
}

struct B
{
    using base: A
}

func delete(x: *A) {}
func delete(x: *B) {}

#test
{
    var b: B
    delete(&b)
}