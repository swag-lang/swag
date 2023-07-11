/**
User attributes are declared like functions, but with the `attr` keyword before instead of `func`.
*/

using Swag
attr AttributeA()

/**
Like functions, attributes can have parameters.
*/
attr AttributeB(x, y: s32, z: string)

/**
So attributes can also have default values.
*/
attr AttributeBA(x: s32, y: string = "string")

/**
You can define a usage before the attribute definition to restrict its usage.
*/
#[AttrUsage(AttributeUsage.Function)]
attr AttributeC()

/**
To use an attribute, the syntax is `#[attribute, attribute...]`. It should be placed **before** the thing you want to tag.
*/
#[AttributeA, AttributeB(0, 0, "string")]
func function1()
{
}

/**
You can declare multiple usages.
*/
#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]
attr AttributeD(x: s32);

#[AttributeD(6)]
func function2()
{
}

#[AttributeD(150)]
struct struct1
{
}

/**
Finaly, attributes can be retrieved at runtime thanks to **type reflection**.
*/
#test
{
    let type = @typeof(function2)                  // Get the type of the function
    @assert(@countof(type.attributes) == 1)     // Check that the function has one attribute associated with it
}