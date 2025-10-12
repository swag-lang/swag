/**
# Tuples in Swag

A tuple represents an anonymous structure (a *struct literal*) that can group multiple values
of different types together without defining a named structure. Tuples are enclosed in curly braces '{}'
and can mix any combination of data types.
*/
#test
{
    let tuple1 = {2, 2}
    let tuple2 = {"string", 2, true}
}

/**
# Accessing Tuple Values

Tuple fields are automatically named 'itemX', where 'X' is the zero-based index of the field.
*/
#test
{
    let tuple = {"string", 2, true}
    @assert(tuple.item0 == "string")
    @assert(tuple.item1 == 2)
    @assert(tuple.item2 == true)
}

/**
# Named Fields in Tuples

Tuple fields can have explicit names. Named fields improve readability but can still be accessed
using their default 'itemX' identifiers.
*/
#test
{
    let tuple = {x: 1.0, y: 2.0}
    @assert(tuple.x == 1.0)
    @assert(tuple.item0 == 1.0)
    @assert(tuple.y == 2.0)
    @assert(tuple.item1 == 2.0)
}

/**
# Automatic Field Naming

When creating a tuple from variables, Swag automatically uses the variable names as field names
unless overridden.
*/
#test
{
    let x = 555
    let y = 666
    let t = {x, y}
    @assert(t.x == 555)
    @assert(t.item0 == t.x)
    @assert(t.y == 666)
    @assert(t.item1 == t.y)
}

/**
# Tuple Assignment and Compatibility

Tuples can be assigned to each other if their field types match, even if field names differ.
*/
#test
{
    var x:
    {
        a: s32, b: s32
    }
    var y:
    {
        c: s32, d: s32
    }

    y = {1, 2}
    x = y

    @assert(x.a == 1)
    @assert(x.b == 2)
    #assert(#typeof(x) != #typeof(y))
}

/**
# Tuple Unpacking

Tuples can be unpacked into separate variables, allowing easy extraction of values.
*/
#test
{
    let tuple1 = {x: 1.0, y: 2.0}
    let (value0, value1) = tuple1
    @assert(value0 == 1.0)
    @assert(value1 == 2.0)

    let tuple2 = {"name", true}
    let (name, value) = tuple2
    @assert(name == "name")
    @assert(value == true)
}

/**
# Ignoring Fields During Unpacking

Use '?' as a placeholder to ignore specific tuple fields when unpacking.
*/
#test
{
    let tuple1 = {x: 1.0, y: 2.0}
    let (x, ?) = tuple1
    @assert(x == 1.0)

    let (?, y) = tuple1
    @assert(y == 2.0)
}
