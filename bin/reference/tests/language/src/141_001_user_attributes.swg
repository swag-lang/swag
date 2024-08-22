/**
# User Attributes in Swag

Attributes in Swag are similar to function declarations but are defined using the `attr` keyword. They allow you to annotate functions, structs, and other elements with metadata that can be used for various purposes, such as code generation, documentation, or runtime reflection.

# Basic Attribute Declaration

To declare an attribute, you use the `attr` keyword followed by the attribute name and its parameters.
*/
using Swag
attr AttributeA()

/**
# Attributes with Parameters

Attributes can have parameters, just like functions. These parameters can be used to pass specific values or configuration data to the attribute.
*/
attr AttributeB(x, y: s32, z: string)

/**
# Attributes with Default Values

Attributes can also have default values for their parameters, making them optional when the attribute is applied.
*/
attr AttributeBA(x: s32, y: string = "string")

/**
# Restricting Attribute Usage

You can restrict the usage of an attribute to specific elements (e.g., functions, structs) by specifying an `AttrUsage`. This helps to ensure that attributes are only applied to relevant elements.
*/
#[AttrUsage(AttributeUsage.Function)]
attr AttributeC()

/**
# Applying Attributes

To apply an attribute, use the syntax `#[attribute, attribute...]` before the element you want to annotate. Multiple attributes can be applied to the same element by separating them with commas.
*/
#[AttributeA, AttributeB(0, 0, "string")]
func function1() {}

/**
# Multiple Usages

An attribute can be applied to multiple types of elements by specifying a bitwise combination of `AttrUsage` values.
*/
#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]
attr AttributeD(x: s32)

#[AttributeD(6)]
func function2() {}

#[AttributeD(150)]
struct struct1 {}

/**
# Retrieving Attributes at Runtime

Attributes can be accessed at runtime using **type reflection**. This allows you to inspect the metadata associated with functions, structs, or other elements and take action based on the presence or values of attributes.
*/
#test
{
    let type = @typeof(function2) // Get the type of the function
    @assert(@countof(type.attributes) == 1) // Check that the function has one attribute associated with it
}
