/**
# 'compileString()' in '@compiler'

The 'compileString()' function, available through the '@compiler' interface,
allows you to inject and compile dynamically generated Swag source code during
compilation. It is typically used inside a '#message' block to generate code
after specific compiler events.
*/

/**
# Example — OpenGL Function Registration

In this example (from 'Std.Ogl'), 'compileString()' is used within a '#message'
hook to find all functions tagged with a custom attribute 'Ogl.Extension'
and automatically generate extension initialization code.
*/

/**
First, define a user attribute for marking OpenGL extension functions.

```swag
#[AttrUsage(AttributeUsage.Function)]
attr Extension()
````

*/

/**
Then, apply it to placeholder OpenGL functions.

```swag
#[Extension, Swag.PlaceHolder]
{
    func glUniformMatrix2x3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat)
    func glUniformMatrix2x4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat)
    func glUniformMatrix3x2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat)
    func glUniformMatrix3x4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat)
    func glUniformMatrix4x2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat)
    func glUniformMatrix4x3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat)
}
```

*/

/**
Track all functions with that attribute.

```swag
struct OneFunc
{
    type: typeinfo
    name: string
}

#[Compiler]
var g_Functions: Array'OneFunc
```

*/

/**
Register all tagged functions as they are typed.

```swag
#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    var msg = itf.getMessage()

    if !Reflection.hasAttribute(msg.type, Extension)
        return

    g_Functions.add({msg.type, msg.name})
}
```

*/

/**
Once semantic analysis finishes, generate code for all registered functions.

```swag
#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    builderInit.appendString("public func glInitExtensions()\n{\n")

    foreach e in g_Functions
    {
        let typeFunc = cast(const *TypeInfoFunc) e.type

        builderVars.appendFormat("var ext_%: %\n", e.name, typeFunc.name)
        builderVars.appendFormat("public func %(", e.name)

        foreach p, i in typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%: %", i, p.pointedType.name)
        }

        if typeFunc.returnType == void
            builderVars.appendFormat(")\n{\n")
        else
            builderVars.appendFormat(")->%\n{\n", typeFunc.returnType.name)

        builderVars.appendFormat("\treturn ext_%(", e.name)

        foreach p, i in typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString(");\n}\n\n")

        builderInit.appendFormat(
            "\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n",
            e.name, typeFunc.name, e.name)
    }

    let itf = @compiler
    var str = builderVars.toString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n")
    str = builderInit.toString()
    itf.compileString(str.toString())
}
```

*/
