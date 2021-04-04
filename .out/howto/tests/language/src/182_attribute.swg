using swag

// Attributes are tags associated with functions, structures etc...
// They are declared like functions, with 'attr' before instead of 'func'
attr attributeA();

// Attributes can have parameters
attr attributeB(x, y: s32, z: string);

// Attributes can have default values
attr attributeBA(x: s32, y: string = "string");

// You can define a usage before the attribute definition to restric its use
#[attributeUsage(AttributeUsage.Function)]
attr attributeC();

// To use an attribute, syntax is #[attribute, attribute...]
#[attributeA, attributeB(0, 0, "string")]
func function1()
{
}

// You can declare multiple usages
#[attributeUsage(AttributeUsage.Function | AttributeUsage.Struct)]
attr attributeD(x: s32);

#[attributeD(6)]
func function2()
{
}

#[attributeD(150)]
struct struct1
{
}

#test
{
    // Attributes can be retreived by reflection
    {
        type := @typeof(function2)
        @assert(@countof(type.attributes) == 1)
        firstOne := type.attributes[0]
        // Full scoped name. '__182_attribute' is the name of the current file private scope.
        // All test files are inside a private scope by default.
        @assert(firstOne.name == "__182_attribute.attributeD")
    }
}