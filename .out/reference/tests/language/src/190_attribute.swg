using Swag

// Attributes are tags associated with functions, structures etc...
// They are declared like functions, with 'attr' before instead of 'func'
attr AttributeA();

// Attributes can have parameters
attr AttributeB(x, y: s32, z: string);

// Attributes can have default values
attr AttributeBA(x: s32, y: string = "string");

// You can define a usage before the attribute definition to restric its usage
#[AttrUsage(AttributeUsage.Function)]
attr AttributeC();

// To use an attribute, syntax is #[attribute, attribute...]
#[AttributeA, AttributeB(0, 0, "string")]
func function1()
{
}

// You can declare multiple usages
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

#test
{
    // Attributes can be retreived by reflection
    {
        type := @typeof(function2)
        @assert(@countof(type.attributes) == 1)
    }
}