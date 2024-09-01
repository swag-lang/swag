/**
# User Attributes in Swag

Attributes in Swag serve as a powerful mechanism for annotating various elements of the code, such as functions and structs, with metadata. These annotations, defined using the `attr` keyword, can be leveraged for a variety of purposes, including code generation, documentation enhancement, and runtime reflection. By attaching attributes, developers can enrich their code with additional information that can be accessed both at compile-time and runtime.
*/

using Swag
attr AttributeA()   // A simple attribute without parameters

/**
# Attributes with Parameters

Attributes in Swag can accept parameters in a manner similar to functions. These parameters enable the customization of the attribute's behavior or configuration when it is applied to a code element.
*/
attr AttributeB(x, y: s32, z: string)   // Attribute with multiple parameters

/**
# Attributes with Default Values

Swag attributes support parameters with default values, providing flexibility when applying attributes. This means that when such attributes are used, some or all of the parameters can be omitted, and the default values will be applied automatically.
*/
attr AttributeBA(x: s32, y: string = "string")  // Attribute with a default parameter

/**
# Restricting Attribute Usage

Swag allows developers to control where an attribute can be applied through the `AttrUsage` specifier. By restricting an attribute's usage to specific elements (such as functions or structs), you ensure that the attribute is only used in appropriate contexts, thereby enhancing code safety and clarity.
*/
#[AttrUsage(AttributeUsage.Function)]
attr AttributeC()   // Attribute restricted to function usage

/**
# Applying Attributes

To apply attributes in Swag, use the syntax `#[attribute, attribute...]` immediately before the element you wish to annotate. This syntax supports the application of multiple attributes to a single element by listing them sequentially, separated by commas.
*/
#[AttributeA, AttributeB(0, 0, "string")]
func function1() {}   // Function annotated with multiple attributes

/**
# Multiple Usages

An attribute in Swag can be designed to be applicable to multiple types of code elements by specifying a combination of `AttrUsage` values using a bitwise OR operation. This capability allows a single attribute to be reused across different contexts, such as both functions and structs.
*/
#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]
attr AttributeD(x: s32)  // Attribute applicable to both functions and structs

#[AttributeD(6)]
func function2() {}      // Function annotated with a multi-usage attribute

#[AttributeD(150)]
struct struct1 {}        // Struct annotated with the same attribute

/**
# Retrieving Attributes at Runtime

Swag supports the retrieval and inspection of attributes at runtime through type reflection. This feature enables dynamic interaction with the metadata associated with various code elements, allowing developers to adapt behavior based on the presence or configuration of attributes.
*/
#test
{
    let type = #typeof(function2)  // Retrieve the type of the function
    @assert(@countof(type.attributes) == 1)  // Assert that the function has exactly one attribute
}
