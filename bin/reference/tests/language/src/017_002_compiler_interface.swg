/**
The `compileString()` function within the `@compiler()` interface is another method to compile
generated code. This function should be invoked at compile time, typically within a `#message`
call.

Below is an example from the `Std.Ogl` module (an OpenGL wrapper), which utilizes `#message` to
identify functions annotated with a specific user attribute, `Ogl.Extension`, and subsequently
generates code for each identified function.
*/

/**
First, we define a new attribute that can be associated with functions.

```swag
#[AttrUsage(AttributeUsage.Function)]
attr Extension()
```

Example of applying the custom attribute to OpenGL functions.

```swag
#[Extension, Swag.PlaceHolder]
{
    func glUniformMatrix2x3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix2x4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix3x2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix3x4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix4x2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix4x3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
}
```
*/

/**
The following will be used to track the functions with that specific attribute.

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
This `#message` will be called for each function of the `Ogl` module.

```swag
#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler()
    var msg = itf.getMessage()

    // If the function does not have our attribute, ignore it
    if !Reflection.hasAttribute(msg.type, Extension)
        return

    // Track all functions with the specified attribute
    g_Functions.add({msg.type, msg.name})
}
```
*/

/**
We will generate a `glInitExtensions` global function, so we register it as a placeholder.

```swag
#placeholder(glInitExtensions)
```
*/

/**
This code is called once all functions of the module have been typed, and it handles the main
code generation process.

```swag
#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    // Generate the `glInitExtensions` function
    builderInit.appendString("public func glInitExtensions()\n{\n");

    // Visit all functions we have registered, i.e., all functions with the `Ogl.Extension` attribute.
    foreach e in g_Functions
    {
        let typeFunc = cast(const *TypeInfoFunc) e.type

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: %\n", e.name, typeFunc.name)

        // Make a wrapper function
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

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n",
                                 e.name, typeFunc.name, e.name);
    }

    // Compile !!
    let itf = @compiler()
    var str = builderVars.toString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.toString()
    itf.compileString(str.toString())
}
```
*/