#global public
#global namespace Reflection
using Swag

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func getAttributeValue(type: nn const *Swag.TypeValue, attribute: nn typeinfo, value: nn string)->any
{
    let attrib = getAttribute(type, attribute)
    if !attrib:
        return null
    return getAttributeValue(attrib, value)
}

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func(T) getAttributeValue(type: nn const *Swag.TypeValue, attribute: nn typeinfo, value: nn string, def: T)->T
{
    let attrib = getAttribute(type, attribute)
    if !attrib:
        return def
    let attribVal = getAttributeValue(attrib, value)
    if attribVal == null:
        return def
    return cast(T) attribVal
}

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func getAttributeValue(type: nn typeinfo, attribute: nn typeinfo, value: nn string)->any
{
    let attrib = getAttribute(type, attribute)
    if !attrib:
        return null
    return getAttributeValue(attrib, value)
}

// Returns the given attribute value, or null
#[Overload, ConstExpr]
func getAttributeValue(attribute: nn const *Swag.Attribute, value: nn string)->any
{
    foreach p in attribute.params where p.name == value:
        return p.value
    return null
}

// Returns the given associated attributes to 'value'
#[Overload, ConstExpr]
func getAttributes(value: nn const *Swag.TypeValue, attribute: nn typeinfo)->Array'(const *Swag.Attribute)
{
    var result: retval
    foreach it in value.attributes where attribute == it.type:
        result.add(it)
    return result
}

// Returns the given associated 'attribute' to 'value' or null
#[Overload, ConstExpr]
func getAttribute(value: nn const *Swag.TypeValue, attribute: nn typeinfo)->const *Swag.Attribute
{
    foreach it in value.attributes where attribute == it.type:
        return it
    return null
}

// Returns true if the given 'value' has the associated 'attribute'
#[Overload, ConstExpr]
func hasAttribute(value: nn const *Swag.TypeValue, attribute: nn typeinfo)->bool
{
    return getAttribute(value, attribute) != null
}

// Returns the given associated attributes to 'type'
#[Overload, ConstExpr]
func getAttributes(type: nn typeinfo, attribute: nn typeinfo)->Array'(const *Swag.Attribute)
{
    var result: retval
    switch type.kind
    {
    case Func:
        let typeFunc = cast(const *TypeInfoFunc) type
        foreach it in typeFunc.attributes where attribute == it.type:
            result.add(it)

    case Struct:
        let typeStruct = cast(const *TypeInfoStruct) type
        foreach it in typeStruct.attributes where attribute == it.type:
            result.add(it)

    case Enum:
        let typeEnum = cast(const *TypeInfoEnum) type
        foreach it in typeEnum.attributes where attribute == it.type:
            result.add(it)
    }

    return result
}

// Returns the given associated 'attribute' to 'type' or null
#[Overload, ConstExpr]
func getAttribute(type: nn typeinfo, attribute: nn typeinfo)->const *Swag.Attribute
{
    switch type.kind
    {
    case Func:
        let typeFunc = cast(const *TypeInfoFunc) type
        foreach it in typeFunc.attributes where attribute == it.type:
            return it

    case Struct:
        let typeStruct = cast(const *TypeInfoStruct) type
        foreach it in typeStruct.attributes where attribute == it.type:
            return it

    case Enum:
        let typeEnum = cast(const *TypeInfoEnum) type
        foreach it in typeEnum.attributes where attribute == it.type:
            return it
    }

    return null
}

// Returns true if the given 'type' has the associated 'attribute'
#[Overload, ConstExpr]
func hasAttribute(type: nn typeinfo, attribute: nn typeinfo)->bool
{
    return getAttribute(type, attribute) != null
}
