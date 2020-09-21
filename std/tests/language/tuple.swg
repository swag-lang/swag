#test
{
    // A tuple is an anonymous structure
    {
        tuple1 := @{2, 2}
        tuple2 := @{"string", 2, true}
    }

    // Tuple values have default names to access them
    {
        tuple := @{"string", 2, true}
        @assert(tuple.item0 == "string")
        @assert(tuple.item1 == 2)
        @assert(tuple.item2 == true)
    }

    // You can specify your own names
    {
        tuple := @{x: 1.0, y: 2.0}
        @assert(tuple.x == 1.0)
        @assert(tuple.item0 == 1.0)
        @assert(tuple.y == 2.0)
        @assert(tuple.item1 == 2.0)
    }

    // A tuple type is of the form struct{type, type...}
    {
        var tuple: struct{x: f32, y: f32}
        tuple.x = 1.0
        tuple.y = 2.0
        @assert(tuple.x == 1.0)
        @assert(tuple.y == 2.0)
    }

    // In a type definition, 'struct' can be omitted
    {
        var tuple: {x: f32, y: f32}
        tuple.x = 1.0
        @assert(tuple.x == 1.0)
    }

    // You can dereference a tuple field by field
    {
        tuple1 := @{x: 1.0, y: 2.0}
        var (value0, value1) = tuple1
        @assert(value0 == 1.0)
        @assert(value1 == 2.0)

        tuple2 := @{"name", true}
        (name, value) := tuple2
        @assert(name == "name")
        @assert(value == true)
    }

    // A tuple can be used to return multiple values in a function
    {
        func myFunction()->{f32, f32}
        {
            return @{1.0, 2.0}
        }

        result := myFunction()
        @assert(result.item0 == 1.0)
        @assert(result.item1 == 2.0)

        let (x, y) = myFunction()
        @assert(x == 1.0)
        @assert(y == 2.0)

        (z, w) := myFunction()
        @assert(z == 1.0)
        @assert(w == 2.0)
    }

    // You can name the fields of the function result too
    {
        func myFunction()->{x: f32, y: bool}
        {
            return @{x: 1.0, y: true}
        }

        result := myFunction()
        @assert(result.x == 1.0)
        @assert(result.y == true)
    }
}
