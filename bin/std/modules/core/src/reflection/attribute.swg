#global public
#global namespace Reflection
using Swag

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func getAttributeValue(type: const *Swag.TypeValue, attribute: typeinfo, value: string)->#null any
{
    let attrib = getAttribute(type, attribute)
    if !attrib do
        return null
    return getAttributeValue(attrib, value)
}

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func(T) getAttributeValue(type: const *Swag.TypeValue, attribute: typeinfo, value: string, def: T)->T
{
    let attrib = getAttribute(type, attribute)
    if !attrib do
        return def
    let attribVal = getAttributeValue(attrib, value)
    if attribVal == null do
        return def
    return cast(T) attribVal
}

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func getAttributeValue(type: typeinfo, attribute: typeinfo, value: string)->#null any
{
    let attrib = getAttribute(type, attribute)
    if !attrib do
        return null
    return getAttributeValue(attrib, value)
}

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func getAttributeValue(attribute: const *Swag.Attribute, value: string)->#null any
{
    foreach p in attribute.params where p.name == value do
        return p.value
    return null
}

// Returns the given associated attributes to 'value'
#[Overload, ConstExpr]
func getAttributes(value: const *Swag.TypeValue, attribute: typeinfo)->Array'(const *Swag.Attribute)
{
    var result: retval
    foreach it in value.attributes where attribute == it.type do
        result.add(it)
    return result
}

// Returns the given associated 'attribute' to 'value' or null
#[Overload, ConstExpr]
func getAttribute(value: const *Swag.TypeValue, attribute: typeinfo)->#null const *Swag.Attribute
{
    foreach it in value.attributes where attribute == it.type do
        return it
    return null
}

// Returns true if the given 'value' has the associated 'attribute'
#[Overload, ConstExpr]
func hasAttribute(value: const *Swag.TypeValue, attribute: typeinfo)->bool
{
    return getAttribute(value, attribute) != null
}

// Returns the given associated attributes to 'type'
#[Overload, ConstExpr]
func getAttributes(type: typeinfo, attribute: typeinfo)->Array'(const *Swag.Attribute)
{
    var result: retval
    switch type.kind
    {
    case Func:
        let typeFunc = cast(const *TypeInfoFunc) type
        foreach it in typeFunc.attributes where attribute == it.type do
            result.add(it)

    case Struct:
        let typeStruct = cast(const *TypeInfoStruct) type
        foreach it in typeStruct.attributes where attribute == it.type do
            result.add(it)

    case Enum:
        let typeEnum = cast(const *TypeInfoEnum) type
        foreach it in typeEnum.attributes where attribute == it.type do
            result.add(it)
    }

    return result
}

// Returns the given associated 'attribute' to 'type' or null
#[Overload, ConstExpr]
func getAttribute(type: typeinfo, attribute: typeinfo)->#null const *Swag.Attribute
{
    switch type.kind
    {
    case Func:
        let typeFunc = cast(const *TypeInfoFunc) type
        foreach it in typeFunc.attributes where attribute == it.type do
            return it

    case Struct:
        let typeStruct = cast(const *TypeInfoStruct) type
        foreach it in typeStruct.attributes where attribute == it.type do
            return it

    case Enum:
        let typeEnum = cast(const *TypeInfoEnum) type
        foreach it in typeEnum.attributes where attribute == it.type do
            return it
    }

    return null
}

// Returns true if the given 'type' has the associated 'attribute'
#[Overload, ConstExpr]
func hasAttribute(type: typeinfo, attribute: typeinfo)->bool
{
    return getAttribute(type, attribute) != null
}
