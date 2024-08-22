/**
# Tuples in Swag

A tuple in Swag is an anonymous structure, also known as a struct literal. The syntax for creating a tuple is simply `{}`, enclosing the elements you want to include. Tuples are useful for grouping different types of data together without the need to formally define a structure.
*/
#test
{
    let tuple1 = {2, 2}                    // A tuple with two integer elements
    let tuple2 = {"string", 2, true}       // A tuple with a string, an integer, and a boolean
}

/**
# Accessing Tuple Values

By default, tuple values are accessed using automatically assigned field names in the form of `itemX`, where `X` is the index of the field starting from 0.
*/
#test
{
    let tuple = {"string", 2, true}        // A tuple with three elements
    @assert(tuple.item0 == "string")       // Access the first element
    @assert(tuple.item1 == 2)              // Access the second element
    @assert(tuple.item2 == true)           // Access the third element
}

/**
# Named Fields in Tuples

You can also specify your own names for the tuple fields, which allows for more descriptive access to the tuple's elements. When you define field names, the default `itemX` names are still available.
*/
#test
{
    let tuple = {x: 1.0, y: 2.0}           // A tuple with named fields 'x' and 'y'
    @assert(tuple.x == 1.0)                // Access using the custom field name 'x'
    @assert(tuple.item0 == 1.0)            // Access using the default 'item0'
    @assert(tuple.y == 2.0)                // Access using the custom field name 'y'
    @assert(tuple.item1 == 2.0)            // Access using the default 'item1'
}

/**
# Automatic Naming of Tuple Fields

When creating a tuple literal with variables, the tuple fields will automatically take the names of the variables, unless you specify different names explicitly.
*/
#test
{
    let x = 555
    let y = 666
    let t = {x, y}                         // Tuple fields automatically named 'x' and 'y'
    @assert(t.x == 555)                    // Access using the variable name 'x'
    @assert(t.item0 == t.x)                // 'item0' corresponds to 'x'
    @assert(t.y == 666)                    // Access using the variable name 'y'
    @assert(t.item1 == t.y)                // 'item1' corresponds to 'y'
}

/**
# Tuple Assignment and Compatibility

Even if two tuples do not have the same field names, they can still be assigned to each other as long as the field types are compatible. This flexibility allows for easy data transfer between different tuple structures.
*/
#test
{
    var x: { a: s32, b: s32 }              // A tuple with fields 'a' and 'b'
    var y: { c: s32, d: s32 }              // Another tuple with fields 'c' and 'd'

    y = {1, 2}                             // Initialize 'y' with values
    x = y                                  // Assign 'y' to 'x', field names do not need to match
    @assert(x.a == 1)                      // 'x.a' takes the value of 'y.c'
    @assert(x.b == 2)                      // 'x.b' takes the value of 'y.d'

    // Even though 'x' and 'y' have different field names, their types are not considered identical.
    #assert @typeof(x) != @typeof(y)
}

/**
# Tuple Unpacking

Tuples can be unpacked, meaning their fields can be extracted into individual variables. This feature is particularly useful for splitting a tuple's data into separate variables for further processing.
*/
#test
{
    var tuple1 = {x: 1.0, y: 2.0}          // A tuple with fields 'x' and 'y'

    // Unpack the tuple: 'value0' gets the value of 'x', and 'value1' gets the value of 'y'
    let (value0, value1) = tuple1
    @assert(value0 == 1.0)
    @assert(value1 == 2.0)

    var tuple2 = {"name", true}            // A tuple with a string and a boolean
    let (name, value) = tuple2             // Unpack into 'name' and 'value'
    @assert(name == "name")
    @assert(value == true)
}

/**
# Ignoring Tuple Fields During Unpacking

You can ignore certain fields when unpacking a tuple by using the `?` placeholder. This is useful when you only need specific elements of a tuple and want to disregard the rest.
*/
#test
{
    var tuple1 = {x: 1.0, y: 2.0}          // A tuple with fields 'x' and 'y'
    let (x, ?) = tuple1                    // Unpack 'x' and ignore the second field
    @assert(x == 1.0)
    let (?, y) = tuple1                    // Ignore the first field and unpack 'y'
    @assert(y == 2.0)
}
