#test
{
    // A tuple is an anonymous structure, aka a struct literal. Syntax is `{}`.
    {
        tuple1 := {2, 2}
        tuple2 := {"string", 2, true}
    }

    // Tuple values have default names to access them, in the form of `itemX` where `X` is the field rank.
    {
        tuple := {"string", 2, true}
        @assert(tuple.item0 == "string")
        @assert(tuple.item1 == 2)
        @assert(tuple.item2 == true)
    }

    // But you can specify your own names.
    {
        tuple := {x: 1.0, y: 2.0}
        @assert(tuple.x == 1.0)
        @assert(tuple.item0 == 1.0)
        @assert(tuple.y == 2.0)
        @assert(tuple.item1 == 2.0)
    }

    // A tuple is an expression, where names of fields can be omitted. But the automatic name `itemX` is always valid.
    {
        var tuple: {x: f32, f32}
        tuple.x = 1.0
        @assert(tuple.x == 1.0)
        @assert(tuple.item0 == 1.0)
        @assert(tuple.item1 == 0.0)
    }

    // When creating a tuple literal with variables, the tuple fields will take the name of the variables (except if specified).
    {
        x := 555
        y := 666
        t := {x, y}
        @assert(t.x == 555)
        @assert(t.item0 == t.x)
        @assert(t.y == 666)
        @assert(t.item1 == t.y)
    }

    // Even if two tuples do not have the same field names, they can be assigned to each other if the field types are the same.
    {
        var x: {a: s32, b: s32}
        var y: {c: s32, d: s32}

        y = {1, 2}
        x = y
        @assert(x.a == 1)
        @assert(x.b == 2)

        // But note that 'x' and 'y' to not have the same type
        #assert @typeof(x) != @typeof(y)
    }
}

/**
# Tuple unpacking
*/
#test
{
    // You can unpack a tuple field by field.
    {
        tuple1 := {x: 1.0, y: 2.0}
        var (value0, value1) = tuple1
        @assert(value0 == 1.0)
        @assert(value1 == 2.0)

        tuple2 := {"name", true}
        (name, value) := tuple2
        @assert(name == "name")
        @assert(value == true)
    }

    // You can ignore a tuple field by naming the variable `?`.
    {
        tuple1 := {x: 1.0, y: 2.0}
        (x, ?) := tuple1
        @assert(x == 1.0)
        (?, y) := tuple1
        @assert(y == 2.0)
    }
}
