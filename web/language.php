<?php include('html_start.php'); include('header.php'); ?>

    <style>
    <?php include('css/style.php'); ?>

    .left {
        overflow-y: scroll;
    }

    .right {
        overflow-y: scroll;
        padding-left: 10px;
        width: 1024px;
    }

    .left h1 {
        margin-bottom: 0px;
    }

    .left ul {
        padding-left: 20px;
    }

    .left a {
        color: black;
    }

    table {
        border:             1px solid LightGrey;
        border-collapse:    collapse;
        width:              80%;
    }
    td {
        padding:            6px;
        border:             1px solid LightGrey;
        border-collapse:    collapse;
        width:              30%;
    }
    blockquote {
        padding:            6px;
        margin:             10px;
    }

    @media (min-width: 1024px) {
        html, body {
            height: 100%;
        }

        pre {
            margin: 20px;
        }
    }
    </style>
<div class="lg:flex flex-1">
<div class="left h-full lg:w-[450px]" x-data="{ open_content: false }">
<div class="flex items-center justify-between border lg:border-0 rounded pt-1 px-2" @click="open_content = ! open_content">
<h1 class="leading-8 m-0 lg:my-5">Content</h1>
<div class="leading-8 block lg:hidden"><img src="imgs/chevron.png" alt="" class="w-5" :class="open_content ? 'rotate-90' : ''" /></div>
</div>
<ul class="mt-5" :class="open_content ? '' : 'hidden lg:block'">
<li><a href="#000_introduction">Introduction</a></li>
<ul>
</ul>
<li><a href="#001_hello_mad_world">Hello mad world</a></li>
<ul>
</ul>
<li><a href="#002_source_code_organization">Source code organization</a></li>
<ul>
</ul>
<li><a href="#003_comments">Comments</a></li>
<ul>
</ul>
<li><a href="#004_identifiers">Identifiers</a></li>
<ul>
</ul>
<li><a href="#005_keywords">Keywords</a></li>
<ul>
<li><a href="#Basic types">Basic types</a></li>
<li><a href="#Language keywords">Language keywords</a></li>
<li><a href="#Compiler keywords">Compiler keywords</a></li>
<li><a href="#Intrinsics libc">Intrinsics libc</a></li>
<li><a href="#Other intrinsics">Other intrinsics</a></li>
<li><a href="#Modifiers">Modifiers</a></li>
</ul>
<li><a href="#006_semicolon">Semicolon</a></li>
<ul>
</ul>
<li><a href="#007_global_declaration_order">Global declaration order</a></li>
<ul>
</ul>
<li><a href="#010_basic_types">Basic types</a></li>
<ul>
<li><a href="#Type reflection">Type reflection</a></li>
</ul>
<li><a href="#011_number_literals">Number literals</a></li>
<ul>
<li><a href="#Postfix">Postfix</a></li>
</ul>
<li><a href="#012_string">String</a></li>
<ul>
<li><a href="#Escape sequence">Escape sequence</a></li>
<li><a href="#Raw string">Raw string</a></li>
<li><a href="#Multiline string">Multiline string</a></li>
<li><a href="#Character">Character</a></li>
<li><a href="#@stringof and @nameof">@stringof and @nameof</a></li>
</ul>
<li><a href="#013_variables">Variables</a></li>
<ul>
</ul>
<li><a href="#014_const">Const</a></li>
<ul>
</ul>
<li><a href="#015_operators">Operators</a></li>
<ul>
<li><a href="#Arithmetic operators">Arithmetic operators</a></li>
<li><a href="#Bitwise operators">Bitwise operators</a></li>
<li><a href="#Assignment operators">Assignment operators</a></li>
<li><a href="#Unary operators">Unary operators</a></li>
<li><a href="#Comparison operators">Comparison operators</a></li>
<li><a href="#Logical operators">Logical operators</a></li>
<li><a href="#Ternary operator">Ternary operator</a></li>
<li><a href="#Spaceshift operator">Spaceshift operator</a></li>
<li><a href="#Null-coalescing operator">Null-coalescing operator</a></li>
<li><a href="#Type promotion">Type promotion</a></li>
<li><a href="#Operator precedence">Operator precedence</a></li>
</ul>
<li><a href="#016_cast">Cast</a></li>
<ul>
<li><a href="#Explicit cast">Explicit cast</a></li>
<li><a href="#acast">acast</a></li>
<li><a href="#bitcast">bitcast</a></li>
<li><a href="#Implicit casts">Implicit casts</a></li>
</ul>
<li><a href="#020_array">Array</a></li>
<ul>
</ul>
<li><a href="#021_slice">Slice</a></li>
<ul>
<li><a href="#The slicing operator">The slicing operator</a></li>
</ul>
<li><a href="#022_pointers">Pointers</a></li>
<ul>
<li><a href="#Single value pointers">Single value pointers</a></li>
<li><a href="#Multiple values pointers">Multiple values pointers</a></li>
</ul>
<li><a href="#023_references">References</a></li>
<ul>
</ul>
<li><a href="#024_any">Any</a></li>
<ul>
</ul>
<li><a href="#025_tuple">Tuple</a></li>
<ul>
<li><a href="#Tuple unpacking">Tuple unpacking</a></li>
</ul>
<li><a href="#030_enum">Enum</a></li>
<ul>
<li><a href="#Enum as flags">Enum as flags</a></li>
<li><a href="#Enum of arrays">Enum of arrays</a></li>
<li><a href="#Enum of slices">Enum of slices</a></li>
<li><a href="#Enum type inference">Enum type inference</a></li>
</ul>
<li><a href="#031_impl">Impl</a></li>
<ul>
</ul>
<li><a href="#035_namespace">Namespace</a></li>
<ul>
</ul>
<li><a href="#050_if">If</a></li>
<ul>
</ul>
<li><a href="#051_loop">Loop</a></li>
<ul>
<li><a href="#break, continue">break, continue</a></li>
<li><a href="#Ranges">Ranges</a></li>
<li><a href="#Infinite loop">Infinite loop</a></li>
</ul>
<li><a href="#052_visit">Visit</a></li>
<ul>
</ul>
<li><a href="#053_for">For</a></li>
<ul>
</ul>
<li><a href="#054_while">While</a></li>
<ul>
</ul>
<li><a href="#055_switch">Switch</a></li>
<ul>
</ul>
<li><a href="#056_break">Break</a></li>
<ul>
</ul>
<li><a href="#060_struct">Struct</a></li>
<ul>
</ul>
<ul>
<li><a href="#061__declaration">Declaration</a></li>
<ul>
</ul>
<li><a href="#062__impl">Impl</a></li>
<ul>
</ul>
<li><a href="#063__special_functions">Special functions</a></li>
<ul>
</ul>
<li><a href="#064__affectation">Affectation</a></li>
<ul>
</ul>
<li><a href="#064__count">Count</a></li>
<ul>
</ul>
<li><a href="#064__post_copy_and_post_move">Post copy and post move</a></li>
<ul>
<li><a href="#moveref">moveref</a></li>
</ul>
<li><a href="#064__visit">Visit</a></li>
<ul>
</ul>
<li><a href="#067__offset">Offset</a></li>
<ul>
</ul>
<li><a href="#068__packing">Packing</a></li>
<ul>
</ul>
</ul>
<li><a href="#070_union">Union</a></li>
<ul>
</ul>
<li><a href="#075_interface">Interface</a></li>
<ul>
</ul>
<li><a href="#100_function">Function</a></li>
<ul>
</ul>
<ul>
<li><a href="#101__declaration">Declaration</a></li>
<ul>
<li><a href="#Multiple return values">Multiple return values</a></li>
</ul>
<li><a href="#102__lambda">Lambda</a></li>
<ul>
<li><a href="#Anonymous functions">Anonymous functions</a></li>
</ul>
<li><a href="#103__closure">Closure</a></li>
<ul>
</ul>
<li><a href="#104__mixin">Mixin</a></li>
<ul>
</ul>
<li><a href="#105__macro">Macro</a></li>
<ul>
</ul>
<li><a href="#105__variadic_parameters">Variadic parameters</a></li>
<ul>
</ul>
<li><a href="#106__ufcs">Ufcs</a></li>
<ul>
</ul>
<li><a href="#107__constexpr">Constexpr</a></li>
<ul>
</ul>
<li><a href="#108__function_overloading">Function overloading</a></li>
<ul>
</ul>
<li><a href="#109__discard">Discard</a></li>
<ul>
</ul>
<li><a href="#110__retval">Retval</a></li>
<ul>
</ul>
<li><a href="#111__foreign">Foreign</a></li>
<ul>
</ul>
</ul>
<li><a href="#120_intrinsics">Intrinsics</a></li>
<ul>
<li><a href="#Base">Base</a></li>
<li><a href="#Buildin">Buildin</a></li>
<li><a href="#Memory related">Memory related</a></li>
<li><a href="#Atomic operations">Atomic operations</a></li>
<li><a href="#Math">Math</a></li>
</ul>
<li><a href="#121_init">Init</a></li>
<ul>
<li><a href="#@init">@init</a></li>
<li><a href="#@drop">@drop</a></li>
</ul>
<li><a href="#130_generic">Generic</a></li>
<ul>
</ul>
<ul>
<li><a href="#131__declaration">Declaration</a></li>
<ul>
</ul>
<li><a href="#132__validif">Validif</a></li>
<ul>
<li><a href="#One time evaluation">One time evaluation</a></li>
<li><a href="#Multiple evaluations">Multiple evaluations</a></li>
</ul>
<li><a href="#133__constraint">Constraint</a></li>
<ul>
</ul>
</ul>
<li><a href="#160_scoping">Scoping</a></li>
<ul>
</ul>
<ul>
<li><a href="#161__defer">Defer</a></li>
<ul>
</ul>
<li><a href="#162__using">Using</a></li>
<ul>
<li><a href="#For a function parameter">For a function parameter</a></li>
<li><a href="#For a field">For a field</a></li>
</ul>
<li><a href="#163__with">With</a></li>
<ul>
</ul>
</ul>
<li><a href="#164_alias">Alias</a></li>
<ul>
<li><a href="#Type alias">Type alias</a></li>
<li><a href="#Name alias">Name alias</a></li>
</ul>
<li><a href="#170_error_management">Error management</a></li>
<ul>
<li><a href="#defer">defer</a></li>
</ul>
<li><a href="#175_safety">Safety</a></li>
<ul>
<li><a href="#overflow">overflow</a></li>
<li><a href="#any">any</a></li>
<li><a href="#boundcheck">boundcheck</a></li>
<li><a href="#math">math</a></li>
<li><a href="#switch">switch</a></li>
<li><a href="#bool">bool</a></li>
<li><a href="#nan">nan</a></li>
</ul>
<li><a href="#180_compiler_declarations">Compiler declarations</a></li>
<ul>
</ul>
<ul>
<li><a href="#181__compile_time_evaluation">Compile time evaluation</a></li>
<ul>
</ul>
<li><a href="#182__special_functions">Special functions</a></li>
<ul>
<li><a href="##test">#test</a></li>
<li><a href="##main">#main</a></li>
<li><a href="##init">#init</a></li>
<li><a href="##drop">#drop</a></li>
<li><a href="##premain">#premain</a></li>
</ul>
<li><a href="#183__run">Run</a></li>
<ul>
</ul>
<li><a href="#184__global">Global</a></li>
<ul>
</ul>
<li><a href="#185__var">Var</a></li>
<ul>
</ul>
</ul>
<li><a href="#190_attributes">Attributes</a></li>
<ul>
</ul>
<ul>
<li><a href="#191__user_attributes">User attributes</a></li>
<ul>
</ul>
<li><a href="#192__predefined_attributes">Predefined attributes</a></li>
<ul>
</ul>
</ul>
<li><a href="#200_type_reflection">Type reflection</a></li>
<ul>
<li><a href="#@decltype">@decltype</a></li>
</ul>
<li><a href="#210_code_inspection">Code inspection</a></li>
<ul>
</ul>
<li><a href="#220_meta_programmation">Meta programmation</a></li>
<ul>
</ul>
<ul>
<li><a href="#221__ast">Ast</a></li>
<ul>
<li><a href="#Struct and enums">Struct and enums</a></li>
<li><a href="#For example">For example</a></li>
<li><a href="#At global scope">At global scope</a></li>
</ul>
<li><a href="#222__compiler_interface">Compiler interface</a></li>
<ul>
</ul>
</ul>
<li><a href="#230_documentation">Documentation</a></li>
<ul>
</ul>
</ul>
</div>
<div class="right lg:w-full lg:h-full lg:pl-2">
<div class="page">
<h1 class="leading-8 mt-10 lg:my-5">Swag language reference</h1>

<h2 id="000_introduction">Introduction</h2>
<p>
This <code>swag-lang/swag/bin/reference/language</code> module provides a brief explanation of the language basic syntax and usage without the need of the <a href="libraries.php">Swag standard modules</a> (<code>Std</code>). </p>
<p>
More advanced features such as dynamic arrays, dynamic strings or hash maps can be found in the <a href="std/std.core.html">Std.Core</a> module and will not be covered in the examples to come. For now we will focus on the language itself. </p>
<p>
As <code>reference/language</code> is written as a test module, you can run it with : </p>
<pre><code><span style="color:#0">swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference</span></code></pre><p>
This will run all the test modules of the specified workspace (including this one). You can also omit the <code>--workspace</code> parameter (or <code>-w</code> in its short form) if you run Swag directly from the workspace folder. </p>
<p>
Note that if you want to compile and run a single module in the workspace, you can specify it with the <code>--module</code> (<code>-m</code>) parameter. </p>
<pre><code><span style="color:#0">swag test -w:c:/swag-lang/swag/bin/reference -m:test_language</span></code></pre><p>
</p>

<h2 id="001_hello_mad_world">Hello mad world</h2>
<p>
Let's start with the most simple version of the "hello world" example. This is a version that does not require external dependencies like the <a href="libraries.php">Swag standard modules</a>. </p>
<p>
<code>#main</code> is the <b>program entry point</b>, a special compiler function (that's why the name starts with <code>#</code>). It must be defined only once for a native executable. <code>@print</code> is an <b>intrinsic</b>, a special built-in function (that's why the name starts with <code>@</code>). It is part of the <a href="std/swag.runtime.html">compiler runtime</a> which comes with the compiler. </p>
<pre><code><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#B4B44A">@print</span><span style="color:#0">(</span><span style="color:#BB6643">"Hello mad world !\n"</span><span style="color:#0">)
}</span></code></pre><p>
</p>
<p>
Next, a version that this time uses the <code>Console.print</code> function in the <a href="std/std.core.html">Std.Core</a> module. The <code>Std.Core</code> module would have to be imported in order to be used, but let's keep it simple. </p>
<pre><code><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">Core</span><span style="color:#0">.</span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">print</span><span style="color:#0">(</span><span style="color:#BB6643">"Hello mad world !"</span><span style="color:#0">, </span><span style="color:#BB6643">"\n"</span><span style="color:#0">)
    </span><span style="color:#3BC3A7">Core</span><span style="color:#0">.</span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">printf</span><span style="color:#0">(</span><span style="color:#BB6643">"%\n"</span><span style="color:#0">, </span><span style="color:#BB6643">"Hello mad world again !"</span><span style="color:#0">)
}</span></code></pre><p>
</p>
<p>
A <code>#run</code> block is executed at <b>compile time</b>, and can make Swag behaves like a kind of a <b>scripting language</b>. So in the following example, the famous message will be printed by the compiler during compilation. </p>
<pre><code><span style="color:#FF6A00">#run</span><span style="color:#0">
{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">Msg</span><span style="color:#0"> = </span><span style="color:#BB6643">"Hello mad world !\n"</span><span style="color:#0">   </span><span style="color:#6A9955">// Creates a compiler constant of type 'string'</span><span style="color:#0">
    </span><span style="color:#3BC3A7">Core</span><span style="color:#0">.</span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">print</span><span style="color:#0">(</span><span style="color:#3BC3A7">Msg</span><span style="color:#0">)             </span><span style="color:#6A9955">// And call 'Console.print' at compile time</span><span style="color:#0">
}</span></code></pre><p>
</p>
<p>
A version that calls a <b>nested function</b> at compile time (only) to initialize the string constant to print. </p>
<pre><code><span style="color:#6A9955">// Brings the 'Core' namespace into scope, to avoid repeating it again and again</span><span style="color:#0">
</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">Core</span><span style="color:#0">

</span><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">nestedFunc</span><span style="color:#0">() => </span><span style="color:#BB6643">"Hello mad world !\n"</span><span style="color:#0">   </span><span style="color:#6A9955">// Function short syntax</span><span style="color:#0">

    </span><span style="color:#6A9955">// nestedFunc() can be called at compile time because it is marked with</span><span style="color:#0">
    </span><span style="color:#6A9955">// the 'Swag.ConstExpr' attribute.</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">Msg</span><span style="color:#0"> = </span><span style="color:#FF6A00">nestedFunc</span><span style="color:#0">()
    </span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">print</span><span style="color:#0">(</span><span style="color:#3BC3A7">Msg</span><span style="color:#0">)
}</span></code></pre><p>
</p>
<p>
Now a stupid version that generates the code to do the print thanks to <b>meta programming</b>. </p>
<pre><code><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">Core</span><span style="color:#0">

</span><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">Msg</span><span style="color:#0"> = </span><span style="color:#BB6643">"Hello mad world !\n"</span><span style="color:#0">

    </span><span style="color:#6A9955">// The result of an '#ast' block is a string that will be compiled in place.</span><span style="color:#0">
    </span><span style="color:#6A9955">// So this whole thing is equivalent to a simple 'Console.print(Msg)'.</span><span style="color:#0">
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> sb = </span><span style="color:#3BC3A7">StrConv</span><span style="color:#0">.</span><span style="color:#3BC3A7">StringBuilder</span><span style="color:#0">{}
        sb.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">"Console.print(Msg)"</span><span style="color:#0">)
        </span><span style="color:#B040BE">return</span><span style="color:#0"> sb.</span><span style="color:#FF6A00">toString</span><span style="color:#0">()
    }
}</span></code></pre><p>
</p>
<p>
And finally let's be more and more crazy. </p>
<pre><code><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">Core</span><span style="color:#0">

</span><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// #run will force the call of mySillyFunction() at compile time even if it's not marked</span><span style="color:#0">
    </span><span style="color:#6A9955">// with #[Swag.ConstExpr]</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">Msg</span><span style="color:#0"> = </span><span style="color:#FF6A00">#run</span><span style="color:#0"> </span><span style="color:#FF6A00">mySillyFunction</span><span style="color:#0">()
    </span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">print</span><span style="color:#0">(</span><span style="color:#3BC3A7">Msg</span><span style="color:#0">)
}

</span><span style="color:#6A9955">// The attribute #[Swag.Compiler] tells Swag that this is a compile time function only.</span><span style="color:#0">
</span><span style="color:#6A9955">// So this function will not be exported to the final executable or module.</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Swag.Compiler]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">mySillyFunction</span><span style="color:#0">()-></span><span style="color:#ED9A11">string</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">print</span><span style="color:#0">(</span><span style="color:#BB6643">"Hello mad world at compile time !\n"</span><span style="color:#0">)

    </span><span style="color:#6A9955">// This creates a constant named 'MyConst'</span><span style="color:#0">
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> sb = </span><span style="color:#3BC3A7">StrConv</span><span style="color:#0">.</span><span style="color:#3BC3A7">StringBuilder</span><span style="color:#0">{}
        sb.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">"const MyConst = \"Hello "</span><span style="color:#0">)
        sb.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">"mad world "</span><span style="color:#0">)
        sb.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">"at runtime !\""</span><span style="color:#0">)
        </span><span style="color:#B040BE">return</span><span style="color:#0"> sb.</span><span style="color:#FF6A00">toString</span><span style="color:#0">()
    }

    </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyConst</span><span style="color:#0">
}</span></code></pre><p>
</p>
<p>
This whole piece of code is equivalent to... </p>
<pre><code><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">Core</span><span style="color:#0">.</span><span style="color:#3BC3A7">Console</span><span style="color:#0">.</span><span style="color:#FF6A00">print</span><span style="color:#0">(</span><span style="color:#BB6643">"Hello mad world at runtime !"</span><span style="color:#0">)
}</span></code></pre><p>
</p>

<h2 id="002_source_code_organization">Source code organization</h2>
<p>
All source files in Swag have the <code>.swg</code> extension, except if you write a simple script with the <code>.swgs</code> extension. They must be encoded in <b>UTF8</b>. </p>
<p>
In Swag you cannot compile a single file (with the exception of <code>.swgs</code> script files). The source code is organized in a <b>workspace</b> which contains one or multiple <b>modules</b>. </p>
<p>
For example, <code>Std</code> is a workspace that contains all the Swag standard modules. </p>
<p>
A module is a <code>dll</code> (under windows) or an executable, and a workspace can include many of them. So a workspace will contain the modules you write (like your main executable) but also all your dependencies (some external modules you use). </p>
<p>
Typically, the entire workspace is compiled. </p>

<h2 id="003_comments">Comments</h2>
<p>
Let's start with the basics. Swag support classical single-line and multi-line comments. </p>
<pre><code><span style="color:#6A9955">// Single-line comment</span><span style="color:#0">

</span><span style="color:#6A9955">/*</span><span style="color:#0">
</span><span style="color:#6A9955">    Multi-line comment on...</span><span style="color:#0">
</span><span style="color:#6A9955">    ... multiple lines</span><span style="color:#0">
</span><span style="color:#6A9955">*/</span><span style="color:#0">

</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0"> </span><span style="color:#6A9955">// This is a constant with the value '0' assigned to it</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#6A9955">/* false */</span><span style="color:#0"> </span><span style="color:#3186CD">true</span></code></pre><p>
Nested comments are supported. </p>
<pre><code><span style="color:#6A9955">/*</span><span style="color:#0">
</span><span style="color:#6A9955">    /* You can also nest multi-line comments */</span><span style="color:#0">
</span><span style="color:#6A9955">*/</span></code></pre>
<h2 id="004_identifiers">Identifiers</h2>
<p>
User identifiers (like variables, constants, function names...) must start with an underscore or an ascii letter. Those identifiers can then contain underscores, ascii letters and digit numbers. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> thisIsAValidIdentifier0   = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> this_is_also_valid        = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> this_1_is_2_also__3_valid = </span><span style="color:#74A35B">0</span></code></pre><p>
But your identifiers cannot start with two underscores. This is reserved by the compiler. </p>
<pre><code><span style="color:#6A9955">// const __this_is_invalid = 0</span></code></pre><p>
Note that some identifiers may start with <code>#</code>. This indicates a <b>compiler special keyword</b>. </p>
<pre><code><span style="color:#7F7F7F">#assert</span><span style="color:#0">
</span><span style="color:#FF6A00">#run</span><span style="color:#0">
</span><span style="color:#FF6A00">#main</span></code></pre><p>
</p>
<p>
Some identifiers can also start with <code>@</code>. This indicates an <b>intrinsic</b> function which can be compile time only or also available at runtime (this depends on the intrinsic). </p>
<pre><code><span style="color:#F2470C">@min</span><span style="color:#0">()
</span><span style="color:#F2470C">@max</span><span style="color:#0">()
</span><span style="color:#F2470C">@sqrt</span><span style="color:#0">()
</span><span style="color:#B4B44A">@print</span><span style="color:#0">()</span></code></pre><p>
</p>

<h2 id="005_keywords">Keywords</h2>
<p>
This is the list of all keywords in the language. </p>
<p>
<h3 id="Basic types">Basic types </h3></p>
<p>
</p>
<pre><code><span style="color:#ED9A11">s8</span><span style="color:#0">
</span><span style="color:#ED9A11">s16</span><span style="color:#0">
</span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#ED9A11">s64</span><span style="color:#0">
</span><span style="color:#ED9A11">u8</span><span style="color:#0">
</span><span style="color:#ED9A11">u16</span><span style="color:#0">
</span><span style="color:#ED9A11">u32</span><span style="color:#0">
</span><span style="color:#ED9A11">u64</span><span style="color:#0">
</span><span style="color:#ED9A11">f32</span><span style="color:#0">
</span><span style="color:#ED9A11">f64</span><span style="color:#0">
</span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#ED9A11">string</span><span style="color:#0">
</span><span style="color:#ED9A11">rune</span><span style="color:#0">
</span><span style="color:#ED9A11">any</span><span style="color:#0">
</span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">
</span><span style="color:#ED9A11">void</span><span style="color:#0">
</span><span style="color:#ED9A11">code</span><span style="color:#0">
</span><span style="color:#ED9A11">cstring</span><span style="color:#0">
</span><span style="color:#ED9A11">cvarargs</span></code></pre><p>
<h3 id="Language keywords">Language keywords </h3></p>
<p>
</p>
<pre><code><span style="color:#B040BE">if</span><span style="color:#0">
</span><span style="color:#B040BE">else</span><span style="color:#0">
</span><span style="color:#B040BE">elif</span><span style="color:#0">
</span><span style="color:#B040BE">and</span><span style="color:#0">
</span><span style="color:#B040BE">or</span><span style="color:#0">
</span><span style="color:#B040BE">switch</span><span style="color:#0">
</span><span style="color:#B040BE">case</span><span style="color:#0">
</span><span style="color:#B040BE">default</span><span style="color:#0">
</span><span style="color:#B040BE">break</span><span style="color:#0">
</span><span style="color:#B040BE">continue</span><span style="color:#0">
</span><span style="color:#B040BE">fallthrough</span><span style="color:#0">
</span><span style="color:#B040BE">unreachable</span><span style="color:#0">
</span><span style="color:#B040BE">for</span><span style="color:#0">
</span><span style="color:#B040BE">loop</span><span style="color:#0">
</span><span style="color:#B040BE">while</span><span style="color:#0">
</span><span style="color:#B040BE">visit</span><span style="color:#0">
</span><span style="color:#B040BE">return</span><span style="color:#0">
</span><span style="color:#B040BE">defer</span><span style="color:#0">

</span><span style="color:#3186CD">false</span><span style="color:#0">
</span><span style="color:#3186CD">true</span><span style="color:#0">
</span><span style="color:#3186CD">null</span><span style="color:#0">
</span><span style="color:#3186CD">undefined</span><span style="color:#0">
</span><span style="color:#3186CD">retval</span><span style="color:#0">

</span><span style="color:#3186CD">const</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0">
</span><span style="color:#3186CD">ref</span><span style="color:#0">
</span><span style="color:#3186CD">acast</span><span style="color:#0">
</span><span style="color:#3186CD">cast</span><span style="color:#0">
</span><span style="color:#3186CD">assume</span><span style="color:#0">
</span><span style="color:#3186CD">try</span><span style="color:#0">
</span><span style="color:#3186CD">catch</span><span style="color:#0">
</span><span style="color:#3186CD">throw</span><span style="color:#0">
</span><span style="color:#3186CD">closure</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0">
</span><span style="color:#3186CD">mtd</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0">
</span><span style="color:#3186CD">enum</span><span style="color:#0">
</span><span style="color:#3186CD">struct</span><span style="color:#0">
</span><span style="color:#3186CD">union</span><span style="color:#0">
</span><span style="color:#3186CD">namespace</span><span style="color:#0">
</span><span style="color:#3186CD">impl</span><span style="color:#0">
</span><span style="color:#3186CD">public</span><span style="color:#0">
</span><span style="color:#3186CD">internal</span><span style="color:#0">
</span><span style="color:#3186CD">private</span><span style="color:#0">
</span><span style="color:#3186CD">interface</span><span style="color:#0">
</span><span style="color:#3186CD">using</span><span style="color:#0">
</span><span style="color:#3186CD">typealias</span><span style="color:#0">
</span><span style="color:#3186CD">namealias</span><span style="color:#0">
</span><span style="color:#3186CD">discard</span><span style="color:#0">
</span><span style="color:#3186CD">dref</span></code></pre><p>
<h3 id="Compiler keywords">Compiler keywords </h3></p>
<p>
</p>
<pre><code><span style="color:#C3973B">#arch</span><span style="color:#0">
</span><span style="color:#C3973B">#backend</span><span style="color:#0">
</span><span style="color:#C3973B">#callerfunction</span><span style="color:#0">
</span><span style="color:#C3973B">#callerlocation</span><span style="color:#0">
</span><span style="color:#C3973B">#cfg</span><span style="color:#0">
</span><span style="color:#C3973B">#code</span><span style="color:#0">
</span><span style="color:#C3973B">#file</span><span style="color:#0">
</span><span style="color:#C3973B">#line</span><span style="color:#0">
</span><span style="color:#C3973B">#location</span><span style="color:#0">
</span><span style="color:#C3973B">#module</span><span style="color:#0">
</span><span style="color:#C3973B">#os</span><span style="color:#0">
</span><span style="color:#C3973B">#self</span><span style="color:#0">
</span><span style="color:#C3973B">#swagbuildnum</span><span style="color:#0">
</span><span style="color:#C3973B">#swagos</span><span style="color:#0">
</span><span style="color:#C3973B">#swagrevision</span><span style="color:#0">
</span><span style="color:#C3973B">#swagversion</span><span style="color:#0">
</span><span style="color:#C3973B">#type</span><span style="color:#0">
</span><span style="color:#C3973B">#up</span><span style="color:#0">

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0">
</span><span style="color:#7F7F7F">#elif</span><span style="color:#0">
</span><span style="color:#7F7F7F">#else</span><span style="color:#0">
</span><span style="color:#7F7F7F">#error</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0">
</span><span style="color:#7F7F7F">#if</span><span style="color:#0">
</span><span style="color:#7F7F7F">#import</span><span style="color:#0">
</span><span style="color:#7F7F7F">#include</span><span style="color:#0">
</span><span style="color:#7F7F7F">#load</span><span style="color:#0">
</span><span style="color:#7F7F7F">#macro</span><span style="color:#0">
</span><span style="color:#7F7F7F">#mixin</span><span style="color:#0">
</span><span style="color:#7F7F7F">#placeholder</span><span style="color:#0">
</span><span style="color:#7F7F7F">#print</span><span style="color:#0">
</span><span style="color:#7F7F7F">#validifx</span><span style="color:#0">
</span><span style="color:#7F7F7F">#validif</span><span style="color:#0">
</span><span style="color:#7F7F7F">#warning</span><span style="color:#0">
</span><span style="color:#7F7F7F">#scope</span><span style="color:#0">

</span><span style="color:#FF6A00">#ast</span><span style="color:#0">
</span><span style="color:#FF6A00">#drop</span><span style="color:#0">
</span><span style="color:#FF6A00">#init</span><span style="color:#0">
</span><span style="color:#FF6A00">#main</span><span style="color:#0">
</span><span style="color:#FF6A00">#message</span><span style="color:#0">
</span><span style="color:#FF6A00">#premain</span><span style="color:#0">
</span><span style="color:#FF6A00">#run</span></code></pre><p>
<h3 id="Intrinsics libc">Intrinsics libc </h3></p>
<p>
</p>
<pre><code><span style="color:#F2470C">@abs</span><span style="color:#0">
</span><span style="color:#F2470C">@acos</span><span style="color:#0">
</span><span style="color:#F2470C">@alloc</span><span style="color:#0">
</span><span style="color:#F2470C">@asin</span><span style="color:#0">
</span><span style="color:#F2470C">@atan</span><span style="color:#0">
</span><span style="color:#F2470C">@atomadd</span><span style="color:#0">
</span><span style="color:#F2470C">@atomand</span><span style="color:#0">
</span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">
</span><span style="color:#F2470C">@atomor</span><span style="color:#0">
</span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">
</span><span style="color:#F2470C">@atomxor</span><span style="color:#0">
</span><span style="color:#F2470C">@bitcountlz</span><span style="color:#0">
</span><span style="color:#F2470C">@bitcountnz</span><span style="color:#0">
</span><span style="color:#F2470C">@bitcounttz</span><span style="color:#0">
</span><span style="color:#F2470C">@byteswap</span><span style="color:#0">
</span><span style="color:#F2470C">@ceil</span><span style="color:#0">
</span><span style="color:#F2470C">@cos</span><span style="color:#0">
</span><span style="color:#F2470C">@cosh</span><span style="color:#0">
</span><span style="color:#F2470C">@cvaarg</span><span style="color:#0">
</span><span style="color:#F2470C">@cvaend</span><span style="color:#0">
</span><span style="color:#F2470C">@cvastart</span><span style="color:#0">
</span><span style="color:#F2470C">@exp</span><span style="color:#0">
</span><span style="color:#F2470C">@exp2</span><span style="color:#0">
</span><span style="color:#F2470C">@floor</span><span style="color:#0">
</span><span style="color:#F2470C">@free</span><span style="color:#0">
</span><span style="color:#F2470C">@log</span><span style="color:#0">
</span><span style="color:#F2470C">@log10</span><span style="color:#0">
</span><span style="color:#F2470C">@log2</span><span style="color:#0">
</span><span style="color:#F2470C">@max</span><span style="color:#0">
</span><span style="color:#F2470C">@memcmp</span><span style="color:#0">
</span><span style="color:#F2470C">@memcpy</span><span style="color:#0">
</span><span style="color:#F2470C">@memmove</span><span style="color:#0">
</span><span style="color:#F2470C">@memset</span><span style="color:#0">
</span><span style="color:#F2470C">@muladd</span><span style="color:#0">
</span><span style="color:#F2470C">@min</span><span style="color:#0">
</span><span style="color:#F2470C">@pow</span><span style="color:#0">
</span><span style="color:#F2470C">@realloc</span><span style="color:#0">
</span><span style="color:#F2470C">@round</span><span style="color:#0">
</span><span style="color:#F2470C">@sin</span><span style="color:#0">
</span><span style="color:#F2470C">@sinh</span><span style="color:#0">
</span><span style="color:#F2470C">@sqrt</span><span style="color:#0">
</span><span style="color:#F2470C">@strcmp</span><span style="color:#0">
</span><span style="color:#F2470C">@strlen</span><span style="color:#0">
</span><span style="color:#F2470C">@tan</span><span style="color:#0">
</span><span style="color:#F2470C">@tanh</span><span style="color:#0">
</span><span style="color:#F2470C">@trunc</span></code></pre><p>
<h3 id="Other intrinsics">Other intrinsics </h3></p>
<p>
</p>
<pre><code><span style="color:#B4B44A">@index</span><span style="color:#0">
</span><span style="color:#B4B44A">@err</span><span style="color:#0">
</span><span style="color:#B4B44A">@errmsg</span><span style="color:#0">
</span><span style="color:#B4B44A">@alias0</span><span style="color:#0">
</span><span style="color:#B4B44A">@alias1</span><span style="color:#0">
</span><span style="color:#B4B44A">@alias2</span><span style="color:#0"> </span><span style="color:#6A9955">// and more generally @aliasN</span><span style="color:#0">
</span><span style="color:#B4B44A">@alignof</span><span style="color:#0">
</span><span style="color:#B4B44A">@args</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">
</span><span style="color:#B4B44A">@breakpoint</span><span style="color:#0">
</span><span style="color:#B4B44A">@compiler</span><span style="color:#0">
</span><span style="color:#B4B44A">@countof</span><span style="color:#0">
</span><span style="color:#B4B44A">@dataof</span><span style="color:#0">
</span><span style="color:#B4B44A">@dbgalloc</span><span style="color:#0">
</span><span style="color:#B4B44A">@defined</span><span style="color:#0">
</span><span style="color:#B4B44A">@drop</span><span style="color:#0">
</span><span style="color:#B4B44A">@compilererror</span><span style="color:#0">
</span><span style="color:#B4B44A">@compilerwarning</span><span style="color:#0">
</span><span style="color:#B4B44A">@getcontext</span><span style="color:#0">
</span><span style="color:#B4B44A">@getpinfos</span><span style="color:#0">
</span><span style="color:#B4B44A">@gettag</span><span style="color:#0">
</span><span style="color:#B4B44A">@hastag</span><span style="color:#0">
</span><span style="color:#B4B44A">@init</span><span style="color:#0">
</span><span style="color:#B4B44A">@isbytecode</span><span style="color:#0">
</span><span style="color:#B4B44A">@isconstexpr</span><span style="color:#0">
</span><span style="color:#B4B44A">@itftableof</span><span style="color:#0">
</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">
</span><span style="color:#B4B44A">@location</span><span style="color:#0">
</span><span style="color:#B4B44A">@mixin0</span><span style="color:#0">
</span><span style="color:#B4B44A">@mixin1</span><span style="color:#0">
</span><span style="color:#B4B44A">@mixin2</span><span style="color:#0"> </span><span style="color:#6A9955">// and more generally @mixinN</span><span style="color:#0">
</span><span style="color:#B4B44A">@mkany</span><span style="color:#0">
</span><span style="color:#B4B44A">@mkcallback</span><span style="color:#0">
</span><span style="color:#B4B44A">@mkinterface</span><span style="color:#0">
</span><span style="color:#B4B44A">@mkslice</span><span style="color:#0">
</span><span style="color:#B4B44A">@mkstring</span><span style="color:#0">
</span><span style="color:#B4B44A">@decltype</span><span style="color:#0">
</span><span style="color:#B4B44A">@modules</span><span style="color:#0">
</span><span style="color:#B4B44A">@nameof</span><span style="color:#0">
</span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">
</span><span style="color:#B4B44A">@panic</span><span style="color:#0">
</span><span style="color:#B4B44A">@postcopy</span><span style="color:#0">
</span><span style="color:#B4B44A">@postmove</span><span style="color:#0">
</span><span style="color:#B4B44A">@print</span><span style="color:#0">
</span><span style="color:#B4B44A">@rtflags</span><span style="color:#0">
</span><span style="color:#B4B44A">@safety</span><span style="color:#0">
</span><span style="color:#B4B44A">@setcontext</span><span style="color:#0">
</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">
</span><span style="color:#B4B44A">@spread</span><span style="color:#0">
</span><span style="color:#B4B44A">@stringcmp</span><span style="color:#0">
</span><span style="color:#B4B44A">@stringof</span><span style="color:#0">
</span><span style="color:#B4B44A">@sysalloc</span><span style="color:#0">
</span><span style="color:#B4B44A">@typecmp</span><span style="color:#0">
</span><span style="color:#B4B44A">@typeof</span></code></pre><p>
<h3 id="Modifiers">Modifiers </h3></p>
<p>
</p>
<pre><code><span style="color:#0">,over
,nodrop
,bit
,move
,moveraw
,up
,unconst</span></code></pre><p>
</p>

<h2 id="006_semicolon">Semicolon</h2>
<p>
In Swag, there's no need to end a statement with <code>;</code> like in C/C++. Most of the time a <code>end of line</code> is enough. </p>
<pre><code><span style="color:#6A9955">// Declare two variables x and y of type s32 (signed 32 bits), and initialize them to 1.</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// Increment x and y by 1.</span><span style="color:#0">
x += </span><span style="color:#74A35B">1</span><span style="color:#0">
y += </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// When running the tests, the '@assert' intrinsic will verify that those values are correct.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">2</span><span style="color:#0">)     </span><span style="color:#6A9955">// Verify that x is equal to 2, and raise an error if not.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == x)     </span><span style="color:#6A9955">// Verify that y is equal to x.</span></code></pre><p>
The semicolons are not mandatory, but it's possible to use them if you want. </p>
<pre><code><span style="color:#6A9955">// You can notice that the type of x and y is not specified here. This is due to type inference (we will see that later).</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> x, y = </span><span style="color:#74A35B">0</span><span style="color:#0">;

</span><span style="color:#6A9955">// You can also notice the short syntax to do the same operation on multiple variables at once.</span><span style="color:#0">
x, y += </span><span style="color:#74A35B">1</span><span style="color:#0">;</span></code></pre><p>
Semicolons can be usefull if you want to do multiple things on the same line. </p>
<pre><code><span style="color:#6A9955">// Two instructions on the same line separated with ';'</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">0</span><span style="color:#0">; </span><span style="color:#3186CD">var</span><span style="color:#0"> y = </span><span style="color:#74A35B">0</span><span style="color:#0">

</span><span style="color:#6A9955">// Two instructions on the same line separated with ';'</span><span style="color:#0">
x += </span><span style="color:#74A35B">1</span><span style="color:#0">; y += </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// Two instructions on the same line separated with ';'</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">1</span><span style="color:#0">); </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">1</span><span style="color:#0">)</span></code></pre>
<h2 id="007_global_declaration_order">Global declaration order</h2>
<p>
The order of all <b>top level</b> declarations does not matter. </p>
<pre><code><span style="color:#6A9955">// Here we declare a constant 'A' and initialize it with 'B', which is not</span><span style="color:#0">
</span><span style="color:#6A9955">// yet known (neither its value or its type).</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#3BC3A7">B</span><span style="color:#0">
</span><span style="color:#6A9955">// Declare a constant 'B' equals to 'C', still unknown at this point.</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#3BC3A7">C</span><span style="color:#0">
</span><span style="color:#6A9955">// Declare a constant C of type 'u64' (unsigned 64 bits integer) and assigned it to 1.</span><span style="color:#0">
</span><span style="color:#6A9955">// At this point A and B are then also defined.</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">C</span><span style="color:#0">: </span><span style="color:#ED9A11">u64</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span></code></pre><p>
In this test, we call the function <code>functionDeclaredLater</code> before it is known. This is fine. </p>
<pre><code><span style="color:#6A9955">// First, call a unknown function named 'functionDeclaredLater'</span><span style="color:#0">
</span><span style="color:#FF6A00">#run</span><span style="color:#0">
{
    </span><span style="color:#FF6A00">functionDeclaredLater</span><span style="color:#0">()
}

</span><span style="color:#6A9955">// Then declare it after</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">functionDeclaredLater</span><span style="color:#0">()
{
}</span></code></pre><p>
Note that the order is not relevant in the same file, but it is also irrelevant across multiple files. You can for example call a function in one file and declare it in another one. Global ordrer does not matter ! </p>

<h2 id="010_basic_types">Basic types</h2>
<p>

  These are all signed integers types <code>s8</code>, <code>s16</code>, <code>s32</code> and <code>s64</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">s8</span><span style="color:#0">  = -</span><span style="color:#74A35B">1</span><span style="color:#0">     </span><span style="color:#6A9955">// 8 bits signed integer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b: </span><span style="color:#ED9A11">s16</span><span style="color:#0"> = -</span><span style="color:#74A35B">2</span><span style="color:#0">     </span><span style="color:#6A9955">// 16 bits signed integer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> c: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = -</span><span style="color:#74A35B">3</span><span style="color:#0">     </span><span style="color:#6A9955">// 32 bits signed integer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> d: </span><span style="color:#ED9A11">s64</span><span style="color:#0"> = -</span><span style="color:#74A35B">4</span><span style="color:#0">     </span><span style="color:#6A9955">// 64 bits signed integer</span><span style="color:#0">

    </span><span style="color:#6A9955">// Remember that the instrinsic '@assert' will raise an error at runtime if</span><span style="color:#0">
    </span><span style="color:#6A9955">// the enclosed expression is false.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == -</span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == -</span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == -</span><span style="color:#74A35B">3</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(d == -</span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#6A9955">// The instrinsic '@sizeof' gives the size, in bytes, of a variable.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">1</span><span style="color:#0">)    </span><span style="color:#6A9955">// 'a' has type 's8', so this should be 1 byte.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(b) == </span><span style="color:#74A35B">2</span><span style="color:#0">)    </span><span style="color:#6A9955">// 'b' has type 's16', so this should be 2 bytes.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(c) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(d) == </span><span style="color:#74A35B">8</span><span style="color:#0">)
}</span></code></pre><p>

  These are all unsigned integers types <code>u8</code>, <code>u16</code>, <code>u32</code> and <code>u64</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">u8</span><span style="color:#0">  = </span><span style="color:#74A35B">1</span><span style="color:#0">      </span><span style="color:#6A9955">// 8 bits unsigned integer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b: </span><span style="color:#ED9A11">u16</span><span style="color:#0"> = </span><span style="color:#74A35B">2</span><span style="color:#0">      </span><span style="color:#6A9955">// 16 bits unsigned integer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> c: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">3</span><span style="color:#0">      </span><span style="color:#6A9955">// 32 bits unsigned integer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> d: </span><span style="color:#ED9A11">u64</span><span style="color:#0"> = </span><span style="color:#74A35B">4</span><span style="color:#0">      </span><span style="color:#6A9955">// 64 bits unsigned integer</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">3</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(d == </span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(b) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(c) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(d) == </span><span style="color:#74A35B">8</span><span style="color:#0">)
}</span></code></pre><p>

  These are all floating point types <code>f32</code> and <code>f64</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = </span><span style="color:#74A35B">3.14</span><span style="color:#0">       </span><span style="color:#6A9955">// 32 bits floating point value</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b: </span><span style="color:#ED9A11">f64</span><span style="color:#0"> = </span><span style="color:#74A35B">3.14159</span><span style="color:#0">    </span><span style="color:#6A9955">// 64 bits floating point value</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">3.14</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">3.14159</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(b) == </span><span style="color:#74A35B">8</span><span style="color:#0">)
}</span></code></pre><p>

  The boolean type <code>bool</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> = </span><span style="color:#3186CD">true</span><span style="color:#0">      </span><span style="color:#6A9955">// Stored in 1 byte</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> = </span><span style="color:#3186CD">false</span><span style="color:#0">     </span><span style="color:#6A9955">// Stored in 1 byte</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#3186CD">false</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(b) == </span><span style="color:#74A35B">1</span><span style="color:#0">)
}</span></code></pre><p>

  The <code>string</code> type. Strings are <b>UTF8</b>, and are stored as two 64 bits (the pointer to the value and the string length in bytes). Note that a string literal also ends with a null byte like in C. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 是"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#BB6643">"string 是"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">2</span><span style="color:#0"> * </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(*</span><span style="color:#ED9A11">void</span><span style="color:#0">))
}</span></code></pre><p>

  The <code>rune</code> type uses the string syntax, postfix with the type <code>rune</code>. It's a 32 bits unicode code point. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">rune</span><span style="color:#0"> = </span><span style="color:#BB6643">"是"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#BB6643">"是"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="Type reflection">Type reflection </h3></p>
<p>
Swag has <b>type reflection</b> at <b>compile time</b> and at <b>runtime</b>. We will see that later in more details. </p>
<p>

  You can use <code>@decltype</code> to create a type based on an expression. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">                   </span><span style="color:#6A9955">// 'a' is inferred to have the 's32' type</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b: </span><span style="color:#B4B44A">@decltype</span><span style="color:#0">(a) = </span><span style="color:#74A35B">1</span><span style="color:#0">     </span><span style="color:#6A9955">// 'b' will have the same type as 'a'</span><span style="color:#0">

    </span><span style="color:#6A9955">// We can verify with '@typeof' that 'a' and 'b' have the same type.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b))
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">)

    </span><span style="color:#6A9955">// As the types of 'a' and 'b' are known by the compiler, we can use '#assert' instead of '@assert'.</span><span style="color:#0">
    </span><span style="color:#6A9955">// The '#assert' will be done at compile time, and will not generate code (unlike '@assert').</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>

  Short notice that types are also values, at compile time and at runtime. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">)   </span><span style="color:#6A9955">// 'x' is now a variable that contains a type</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#ED9A11">s32</span><span style="color:#0">)   </span><span style="color:#6A9955">// So it can be compared to a type</span><span style="color:#0">

    </span><span style="color:#6A9955">// A type is a predefined struct, which contains some fields to inspect it.</span><span style="color:#0">
    </span><span style="color:#6A9955">// You can for example get the name.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x.name == </span><span style="color:#BB6643">"s32"</span><span style="color:#0">)

    </span><span style="color:#6A9955">// @typeof is not really necessary when there's no ambiguity about the expression.</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> y = </span><span style="color:#ED9A11">bool</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#ED9A11">bool</span><span style="color:#0">)
}</span></code></pre>
<h2 id="011_number_literals">Number literals</h2>
<p>

  Integers in <i>decimal</i>, <i>hexadecimal</i> or <i>binary</i> forms. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">123456</span><span style="color:#0">           </span><span style="color:#6A9955">// Decimal</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> b: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">0xFFFF</span><span style="color:#0">           </span><span style="color:#6A9955">// Hexadecimal, with '0x'</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> c: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">0b00001111</span><span style="color:#0">       </span><span style="color:#6A9955">// Binary, with '0b'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">123456</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">65535</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">15</span><span style="color:#0">)
}</span></code></pre><p>

  You can separate the digits with the <code>_</code> character. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">123</span><span style="color:#0">_</span><span style="color:#74A35B">456</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> b: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">0xF_F_F_F</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> c: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">0b0000_1111</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">123456</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">65</span><span style="color:#0">_</span><span style="color:#74A35B">535</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">15</span><span style="color:#0">)
}</span></code></pre><p>

  The default type of an hexadecimal number or a binary number is <code>u32</code> or <code>u64</code> depending on its value. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// The compiler will deduce that the type of 'a' is 'u32'.</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#74A35B">0xFF</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">u32</span><span style="color:#0">

    </span><span style="color:#6A9955">// The compiler will deduce that the type of 'b' is 'u64' because the constant</span><span style="color:#0">
    </span><span style="color:#6A9955">// is too big for 32 bits.</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> b = </span><span style="color:#74A35B">0xF_FFFFF_FFFFFF</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b) == </span><span style="color:#ED9A11">u64</span><span style="color:#0">

    </span><span style="color:#3186CD">const</span><span style="color:#0"> c = </span><span style="color:#74A35B">0b00000001</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(c) == </span><span style="color:#ED9A11">u32</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> d = </span><span style="color:#74A35B">0b00000001_00000001_00000001_00000001_00000001</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(d) == </span><span style="color:#ED9A11">u64</span><span style="color:#0">
}</span></code></pre><p>

  A boolean is <code>true</code> or <code>false</code>. Note again that as constants are known at compile time, we can  use <code>#assert</code> to check the values. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#3186CD">true</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#3186CD">true</span><span style="color:#0">

    </span><span style="color:#3186CD">const</span><span style="color:#0"> b, c = </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> b == </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> c == </span><span style="color:#3186CD">false</span><span style="color:#0">
}</span></code></pre><p>

  A floating point value has the usual C/C++ form. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">1.5</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">

    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#74A35B">0.11</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">0.11</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> c = </span><span style="color:#74A35B">15e2</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">15e2</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> d = </span><span style="color:#74A35B">15e</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(d == </span><span style="color:#74A35B">15e2</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> e = -</span><span style="color:#74A35B">1E</span><span style="color:#0">-</span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(e == -</span><span style="color:#74A35B">0.1</span><span style="color:#0">)
}</span></code></pre><p>

  By default, a floating point value is <code>f32</code>, not <code>f64</code> (aka <code>double</code>) like in C/C++. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">1.5</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) != </span><span style="color:#ED9A11">f64</span><span style="color:#0">
}</span></code></pre><p>
<h3 id="Postfix">Postfix </h3></p>
<p>
You can also <b>postfix</b> a literal number by a type. </p>
<pre><code><span style="color:#6A9955">// Declare 'a' to be a 'f64' variable assigned to '1.5'</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">1.5</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1.5</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">)
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">

</span><span style="color:#6A9955">// 'b' is a new variable of type 'u8' initialized with '10'.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#74A35B">10</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b) == </span><span style="color:#ED9A11">u8</span><span style="color:#0">

</span><span style="color:#3186CD">let</span><span style="color:#0"> c = </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u32</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(c) == </span><span style="color:#ED9A11">u32</span></code></pre>
<h2 id="012_string">String</h2>
<p>
In Swag, strings are encoded in UTF8. </p>
<p>

  They also can be compared. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"this is a chinese character: 是"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#BB6643">"this is a chinese character: 是"</span><span style="color:#0">

    </span><span style="color:#3186CD">const</span><span style="color:#0"> b = </span><span style="color:#BB6643">"these are some cyrillic characters: ӜИ"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> b == </span><span style="color:#BB6643">"these are some cyrillic characters: ӜИ"</span><span style="color:#0">
}</span></code></pre><p>

  A rune is an unicode codepoint, and is 32 bits. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"是"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#BB6643">"是"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u32</span><span style="color:#0">)
}</span></code></pre><p>
You cannot index a string to get a rune, except in ascii strings. This is because we didn't want the runtime to come with the cost of UTF8 encoding/decoding. But note that the <code>Std.Core</code> module will have all you need to manipulate UTF8 strings. </p>
<p>

  So in that case you will retrieve a byte. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"this is a chinese character: 是"</span><span style="color:#0">

    </span><span style="color:#6A9955">// Get the first byte of the string</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> b = a[</span><span style="color:#74A35B">0</span><span style="color:#0">]
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> b == </span><span style="color:#BB6643">"t"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u8</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Here, the 'X' character in the middle does not have the index '1', because the</span><span style="color:#0">
    </span><span style="color:#6A9955">// chinese character before is encoded in UTF8 with more than 1 byte.</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> c = </span><span style="color:#BB6643">"是X是"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> c[</span><span style="color:#74A35B">1</span><span style="color:#0">] != </span><span style="color:#BB6643">"X"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> </span><span style="color:#6A9955">// False because the byte number 1 is not the character 'X'</span><span style="color:#0">
}</span></code></pre><p>
Multiple adjacent strings are compiled as one. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"this is "</span><span style="color:#0">   </span><span style="color:#BB6643">"a"</span><span style="color:#0">   </span><span style="color:#BB6643">" string"</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#BB6643">"this is a string"</span></code></pre><p>
You can concatenate some values if the values are known at compile time. Use the <code>++</code> operator for that. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"the devil number is "</span><span style="color:#0"> ++ </span><span style="color:#74A35B">666</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#BB6643">"the devil number is 666"</span><span style="color:#0">

</span><span style="color:#3186CD">const</span><span style="color:#0"> b = </span><span style="color:#74A35B">666</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> c = </span><span style="color:#BB6643">"the devil number is not "</span><span style="color:#0"> ++ (b + </span><span style="color:#74A35B">1</span><span style="color:#0">) ++ </span><span style="color:#BB6643">"!"</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#BB6643">"the devil number is not 667!"</span><span style="color:#0">)

</span><span style="color:#3186CD">let</span><span style="color:#0"> d = </span><span style="color:#BB6643">"they are "</span><span style="color:#0"> ++ </span><span style="color:#74A35B">4</span><span style="color:#0"> ++ </span><span style="color:#BB6643">" apples in "</span><span style="color:#0"> ++ (</span><span style="color:#74A35B">2</span><span style="color:#0">*</span><span style="color:#74A35B">2</span><span style="color:#0">) ++ </span><span style="color:#BB6643">" baskets"</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(d == </span><span style="color:#BB6643">"they are 4 apples in 4 baskets"</span><span style="color:#0">)</span></code></pre><p>
A string can be <code>null</code> if not defined. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#ED9A11">string</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#3186CD">null</span><span style="color:#0">)
a = </span><span style="color:#BB6643">"null"</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a != </span><span style="color:#3186CD">null</span><span style="color:#0">)
a = </span><span style="color:#3186CD">null</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#3186CD">null</span><span style="color:#0">)</span></code></pre><p>
<h3 id="Escape sequence">Escape sequence </h3></p>
<p>
A string and a rune can contain some <i>escape sequences</i> to specify special characters. </p>
<p>

  An escape sequence starts with <code>\</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"this is code ascii 0x00:   \0"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> b = </span><span style="color:#BB6643">"this is code ascii 0x07:   \a"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> c = </span><span style="color:#BB6643">"this is code ascii 0x08:   \b"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> d = </span><span style="color:#BB6643">"this is code ascii 0x09:   \t"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> e = </span><span style="color:#BB6643">"this is code ascii 0x0A:   \n"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> f = </span><span style="color:#BB6643">"this is code ascii 0x0B:   \v"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> g = </span><span style="color:#BB6643">"this is code ascii 0x0C:   \f"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> h = </span><span style="color:#BB6643">"this is code ascii 0x0D:   \r"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> i = </span><span style="color:#BB6643">"this is code ascii 0x22:   \""</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> j = </span><span style="color:#BB6643">"this is code ascii 0x27:   \'"</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> k = </span><span style="color:#BB6643">"this is code ascii 0x5C:   \\"</span><span style="color:#0">
}</span></code></pre><p>

  An escape sequence can also be used to specify an ascii or a unicode value. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"\x26"</span><span style="color:#0">        </span><span style="color:#6A9955">// 1 byte, hexadecimal, extended ascii</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> b = </span><span style="color:#BB6643">"\u2626"</span><span style="color:#0">      </span><span style="color:#6A9955">// 2 bytes, hexadecimal, unicode 16 bits</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> c = </span><span style="color:#BB6643">"\U26262626"</span><span style="color:#0">  </span><span style="color:#6A9955">// 4 bytes, hexadecimal, unicode 32 bits</span><span style="color:#0">

    </span><span style="color:#3186CD">const</span><span style="color:#0"> d = </span><span style="color:#BB6643">"\u2F46"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> d == </span><span style="color:#BB6643">"⽆"</span><span style="color:#0">
}</span></code></pre><p>
<h3 id="Raw string">Raw string </h3></p>
<p>
A <i>raw string</i> does not transform the escape sequences inside it. </p>
<p>

  A raw string starts and ends with the character <code>@</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#3186CD">@</span><span style="color:#BB6643">"\u2F46"</span><span style="color:#3186CD">@</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a != </span><span style="color:#BB6643">"⽆"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#3186CD">@</span><span style="color:#BB6643">"\u2F46"</span><span style="color:#3186CD">@</span><span style="color:#0">
}</span></code></pre><p>

  A raw string can be declared on several lines because <code>eol</code> is now part of the string. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#3186CD">@</span><span style="color:#BB6643">"this is</span><span style="color:#0">
</span><span style="color:#BB6643">                a</span><span style="color:#0">
</span><span style="color:#BB6643">                string</span><span style="color:#0">
</span><span style="color:#BB6643">                "</span><span style="color:#3186CD">@</span><span style="color:#0">

    </span><span style="color:#6A9955">// Every blanks before the ending mark `"@` will be removed from every other lines,</span><span style="color:#0">
    </span><span style="color:#6A9955">// so the string before is equivalent to :</span><span style="color:#0">

    </span><span style="color:#6A9955">// this is</span><span style="color:#0">
    </span><span style="color:#6A9955">// a</span><span style="color:#0">
    </span><span style="color:#6A9955">// string</span><span style="color:#0">
}</span></code></pre><p>
<h3 id="Multiline string">Multiline string </h3></p>
<p>
A multiline string starts and ends with <code>"""</code>. Unlike raw strings, they still evaluate escape sequences. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"""this is</span><span style="color:#0">
</span><span style="color:#BB6643">                 a</span><span style="color:#0">
</span><span style="color:#BB6643">                 string</span><span style="color:#0">
</span><span style="color:#BB6643">                 """</span><span style="color:#0">

    </span><span style="color:#6A9955">// Equivalent to :</span><span style="color:#0">

    </span><span style="color:#6A9955">// this is</span><span style="color:#0">
    </span><span style="color:#6A9955">// a</span><span style="color:#0">
    </span><span style="color:#6A9955">// string</span><span style="color:#0">
}</span></code></pre><p>

  In a multiline or a raw string, if you end a line with <code>\</code>, the following "eol" will <b>not</b> be part of the string. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#BB6643">"""\</span><span style="color:#0">
</span><span style="color:#BB6643">                 this is</span><span style="color:#0">
</span><span style="color:#BB6643">                 a</span><span style="color:#0">
</span><span style="color:#BB6643">                 string</span><span style="color:#0">
</span><span style="color:#BB6643">                 """</span><span style="color:#0">
    </span><span style="color:#6A9955">// Is equivalent to :</span><span style="color:#0">
    </span><span style="color:#6A9955">// this is</span><span style="color:#0">
    </span><span style="color:#6A9955">// a</span><span style="color:#0">
    </span><span style="color:#6A9955">// string</span><span style="color:#0">
}</span></code></pre><p>
<h3 id="Character">Character </h3></p>
<p>
A single <i>character</i> can be casted to every unsigned integer type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#BB6643">"0"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">48</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u8</span><span style="color:#0">))

    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#BB6643">"1"</span><span style="color:#0">'</span><span style="color:#ED9A11">u16</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">49</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u16</span><span style="color:#0">))

    </span><span style="color:#3186CD">let</span><span style="color:#0"> c = </span><span style="color:#BB6643">"2"</span><span style="color:#0">'</span><span style="color:#ED9A11">u32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">50</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(c) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u32</span><span style="color:#0">))

    </span><span style="color:#3186CD">let</span><span style="color:#0"> d = </span><span style="color:#BB6643">"3"</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(d == </span><span style="color:#74A35B">51</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(d) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u64</span><span style="color:#0">))

    </span><span style="color:#3186CD">let</span><span style="color:#0"> e = </span><span style="color:#BB6643">"4"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(e == </span><span style="color:#74A35B">52</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(e) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">rune</span><span style="color:#0">))
}</span></code></pre><p>
<h3 id="@stringof and @nameof">@stringof and @nameof </h3></p>
<p>
You can use the instrinsic <code>@stringof</code> to return at compile time the result of a constant expression as a string. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">X</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@stringof</span><span style="color:#0">(</span><span style="color:#3BC3A7">X</span><span style="color:#0">) == </span><span style="color:#BB6643">"1"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@stringof</span><span style="color:#0">(</span><span style="color:#3BC3A7">X</span><span style="color:#0"> + </span><span style="color:#74A35B">10</span><span style="color:#0">) == </span><span style="color:#BB6643">"11"</span><span style="color:#0">
}</span></code></pre><p>

  You can also use <code>@nameof</code> to return the name of a variable, function etc. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">X</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@nameof</span><span style="color:#0">(</span><span style="color:#3BC3A7">X</span><span style="color:#0">) == </span><span style="color:#BB6643">"X"</span><span style="color:#0">
}</span></code></pre>
<h2 id="013_variables">Variables</h2>
<p>
To declare a variable, use the <code>let</code> or <code>var</code> keyword, followed by <code>:</code> and then the type. </p>
<p>
<code>let</code> is used for a variable that cannot be changed after the initial affectation. </p>
<p>
<code>var</code> is used for a variable that can be changed again after the initial affectation. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// Variable 'a' is of type 'u32' and its value is '1'. It cannot be changed.</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> b: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Variable 'c' is declared with 'var', so it can be changed.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> c: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">42</span><span style="color:#0">
    c += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">43</span><span style="color:#0">)
}</span></code></pre><p>

  We have already seen that we can declare multiple variables on the same line. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a, b: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">123</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">123</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">123</span><span style="color:#0">)
}</span></code></pre><p>

  Or </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">12</span><span style="color:#0">, b: </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = </span><span style="color:#74A35B">1.5</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">12</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
}</span></code></pre><p>
If you don't assign a value, then the variable will be initialized with its default value (0). So a variable is <b>always</b> initialized. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#3186CD">false</span><span style="color:#0">)

</span><span style="color:#3186CD">var</span><span style="color:#0"> b: </span><span style="color:#ED9A11">string</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#3186CD">null</span><span style="color:#0">)

</span><span style="color:#3186CD">var</span><span style="color:#0"> c: </span><span style="color:#ED9A11">f64</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
But if you really do not want the variable to be initialized, you can assign it with <code>undefined</code>. To use with care, of course, but this is sometimes necessary to avoid the initialization cost. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> b: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span></code></pre><p>
We have seen that the type is optional in the declaration if it can be deduced from the assignment. </p>
<p>

  These are a bunch of <b>type inferences</b>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">1.5</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">

    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b) == </span><span style="color:#ED9A11">string</span><span style="color:#0">

    </span><span style="color:#3186CD">let</span><span style="color:#0"> c = </span><span style="color:#74A35B">1.5</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(c) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">
}</span></code></pre><p>

  The same goes for multiple variables. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a, b = </span><span style="color:#3186CD">true</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(b) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> c = </span><span style="color:#74A35B">1.5</span><span style="color:#0">, d = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#74A35B">1.5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(d == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(c) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(d) == </span><span style="color:#ED9A11">string</span><span style="color:#0">
}</span></code></pre>
<h2 id="014_const">Const</h2>
<p>
If you use <code>const</code> instead of <code>var</code>, the value must be known by the compiler. There's no memory footprint if the type is a value or a string. </p>
<pre><code><span style="color:#6A9955">// These are constants and not variables. So they cannot be changed after the declaration.</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#74A35B">666</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a == </span><span style="color:#74A35B">666</span><span style="color:#0">

</span><span style="color:#3186CD">const</span><span style="color:#0"> b: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> b == </span><span style="color:#BB6643">"string"</span></code></pre><p>
Constants can have more than just simple types. In that case, there's a memory footprint, because those constants are stored in the data segment. But that means also you could take the address of such constants at runtime. </p>
<p>

  This is our first static array. It contains '3' elements, and the type of the elements is <code>s32</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> a: [</span><span style="color:#74A35B">3</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]

    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = &a[</span><span style="color:#74A35B">0</span><span style="color:#0">] </span><span style="color:#6A9955">// Take the address of the first element</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)

    </span><span style="color:#6A9955">// But as this is a constant, we can also test the values at compile time.</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> a[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">
}</span></code></pre><p>

  An example of a multidimensional array as a constant. We will detail arrays later. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">M4x4</span><span style="color:#0">: [</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = [
        [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">],
        [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">],
        [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">],
        [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">]
    ]
}</span></code></pre><p>
So the difference between <code>let</code> and <code>const</code> is that the value of a constant should be known at compile time, where the value of a <code>let</code> can be dynamic. Other than that, both must be assigned once and only once. </p>

<h2 id="015_operators">Operators</h2>
<p>
These are all the Swag operators that can be used to manipulate variables and values. </p>
<p>
<h3 id="Arithmetic operators">Arithmetic operators </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">

</span><span style="color:#6A9955">// Addition</span><span style="color:#0">
x = x + </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// Substraction</span><span style="color:#0">
x = x - </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// Multiplication</span><span style="color:#0">
x = x * </span><span style="color:#74A35B">2</span><span style="color:#0">

</span><span style="color:#6A9955">// Division</span><span style="color:#0">
x = x / </span><span style="color:#74A35B">2</span><span style="color:#0">

</span><span style="color:#6A9955">// Modulus</span><span style="color:#0">
x = x % </span><span style="color:#74A35B">2</span></code></pre><p>
<h3 id="Bitwise operators">Bitwise operators </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">

</span><span style="color:#6A9955">// Xor</span><span style="color:#0">
x = x ^ </span><span style="color:#74A35B">2</span><span style="color:#0">

</span><span style="color:#6A9955">// Bitmask and</span><span style="color:#0">
x = x & </span><span style="color:#74A35B">0b0000_0001</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">

</span><span style="color:#6A9955">// Bitmask or</span><span style="color:#0">
x = x | </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#74A35B">0b0000_0001</span><span style="color:#0">

</span><span style="color:#6A9955">// Shift bits left</span><span style="color:#0">
x = x << </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// Shift bits right</span><span style="color:#0">
x = x >> </span><span style="color:#74A35B">1</span></code></pre><p>
<h3 id="Assignment operators">Assignment operators </h3></p>
<p>
All the arithmetic and bitwise operators have an <code>affect</code> version. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">

x += </span><span style="color:#74A35B">1</span><span style="color:#0">
x -= </span><span style="color:#74A35B">1</span><span style="color:#0">
x *= </span><span style="color:#74A35B">2</span><span style="color:#0">
x /= </span><span style="color:#74A35B">2</span><span style="color:#0">
x %= </span><span style="color:#74A35B">2</span><span style="color:#0">
x ^= </span><span style="color:#74A35B">2</span><span style="color:#0">
x |= </span><span style="color:#74A35B">0b0000_0001</span><span style="color:#0">
x &= </span><span style="color:#74A35B">0b0000_0001</span><span style="color:#0">
x <<= </span><span style="color:#74A35B">1</span><span style="color:#0">
x >>= </span><span style="color:#74A35B">1</span></code></pre><p>
<h3 id="Unary operators">Unary operators </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#3186CD">true</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> y = </span><span style="color:#74A35B">0b0000_0001</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> z = </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// Invert boolean</span><span style="color:#0">
x = !x

</span><span style="color:#6A9955">// Invert bits</span><span style="color:#0">
y = ~y

</span><span style="color:#6A9955">// Negative</span><span style="color:#0">
z = -z

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(z == -</span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#3186CD">false</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">0b1111_1110</span><span style="color:#0">)</span></code></pre><p>
<h3 id="Comparison operators">Comparison operators </h3></p>
<p>
</p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#3186CD">false</span><span style="color:#0">

    </span><span style="color:#6A9955">// Equal</span><span style="color:#0">
    a = </span><span style="color:#74A35B">1</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0"> ? </span><span style="color:#3186CD">true</span><span style="color:#0"> : </span><span style="color:#3186CD">false</span><span style="color:#0">

    </span><span style="color:#6A9955">// Not equal</span><span style="color:#0">
    a = </span><span style="color:#74A35B">1</span><span style="color:#0"> != </span><span style="color:#74A35B">1</span><span style="color:#0"> ? </span><span style="color:#3186CD">true</span><span style="color:#0"> : </span><span style="color:#3186CD">false</span><span style="color:#0">

    </span><span style="color:#6A9955">// Lower equal</span><span style="color:#0">
    a = </span><span style="color:#74A35B">1</span><span style="color:#0"> <= </span><span style="color:#74A35B">1</span><span style="color:#0"> ? </span><span style="color:#3186CD">true</span><span style="color:#0"> : </span><span style="color:#3186CD">false</span><span style="color:#0">

    </span><span style="color:#6A9955">// Greater equal</span><span style="color:#0">
    a = </span><span style="color:#74A35B">1</span><span style="color:#0"> >= </span><span style="color:#74A35B">1</span><span style="color:#0"> ? </span><span style="color:#3186CD">true</span><span style="color:#0"> : </span><span style="color:#3186CD">false</span><span style="color:#0">

    </span><span style="color:#6A9955">// Lower</span><span style="color:#0">
    a = </span><span style="color:#74A35B">1</span><span style="color:#0"> < </span><span style="color:#74A35B">1</span><span style="color:#0"> ? </span><span style="color:#3186CD">true</span><span style="color:#0"> : </span><span style="color:#3186CD">false</span><span style="color:#0">

    </span><span style="color:#6A9955">// Greater</span><span style="color:#0">
    a = </span><span style="color:#74A35B">1</span><span style="color:#0"> > </span><span style="color:#74A35B">1</span><span style="color:#0"> ? </span><span style="color:#3186CD">true</span><span style="color:#0"> : </span><span style="color:#3186CD">false</span><span style="color:#0">
}

{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">5</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> y = </span><span style="color:#74A35B">10</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x != </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x <= </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x < </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x >= </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x > </span><span style="color:#74A35B">0</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="Logical operators">Logical operators </h3></p>
<p>
This is <b>not</b> <code>&&</code> and <code>||</code> like in C/C++, but <code>and</code> and <code>or</code>. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#3186CD">false</span><span style="color:#0">
a = (</span><span style="color:#74A35B">1</span><span style="color:#0"> > </span><span style="color:#74A35B">10</span><span style="color:#0">) </span><span style="color:#B040BE">and</span><span style="color:#0"> (</span><span style="color:#74A35B">10</span><span style="color:#0"> < </span><span style="color:#74A35B">1</span><span style="color:#0">)
a = (</span><span style="color:#74A35B">1</span><span style="color:#0"> > </span><span style="color:#74A35B">10</span><span style="color:#0">) </span><span style="color:#B040BE">or</span><span style="color:#0"> (</span><span style="color:#74A35B">10</span><span style="color:#0"> < </span><span style="color:#74A35B">1</span><span style="color:#0">)</span></code></pre><p>
<h3 id="Ternary operator">Ternary operator </h3></p>
<p>
The ternary operator will test an expression, and will return a value depending on the result of the test. <code>A = Expression ? B : C</code> will return <code>B</code> if the expression is true, and will return <code>C</code> if the expression is false. </p>
<pre><code><span style="color:#6A9955">// Returns 1 because the expression 'true' is... true.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#3186CD">true</span><span style="color:#0"> ? </span><span style="color:#74A35B">1</span><span style="color:#0"> : </span><span style="color:#74A35B">666</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">1</span><span style="color:#0">)

</span><span style="color:#6A9955">// Returns 666 because the expression 'x == 52' is false.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y = (x == </span><span style="color:#74A35B">52</span><span style="color:#0">) ? </span><span style="color:#74A35B">1</span><span style="color:#0"> : </span><span style="color:#74A35B">666</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">666</span><span style="color:#0">)</span></code></pre><p>
<h3 id="Spaceshift operator">Spaceshift operator </h3></p>
<p>
Operator <code><=></code> will return -1, 0 or 1 if the expression on the left is <b>lower</b>, <b>equal</b> or <b>greater</b> than the expression on the right. The returned type is <code>s32</code>. </p>
<pre><code><span style="color:#3BC3A7">A</span><span style="color:#0"> <=> </span><span style="color:#3BC3A7">B</span><span style="color:#0"> == -</span><span style="color:#74A35B">1</span><span style="color:#0"> </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0"> < </span><span style="color:#3BC3A7">B</span><span style="color:#0">
</span><span style="color:#3BC3A7">A</span><span style="color:#0"> <=> </span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">  </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#3BC3A7">B</span><span style="color:#0">
</span><span style="color:#3BC3A7">A</span><span style="color:#0"> <=> </span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">  </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0"> > </span><span style="color:#3BC3A7">B</span></code></pre><p>
</p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = -</span><span style="color:#74A35B">1.5</span><span style="color:#0"> <=> </span><span style="color:#74A35B">2.31</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == -</span><span style="color:#74A35B">1</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(-</span><span style="color:#74A35B">10</span><span style="color:#0"> <=>  </span><span style="color:#74A35B">10</span><span style="color:#0"> == -</span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">( </span><span style="color:#74A35B">10</span><span style="color:#0"> <=> -</span><span style="color:#74A35B">10</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">( </span><span style="color:#74A35B">10</span><span style="color:#0"> <=>  </span><span style="color:#74A35B">10</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">)
}

{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x1 = </span><span style="color:#74A35B">10</span><span style="color:#0"> <=> </span><span style="color:#74A35B">20</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x1 == -</span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x2 = </span><span style="color:#74A35B">20</span><span style="color:#0"> <=> </span><span style="color:#74A35B">10</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x2 == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x3 = </span><span style="color:#74A35B">20</span><span style="color:#0"> <=> </span><span style="color:#74A35B">20</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x3 == </span><span style="color:#74A35B">0</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="Null-coalescing operator">Null-coalescing operator </h3></p>
<p>
The operator <code>orelse</code> will return the left expression if it is not zero, otherwise it will return the right expression. </p>
<p>

  Works with strings, pointers and structures with the <code>opData</code> special function (we'll see that later). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#BB6643">"string1"</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#BB6643">"string2"</span><span style="color:#0">

    </span><span style="color:#6A9955">// c = a if a is not null, else c = b.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> c = a </span><span style="color:#B040BE">orelse</span><span style="color:#0"> b
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#BB6643">"string1"</span><span style="color:#0">)

    a = </span><span style="color:#3186CD">null</span><span style="color:#0">
    c = a </span><span style="color:#B040BE">orelse</span><span style="color:#0"> b
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == </span><span style="color:#BB6643">"string2"</span><span style="color:#0">)
}</span></code></pre><p>

  Works also for basic types like integers. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> c = a </span><span style="color:#B040BE">orelse</span><span style="color:#0"> b
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c == b)
}</span></code></pre><p>
<h3 id="Type promotion">Type promotion </h3></p>
<p>
Unlike C, types are not promoted to 32 bits when dealing with 8 or 16 bits types. But types will be promoted if the two sides of an operation do not have the same type. </p>
<pre><code><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)  == </span><span style="color:#ED9A11">u8</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u16</span><span style="color:#0">) == </span><span style="color:#ED9A11">u16</span><span style="color:#0">    </span><span style="color:#6A9955">// Priority to bigger type</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u32</span><span style="color:#0">) == </span><span style="color:#ED9A11">u32</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">) == </span><span style="color:#ED9A11">u64</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s8</span><span style="color:#0">)  == </span><span style="color:#ED9A11">s8</span><span style="color:#0">     </span><span style="color:#6A9955">// Priority to signed type</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s16</span><span style="color:#0">) == </span><span style="color:#ED9A11">s16</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s64</span><span style="color:#0">) == </span><span style="color:#ED9A11">s64</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">s8</span><span style="color:#0"> + </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u16</span><span style="color:#0">) == </span><span style="color:#ED9A11">u16</span><span style="color:#0">   </span><span style="color:#6A9955">// Priority to bigger type also</span></code></pre><p>
This means that a 8/16 bits operation (like an addition) can more easily <b>overflow</b> if you do not take care. In that case, you can use the <code>,up</code> <b>modifier</b> on the operation, which will promote the type to at least 32 bits. The operation will be done accordingly. </p>
<pre><code><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> +,up </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">) == </span><span style="color:#ED9A11">u32</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> +,up </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> == </span><span style="color:#74A35B">256</span><span style="color:#0"> </span><span style="color:#6A9955">// No overflow, because the operation is done in 32 bits.</span></code></pre><p>
We'll see later how Swag deals with that kind of overflow, and more generally, with <b>safety</b>. </p>
<p>
<h3 id="Operator precedence">Operator precedence </h3></p>
<p>
</p>
<pre><code><span style="color:#0">~
* / %
+ -
>> <<
&
|
^
<=>
== !=
< <= > >=</span></code></pre><p>
</p>

<h2 id="016_cast">Cast</h2>
<p>
<h3 id="Explicit cast">Explicit cast </h3></p>
<p>
Sometimes it can be necessary to change the type of a value. Use <code>cast(type)</code> to cast from one type to another. </p>
<pre><code><span style="color:#6A9955">// 'x' is a float</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">1.0</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">

</span><span style="color:#6A9955">// 'y' is a 32 bits integer</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) x
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(y) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">1</span><span style="color:#0">)</span></code></pre><p>
<h3 id="acast">acast </h3></p>
<p>
<code>acast</code> stands for <code>automatic</code> cast. Use it to let the compiler automatically cast to the expression on the left. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = </span><span style="color:#74A35B">1.0</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#3186CD">acast</span><span style="color:#0"> x    </span><span style="color:#6A9955">// cast 'x' to 's32'</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(y) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">1</span><span style="color:#0">)</span></code></pre><p>
<h3 id="bitcast">bitcast </h3></p>
<p>
Use <code>cast,bit</code> to convert a native type to another without converting the value. Works only if the two types are of the same size. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = </span><span style="color:#74A35B">1.0</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#3186CD">cast</span><span style="color:#0">,</span><span style="color:#FF6A00">bit</span><span style="color:#0">(</span><span style="color:#ED9A11">u32</span><span style="color:#0">) x
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">0x3f800000</span><span style="color:#0">)
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3186CD">cast</span><span style="color:#0">,</span><span style="color:#FF6A00">bit</span><span style="color:#0">(</span><span style="color:#ED9A11">u32</span><span style="color:#0">) </span><span style="color:#74A35B">1.0</span><span style="color:#0"> == </span><span style="color:#74A35B">0x3f800000</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3186CD">cast</span><span style="color:#0">,</span><span style="color:#FF6A00">bit</span><span style="color:#0">(</span><span style="color:#ED9A11">f32</span><span style="color:#0">) </span><span style="color:#74A35B">0x3f800000</span><span style="color:#0"> == </span><span style="color:#74A35B">1.0</span></code></pre><p>
<h3 id="Implicit casts">Implicit casts </h3></p>
<p>
Swag can sometimes cast from one type to another for you. This is an <i>implicit</i> cast. </p>
<pre><code><span style="color:#6A9955">// An implicit cast is done if there's no loss of precision. In that case, you can affect different types.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#ED9A11">s16</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s8</span><span style="color:#0">   </span><span style="color:#6A9955">// 8 bits to 16 bits, fine</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s16</span><span style="color:#0">  </span><span style="color:#6A9955">// 16 bits to 32 bits, fine</span><span style="color:#0">

</span><span style="color:#6A9955">// But the following would generate an error as you cannot affect 's16' to 's8'</span><span style="color:#0">
</span><span style="color:#6A9955">// without an explicit cast(s16).</span><span style="color:#0">

</span><span style="color:#6A9955">/*</span><span style="color:#0">
</span><span style="color:#6A9955">var z0: s16 = 1</span><span style="color:#0">
</span><span style="color:#6A9955">var z1: s8 = z0</span><span style="color:#0">
</span><span style="color:#6A9955">*/</span></code></pre>
<h2 id="020_array">Array</h2>
<p>
Remember that dynamic arrays are part of the <code>Std.Core</code> module. Here we are only talking about native static arrays. </p>
<p>

  A static array is declared with <code>[N]</code> followed by the type, where <code>N</code> is the dimension. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">  </span><span style="color:#6A9955">// Static array of two s32</span><span style="color:#0">
    array[</span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">1</span><span style="color:#0">
    array[</span><span style="color:#74A35B">1</span><span style="color:#0">] = </span><span style="color:#74A35B">2</span><span style="color:#0">
}</span></code></pre><p>

  You can get the number of elements of an array with <code>@countof</code>, and the size in bytes with <code>@sizeof</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(array) == </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(array) == </span><span style="color:#74A35B">2</span><span style="color:#0"> * </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">)
}</span></code></pre><p>

  You can get the address of the array with <code>@dataof</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr0 = </span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(array)
    ptr0[</span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">1</span><span style="color:#0">

    </span><span style="color:#6A9955">// This is equivalent of taking the address of the first element</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr1 = &array[</span><span style="color:#74A35B">0</span><span style="color:#0">]
    ptr1[</span><span style="color:#74A35B">1</span><span style="color:#0">] = </span><span style="color:#74A35B">2</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  An <b>array literal</b> has the form <code>[A, B, ...]</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#6A9955">// An array of four s32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(arr) == </span><span style="color:#74A35B">4</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(arr) == </span><span style="color:#C3973B">#type</span><span style="color:#0"> [</span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>

  The size of the array can be deduced from the initialisation expression. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// Here the dimension is not specified, but as the array is initialized with 2 elements, it can be deduced.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(array) == </span><span style="color:#74A35B">2</span><span style="color:#0">

    </span><span style="color:#6A9955">// Here both dimensions and types are deduced thanks to the initialization expression.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array1 = [</span><span style="color:#BB6643">"10"</span><span style="color:#0">, </span><span style="color:#BB6643">"20"</span><span style="color:#0">, </span><span style="color:#BB6643">"30"</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array1[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#BB6643">"10"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array1[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#BB6643">"20"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array1[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#BB6643">"30"</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(array1) == </span><span style="color:#74A35B">3</span><span style="color:#0">
}</span></code></pre><p>

  Like every other types, an array is initialized by default to 0 </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">)
}</span></code></pre><p>

  But for speed, you can force the array to be not initialized with <code>undefined</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">100</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span><span style="color:#0">
}</span></code></pre><p>

  A static array (with compile time values) can be stored as a constant. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> array = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> array[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">   </span><span style="color:#6A9955">// Dereference is done at compile time</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> array[</span><span style="color:#74A35B">3</span><span style="color:#0">] == </span><span style="color:#74A35B">4</span><span style="color:#0">
}</span></code></pre><p>

  If the type of the array is not specified, the type of the <b>first</b> literal value will be used for all other members. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]    </span><span style="color:#6A9955">// Every values are forced to be 'f64'</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(arr) == </span><span style="color:#74A35B">4</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(arr) == </span><span style="color:#C3973B">#type</span><span style="color:#0"> [</span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#ED9A11">f64</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr[</span><span style="color:#74A35B">3</span><span style="color:#0">] == </span><span style="color:#74A35B">4.0</span><span style="color:#0">)
}</span></code></pre><p>
Of course an array can have multiple dimensions. </p>
<p>

  Syntax is <code>[X, Y, Z...]</code> where <code>X</code>, <code>Y</code> and <code>Z</code> are dimensions. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">   </span><span style="color:#6A9955">// Declare a 2x2 array</span><span style="color:#0">
    array[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">1</span><span style="color:#0">         </span><span style="color:#6A9955">// To access it, the same syntax is used</span><span style="color:#0">
    array[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] = </span><span style="color:#74A35B">2</span><span style="color:#0">
    array[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">3</span><span style="color:#0">
    array[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] = </span><span style="color:#74A35B">4</span><span style="color:#0">
}</span></code></pre><p>

  But the C/C++ syntax is also accepted. You will then declare an array of array instead of an array with multiple dimensions, which in fact is the same... </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">][</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    array[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">1</span><span style="color:#0">
    array[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] = </span><span style="color:#74A35B">2</span><span style="color:#0">
    array[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">3</span><span style="color:#0">
    array[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] = </span><span style="color:#74A35B">4</span><span style="color:#0">
}</span></code></pre><p>

  The sizes can be deduced from the initialization expression too. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array1 = [[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">], [</span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]]

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(array) == </span><span style="color:#74A35B">4</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(array1) == </span><span style="color:#74A35B">2</span><span style="color:#0">
}</span></code></pre><p>
You can initialize a whole array variable (but <b>not a constant</b>) with one single value. Only basic types are accepted (integers, float, string, bool, rune). </p>
<pre><code><span style="color:#6A9955">// The whole array is initialized with 'true'</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> arr: [</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">bool</span><span style="color:#0"> = </span><span style="color:#3186CD">true</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#3186CD">true</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#3186CD">true</span><span style="color:#0">)

</span><span style="color:#6A9955">// The whole array is initialized with 'string'</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> arr1: [</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">10</span><span style="color:#0">] </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr1[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr1[</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#74A35B">9</span><span style="color:#0">] == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)</span></code></pre>
<h2 id="021_slice">Slice</h2>
<p>
A slice is a pointer on a buffer of datas, and a <code>u64</code> to count the number of elements. Unlike a static array, its value can be changed at runtime. </p>
<p>

  It is declared with <code>[..]</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a: [..] </span><span style="color:#ED9A11">bool</span><span style="color:#0">    </span><span style="color:#6A9955">// Slice of bools</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(a) == </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(*</span><span style="color:#ED9A11">void</span><span style="color:#0">) + </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#ED9A11">u64</span><span style="color:#0">)
}</span></code></pre><p>

  You can initialize a slice like an array. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">, </span><span style="color:#74A35B">50</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a[</span><span style="color:#74A35B">4</span><span style="color:#0">] == </span><span style="color:#74A35B">50</span><span style="color:#0">)

    </span><span style="color:#6A9955">// But as this is a slice and not a static array, you can change the value at runtime.</span><span style="color:#0">
    </span><span style="color:#6A9955">// So now 'a' is a slice of two s32 values.</span><span style="color:#0">
    a = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  At runtime, <code>@dataof</code> will return the address of the values, <code>@countof</code> the number of elements. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">, </span><span style="color:#74A35B">50</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> count = </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(a)
    </span><span style="color:#3186CD">let</span><span style="color:#0"> addr = </span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(a)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(count == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(addr[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(addr[</span><span style="color:#74A35B">4</span><span style="color:#0">] == </span><span style="color:#74A35B">50</span><span style="color:#0">)

    a = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(a) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  You can create a slice with your own <code>pointer</code> and <code>count</code> using <code>@mkslice</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]

    </span><span style="color:#6A9955">// Creates a slice of 'array' (a view, or subpart) starting at index 2, with 2 elements.</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice: [..] </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#B4B44A">@mkslice</span><span style="color:#0">(&array[</span><span style="color:#74A35B">0</span><span style="color:#0">] + </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(slice) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">40</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    slice[</span><span style="color:#74A35B">0</span><span style="color:#0">] = </span><span style="color:#74A35B">314</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">314</span><span style="color:#0">)
}</span></code></pre><p>

  For a string, the slice must be <code>const</code> because a string is immutable. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> str = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> strSlice: </span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">u8</span><span style="color:#0"> = </span><span style="color:#B4B44A">@mkslice</span><span style="color:#0">(</span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(str), </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(strSlice[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#BB6643">"s"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(strSlice[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#BB6643">"t"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="The slicing operator">The slicing operator </h3></p>
<p>
</p>
<p>

  Instead of <code>@mkslice</code>, you can slice something with the <code>..</code> operator. For example you can slice a string. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> str = </span><span style="color:#BB6643">"string"</span><span style="color:#0">

    </span><span style="color:#6A9955">// Creates a slice starting at byte 0 and ending at byte 3.</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice = str[</span><span style="color:#74A35B">1</span><span style="color:#0">..</span><span style="color:#74A35B">3</span><span style="color:#0">]

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice == </span><span style="color:#BB6643">"tri"</span><span style="color:#0">)
}</span></code></pre><p>

  The upper limit is <b>included</b> by default. If you want to exclude it, use <code>..<</code> insteand of <code>..</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> str = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice = str[</span><span style="color:#74A35B">1</span><span style="color:#0">..<</span><span style="color:#74A35B">3</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice == </span><span style="color:#BB6643">"tr"</span><span style="color:#0">)
}</span></code></pre><p>

  You can omit the upper bound if you want to slice to the end. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> str = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice = str[</span><span style="color:#74A35B">2</span><span style="color:#0">..]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice == </span><span style="color:#BB6643">"ring"</span><span style="color:#0">)
}</span></code></pre><p>

  You can omit the lower bound if you want to slice from the start (0). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> str = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice = str[..</span><span style="color:#74A35B">2</span><span style="color:#0">]        </span><span style="color:#6A9955">// Index 2 is included</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice == </span><span style="color:#BB6643">"str"</span><span style="color:#0">)
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice1 = str[..<</span><span style="color:#74A35B">2</span><span style="color:#0">]      </span><span style="color:#6A9955">// Index 2 is excluded</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice1 == </span><span style="color:#BB6643">"st"</span><span style="color:#0">)
}</span></code></pre><p>

  You can also slice an array. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice = arr[</span><span style="color:#74A35B">2</span><span style="color:#0">..</span><span style="color:#74A35B">3</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">40</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(slice) == </span><span style="color:#74A35B">2</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Creates a slice for the whole array</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice1 = arr[..]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(slice1) == </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(arr))
}</span></code></pre><p>

  You can slice another slice. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice1 = arr[</span><span style="color:#74A35B">1</span><span style="color:#0">..</span><span style="color:#74A35B">3</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice1[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">20</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice1[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice1[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">40</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice2 = slice1[</span><span style="color:#74A35B">1</span><span style="color:#0">..</span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice2[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice2[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">40</span><span style="color:#0">)
}</span></code></pre><p>

  You can transform a pointer to a slice. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = &arr[</span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> slice = ptr[</span><span style="color:#74A35B">0</span><span style="color:#0">..</span><span style="color:#74A35B">1</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(slice[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">40</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(slice) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre>
<h2 id="022_pointers">Pointers</h2>
<p>
<h3 id="Single value pointers">Single value pointers </h3></p>
<p>
</p>
<p>

  A pointer to a <b>single element</b> is declared with <code>*</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr1: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">   </span><span style="color:#6A9955">// This is a pointer to one single 'u8'</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr2: **</span><span style="color:#ED9A11">u8</span><span style="color:#0">  </span><span style="color:#6A9955">// This is a pointer to one other pointer to one single 'u8'</span><span style="color:#0">
}</span></code></pre><p>

  A pointer can be <code>null</code> (i know some of you will collapse here). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr1: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr1 == </span><span style="color:#3186CD">null</span><span style="color:#0">)
}</span></code></pre><p>

  You can take the address of something with <code>&</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = &arr </span><span style="color:#6A9955">// Take the address of the variable 'arr'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(ptr) == *</span><span style="color:#ED9A11">s32</span><span style="color:#0">)
}</span></code></pre><p>

  You can get the pointed value with <code>dref</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = </span><span style="color:#74A35B">42</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = &arr
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">dref</span><span style="color:#0"> ptr == </span><span style="color:#74A35B">42</span><span style="color:#0">)
}</span></code></pre><p>

  Pointers can be <code>const</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> str = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">u8</span><span style="color:#0"> = </span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(str)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">dref</span><span style="color:#0"> ptr == </span><span style="color:#BB6643">"s"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
}</span></code></pre><p>

  You can be weird, but is this necessary ? </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr:  *</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">u8</span><span style="color:#0">        </span><span style="color:#6A9955">// Normal pointer to a const pointer</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr1: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">u8</span><span style="color:#0">  </span><span style="color:#6A9955">// Const pointer to a const pointer</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr2: </span><span style="color:#3186CD">const</span><span style="color:#0"> **</span><span style="color:#ED9A11">u8</span><span style="color:#0">        </span><span style="color:#6A9955">// Const pointer to a normal pointer</span><span style="color:#0">
}</span></code></pre><p>
<h3 id="Multiple values pointers">Multiple values pointers </h3></p>
<p>
If you want to enable <b>pointer arithmetic</b>, and make a pointer to <b>multiple values</b>, declare your pointer with <code>^</code> instead of <code>*</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// `ptr` is a pointer to a memory block of `u8`.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr: ^</span><span style="color:#ED9A11">u8</span><span style="color:#0">

    </span><span style="color:#6A9955">// Pointer arithmetic is now possible</span><span style="color:#0">
    ptr = ptr - </span><span style="color:#74A35B">1</span><span style="color:#0">
}</span></code></pre><p>

  Taking the address of an array element enables pointer arithmetic. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: [</span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> ptr = &x[</span><span style="color:#74A35B">1</span><span style="color:#0">]
    ptr = ptr - </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(ptr) == ^</span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>

  As pointer arithmetic is enabled, you can dereference that kind of pointer by index. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = &arr[</span><span style="color:#74A35B">0</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(ptr) == ^</span><span style="color:#ED9A11">s32</span><span style="color:#0">)

    </span><span style="color:#6A9955">// The type of 'ptr' is ^s32, so it can be dereferenced by index</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value1 = ptr[</span><span style="color:#74A35B">0</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value1 == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(value1) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">

    </span><span style="color:#3186CD">let</span><span style="color:#0"> value2 = ptr[</span><span style="color:#74A35B">1</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value2 == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(value2) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">

    </span><span style="color:#6A9955">// But 'dref' still works for the first element</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#3186CD">dref</span><span style="color:#0"> ptr
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(value) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre>
<h2 id="023_references">References</h2>
<p>
Swag has also <b>references</b>, which are pointers that behave like values. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">42</span><span style="color:#0">

</span><span style="color:#6A9955">// Use 'ref' to declare a reference.</span><span style="color:#0">
</span><span style="color:#6A9955">// Here we declare a reference to the variable 'x'.</span><span style="color:#0">
</span><span style="color:#6A9955">// Note that unlike C++, you have to take the address of 'x' to convert it to a reference.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> myRef: </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3186CD">ref</span><span style="color:#0"> </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = &x

</span><span style="color:#6A9955">// This is a pointer that behaves like a value, so no explicit dereferencing is necessary.</span><span style="color:#0">
</span><span style="color:#6A9955">// You can see this as a kind of an alias.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myRef == </span><span style="color:#74A35B">42</span><span style="color:#0">)</span></code></pre><p>
When an affectation is done outside of an initialization, you will change the pointed value, and not the reference itself. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">42</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> myRef: </span><span style="color:#3186CD">ref</span><span style="color:#0"> </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = &x </span><span style="color:#6A9955">// Note here that the reference is no more 'const'</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myRef == </span><span style="color:#74A35B">42</span><span style="color:#0">)

</span><span style="color:#6A9955">// Here we will change the pointed value 'x'</span><span style="color:#0">
myRef = </span><span style="color:#74A35B">66</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myRef == </span><span style="color:#74A35B">66</span><span style="color:#0">)

</span><span style="color:#6A9955">// Remember that 'myRef' is an alias for 'x', so 'x' has also been changed.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">66</span><span style="color:#0">)</span></code></pre><p>
But unlike C++, you can change the reference (reassign it) and not the pointed value if you want. You must then use <code>ref</code> in the affectation. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">1</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> y = </span><span style="color:#74A35B">1000</span><span style="color:#0">

</span><span style="color:#3186CD">var</span><span style="color:#0"> myRef: </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3186CD">ref</span><span style="color:#0"> </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = &x
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myRef == </span><span style="color:#74A35B">1</span><span style="color:#0">)

</span><span style="color:#6A9955">// Here we force 'myRef' to point to 'y' and not to 'x' anymore.</span><span style="color:#0">
</span><span style="color:#6A9955">// We do *NOT* change the value of 'x'.</span><span style="color:#0">
myRef = </span><span style="color:#3186CD">ref</span><span style="color:#0"> &y
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myRef == </span><span style="color:#74A35B">1000</span><span style="color:#0">)</span></code></pre><p>
Most of the time, you have to take the address of a variable to make a reference to it. The only exception are function parameters, if the reference is <code>const</code>. In that case, taking the address is not necessary </p>
<pre><code><span style="color:#6A9955">// We can pass a literal because the parameter 'x' of 'toto' is 'const ref' and not just 'ref'.</span><span style="color:#0">
</span><span style="color:#FF6A00">toto</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">)</span></code></pre><pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">(x: </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3186CD">ref</span><span style="color:#0"> </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Under the hood, you will get a const address to an 's32'</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = &x
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">dref</span><span style="color:#0"> ptr == </span><span style="color:#74A35B">4</span><span style="color:#0">)
}</span></code></pre><p>
This is usefull for structs for examples, as you can directly pass a literal to a function. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0"> {x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> }  </span><span style="color:#6A9955">// Our first little struct !</span></code></pre><pre><code><span style="color:#FF6A00">titi0</span><span style="color:#0">({</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">})
</span><span style="color:#FF6A00">titi1</span><span style="color:#0">({</span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">})
</span><span style="color:#FF6A00">titi2</span><span style="color:#0">({</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">6</span><span style="color:#0">})</span></code></pre><pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">titi0</span><span style="color:#0">(param: </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3186CD">ref</span><span style="color:#0"> {x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">})
{
    </span><span style="color:#6A9955">// We'll see later about tuples and naming of fields.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(param.item0 == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(param.item1 == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>
Note that declaring a tuple type or a struct type is equivalent to a constant reference. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">titi1</span><span style="color:#0">(param: {x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">})
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(param.x == </span><span style="color:#74A35B">3</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(param.y == </span><span style="color:#74A35B">4</span><span style="color:#0">)
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">titi2</span><span style="color:#0">(param: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">)
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(param.x == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(param.y == </span><span style="color:#74A35B">6</span><span style="color:#0">)
}</span></code></pre>
<h2 id="024_any">Any</h2>
<p>
<code>any</code> is a specific type that can store every other types. <code>any</code> is <b>not a variant</b>. It's a dynamic typed reference to an existing value. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#ED9A11">any</span><span style="color:#0">

    </span><span style="color:#6A9955">// Store a 's32' literal value</span><span style="color:#0">
    a = </span><span style="color:#74A35B">6</span><span style="color:#0">

    </span><span style="color:#6A9955">// In order to reference the value inside the 'any', you need to cast to the wanted type.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) a == </span><span style="color:#74A35B">6</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Then now we store a string instead of the 's32' value</span><span style="color:#0">
    a = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">string</span><span style="color:#0">) a == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Then we store a bool instead</span><span style="color:#0">
    a = </span><span style="color:#3186CD">true</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">bool</span><span style="color:#0">) a == </span><span style="color:#3186CD">true</span><span style="color:#0">)
}</span></code></pre><p>

  <code>any</code> is in fact a pointer to the value, and a <code>typeinfo</code>. <code>@dataof</code> can be used to retrieve the pointer to the value. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a: </span><span style="color:#ED9A11">any</span><span style="color:#0"> = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(a)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">dref</span><span style="color:#0"> ptr == </span><span style="color:#74A35B">6</span><span style="color:#0">)
}</span></code></pre><p>

  <code>@typeof</code> will give you the type <code>any</code>, but <code>@kindof</code> will give you the real underlying type. In that case, <code>@kindof</code> is evaluted at runtime. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#ED9A11">any</span><span style="color:#0"> = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">any</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">string</span><span style="color:#0">)

    a = </span><span style="color:#3186CD">true</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(a) == </span><span style="color:#ED9A11">bool</span><span style="color:#0">)
}</span></code></pre><p>

  You can declare an array with multiple types, with <code>any</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [] </span><span style="color:#ED9A11">any</span><span style="color:#0"> = [</span><span style="color:#3186CD">true</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3.0</span><span style="color:#0">, </span><span style="color:#BB6643">"4"</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(array[</span><span style="color:#74A35B">0</span><span style="color:#0">]) == </span><span style="color:#ED9A11">bool</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(array[</span><span style="color:#74A35B">1</span><span style="color:#0">]) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(array[</span><span style="color:#74A35B">2</span><span style="color:#0">]) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(array[</span><span style="color:#74A35B">3</span><span style="color:#0">]) == </span><span style="color:#ED9A11">string</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">bool</span><span style="color:#0">) array[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) array[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">f32</span><span style="color:#0">) array[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">3.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">string</span><span style="color:#0">) array[</span><span style="color:#74A35B">3</span><span style="color:#0">] == </span><span style="color:#BB6643">"4"</span><span style="color:#0">)
}</span></code></pre><p>

  An <code>any</code> can be set to null, and tested against null. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#ED9A11">any</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#3186CD">null</span><span style="color:#0">)
    x = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x != </span><span style="color:#3186CD">null</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) x == </span><span style="color:#74A35B">6</span><span style="color:#0">)
    x = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x != </span><span style="color:#3186CD">null</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">string</span><span style="color:#0">) x == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)
    x = </span><span style="color:#3186CD">null</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#3186CD">null</span><span style="color:#0">)
}</span></code></pre>
<h2 id="025_tuple">Tuple</h2>
<p>

  A tuple is an anonymous structure, aka a struct literal. Syntax is <code>{}</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> tuple1 = {</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}
    </span><span style="color:#3186CD">let</span><span style="color:#0"> tuple2 = {</span><span style="color:#BB6643">"string"</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#3186CD">true</span><span style="color:#0">}
}</span></code></pre><p>

  Tuple values have default names to access them, in the form of <code>itemX</code> where <code>X</code> is the field rank. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> tuple = {</span><span style="color:#BB6643">"string"</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#3186CD">true</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.item0 == </span><span style="color:#BB6643">"string"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.item1 == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.item2 == </span><span style="color:#3186CD">true</span><span style="color:#0">)
}</span></code></pre><p>

  But you can specify your own names. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> tuple = {x = </span><span style="color:#74A35B">1.0</span><span style="color:#0">, y = </span><span style="color:#74A35B">2.0</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.x == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.item0 == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.y == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.item1 == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)
}</span></code></pre><p>

  When creating a tuple literal with variables, the tuple fields will take the name of the variables (except if specified). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">555</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> y = </span><span style="color:#74A35B">666</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> t = {x, y}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(t.x == </span><span style="color:#74A35B">555</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(t.item0 == t.x)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(t.y == </span><span style="color:#74A35B">666</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(t.item1 == t.y)
}</span></code></pre><p>

  Even if two tuples do not have the same field names, they can be assigned to each other if the field types are the same. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: {a: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, b: </span><span style="color:#ED9A11">s32</span><span style="color:#0">}
    </span><span style="color:#3186CD">var</span><span style="color:#0"> y: {c: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, d: </span><span style="color:#ED9A11">s32</span><span style="color:#0">}

    y = {</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}
    x = y
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x.a == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x.b == </span><span style="color:#74A35B">2</span><span style="color:#0">)

    </span><span style="color:#6A9955">// But note that 'x' and 'y' to not have the same type</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x) != </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(y)
}</span></code></pre><p>
<h3 id="Tuple unpacking">Tuple unpacking </h3></p>
<p>
</p>
<p>

  You can unpack a tuple field by field. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> tuple1 = {x = </span><span style="color:#74A35B">1.0</span><span style="color:#0">, y = </span><span style="color:#74A35B">2.0</span><span style="color:#0">}

    </span><span style="color:#6A9955">// 'value0' will be assigned with 'x', and 'value1' will be assigned with 'y'.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> (value0, value1) = tuple1
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value0 == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value1 == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)

    </span><span style="color:#3186CD">var</span><span style="color:#0"> tuple2 = {</span><span style="color:#BB6643">"name"</span><span style="color:#0">, </span><span style="color:#3186CD">true</span><span style="color:#0">}
    </span><span style="color:#3186CD">var</span><span style="color:#0"> (name, value) = tuple2
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(name == </span><span style="color:#BB6643">"name"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#3186CD">true</span><span style="color:#0">)
}</span></code></pre><p>

  You can ignore a tuple field by naming the variable <code>?</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> tuple1 = {x = </span><span style="color:#74A35B">1.0</span><span style="color:#0">, y = </span><span style="color:#74A35B">2.0</span><span style="color:#0">}
    </span><span style="color:#3186CD">var</span><span style="color:#0"> (x, ?) = tuple1
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#3186CD">var</span><span style="color:#0"> (?, y) = tuple1
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)
}</span></code></pre>
<h2 id="030_enum">Enum</h2>
<p>

  Enums values, unlike C/C++, can end with <code>;</code> or <code>,</code> or an <code>eol</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values0</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values1</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">,
        </span><span style="color:#3BC3A7">B</span><span style="color:#0">,
    }

    </span><span style="color:#6A9955">// The last comma is not necessary</span><span style="color:#0">
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values2</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">,
        </span><span style="color:#3BC3A7">B</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values3</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">, </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values4</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values5</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }
}</span></code></pre><p>

  By default, an enum is of type <code>s32</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">, </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }
    </span><span style="color:#3186CD">let</span><span style="color:#0"> type = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Values</span><span style="color:#0">)

    </span><span style="color:#6A9955">// 'type' here is a 'typeinfo' dedicated to the enum type.</span><span style="color:#0">
    </span><span style="color:#6A9955">// In that case, 'type.rawType' is the enum underlying type.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(type.rawType == </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Values</span><span style="color:#0">) == </span><span style="color:#3BC3A7">Values</span><span style="color:#0">
}</span></code></pre><p>

  <code>@kindof</code> will return the underlying type of the enum. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0"> { </span><span style="color:#3BC3A7">R</span><span style="color:#0">, </span><span style="color:#3BC3A7">G</span><span style="color:#0">, </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">) == </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">) != </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>

  You can specify your own type after the enum name. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values1</span><span style="color:#0">: </span><span style="color:#ED9A11">s64</span><span style="color:#0"> </span><span style="color:#6A9955">// Forced to be 's64' instead of 's32'</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Values1</span><span style="color:#0">) == </span><span style="color:#ED9A11">s64</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Values1</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">) == </span><span style="color:#3BC3A7">Values1</span><span style="color:#0">
}</span></code></pre><p>

  Enum values, if not specified, start at 0 and are increased by 1 at each new value. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">: </span><span style="color:#ED9A11">s64</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">2</span><span style="color:#0">
}</span></code></pre><p>

  You can specify your own values. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">: </span><span style="color:#ED9A11">s64</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#74A35B">20</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0"> = </span><span style="color:#74A35B">30</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">10</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">20</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">30</span><span style="color:#0">
}</span></code></pre><p>

  If you omit one value, it will be assigned to the previous value + 1. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">: </span><span style="color:#ED9A11">u32</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0"> </span><span style="color:#6A9955">// 11</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0"> </span><span style="color:#6A9955">// 12</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">10</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">11</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">12</span><span style="color:#0">
}</span></code></pre><p>

  For non integer types, you <b>must</b> specify the values as they cannot be deduced. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value1</span><span style="color:#0">: </span><span style="color:#ED9A11">string</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 1"</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 2"</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 3"</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value1</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#BB6643">"string 1"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value1</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#BB6643">"string 2"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value1</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#BB6643">"string 3"</span><span style="color:#0">

    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value2</span><span style="color:#0">: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#74A35B">1.0</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#74A35B">3.14</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0"> = </span><span style="color:#74A35B">6</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value2</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">1.0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value2</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">3.14</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value2</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">6</span><span style="color:#0">
}</span></code></pre><p>

  <code>@countof</code> returns the number of values of an enum. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">: </span><span style="color:#ED9A11">string</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 1"</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 2"</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0"> = </span><span style="color:#BB6643">"string 3"</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">) == </span><span style="color:#74A35B">3</span><span style="color:#0">)
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">) == </span><span style="color:#74A35B">3</span><span style="color:#0">
}</span></code></pre><p>

  You can use the keyword <code>using</code> for an enum. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">
        </span><span style="color:#3BC3A7">B</span><span style="color:#0">
        </span><span style="color:#3BC3A7">C</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">

    </span><span style="color:#6A9955">// No need to say 'Value.A' thanks to the 'using' above</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">2</span><span style="color:#0">)

    </span><span style="color:#6A9955">// But the normal form is still possible</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="Enum as flags">Enum as flags </h3></p>
<p>
An enum can be a set of flags if you declare it with the <code>#[Swag.EnumFlags]</code> <b>attribute</b>. Its type should be <code>u8</code>, <code>u16</code>, <code>u32</code> or <code>u64</code>. </p>
<p>
That kind of enum starts by default at 1, and not 0, and each value should be a power of 2. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.EnumFlags]</span><span style="color:#0">
</span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">: </span><span style="color:#ED9A11">u8</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">A</span><span style="color:#0">   </span><span style="color:#6A9955">// First value is 1 and *not* 0</span><span style="color:#0">
    </span><span style="color:#3BC3A7">B</span><span style="color:#0">   </span><span style="color:#6A9955">// Value is 2</span><span style="color:#0">
    </span><span style="color:#3BC3A7">C</span><span style="color:#0">   </span><span style="color:#6A9955">// Value is 4</span><span style="color:#0">
    </span><span style="color:#3BC3A7">D</span><span style="color:#0">   </span><span style="color:#6A9955">// Value is 8</span><span style="color:#0">
}

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> == </span><span style="color:#74A35B">0b00000001</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#74A35B">0b00000010</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#74A35B">0b00000100</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">D</span><span style="color:#0"> == </span><span style="color:#74A35B">0b00001000</span><span style="color:#0">

</span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> | </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#74A35B">0b00000110</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value & </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0"> == </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value & </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0"> == </span><span style="color:#3BC3A7">MyFlags</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0">)</span></code></pre><p>
<h3 id="Enum of arrays">Enum of arrays </h3></p>
<p>
You can have an enum of const static arrays. </p>
<pre><code><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">: </span><span style="color:#3186CD">const</span><span style="color:#0"> [</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">]
}

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">10</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">20</span></code></pre><p>
<h3 id="Enum of slices">Enum of slices </h3></p>
<p>
You can have an enum of const slices. </p>
<pre><code><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0">: </span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]
}

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">) == </span><span style="color:#74A35B">2</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">

</span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#3186CD">let</span><span style="color:#0"> y = </span><span style="color:#3BC3A7">Value</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">20</span><span style="color:#0">)</span></code></pre><p>
<h3 id="Enum type inference">Enum type inference </h3></p>
<p>
</p>
<p>

  The type of the enum is not necessary in the assignement expression when declaring a variable. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// The normal form</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> = </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">

    </span><span style="color:#6A9955">// But in fact 'Values' is not necessary because it can be deduced</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> y: </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> = </span><span style="color:#3BC3A7">A</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == y)
}</span></code></pre><p>

  The enum type is not necessary in a <code>case</code> expression of a <code>switch</code> block (it is deduced from the switch expression). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">

    </span><span style="color:#6A9955">// The 'normal' form</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> x
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">: </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">: </span><span style="color:#B040BE">break</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// The 'simplified' form</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> x
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0">: </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">B</span><span style="color:#0">: </span><span style="color:#B040BE">break</span><span style="color:#0">
    }
}</span></code></pre><p>

  In an expression, and if the enum name can be deduced, you can omit it and use the <code>.Value</code> syntax. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">, </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }

    </span><span style="color:#6A9955">// The normal form</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">

    </span><span style="color:#6A9955">// The simplified form, because 'Values' can be deduced from type of x</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == .</span><span style="color:#3BC3A7">A</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x != .</span><span style="color:#3BC3A7">B</span><span style="color:#0">)
}</span></code></pre><p>

  Works also for flags. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.EnumFlags]</span><span style="color:#0">
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">, </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }

    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0"> | </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">((x & .</span><span style="color:#3BC3A7">A</span><span style="color:#0">) </span><span style="color:#B040BE">and</span><span style="color:#0"> (x & .</span><span style="color:#3BC3A7">B</span><span style="color:#0">))
}</span></code></pre><p>

  Works also (most of the time), for functions. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">(v1, v2: </span><span style="color:#3BC3A7">Values</span><span style="color:#0">) {}
    </span><span style="color:#FF6A00">toto</span><span style="color:#0">(.</span><span style="color:#3BC3A7">A</span><span style="color:#0">, .</span><span style="color:#3BC3A7">B</span><span style="color:#0">)
}</span></code></pre><p>
By type reflection, you can loop/visit all values of an enum. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0"> { </span><span style="color:#3BC3A7">R</span><span style="color:#0">, </span><span style="color:#3BC3A7">G</span><span style="color:#0">, </span><span style="color:#3BC3A7">B</span><span style="color:#0"> }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> idx: </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">3</span><span style="color:#0">)

    </span><span style="color:#B040BE">visit</span><span style="color:#0"> val: </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">switch</span><span style="color:#0"> val
        {
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">R</span><span style="color:#0">:     </span><span style="color:#B040BE">break</span><span style="color:#0">
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0">:     </span><span style="color:#B040BE">break</span><span style="color:#0">
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">B</span><span style="color:#0">:     </span><span style="color:#B040BE">break</span><span style="color:#0">
        </span><span style="color:#B040BE">default</span><span style="color:#0">:    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
        }
    }
}</span></code></pre>
<h2 id="031_impl">Impl</h2>
<p>
<code>impl</code> can be used to declare some stuff in the scope of an enum. The keyword <code>self</code> represents the enum value. </p>
<pre><code><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0"> { </span><span style="color:#3BC3A7">R</span><span style="color:#0">; </span><span style="color:#3BC3A7">G</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }</span></code></pre><p>
Note the <code>impl enum</code> syntax. We'll see later that <code>impl</code> is also used for structs. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isRed</span><span style="color:#0">(</span><span style="color:#ED9A11">self</span><span style="color:#0">)       => </span><span style="color:#ED9A11">self</span><span style="color:#0"> == </span><span style="color:#3BC3A7">R</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isRedOrBlue</span><span style="color:#0">(</span><span style="color:#ED9A11">self</span><span style="color:#0">) => </span><span style="color:#ED9A11">self</span><span style="color:#0"> == </span><span style="color:#3BC3A7">R</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0"> == </span><span style="color:#3BC3A7">B</span><span style="color:#0">
}</span></code></pre><pre><code><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#FF6A00">isRed</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#3BC3A7">R</span><span style="color:#0">))
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#FF6A00">isRedOrBlue</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">))

</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">isRedOrBlue</span><span style="color:#0">(</span><span style="color:#3BC3A7">R</span><span style="color:#0">))
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">isRedOrBlue</span><span style="color:#0">(</span><span style="color:#3BC3A7">B</span><span style="color:#0">))

</span><span style="color:#6A9955">// A first look at 'ufcs' (uniform function call syntax)</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">R</span><span style="color:#0">.</span><span style="color:#FF6A00">isRedOrBlue</span><span style="color:#0">())
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(!</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#3BC3A7">G</span><span style="color:#0">.</span><span style="color:#FF6A00">isRedOrBlue</span><span style="color:#0">())</span></code></pre>
<h2 id="035_namespace">Namespace</h2>
<p>
You can create a global scope with a namespace. All symbols inside the namespace will be in the corresponding global scope. </p>
<pre><code><span style="color:#3186CD">namespace</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">a</span><span style="color:#0">() => </span><span style="color:#74A35B">1</span><span style="color:#0">
}</span></code></pre><p>
You can also specify more than one name. Here <code>C</code> will be a namespace inside <code>B</code> which is itself inside <code>A</code>. </p>
<pre><code><span style="color:#3186CD">namespace</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">a</span><span style="color:#0">() => </span><span style="color:#74A35B">2</span><span style="color:#0">
}</span></code></pre><pre><code><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">A</span><span style="color:#0">.</span><span style="color:#FF6A00">a</span><span style="color:#0">() == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">A</span><span style="color:#0">.</span><span style="color:#3BC3A7">B</span><span style="color:#0">.</span><span style="color:#3BC3A7">C</span><span style="color:#0">.</span><span style="color:#FF6A00">a</span><span style="color:#0">() == </span><span style="color:#74A35B">2</span><span style="color:#0">)</span></code></pre><p>
You can also put <code>using</code> in front of the namespace to be able to access the content without scoping in the <b>current</b> file. </p>
<pre><code><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3186CD">namespace</span><span style="color:#0"> </span><span style="color:#3BC3A7">Private</span><span style="color:#0">
{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">FileSymbol</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">
}

</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">B</span><span style="color:#0"> = </span><span style="color:#3BC3A7">Private</span><span style="color:#0">.</span><span style="color:#3BC3A7">FileSymbol</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">C</span><span style="color:#0"> = </span><span style="color:#3BC3A7">FileSymbol</span><span style="color:#0"> </span><span style="color:#6A9955">// No need to specify 'Private' because of the 'using'</span></code></pre><p>
This is equivalent to <code>private</code>, but you don't have to specify a name, the compiler will generate it for you. </p>
<pre><code><span style="color:#3186CD">private</span><span style="color:#0"> {
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">OtherSymbol</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">
}

</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">D</span><span style="color:#0"> = </span><span style="color:#3BC3A7">OtherSymbol</span></code></pre><p>
All symbols from a Swag source file are exported to other files of the same module. So using <code>private</code> can protect from name conflicts. </p>

<h2 id="050_if">If</h2>
<p>
A basic test with <code>if</code>. Curlies are optional, and the expression doesn't need to be enclosed with <code>()</code> like in C/C++. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
</span><span style="color:#B040BE">if</span><span style="color:#0"> (a == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
</span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">0</span><span style="color:#0"> {
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)</span></code></pre><pre><code><span style="color:#0">    </span><span style="color:#6A9955">// 'else' is of course also a thing</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">0</span><span style="color:#0">
        a += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B040BE">else</span><span style="color:#0">
        a += </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1</span><span style="color:#0">)

    </span><span style="color:#6A9955">// 'elif' is equivalent to 'else if'</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">1</span><span style="color:#0">
        a += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B040BE">else</span><span style="color:#0"> </span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">2</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">elif</span><span style="color:#0"> a == </span><span style="color:#74A35B">3</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">elif</span><span style="color:#0"> a == </span><span style="color:#74A35B">4</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Boolean expression with 'and' and 'or'</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">0</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> a == </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">0</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> a == </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
}</span></code></pre><p>
You can also at the same time declare and test one variable in an <code>if</code> expression. <code>var</code>, <code>let</code> or <code>const</code> is mandatory in that case. </p>
<pre><code><span style="color:#6A9955">// This will declare a variable 'a', and test it against 0.</span><span style="color:#0">
</span><span style="color:#6A9955">// 'a' is then only visible in the 'if' block, and not outside.</span><span style="color:#0">
</span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
}

</span><span style="color:#6A9955">// So you can redeclare 'a' (this time as a constant).</span><span style="color:#0">
</span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3186CD">const</span><span style="color:#0"> a = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B040BE">else</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)

</span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B040BE">else</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)</span></code></pre>
<h2 id="051_loop">Loop</h2>
<p>
<code>loop</code> are used to iterate a given amount of time. </p>
<p>

  The loop expression value is evaluated <b>once</b>, and must be a <b>positive value</b>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0"> </span><span style="color:#6A9955">// Loops 10 times</span><span style="color:#0">
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}</span></code></pre><p>

  The intrinsic <code>@index</code> returns the current index of the loop (starting at 0). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">5</span><span style="color:#0">
    {
        cpt += </span><span style="color:#B4B44A">@index</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">0</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)
}</span></code></pre><p>

  You can name that index if you want. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt1 = </span><span style="color:#74A35B">0</span><span style="color:#0">

    </span><span style="color:#B040BE">loop</span><span style="color:#0"> i: </span><span style="color:#74A35B">5</span><span style="color:#0">   </span><span style="color:#6A9955">// index is named 'i'</span><span style="color:#0">
    {
        cpt  += i
        cpt1 += </span><span style="color:#B4B44A">@index</span><span style="color:#0">  </span><span style="color:#6A9955">// @index is always available, even when named</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt  == </span><span style="color:#74A35B">0</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt1 == cpt)
}</span></code></pre><p>
<code>loop</code> can be used on every types that accept the <code>@countof</code> intrinsic. So you can loop on a slice, an array, a string... and we'll see later, even on a struct. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(arr) == </span><span style="color:#74A35B">4</span><span style="color:#0">

</span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#B040BE">loop</span><span style="color:#0"> arr    </span><span style="color:#6A9955">// The array contains 4 elements, so the loop count is 4</span><span style="color:#0">
    cpt += arr[</span><span style="color:#B4B44A">@index</span><span style="color:#0">]
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">20</span><span style="color:#0">+</span><span style="color:#74A35B">30</span><span style="color:#0">+</span><span style="color:#74A35B">40</span><span style="color:#0">)</span></code></pre><p>
</p>
<blockquote><p>
<b>Warning !</b> On a string, it will loop for each byte, <b>not</b> runes (if a rune is encoded in more than one byte). If you want to iterate on all runes, you will have to use the <code>Std.Core</code> module. </p></blockquote>
<p>
</p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#BB6643">"⻘"</span><span style="color:#0">
    cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#6A9955">// cpt is equal to 3 because '⻘' is encoded with 3 bytes</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">3</span><span style="color:#0">)</span></code></pre><p>
<h3 id="break, continue">break, continue </h3></p>
<p>
<code>break</code> and <code>continue</code> can be used inside a loop. </p>
<p>

  You can exit a loop with <code>break</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> x: </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> x == </span><span style="color:#74A35B">5</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">5</span><span style="color:#0">)
}</span></code></pre><p>

  You can force to return to the loop evaluation with <code>continue</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> x: </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> x == </span><span style="color:#74A35B">5</span><span style="color:#0">
            </span><span style="color:#B040BE">continue</span><span style="color:#0"> </span><span style="color:#6A9955">// Do not count 5</span><span style="color:#0">
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">9</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="Ranges">Ranges </h3></p>
<p>
Loop can also be used to iterate on a <b>range</b> of signed values. </p>
<p>

  Syntax is <code>lower bound..upper bound</code> </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> count = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> sum = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> i: -</span><span style="color:#74A35B">1</span><span style="color:#0">..</span><span style="color:#74A35B">1</span><span style="color:#0">   </span><span style="color:#6A9955">// loop from -1 to 1, all included</span><span style="color:#0">
    {
        count += </span><span style="color:#74A35B">1</span><span style="color:#0">
        sum += i
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(sum == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(count == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  With a range, you can loop in reverse order. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// Loop from 5 to 0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">5</span><span style="color:#0">..</span><span style="color:#74A35B">0</span><span style="color:#0">
    {
    }

    </span><span style="color:#6A9955">// Loop from 1 to -1</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">..-</span><span style="color:#74A35B">1</span><span style="color:#0">
    {
    }
}</span></code></pre><p>

  You can exclude the last value with the <code>..<</code> syntax. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// Will loop from 1 to 2 and **not** 1 to 3</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> i: </span><span style="color:#74A35B">1</span><span style="color:#0">..<</span><span style="color:#74A35B">3</span><span style="color:#0">
    {
        cpt += i
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="Infinite loop">Infinite loop </h3></p>
<p>
A loop without an expression but with a block is infinite. This is equivalent to <code>while true {}</code>. </p>
<pre><code><span style="color:#B040BE">loop</span><span style="color:#0">
{
    </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@index</span><span style="color:#0"> == </span><span style="color:#74A35B">4</span><span style="color:#0"> </span><span style="color:#6A9955">// @index is still valid in that case (but cannot be renamed)</span><span style="color:#0">
        </span><span style="color:#B040BE">break</span><span style="color:#0">
}</span></code></pre>
<h2 id="052_visit">Visit</h2>
<p>

  <code>visit</code> is used to visit all the elements of a collection. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// Here we visit every bytes of the string.</span><span style="color:#0">
    </span><span style="color:#6A9955">// At each iteration, the byte will be stored in the variable 'value'</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> value: </span><span style="color:#BB6643">"ABC"</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// '@index' is also available. It stores the loop index.</span><span style="color:#0">
        </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#B4B44A">@index</span><span style="color:#0">
        </span><span style="color:#B040BE">switch</span><span style="color:#0"> a
        {
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#BB6643">"A"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#BB6643">"B"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#BB6643">"C"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        }
    }
}</span></code></pre><p>

  You can name both the <b>value</b> and the loop <b>index</b>, in that order. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> value, index: </span><span style="color:#BB6643">"ABC"</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">let</span><span style="color:#0"> a = index
        </span><span style="color:#B040BE">switch</span><span style="color:#0"> a
        {
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#BB6643">"A"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#BB6643">"B"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#BB6643">"C"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        }
    }
}</span></code></pre><p>

  Both names are optional. In that case, you can use <code>@alias0</code> and <code>@alias1</code>. <code>@alias0</code> for the value, and <code>@alias1</code> for the index. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> </span><span style="color:#BB6643">"ABC"</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#B4B44A">@alias1</span><span style="color:#0">    </span><span style="color:#6A9955">// This is the index</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#B4B44A">@index</span><span style="color:#0">)
        </span><span style="color:#B040BE">switch</span><span style="color:#0"> a
        {
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> == </span><span style="color:#BB6643">"A"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> == </span><span style="color:#BB6643">"B"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> == </span><span style="color:#BB6643">"C"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">)
        }
    }
}</span></code></pre><p>

  You can visit arrays or slices. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array = [</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">]

    </span><span style="color:#3186CD">var</span><span style="color:#0"> result = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> it: array
        result += it

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">20</span><span style="color:#0">+</span><span style="color:#74A35B">30</span><span style="color:#0">)
}</span></code></pre><p>

  Works also for multi dimensional arrays. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = [[</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">], [</span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">]]

    </span><span style="color:#3186CD">var</span><span style="color:#0"> result = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> it: array
        result += it

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">20</span><span style="color:#0">+</span><span style="color:#74A35B">30</span><span style="color:#0">+</span><span style="color:#74A35B">40</span><span style="color:#0">)
}</span></code></pre><p>

  You can visit with a pointer to the value, and not the value itself, by adding <code>&</code> before the value name. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> array: [</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = [[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">], [</span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]]

    </span><span style="color:#3186CD">var</span><span style="color:#0"> result = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> &it: array
    {
        result += </span><span style="color:#3186CD">dref</span><span style="color:#0"> it
        </span><span style="color:#3186CD">dref</span><span style="color:#0"> it = </span><span style="color:#74A35B">555</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result == </span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">555</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">555</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">555</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(array[</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">555</span><span style="color:#0">)
}</span></code></pre>
<h2 id="053_for">For</h2>
<p>

  <code>for</code> accepts a <i>start statement</i>, an <i>expression to test</i>, and an <i>ending statement</i>. This is in fact the same as the C/C++ <code>for</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">; i < </span><span style="color:#74A35B">10</span><span style="color:#0">; i += </span><span style="color:#74A35B">1</span><span style="color:#0">
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">10</span><span style="color:#0">)

    </span><span style="color:#6A9955">// ';' can be replaced by a newline (like always)</span><span style="color:#0">
    </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">
        i < </span><span style="color:#74A35B">10</span><span style="color:#0">
        i += </span><span style="color:#74A35B">1</span><span style="color:#0">
    {
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">20</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Instead of one single expression, you can use a statement</span><span style="color:#0">
    </span><span style="color:#B040BE">for</span><span style="color:#0"> { </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">; cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">; }
        i < </span><span style="color:#74A35B">10</span><span style="color:#0">
        i += </span><span style="color:#74A35B">1</span><span style="color:#0">
    {
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">10</span><span style="color:#0">)

    </span><span style="color:#B040BE">for</span><span style="color:#0"> { </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">; cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">; }
        i < </span><span style="color:#74A35B">10</span><span style="color:#0">
        { i += </span><span style="color:#74A35B">2</span><span style="color:#0">; i -= </span><span style="color:#74A35B">1</span><span style="color:#0">; }
    {
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}</span></code></pre><p>

  Like <code>loop</code>, <code>visit</code> and <code>while</code>, you have access to <code>@index</code>, the <b>current loop index</b>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">
    </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> i: </span><span style="color:#ED9A11">u32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">; i < </span><span style="color:#74A35B">15</span><span style="color:#0">; i += </span><span style="color:#74A35B">1</span><span style="color:#0">
        cpt += </span><span style="color:#B4B44A">@index</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">0</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt1 = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">
    </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">10</span><span style="color:#0">; i < </span><span style="color:#74A35B">15</span><span style="color:#0">; i += </span><span style="color:#74A35B">1</span><span style="color:#0">
        cpt1 += </span><span style="color:#B4B44A">@index</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt1 == </span><span style="color:#74A35B">0</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)
}</span></code></pre>
<h2 id="054_while">While</h2>
<p>

  <code>while</code> is a loop that runs <b>until the expression is false</b>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">while</span><span style="color:#0"> i < </span><span style="color:#74A35B">10</span><span style="color:#0">
        i += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(i == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}</span></code></pre><p>

  You can also <code>break</code> and <code>continue</code> inside a <code>while</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">while</span><span style="color:#0"> i < </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> i == </span><span style="color:#74A35B">5</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        i += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(i == </span><span style="color:#74A35B">5</span><span style="color:#0">)
}</span></code></pre>
<h2 id="055_switch">Switch</h2>
<p>

  <code>switch</code> works like in C/C++, except that no <code>break</code> is necessary (except if the <code>case</code> is empty). That means that there's no automatic <code>fallthrough</code> from one case to another. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">:  </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">5</span><span style="color:#0">:  </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">6</span><span style="color:#0">:  </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }

    </span><span style="color:#3186CD">let</span><span style="color:#0"> ch = </span><span style="color:#BB6643">"A"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> ch
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"B"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"A"</span><span style="color:#0">'</span><span style="color:#ED9A11">rune</span><span style="color:#0">: </span><span style="color:#B040BE">break</span><span style="color:#0">
    }
}</span></code></pre><p>

  You can put multiple values on the same <code>case</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#74A35B">6</span><span style="color:#0">:   </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">:        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }

    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">,
         </span><span style="color:#74A35B">4</span><span style="color:#0">,
         </span><span style="color:#74A35B">6</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  <code>switch</code> works with every types that accept the <code>==</code> operator. So you can switch on strings for example. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#BB6643">"myString"</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"myString"</span><span style="color:#0">:    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"otherString"</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">:            </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  If you want to pass from one case to another like in C/C++, use <code>fallthrough</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">6</span><span style="color:#0">:
        </span><span style="color:#B040BE">fallthrough</span><span style="color:#0">
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">7</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value == </span><span style="color:#74A35B">6</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    }
}</span></code></pre><p>

  <code>break</code> can be used to exit the current <code>case</code> statement. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">6</span><span style="color:#0">:
        </span><span style="color:#B040BE">if</span><span style="color:#0"> value == </span><span style="color:#74A35B">6</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  A <code>case</code> statement cannot be empty. Use <code>break</code> if you want to do nothing. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> value
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">5</span><span style="color:#0">:     </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">6</span><span style="color:#0">:     </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">default</span><span style="color:#0">:    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  <code>switch</code> can be marked with <code>Swag.Complete</code> to force all the cases to be covered. If one or more values are missing, an error will be raised by the compiler. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Color</span><span style="color:#0"> { </span><span style="color:#3BC3A7">Red</span><span style="color:#0">, </span><span style="color:#3BC3A7">Green</span><span style="color:#0">, </span><span style="color:#3BC3A7">Blue</span><span style="color:#0"> }
    </span><span style="color:#3186CD">let</span><span style="color:#0"> color = </span><span style="color:#3BC3A7">Color</span><span style="color:#0">.</span><span style="color:#3BC3A7">Red</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#[Swag.Complete]</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> color
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">Color</span><span style="color:#0">.</span><span style="color:#3BC3A7">Red</span><span style="color:#0">:     </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">Color</span><span style="color:#0">.</span><span style="color:#3BC3A7">Green</span><span style="color:#0">:   </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#3BC3A7">Color</span><span style="color:#0">.</span><span style="color:#3BC3A7">Blue</span><span style="color:#0">:    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  If the switch expression is omitted, then it will behave like a serie of if/else, resolved in order. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> value1 = </span><span style="color:#BB6643">"true"</span><span style="color:#0">

    </span><span style="color:#B040BE">switch</span><span style="color:#0">
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> value == </span><span style="color:#74A35B">6</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> value == </span><span style="color:#74A35B">7</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
        </span><span style="color:#B040BE">fallthrough</span><span style="color:#0">
    </span><span style="color:#B040BE">case</span><span style="color:#0"> value1 == </span><span style="color:#BB6643">"true"</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B040BE">default</span><span style="color:#0">:
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  When used on an <code>any</code> variable, switch is done on the underlying variable type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#ED9A11">any</span><span style="color:#0"> = </span><span style="color:#BB6643">"value"</span><span style="color:#0">

    </span><span style="color:#B040BE">switch</span><span style="color:#0"> x
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#ED9A11">string</span><span style="color:#0">: </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">default</span><span style="color:#0">:     </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre><p>

  A <code>switch</code> can also be used with a (constant) range of values. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> success = </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> x
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">..</span><span style="color:#74A35B">5</span><span style="color:#0">:  </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">6</span><span style="color:#0">..</span><span style="color:#74A35B">15</span><span style="color:#0">: success = </span><span style="color:#3186CD">true</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(success)
}</span></code></pre><p>

  If they overlap, the first valid range will be used. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> success = </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">6</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> x
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">..</span><span style="color:#74A35B">10</span><span style="color:#0">:  success = </span><span style="color:#3186CD">true</span><span style="color:#0">
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">5</span><span style="color:#0">..<</span><span style="color:#74A35B">15</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(success)
}</span></code></pre><p>

  A <code>case</code> expression doesn't need to be constant. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> test = </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#74A35B">1</span><span style="color:#0">

    </span><span style="color:#B040BE">switch</span><span style="color:#0"> test
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> a:     </span><span style="color:#6A9955">// Test with a variable</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> b:     </span><span style="color:#6A9955">// Test with a variable</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> b + </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#6A9955">// Test with an expression</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    }
}</span></code></pre>
<h2 id="056_break">Break</h2>
<p>

  We have already seen than <code>break</code> is used to exit a <code>loop</code>, <code>visit</code>, <code>while</code>, <code>for</code>, <code>switch</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
        </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> i = </span><span style="color:#74A35B">0</span><span style="color:#0">; i < </span><span style="color:#74A35B">10</span><span style="color:#0">; i += </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B040BE">break</span><span style="color:#0">
    </span><span style="color:#B040BE">while</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">
        </span><span style="color:#B040BE">break</span><span style="color:#0">
}</span></code></pre><p>

  By default, <code>break</code> will exit the parent scope only. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
        {
            </span><span style="color:#B040BE">break</span><span style="color:#0">   </span><span style="color:#6A9955">// Exit the inner loop...</span><span style="color:#0">
        }

        </span><span style="color:#6A9955">// ...and continue execution here</span><span style="color:#0">
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}</span></code></pre><p>

  But you can <b>name a scope</b> with the <code>#scope</code> compiler keyword, and exit to the end of it with a <code>break</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">

    </span><span style="color:#6A9955">// Creates a scope named 'BigScope'</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#scope</span><span style="color:#0"> </span><span style="color:#3BC3A7">BigScope</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
        {
            cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0"> </span><span style="color:#3BC3A7">BigScope</span><span style="color:#0">  </span><span style="color:#6A9955">// Break to the outer scope...</span><span style="color:#0">
        }

        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }

    </span><span style="color:#6A9955">// ...and continue execution here</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">1</span><span style="color:#0">)
}</span></code></pre><p>

  When used with a scope, a continue is a way to go back to the start of the scope. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#scope</span><span style="color:#0"> </span><span style="color:#3BC3A7">Loop</span><span style="color:#0">
    {
        cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B040BE">if</span><span style="color:#0"> cpt == </span><span style="color:#74A35B">5</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        </span><span style="color:#B040BE">continue</span><span style="color:#0">    </span><span style="color:#6A9955">// Loop</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">5</span><span style="color:#0">)
}</span></code></pre><p>

  You are not obliged to name the scope, so this can also be used (for example) as an alternative of a bunch of if/else. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#scope</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> cpt == </span><span style="color:#74A35B">1</span><span style="color:#0">
        {
            </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">1</span><span style="color:#0">)
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        }

        </span><span style="color:#B040BE">if</span><span style="color:#0"> cpt == </span><span style="color:#74A35B">2</span><span style="color:#0">
        {
            </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">2</span><span style="color:#0">)
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        }

        </span><span style="color:#B040BE">if</span><span style="color:#0"> cpt == </span><span style="color:#74A35B">3</span><span style="color:#0">
        {
            </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">3</span><span style="color:#0">)
            </span><span style="color:#B040BE">break</span><span style="color:#0">
        }
    }
}</span></code></pre><p>

  Note that a scope can be followed by a simple statement, not always a block. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#scope</span><span style="color:#0"> </span><span style="color:#3BC3A7">Up</span><span style="color:#0"> </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
        {
            </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@index</span><span style="color:#0"> == </span><span style="color:#74A35B">5</span><span style="color:#0">
                </span><span style="color:#B040BE">break</span><span style="color:#0"> </span><span style="color:#3BC3A7">Up</span><span style="color:#0">
        }

        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
    }
}</span></code></pre>
<h2 id="060_struct">Struct</h2>

<h3 id="061__declaration">Declaration</h3>
<p>

  This is a <code>struct</code> declaration. <code>var</code> is not necessary for the fields. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        name: </span><span style="color:#ED9A11">string</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">
    {
        x:      </span><span style="color:#ED9A11">s32</span><span style="color:#0">
        y, z:   </span><span style="color:#ED9A11">s32</span><span style="color:#0">
        val:    </span><span style="color:#ED9A11">bool</span><span style="color:#0">
        myS:    </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    }
}</span></code></pre><p>

  Variables can also be separated with <code>;</code> or <code>,</code>. The last one is not mandatory. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{name: </span><span style="color:#ED9A11">string</span><span style="color:#0">, val1: </span><span style="color:#ED9A11">bool</span><span style="color:#0">}
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">{x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; y, z: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; val: </span><span style="color:#ED9A11">bool</span><span style="color:#0">; myS: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">}
}</span></code></pre><p>

  A struct can be anonymous when declared as a variable type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> tuple: </span><span style="color:#3186CD">struct</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    }
    </span><span style="color:#3186CD">var</span><span style="color:#0"> tuple1: </span><span style="color:#3186CD">struct</span><span style="color:#0">{x, y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">}

    tuple.x = </span><span style="color:#74A35B">1.0</span><span style="color:#0">
    tuple.y = </span><span style="color:#74A35B">2.0</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.x == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(tuple.y == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)
}

{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        rgb: </span><span style="color:#3186CD">struct</span><span style="color:#0">{x, y, z: </span><span style="color:#ED9A11">f32</span><span style="color:#0">}
        hsl: </span><span style="color:#3186CD">struct</span><span style="color:#0">{h, s, l: </span><span style="color:#ED9A11">f32</span><span style="color:#0">}
    }
}</span></code></pre><p>

  The fields of a struct can be initialized at the declaration. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">    = </span><span style="color:#74A35B">666</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"454"</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> v = </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">666</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#BB6643">"454"</span><span style="color:#0">)
}</span></code></pre><p>

  You can initialize a struct variable in different ways. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// At fields declaration</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// Without parameters, all fields will have the default values as defined</span><span style="color:#0">
    </span><span style="color:#6A9955">// in the struct itself.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> v0: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v0.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v0.y == </span><span style="color:#74A35B">1</span><span style="color:#0">)

    </span><span style="color:#6A9955">// You can add parameters between {..}</span><span style="color:#0">
    </span><span style="color:#6A9955">// The initialization must be done in the order of the fields.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> v1: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v1.x == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v1.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)

    </span><span style="color:#6A9955">// You can name fields, and omit some.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> v2 = </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{y = </span><span style="color:#74A35B">20</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v2.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v2.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)

    </span><span style="color:#6A9955">// You can assign a tuple as long as the types are correct.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> v3: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0"> = {</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v3.x == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v3.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)
}</span></code></pre><p>

  A struct can be affected to a constant, as long as it can be evaluated at compile time. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">    = </span><span style="color:#74A35B">666</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"454"</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">X</span><span style="color:#0">: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{</span><span style="color:#74A35B">50</span><span style="color:#0">, </span><span style="color:#BB6643">"value"</span><span style="color:#0">}
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">X</span><span style="color:#0">.x == </span><span style="color:#74A35B">50</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">X</span><span style="color:#0">.y == </span><span style="color:#BB6643">"value"</span><span style="color:#0">
}</span></code></pre><p>
A function can take an argument of type <code>struct</code>. No copy is done (this is equivalent to a const reference in C++). </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Struct3</span><span style="color:#0">
{
    x, y, z: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">666</span><span style="color:#0">
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">(v: </span><span style="color:#3BC3A7">Struct3</span><span style="color:#0">)
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.z == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">titi</span><span style="color:#0">(v: </span><span style="color:#3BC3A7">Struct3</span><span style="color:#0">)
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}</span></code></pre><pre><code><span style="color:#6A9955">// Call with a struct literal</span><span style="color:#0">
</span><span style="color:#FF6A00">toto</span><span style="color:#0">(</span><span style="color:#3BC3A7">Struct3</span><span style="color:#0">{</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">666</span><span style="color:#0">})

</span><span style="color:#6A9955">// Type can be deduced from the argument</span><span style="color:#0">
</span><span style="color:#FF6A00">toto</span><span style="color:#0">({</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">666</span><span style="color:#0">})

</span><span style="color:#6A9955">// You can also just specify some parts of the struct, in the declaration order of the fields</span><span style="color:#0">
</span><span style="color:#FF6A00">titi</span><span style="color:#0">({</span><span style="color:#74A35B">5</span><span style="color:#0">})      </span><span style="color:#6A9955">// Initialize x, which is the first field</span><span style="color:#0">
</span><span style="color:#FF6A00">titi</span><span style="color:#0">({</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">666</span><span style="color:#0">}) </span><span style="color:#6A9955">// Initialize x and y</span><span style="color:#0">

</span><span style="color:#6A9955">// You can also name the fields, and omit some of them</span><span style="color:#0">
</span><span style="color:#FF6A00">titi</span><span style="color:#0">({x = </span><span style="color:#74A35B">5</span><span style="color:#0">, z = </span><span style="color:#74A35B">5</span><span style="color:#0">}) </span><span style="color:#6A9955">// Here y will stay to the default value, which is 666</span></code></pre>
<h3 id="062__impl">Impl</h3>
<p>
Like for an enum, <code>impl</code> is used to declare some stuff in the scope of a struct. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.ExportType("methods")]</span><span style="color:#0">   </span><span style="color:#6A9955">// See later, used to export 'methods' in type reflection</span><span style="color:#0">
</span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    x: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">5</span><span style="color:#0">
    y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">
    z: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">20</span><span style="color:#0">
}

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// Declare a constant in the scope of the struct</span><span style="color:#0">
    </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyConst</span><span style="color:#0"> = </span><span style="color:#3186CD">true</span><span style="color:#0">

    </span><span style="color:#6A9955">// Declare a function in the scope of the struct</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">returnTrue</span><span style="color:#0">() => </span><span style="color:#3186CD">true</span><span style="color:#0">
}</span></code></pre><p>
So to access the constant and the function, you have to use the <code>MyStruct</code> namespace. </p>
<pre><code><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">.</span><span style="color:#3BC3A7">MyConst</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">.</span><span style="color:#FF6A00">returnTrue</span><span style="color:#0">())</span></code></pre><p>
You can have multiple <code>impl</code> blocks. The difference with a namespace is that <code>self</code> and <code>Self</code> are defined inside an <code>impl</code> block. They refere to the corresponding type. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// 'self' is an alias for 'var self: Self'</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">returnX</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">) => x
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">returnY</span><span style="color:#0">(</span><span style="color:#ED9A11">self</span><span style="color:#0">)       => </span><span style="color:#ED9A11">self</span><span style="color:#0">.y

    </span><span style="color:#6A9955">// 'Self' is the corresponding type, in that case 'MyStruct'</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">returnZ</span><span style="color:#0">(me: </span><span style="color:#ED9A11">Self</span><span style="color:#0">)   => me.z
}</span></code></pre><p>
If you declare your function with <code>mtd</code> (method) instead of <code>func</code>, then the first parameter is forced to be <code>using self</code>. If you declare your function with <code>mtd const</code> (method const) instead of <code>func</code>, then the first parameter is forced to be <code>const using self</code>. Other than that, it's exactly the same. So this is just <b>syntaxic sugar</b> to avoid repeating the <code>using self</code>. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#3186CD">mtd</span><span style="color:#0">  </span><span style="color:#FF6A00">methodReturnX</span><span style="color:#0">()          => x
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">funcReturnX</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">)  => x
}</span></code></pre><pre><code><span style="color:#3186CD">var</span><span style="color:#0"> c: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c.</span><span style="color:#FF6A00">returnX</span><span style="color:#0">() == </span><span style="color:#74A35B">5</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c.</span><span style="color:#FF6A00">methodReturnX</span><span style="color:#0">() == </span><span style="color:#74A35B">5</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c.</span><span style="color:#FF6A00">funcReturnX</span><span style="color:#0">() == </span><span style="color:#74A35B">5</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c.</span><span style="color:#FF6A00">returnY</span><span style="color:#0">() == </span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(c.</span><span style="color:#FF6A00">returnZ</span><span style="color:#0">() == </span><span style="color:#74A35B">20</span><span style="color:#0">)</span></code></pre><p>
All functions in an <code>impl</code> block can be retrieved by reflection, as long as the struct is declared with <code>#[Swag.ExportType("methods")]</code> (by default, methods are not exported). </p>
<pre><code><span style="color:#6A9955">// Creates a type alias named 'Lambda'</span><span style="color:#0">
</span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">Lambda</span><span style="color:#0"> = </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">

</span><span style="color:#3186CD">var</span><span style="color:#0"> fnX: </span><span style="color:#3BC3A7">Lambda</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> fnY: </span><span style="color:#3BC3A7">Lambda</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> fnZ: </span><span style="color:#3BC3A7">Lambda</span><span style="color:#0">

</span><span style="color:#6A9955">// The 'typeinfo' of a struct contains a field 'methods' which is a slice</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> t = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">)
</span><span style="color:#B040BE">visit</span><span style="color:#0"> p: t.methods
{
    </span><span style="color:#6A9955">// When visiting 'methods', the 'value' field contains the function pointer,</span><span style="color:#0">
    </span><span style="color:#6A9955">// which can be casted to the correct type</span><span style="color:#0">
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> p.name
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"returnX"</span><span style="color:#0">: fnX = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">Lambda</span><span style="color:#0">) p.value
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"returnY"</span><span style="color:#0">: fnY = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">Lambda</span><span style="color:#0">) p.value
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#BB6643">"returnZ"</span><span style="color:#0">: fnZ = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">Lambda</span><span style="color:#0">) p.value
    }
}

</span><span style="color:#6A9955">// These are now valid functions, which can be called</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fnX</span><span style="color:#0">(v) == </span><span style="color:#74A35B">5</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fnY</span><span style="color:#0">(v) == </span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fnZ</span><span style="color:#0">(v) == </span><span style="color:#74A35B">20</span><span style="color:#0">)</span></code></pre>
<h3 id="063__special_functions">Special functions</h3>
<p>
A struct can have special operations in the <code>impl</code> block. This operations are predefined, and known by the compiler.This is the way to go to <b>overload operators</b> for example. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
{
    x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}

</span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">      = </span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">  = </span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">WhateverType</span><span style="color:#0"> = </span><span style="color:#ED9A11">bool</span><span style="color:#0">

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// Called each time a variable needs to be dropped</span><span style="color:#0">
    </span><span style="color:#6A9955">// This is the destructor in C++</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opDrop</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Called after a raw copy operation from one value to another</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opPostCopy</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Called after a move semantic operation from one value to another. We'll see that later.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opPostMove</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Get value by slice [low..up]</span><span style="color:#0">
    </span><span style="color:#6A9955">// Must return a string or a slice</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opSlice</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, low, up: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">string</span><span style="color:#0"> { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#BB6643">"true"</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// Get value by index</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opIndex</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, index: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#3BC3A7">WhateverType</span><span style="color:#0"> { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">true</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// Called by @countof</span><span style="color:#0">
    </span><span style="color:#6A9955">// Use in a 'loop' block for example</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opCount</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">          { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">; }
    </span><span style="color:#6A9955">// Called by @dataof</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opData</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">)->*</span><span style="color:#3BC3A7">WhateverType</span><span style="color:#0">  { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">null</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// Called for explicit/implicit casting between struct value and return type</span><span style="color:#0">
    </span><span style="color:#6A9955">// Can be overloaded by a different return type</span><span style="color:#0">
    </span><span style="color:#6A9955">// Example: var x = cast(OneType) v</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opCast</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">)-></span><span style="color:#3BC3A7">OneType</span><span style="color:#0">      { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">true</span><span style="color:#0">; }
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opCast</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">)-></span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">  { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// Called to compare the struct value with something else</span><span style="color:#0">
    </span><span style="color:#6A9955">// Can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Returns true if they are equal, otherwise returns false</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '==', '!='</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opEquals</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">)-></span><span style="color:#ED9A11">bool</span><span style="color:#0">      { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">; }
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opEquals</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">)-></span><span style="color:#ED9A11">bool</span><span style="color:#0">  { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// Called to compare the struct value with something else</span><span style="color:#0">
    </span><span style="color:#6A9955">// Can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Returns -1, 0 or 1</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '<', '>', '<=', '>=', '<=>'</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opCmp</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">      { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">; }
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opCmp</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">  { </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">; }

    </span><span style="color:#6A9955">// Affect struct with another value</span><span style="color:#0">
    </span><span style="color:#6A9955">// Can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '='</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">) {}
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Affect struct with a literal value with a specified suffix</span><span style="color:#0">
    </span><span style="color:#6A9955">// Generic function, can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '='</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(suffix: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opAffectSuffix</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">) {}
    </span><span style="color:#3186CD">func</span><span style="color:#0">(suffix: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opAffectSuffix</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Affect struct with another value at a given index</span><span style="color:#0">
    </span><span style="color:#6A9955">// Can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '[] ='</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opIndexAffect</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, index: </span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">) {}
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">opIndexAffect</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, index: </span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Binary operator. 'op' generic argument contains the operator string</span><span style="color:#0">
    </span><span style="color:#6A9955">// Generic function, can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '+', '-', '*', '/', '%', '|', '&', '^', '<<', '>>'</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opBinary</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">)-></span><span style="color:#ED9A11">Self</span><span style="color:#0">     { </span><span style="color:#B040BE">return</span><span style="color:#0"> {</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}; }
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opBinary</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">)-></span><span style="color:#ED9A11">Self</span><span style="color:#0"> { </span><span style="color:#B040BE">return</span><span style="color:#0"> {</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}; }

    </span><span style="color:#6A9955">// Unary operator. 'op' generic argument contains the operator string (see below)</span><span style="color:#0">
    </span><span style="color:#6A9955">// Generic function</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '!', '-', '~'</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opUnary</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">)-></span><span style="color:#ED9A11">Self</span><span style="color:#0"> { </span><span style="color:#B040BE">return</span><span style="color:#0"> {</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}; }

    </span><span style="color:#6A9955">// Affect operator. 'op' generic argument contains the operator string (see below)</span><span style="color:#0">
    </span><span style="color:#6A9955">// Generic function, can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opAssign</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">) {}
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opAssign</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, other: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">)  {}

    </span><span style="color:#6A9955">// Affect operator. 'op' generic argument contains the operator string (see below)</span><span style="color:#0">
    </span><span style="color:#6A9955">// Generic function, can be overloaded</span><span style="color:#0">
    </span><span style="color:#6A9955">// Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opIndexAssign</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, index: </span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">OneType</span><span style="color:#0">) {}
    </span><span style="color:#3186CD">func</span><span style="color:#0">(op: </span><span style="color:#ED9A11">string</span><span style="color:#0">) </span><span style="color:#B4B44A">opIndexAssign</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, index: </span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">AnotherType</span><span style="color:#0">) {}

    </span><span style="color:#6A9955">// Called by a 'visit' block</span><span style="color:#0">
    </span><span style="color:#6A9955">// Can have multiple versions, by adding a name after 'opVisit'</span><span style="color:#0">
    </span><span style="color:#6A9955">// This is the way to go for iterators</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">func</span><span style="color:#0">(ptr: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) </span><span style="color:#B4B44A">opVisit</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, stmt: </span><span style="color:#ED9A11">code</span><span style="color:#0">) {}
        </span><span style="color:#3186CD">func</span><span style="color:#0">(ptr: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) </span><span style="color:#B4B44A">opVisitWhatever</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, stmt: </span><span style="color:#ED9A11">code</span><span style="color:#0">) {}
        </span><span style="color:#3186CD">func</span><span style="color:#0">(ptr: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) </span><span style="color:#B4B44A">opVisitAnother</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">, stmt: </span><span style="color:#ED9A11">code</span><span style="color:#0">) {}
    }
}</span></code></pre>
<h3 id="064__affectation">Affectation</h3>
<p>
<code>opAffect</code> is a way of assigning to a struct with <code>=</code>. You can have more the one <code>opAffect</code> with different types. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
{
    x, y, z: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">666</span><span style="color:#0">
}

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
{
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)  { x, y = value; }
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) { x, y = value ? </span><span style="color:#74A35B">1</span><span style="color:#0"> : </span><span style="color:#74A35B">0</span><span style="color:#0">; }
}</span></code></pre><pre><code><span style="color:#6A9955">// This will initialize v and then call opAffect(s32) with '4'</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0"> = </span><span style="color:#74A35B">4</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">4</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">4</span><span style="color:#0">)
</span><span style="color:#6A9955">// Note that variable 'v' is also initiliazed with the default values.</span><span style="color:#0">
</span><span style="color:#6A9955">// So here 'z' is still 666 because 'opAffect' does not change it.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.z == </span><span style="color:#74A35B">666</span><span style="color:#0">)

</span><span style="color:#6A9955">// This will call opAffect(bool) with 'true'</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> v1: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0"> = </span><span style="color:#3186CD">true</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v1.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v1.y == </span><span style="color:#74A35B">1</span><span style="color:#0">)

</span><span style="color:#6A9955">// This will call opAffect(bool) with 'false'</span><span style="color:#0">
v1 = </span><span style="color:#3186CD">false</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v1.x == </span><span style="color:#74A35B">0</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v1.y == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
If <code>opAffect</code> is supposed to initialize the full content of the struct, you can add <code>#[Swag.Complete]</code>. This will avoid every variables to be initialized to the default values, then changed later with the <code>opAffect</code> call. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#[Swag.Complete]</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)  { x, y, z = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) value; }

    </span><span style="color:#6A9955">// For later</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#[Swag.Implicit]</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)  { x, y = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">) value; }
}</span></code></pre><p>
Here the variable <code>v</code> will not be initialized prior to the affectation. This is more optimal, as there's only one initialization. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0"> = </span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.z == </span><span style="color:#74A35B">2</span><span style="color:#0">)</span></code></pre><pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">(v: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">)
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.z == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">titi</span><span style="color:#0">(v: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">)
{
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}

</span><span style="color:#6A9955">// By default, there's no automatic conversion for a function argument, so you must cast.</span><span style="color:#0">
</span><span style="color:#6A9955">// Here, this will create a 'Struct' on the stack, initialized with a call to 'opAffect(s32)'</span><span style="color:#0">
</span><span style="color:#FF6A00">toto</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">Struct</span><span style="color:#0">) </span><span style="color:#74A35B">5</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">)

</span><span style="color:#6A9955">// But if opAffect is marked with #[Swag.Implicit], automatic conversion can occur.</span><span style="color:#0">
</span><span style="color:#6A9955">// No need for an explicit cast.</span><span style="color:#0">
</span><span style="color:#FF6A00">toto</span><span style="color:#0">(</span><span style="color:#74A35B">5</span><span style="color:#0">'</span><span style="color:#ED9A11">u16</span><span style="color:#0">)</span></code></pre><p>
A structure can be converted to a constant by a call to <code>opAffect</code> if the function is marked with <code>Swag.ConstExpr</code>. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector2</span><span style="color:#0">
{
    x, y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
}

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector2</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opAffect</span><span style="color:#0">(one: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)
    {
        x, y = one
    }
}

</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">One</span><span style="color:#0">: </span><span style="color:#3BC3A7">Vector2</span><span style="color:#0"> = </span><span style="color:#74A35B">1.0</span><span style="color:#0">    </span><span style="color:#6A9955">// This will call opAffect(1.0) at compile time</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">One</span><span style="color:#0">.x == </span><span style="color:#74A35B">1.0</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">One</span><span style="color:#0">.y == </span><span style="color:#74A35B">1.0</span></code></pre>
<h3 id="064__count">Count</h3>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
}

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opCount</span><span style="color:#0">() => </span><span style="color:#74A35B">4</span><span style="color:#0">'</span><span style="color:#ED9A11">u64</span><span style="color:#0">
}</span></code></pre><p>
You can loop on a struct value if <code>opCount</code> has been defined. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> v = </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{}

</span><span style="color:#6A9955">// '@countof' will call 'opCount'</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(v) == </span><span style="color:#74A35B">4</span><span style="color:#0">)

</span><span style="color:#6A9955">// You can then loop on a struct value if 'opCount' has been defined</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#B040BE">loop</span><span style="color:#0"> v
    cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">4</span><span style="color:#0">)</span></code></pre>
<h3 id="064__post_copy_and_post_move">Post copy and post move</h3>
<p>
Swag accepts copy and move semantics for structures. In this examples, we use a <code>Vector3</code> to illustrate, even if of course that kind of struct does not need a move semantic, as there's no heap involved. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">
{
    x, y, z: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">666</span><span style="color:#0">
}

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// This is used for 'copy semantic'.</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opPostCopy</span><span style="color:#0">()
    {
        x, y, z += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// This is used for 'move semantic'.</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opPostMove</span><span style="color:#0">()
    {
        x, y, z += </span><span style="color:#74A35B">2</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// Just imagine that we have something to drop. Like a memory allocated buffer.</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#B4B44A">opDrop</span><span style="color:#0">()
    {
    }
}</span></code></pre><pre><code><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">{}
</span><span style="color:#3186CD">var</span><span style="color:#0"> b = </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">{</span><span style="color:#74A35B">100</span><span style="color:#0">, </span><span style="color:#74A35B">200</span><span style="color:#0">, </span><span style="color:#74A35B">300</span><span style="color:#0">}

</span><span style="color:#6A9955">// "copy semantic". The default behaviour.</span><span style="color:#0">
</span><span style="color:#6A9955">// 1. This will call 'opDrop' on 'a' if the function exists because 'a' could already be assigned.</span><span style="color:#0">
</span><span style="color:#6A9955">// 2. This will raw copy 'b' to 'a'.</span><span style="color:#0">
</span><span style="color:#6A9955">// 3. This will call 'opPostCopy' on 'a' if it exists.</span><span style="color:#0">
a = b
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.x == </span><span style="color:#74A35B">101</span><span style="color:#0">)     </span><span style="color:#6A9955">// +1 because of the call to opPostCopy</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.y == </span><span style="color:#74A35B">201</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.z == </span><span style="color:#74A35B">301</span><span style="color:#0">)

</span><span style="color:#6A9955">// "move semantic" by adding the modifier ',move' just after '='.</span><span style="color:#0">
</span><span style="color:#6A9955">// 1. This will call 'opDrop' on 'a' if it exists</span><span style="color:#0">
</span><span style="color:#6A9955">// 2. This will raw copy 'b' to 'a'</span><span style="color:#0">
</span><span style="color:#6A9955">// 3. This will call 'opPostMove' on 'a' if it exists</span><span style="color:#0">
</span><span style="color:#6A9955">// 4. This will reinitialize 'b' to the default values if 'opDrop' exists</span><span style="color:#0">
a =,move b
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.x == </span><span style="color:#74A35B">102</span><span style="color:#0">)     </span><span style="color:#6A9955">// +2 because of the call to opPostMove</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.y == </span><span style="color:#74A35B">202</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.z == </span><span style="color:#74A35B">302</span><span style="color:#0">)

</span><span style="color:#6A9955">// 'Vector3' contains an 'opDrop' special function, so 'b' will be reinitialized to</span><span style="color:#0">
</span><span style="color:#6A9955">// the default values after the 'move'. Default values are 666.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b.x == </span><span style="color:#74A35B">666</span><span style="color:#0">)

</span><span style="color:#6A9955">// If you know what you're doing, you can avoid the first call to 'opDrop' with '=,nodrop'</span><span style="color:#0">
</span><span style="color:#6A9955">// Do it when you know the state of 'a' and do not want an extra unnecessary call</span><span style="color:#0">
</span><span style="color:#6A9955">// (for example if 'a' is in an undetermined state).</span><span style="color:#0">

a =,nodrop b            </span><span style="color:#6A9955">// Copy b to a without dropping 'a' first</span><span style="color:#0">
a =,nodrop,move b       </span><span style="color:#6A9955">// Move b to a without dropping 'a' first</span><span style="color:#0">

</span><span style="color:#6A9955">// For the 'move semantic', you can avoid the last reinitialization by using '=,moveraw'. Of course, do this at your own risk, if you know that 'b' will never by dropped by the compiler or if you reinitialize its state yourself.</span><span style="color:#0">

</span><span style="color:#6A9955">// instead of '=,move'</span><span style="color:#0">
a =,moveraw b
a =,nodrop,moveraw b</span></code></pre><p>
<h4 id="moveref">moveref </h4></p>
<p>
<code>moveref</code> can be used instead of <code>ref</code> in a function parameter to declare a <b>move semantic</b> intention. </p>
<pre><code><span style="color:#6A9955">// This is the 'move' version of 'assign'. With 'moveref', we tell the compiler that this version will take the owership on 'from'.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">assign</span><span style="color:#0">(to: </span><span style="color:#3186CD">ref</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">, from: </span><span style="color:#3186CD">moveref</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">)
{
    to =,move from
}

</span><span style="color:#6A9955">// This is the normal 'copy' version. In this version, 'from' will not be changed, that's why it's constant (not a ref).</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">assign</span><span style="color:#0">(to: </span><span style="color:#3186CD">ref</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">, from: </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">)
{
    to = from
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">{</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">}
</span><span style="color:#3186CD">var</span><span style="color:#0"> b: </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">

</span><span style="color:#6A9955">// Call the 'copy' version of 'assign'.</span><span style="color:#0">
</span><span style="color:#FF6A00">assign</span><span style="color:#0">(&b, a)
</span><span style="color:#6A9955">// As this will call 'opPostCopy', we have +1 on each field.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b.x == </span><span style="color:#74A35B">2</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> b.y == </span><span style="color:#74A35B">3</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> b.z == </span><span style="color:#74A35B">4</span><span style="color:#0">)
</span><span style="color:#6A9955">// 'a' remains unchanged</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.x == </span><span style="color:#74A35B">1</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> a.y == </span><span style="color:#74A35B">2</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> a.z == </span><span style="color:#74A35B">3</span><span style="color:#0">)

</span><span style="color:#6A9955">// Now we tell the compiler to use the 'moveref' version of 'assign'.</span><span style="color:#0">
</span><span style="color:#FF6A00">assign</span><span style="color:#0">(&b, </span><span style="color:#3186CD">moveref</span><span style="color:#0"> &a)
</span><span style="color:#6A9955">// As 'opPostMove' has been called, we have +2 on each field.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b.x == </span><span style="color:#74A35B">3</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> b.y == </span><span style="color:#74A35B">4</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> b.z == </span><span style="color:#74A35B">5</span><span style="color:#0">)
</span><span style="color:#6A9955">// And as this is a move, then 'a' is now reinitialized to its default values.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a.x == </span><span style="color:#74A35B">666</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> a.y == </span><span style="color:#74A35B">666</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> a.z == </span><span style="color:#74A35B">666</span><span style="color:#0">)</span></code></pre>
<h3 id="064__visit">Visit</h3>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    x: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">
    y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">20</span><span style="color:#0">
    z: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">30</span><span style="color:#0">
}</span></code></pre><p>
You can visit a struct variable if a macro <code>opVisit</code> has been defined. This is the equivalent of an <b>iterator</b>. </p>
<p>
<code>opVisit</code> is a macro, so it should be marked with the <code>#[Swag.Macro]</code> attribute. <code>opVisit</code> is also a generic function which takes a constant generic parameter of type <code>bool</code>. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(ptr: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) </span><span style="color:#B4B44A">opVisit</span><span style="color:#0">(</span><span style="color:#ED9A11">self</span><span style="color:#0">, stmt: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#6A9955">// 'ptr' is a generic parameter that tells if we want to visit by pointer or by value.</span><span style="color:#0">
        </span><span style="color:#6A9955">// We do not use it in this example, so we check at compile time that it's not true.</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#if</span><span style="color:#0"> ptr </span><span style="color:#7F7F7F">#error</span><span style="color:#0"> </span><span style="color:#BB6643">"visiting myStruct by pointer is not supported"</span><span style="color:#0">

        </span><span style="color:#6A9955">// Loop on the 3 fields</span><span style="color:#0">
        </span><span style="color:#B040BE">loop</span><span style="color:#0"> idx: </span><span style="color:#74A35B">3</span><span style="color:#0">
        {
            </span><span style="color:#6A9955">// The '#macro' keyword forces its body to be in the scope of the caller</span><span style="color:#0">
            </span><span style="color:#7F7F7F">#macro</span><span style="color:#0">
            {
                </span><span style="color:#6A9955">// @alias0 will be the value</span><span style="color:#0">
                </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias0</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span><span style="color:#0">

                </span><span style="color:#6A9955">// As this code is in the caller scope, with need to add a '#up' before 'idx' to</span><span style="color:#0">
                </span><span style="color:#6A9955">// reference the variable of this function (and not a potential variable in</span><span style="color:#0">
                </span><span style="color:#6A9955">// the caller scope)</span><span style="color:#0">
                </span><span style="color:#B040BE">switch</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> idx
                {
                </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = </span><span style="color:#C3973B">#up</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">.x   </span><span style="color:#6A9955">// Same for function parameter 'self'</span><span style="color:#0">
                </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = </span><span style="color:#C3973B">#up</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">.y
                </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = </span><span style="color:#C3973B">#up</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">.z
                }

                </span><span style="color:#6A9955">// @alias1 will be the index</span><span style="color:#0">
                </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias1</span><span style="color:#0"> = </span><span style="color:#B4B44A">@index</span><span style="color:#0">

                </span><span style="color:#6A9955">// include user code</span><span style="color:#0">
                </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> stmt
            }
        }
    }
}</span></code></pre><p>
So now that the <code>opVisit</code> has been defined, we can <code>visit</code> it. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> myStruct = </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{}
</span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">

</span><span style="color:#6A9955">// Visiting each field in declaration order</span><span style="color:#0">
</span><span style="color:#6A9955">// 'v' is an alias for @alias0 (value)</span><span style="color:#0">
</span><span style="color:#6A9955">// 'i' is an alias for @alias1 (index)</span><span style="color:#0">
</span><span style="color:#B040BE">visit</span><span style="color:#0"> v, i: myStruct
{
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> i
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">20</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    }

    cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
}

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">3</span><span style="color:#0">)</span></code></pre><p>
You can have variants of <code>opVisit</code> by specifying an <b>additional name</b>. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0">(ptr: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) </span><span style="color:#B4B44A">opVisitReverse</span><span style="color:#0">(stmt: </span><span style="color:#ED9A11">code</span><span style="color:#0">)   </span><span style="color:#6A9955">// We add 'Reverse' in the name</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// Visit fields in reverse order (z, y then x)</span><span style="color:#0">
        </span><span style="color:#B040BE">loop</span><span style="color:#0"> idx: </span><span style="color:#74A35B">3</span><span style="color:#0">
        {
            </span><span style="color:#7F7F7F">#macro</span><span style="color:#0">
            {
                </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias0</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span><span style="color:#0">
                </span><span style="color:#B040BE">switch</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> idx
                {
                </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = </span><span style="color:#C3973B">#up</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">.z
                </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = </span><span style="color:#C3973B">#up</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">.y
                </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = </span><span style="color:#C3973B">#up</span><span style="color:#0"> </span><span style="color:#ED9A11">self</span><span style="color:#0">.x
                }

                </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias1</span><span style="color:#0"> = </span><span style="color:#B4B44A">@index</span><span style="color:#0">
                </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> stmt
            }
        }
    }
}</span></code></pre><pre><code><span style="color:#3186CD">var</span><span style="color:#0"> myStruct = </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{}
</span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">0</span><span style="color:#0">

</span><span style="color:#6A9955">// To call a variant, add the extra name between parenthesis.</span><span style="color:#0">
</span><span style="color:#B040BE">visit</span><span style="color:#0">(</span><span style="color:#3BC3A7">Reverse</span><span style="color:#0">) v, i: myStruct
{
    </span><span style="color:#B040BE">switch</span><span style="color:#0"> i
    {
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">30</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">20</span><span style="color:#0">)
    </span><span style="color:#B040BE">case</span><span style="color:#0"> </span><span style="color:#74A35B">2</span><span style="color:#0">: </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    }

    cpt += </span><span style="color:#74A35B">1</span><span style="color:#0">
}

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">3</span><span style="color:#0">)</span></code></pre>
<h3 id="067__offset">Offset</h3>
<p>

  You can force the layout of a field with the <code>Swag.Offset</code> attribute. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">

        </span><span style="color:#6A9955">// 'y' is located at the same offset as 'x', so they share the same space</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#[Swag.Offset("x")]</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// They are 2 fields in the struct but stored in only one s32, so the total size is 4 bytes.</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">

    </span><span style="color:#3186CD">var</span><span style="color:#0"> v = </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">{}
    v.x = </span><span style="color:#74A35B">666</span><span style="color:#0">

    </span><span style="color:#6A9955">// As 'y' and 'x' are sharing the same space, affecting to 'x' also affects to 'y'.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}</span></code></pre><p>

  An example to reference a field by index. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        x, y, z:    </span><span style="color:#ED9A11">f32</span><span style="color:#0">

        </span><span style="color:#7F7F7F">#[Swag.Offset("x")]</span><span style="color:#0">
        idx:        [</span><span style="color:#74A35B">3</span><span style="color:#0">] </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    v.x = </span><span style="color:#74A35B">10</span><span style="color:#0">; v.y = </span><span style="color:#74A35B">20</span><span style="color:#0">; v.z = </span><span style="color:#74A35B">30</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.idx[</span><span style="color:#74A35B">0</span><span style="color:#0">] == v.x)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.idx[</span><span style="color:#74A35B">1</span><span style="color:#0">] == v.y)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.idx[</span><span style="color:#74A35B">2</span><span style="color:#0">] == v.z)
}</span></code></pre>
<h3 id="068__packing">Packing</h3>
<p>
You can also control the struct layout with two attributes: <code>#[Swag.Pack]</code> and <code>#[Swag.Align]</code>. </p>
<p>

  The default struct packing is the same as in C: each field is aligned to the size of the type. This is the equivalent of <code>#[Swag.Pack(0)]</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0">     </span><span style="color:#6A9955">// offset 0: aligned to 1 byte</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">      </span><span style="color:#6A9955">// offset 4: s32 is aligned to 4 bytes (so here there's 3 bytes of padding before)</span><span style="color:#0">
        z: </span><span style="color:#ED9A11">s64</span><span style="color:#0">      </span><span style="color:#6A9955">// offset 8: aligned to 8 bytes</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// '@offsetof' can be used to retrieve the offset of a field</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">.x) == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">.y) == </span><span style="color:#74A35B">4</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">.z) == </span><span style="color:#74A35B">8</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">)     == </span><span style="color:#74A35B">16</span><span style="color:#0">
}</span></code></pre><p>

  You can <i>reduce</i> the packing of the fields with <code>#[Swag.Pack]</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Pack(1)]</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 0: 1 byte</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">  </span><span style="color:#6A9955">// offset 1: 4 bytes (no padding)</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">.x) == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">.y) == </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">)     == </span><span style="color:#74A35B">5</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#[Swag.Pack(2)]</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 0: 1 byte</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">  </span><span style="color:#6A9955">// offset 2: 4 bytes (1 byte of padding before)</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">.x) == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">.y) == </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">)     == </span><span style="color:#74A35B">6</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#[Swag.Pack(4)]</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct3</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 0: 1 byte</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">s64</span><span style="color:#0">  </span><span style="color:#6A9955">// offset 4: 8 bytes (3 bytes of padding before)</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct3</span><span style="color:#0">.x) == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct3</span><span style="color:#0">.y) == </span><span style="color:#74A35B">4</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct3</span><span style="color:#0">)     == </span><span style="color:#74A35B">12</span><span style="color:#0">
}</span></code></pre><p>

  The total struct size is always a multiple of the biggest alignement of the fields. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">  </span><span style="color:#6A9955">// 4 bytes</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// 1 byte</span><span style="color:#0">
        </span><span style="color:#6A9955">// 3 bytes of padding because of 'x', to aligned on 's32'</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">) == </span><span style="color:#74A35B">8</span><span style="color:#0">
}</span></code></pre><p>

  You can force the struct alignement with <code>#[Swag.Align]</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0">     </span><span style="color:#6A9955">// 1 byte</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">bool</span><span style="color:#0">     </span><span style="color:#6A9955">// 1 byte</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">.x) == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">.y) == </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">)     == </span><span style="color:#74A35B">2</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#[Swag.Align(8)]</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0">     </span><span style="color:#6A9955">// 1 byte</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">bool</span><span style="color:#0">     </span><span style="color:#6A9955">// 1 byte</span><span style="color:#0">
        </span><span style="color:#6A9955">// 6 bytes of padding to be a multiple of '8'</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">.x) == </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">.y) == </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">)     == </span><span style="color:#74A35B">8</span><span style="color:#0">
}</span></code></pre><p>

  You can also force each field to be aligned on a specific value. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 0: 1 byte</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#[Swag.Align(8)]</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 8: aligned to 8 (7 bytes of padding before)</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct1</span><span style="color:#0">) == </span><span style="color:#74A35B">9</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#[Swag.Align(8)]</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">
    {
        x: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 0: 1 byte</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#[Swag.Align(4)]</span><span style="color:#0">
        y: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> </span><span style="color:#6A9955">// offset 4: aligned to 4 (3 bytes of padding before)</span><span style="color:#0">
        </span><span style="color:#6A9955">// 3 bytes of padding to be a multiple of 8</span><span style="color:#0">
    }

    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyStruct2</span><span style="color:#0">) == </span><span style="color:#74A35B">8</span><span style="color:#0">
}</span></code></pre>
<h2 id="070_union">Union</h2>
<p>
An union is just a struct where all fields are located at offset 0. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">union</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyUnion</span><span style="color:#0">
    {
        x, y, z: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> v = </span><span style="color:#3BC3A7">MyUnion</span><span style="color:#0">{x = </span><span style="color:#74A35B">666</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">666</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.z == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}</span></code></pre>
<h2 id="075_interface">Interface</h2>
<p>
Interfaces are <b>virtual tables</b> (a list of function pointers) that can be associated to a struct. </p>
<p>
Unlike C++, the virtual table is not embedded with the struct. It is a separate object. You can then <i>implement</i> an interface for a given struct without changing the struct definition. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
{
    x, y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
}

</span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">
{
    x, y, z: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
}</span></code></pre><p>
Here we declare an interface, with two functions <code>set</code> and <code>reset</code>. </p>
<pre><code><span style="color:#3186CD">interface</span><span style="color:#0"> </span><span style="color:#3BC3A7">IReset</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// The first parameter must be 'self'</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#ED9A11">self</span><span style="color:#0">, val: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)

    </span><span style="color:#6A9955">// You can also use the 'mtd' declaration to avoid specifying the 'self' yourself</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#FF6A00">reset</span><span style="color:#0">()
}</span></code></pre><p>
You can implement an interface for any given struct with <code>impl</code> and <code>for</code>. For example here, we implement interface <code>IReset</code> for struct <code>Point2</code>. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">IReset</span><span style="color:#0"> </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// You must add 'impl' to indicate that you want to implement a function of the interface.</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#FF6A00">set</span><span style="color:#0">(val: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)
    {
        x = val
        y = val+</span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// Don't forget that 'mtd' is just syntaxic sugar. 'func' still works.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#FF6A00">reset</span><span style="color:#0">(</span><span style="color:#ED9A11">self</span><span style="color:#0">)
    {
        </span><span style="color:#ED9A11">self</span><span style="color:#0">.x, </span><span style="color:#ED9A11">self</span><span style="color:#0">.y = </span><span style="color:#74A35B">0</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// Not that you can also declare 'normal' functions or methods in an 'impl block'.</span><span style="color:#0">
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#FF6A00">myOtherMethod</span><span style="color:#0">()
    {
    }
}</span></code></pre><p>
And we implement interface <code>IReset</code> also for struct <code>Point3</code>. </p>
<pre><code><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">IReset</span><span style="color:#0"> </span><span style="color:#B040BE">for</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">
{
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#FF6A00">set</span><span style="color:#0">(val: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)
    {
        x = val
        y = val+</span><span style="color:#74A35B">1</span><span style="color:#0">
        z = val+</span><span style="color:#74A35B">2</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#FF6A00">reset</span><span style="color:#0">()
    {
        x, y, z = </span><span style="color:#74A35B">0</span><span style="color:#0">
    }
}</span></code></pre><p>
We can then use these interfaces on either <code>Vector2</code> or <code>Vector3</code>. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> pt2: </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> pt3: </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">

</span><span style="color:#6A9955">// To get the interface associated to a given struct, use the 'cast' operator.</span><span style="color:#0">
</span><span style="color:#6A9955">// If the compiler does not find the corresponding implementation, it will raise an error.</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt2
itf.</span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt2.x == </span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt2.y == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">)

itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt3
itf.</span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt3.x == </span><span style="color:#74A35B">10</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt3.y == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt3.z == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">)
itf.</span><span style="color:#FF6A00">reset</span><span style="color:#0">()
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt3.x == </span><span style="color:#74A35B">0</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> pt3.y == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
You can also access, with a normal call, all functions declared in an interface implementation block for a given struct. They are located in a dedicated scope. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> pt2: </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> pt3: </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">

pt2.</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">.</span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">)
pt2.</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">.</span><span style="color:#FF6A00">reset</span><span style="color:#0">()
pt3.</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">.</span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">)
pt3.</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">.</span><span style="color:#FF6A00">reset</span><span style="color:#0">()</span></code></pre><p>
An interface is a real type, with a size equivalent to 2 pointers. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> pt2: </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> pt3: </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt2
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(itf) == </span><span style="color:#74A35B">2</span><span style="color:#0"> * </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(*</span><span style="color:#ED9A11">void</span><span style="color:#0">)

</span><span style="color:#6A9955">// You can retrieve the concrete type associated with an interface instance with '@kindof'.</span><span style="color:#0">
itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt2
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(itf) == </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">)
itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt3
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(itf) == </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">)

</span><span style="color:#6A9955">// You can retrieve the concrete data associated with an interface instance with '@dataof'</span><span style="color:#0">
itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt2
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(itf) == &pt2)
itf = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">IReset</span><span style="color:#0">) pt3
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(itf) == &pt3)</span></code></pre>
<h2 id="100_function">Function</h2>

<h3 id="101__declaration">Declaration</h3>
<p>
A function declaration usually starts with the <code>func</code> keyword followed by the function name. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">()
{
}</span></code></pre><p>
If the function needs to return a value, you must add <code>-></code> followed by the type. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto1</span><span style="color:#0">()-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">0</span><span style="color:#0">
}</span></code></pre><p>
The return type can be deduced in case of a simple expression, by using <code>=></code> instead of <code>-></code>. Here the return type will be deduced to be <code>s32</code>. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x + y</span></code></pre><p>
A short version exists, in case of a function returning nothing. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">print</span><span style="color:#0">(val: </span><span style="color:#ED9A11">string</span><span style="color:#0">) = </span><span style="color:#B4B44A">@print</span><span style="color:#0">(val)</span></code></pre><p>
Parameters are specified after the function name, between parenthesis. Here we declare two parameters <code>x</code> and <code>y</code> of type <code>s32</code>, and one last parameter of type <code>f32</code>. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum1</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, unused: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#B040BE">return</span><span style="color:#0"> x + y
}</span></code></pre><p>
Parameters can have default values. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum2</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, unused: </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = </span><span style="color:#74A35B">666</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#B040BE">return</span><span style="color:#0"> x + y
}</span></code></pre><p>
The type of the parameters can be deduced if you specify a default value. Here <code>x</code> and <code>y</code> have the type <code>f32</code> because <code>0.0</code> is a 32 bits floating point literal. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum3</span><span style="color:#0">(x, y = </span><span style="color:#74A35B">0.0</span><span style="color:#0">)
{
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(y) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">
}

</span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Values</span><span style="color:#0"> { </span><span style="color:#3BC3A7">A</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y = </span><span style="color:#3BC3A7">Values</span><span style="color:#0">.</span><span style="color:#3BC3A7">A</span><span style="color:#0">)
{
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(y) == </span><span style="color:#3BC3A7">Values</span><span style="color:#0">
}</span></code></pre><pre><code><span style="color:#6A9955">// Functions can be nested inside other functions.</span><span style="color:#0">
</span><span style="color:#6A9955">// This are not closure but just functions in a sub scope.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sub</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x - y</span></code></pre><p>

  Simple call. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#FF6A00">sub</span><span style="color:#0">(</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>
You can name parameters, and don't have to respect parameters order in that case. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sub</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x - y

{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x1 = </span><span style="color:#FF6A00">sub</span><span style="color:#0">(x = </span><span style="color:#74A35B">5</span><span style="color:#0">, y = </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x1 == </span><span style="color:#74A35B">3</span><span style="color:#0">)
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x2 = </span><span style="color:#FF6A00">sub</span><span style="color:#0">(y = </span><span style="color:#74A35B">5</span><span style="color:#0">, x = </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x2 == -</span><span style="color:#74A35B">3</span><span style="color:#0">)
}

{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">returnMe</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">) => x + y * </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">returnMe</span><span style="color:#0">(x = </span><span style="color:#74A35B">10</span><span style="color:#0">) == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">returnMe</span><span style="color:#0">(y = </span><span style="color:#74A35B">10</span><span style="color:#0">) == </span><span style="color:#74A35B">20</span><span style="color:#0">)
}</span></code></pre><p>
<h4 id="Multiple return values">Multiple return values </h4></p>
<p>
</p>
<p>

  An <b>anonymous struct</b> can be used to return multiple values in a function. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">() -> {x, y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">}
    {
        </span><span style="color:#B040BE">return</span><span style="color:#0"> {</span><span style="color:#74A35B">1.0</span><span style="color:#0">, </span><span style="color:#74A35B">2.0</span><span style="color:#0">}
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> result = </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result.item0 == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result.item1 == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)

    </span><span style="color:#3186CD">var</span><span style="color:#0"> (x, y) = </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)

    </span><span style="color:#3186CD">var</span><span style="color:#0"> (z, w) = </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(z == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(w == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">returns2</span><span style="color:#0">() -> {x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">}
{
    </span><span style="color:#6A9955">// You can return a tuple literal as long as the types match</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> {</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}

    </span><span style="color:#6A9955">// Or use the specifal type 'retval' which is an typealias to the</span><span style="color:#0">
    </span><span style="color:#6A9955">// function return type (but not only, we'll see later)</span><span style="color:#0">
    </span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> result: </span><span style="color:#3186CD">retval</span><span style="color:#0">
    x = </span><span style="color:#74A35B">1</span><span style="color:#0">
    y = </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#B040BE">return</span><span style="color:#0"> result
}

</span><span style="color:#6A9955">// You can access the struct fields with the names or with 'item?'</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> result = </span><span style="color:#FF6A00">returns2</span><span style="color:#0">()
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result.item0 == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result.item1 == </span><span style="color:#74A35B">2</span><span style="color:#0">)

</span><span style="color:#6A9955">// You can deconstruct the returned struct</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> (x, y) = </span><span style="color:#FF6A00">returns2</span><span style="color:#0">()
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">2</span><span style="color:#0">)</span></code></pre>
<h3 id="102__lambda">Lambda</h3>
<p>
A lambda is just a <b>pointer to a function</b>. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunction0</span><span style="color:#0">() {}
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunction1</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x * x

</span><span style="color:#6A9955">// 'ptr0' is declared as a pointer to a function that takes no parameter, and returns nothing</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> ptr0: </span><span style="color:#3186CD">func</span><span style="color:#0">() = &myFunction0
</span><span style="color:#FF6A00">ptr0</span><span style="color:#0">()

</span><span style="color:#6A9955">// Here type of 'ptr1' is deduced from 'myFunction1'</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> ptr1 = &myFunction1
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunction1</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">ptr1</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">)</span></code></pre><p>
A lambda can be null. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> lambda: </span><span style="color:#3186CD">func</span><span style="color:#0">()-></span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(lambda == </span><span style="color:#3186CD">null</span><span style="color:#0">)</span></code></pre><p>
You can use a lambda as a function parameter type. </p>
<pre><code><span style="color:#3186CD">typealias</span><span style="color:#0"> callback = </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toDo</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, ptr: callback)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> => </span><span style="color:#FF6A00">ptr</span><span style="color:#0">(value)

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">square</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x * x
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, &square) == </span><span style="color:#74A35B">16</span><span style="color:#0">)</span></code></pre><p>
<h4 id="Anonymous functions">Anonymous functions </h4></p>
<p>
You can also create <i>anonymous functions</i> (aka functions as literals). </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> cb = </span><span style="color:#3186CD">func</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> => x * x
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">cb</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">) == </span><span style="color:#74A35B">16</span><span style="color:#0">)
cb = </span><span style="color:#3186CD">func</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> => x * x * x
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">cb</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">) == </span><span style="color:#74A35B">64</span><span style="color:#0">)</span></code></pre><p>
Anonymous functions can be passed as parameters to another function. </p>
<pre><code><span style="color:#3186CD">typealias</span><span style="color:#0"> callback = </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toDo</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, ptr: callback)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> => </span><span style="color:#FF6A00">ptr</span><span style="color:#0">(value)

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#3186CD">func</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x * x) == </span><span style="color:#74A35B">16</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#3186CD">func</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x + x) == </span><span style="color:#74A35B">8</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#3186CD">func</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> { </span><span style="color:#B040BE">return</span><span style="color:#0"> x - x; }) == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
The types of the parameters can be deduced. </p>
<pre><code><span style="color:#3186CD">typealias</span><span style="color:#0"> callback = </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toDo</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, ptr: callback)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> => </span><span style="color:#FF6A00">ptr</span><span style="color:#0">(value)

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#3186CD">func</span><span style="color:#0">(x) => x * x) == </span><span style="color:#74A35B">16</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#3186CD">func</span><span style="color:#0">(x) => x + x) == </span><span style="color:#74A35B">8</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toDo</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#3186CD">func</span><span style="color:#0">(x) { </span><span style="color:#B040BE">return</span><span style="color:#0"> x - x; }) == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
When you affect a lambda to a variable, the type of parameters and the return type can also be omitted, as they will be deduced from the variable type. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> fct: </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">bool</span><span style="color:#0">

fct = </span><span style="color:#3186CD">func</span><span style="color:#0">(x, y) => x == y
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fct</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">10</span><span style="color:#0">))

fct = </span><span style="color:#3186CD">func</span><span style="color:#0">(x, y) { </span><span style="color:#B040BE">return</span><span style="color:#0"> x != y; }
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fct</span><span style="color:#0">(</span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">120</span><span style="color:#0">))</span></code></pre><p>
Lambdas can have default parameters values. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#3186CD">func</span><span style="color:#0">(val = </span><span style="color:#3186CD">true</span><span style="color:#0">) {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(val == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    }

    </span><span style="color:#FF6A00">x</span><span style="color:#0">()</span></code></pre><pre><code><span style="color:#0">    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#3186CD">func</span><span style="color:#0">(val: </span><span style="color:#ED9A11">bool</span><span style="color:#0"> = </span><span style="color:#3186CD">true</span><span style="color:#0">)

        x = </span><span style="color:#3186CD">func</span><span style="color:#0">(val) {
            </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(val == </span><span style="color:#3186CD">true</span><span style="color:#0">)
        }

        </span><span style="color:#FF6A00">x</span><span style="color:#0">()
        </span><span style="color:#FF6A00">x</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
    }
}</span></code></pre>
<h3 id="103__closure">Closure</h3>
<p>
Swag supports a limited set of the <code>closure</code> concept. </p>
<p>
Only a given amount of bytes of capture are possible (for now 48 bytes). That way there's never an hidden allocation. Another limitation is that you can only capture 'simple' variables (no struct with <code>opDrop</code>, <code>opPostCopy</code>, <code>opPostMove</code> for example). </p>
<p>

  A closure is declared like a lambda, with the captured variables between <code>|...|</code> before the function parameters. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">125</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#74A35B">521</span><span style="color:#0">

    </span><span style="color:#6A9955">// Capture 'a' and 'b'</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> fct: </span><span style="color:#3186CD">closure</span><span style="color:#0">() = </span><span style="color:#3186CD">closure</span><span style="color:#0">|a, b|()
    {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">125</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">521</span><span style="color:#0">)
    }

    </span><span style="color:#FF6A00">fct</span><span style="color:#0">()
}</span></code></pre><p>

  You can also capture by pointer with <code>&</code> (otherwise it's a copy). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">125</span><span style="color:#0">

    </span><span style="color:#6A9955">// Capture 'a' by pointer</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> fct: </span><span style="color:#3186CD">closure</span><span style="color:#0">() = </span><span style="color:#3186CD">closure</span><span style="color:#0">|&a|()
    {
        </span><span style="color:#6A9955">// We can change the value of the local variable 'a'</span><span style="color:#0">
        </span><span style="color:#3186CD">dref</span><span style="color:#0"> a += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#FF6A00">fct</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">126</span><span style="color:#0">)
    </span><span style="color:#FF6A00">fct</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">127</span><span style="color:#0">)
}</span></code></pre><p>

  You can also capture by reference with <code>ref</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">125</span><span style="color:#0">

    </span><span style="color:#6A9955">// Capture 'a' by reference</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> fct = </span><span style="color:#3186CD">closure</span><span style="color:#0">|</span><span style="color:#3186CD">ref</span><span style="color:#0"> a|()
    {
        </span><span style="color:#6A9955">// We can change the value of the local variable 'a'</span><span style="color:#0">
        a += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#FF6A00">fct</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">126</span><span style="color:#0">)
    </span><span style="color:#FF6A00">fct</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">127</span><span style="color:#0">)
}</span></code></pre><p>

  You can assign a normal lambda (no capture) to a closure variable. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> fct: </span><span style="color:#3186CD">closure</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">

    fct = </span><span style="color:#3186CD">func</span><span style="color:#0">(x, y) => x + y
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fct</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  You can capture arrays, structs, slices etc... as long as it fits in the maximum storage of 'n' bytes (and as long as the struct is a pod). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">]

    </span><span style="color:#3186CD">var</span><span style="color:#0"> fct: </span><span style="color:#3186CD">closure</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">

    </span><span style="color:#6A9955">// Capture the array 'x' by making a copy</span><span style="color:#0">
    fct = </span><span style="color:#3186CD">closure</span><span style="color:#0">|x|(toAdd)
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> res = </span><span style="color:#74A35B">0</span><span style="color:#0">
        </span><span style="color:#B040BE">visit</span><span style="color:#0"> v: x
            res += v
        res += toAdd
        </span><span style="color:#B040BE">return</span><span style="color:#0"> res
    }

    </span><span style="color:#3186CD">let</span><span style="color:#0"> result = </span><span style="color:#FF6A00">fct</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(result == </span><span style="color:#74A35B">1</span><span style="color:#0"> + </span><span style="color:#74A35B">2</span><span style="color:#0"> + </span><span style="color:#74A35B">3</span><span style="color:#0"> + </span><span style="color:#74A35B">4</span><span style="color:#0">)
}</span></code></pre><p>

  Captured variables are mutable, and part of the closure. So you can modify them. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">getInc</span><span style="color:#0">()-></span><span style="color:#3186CD">closure</span><span style="color:#0">()-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">10</span><span style="color:#0">

        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">closure</span><span style="color:#0">|x|()-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> {
            x += </span><span style="color:#74A35B">1</span><span style="color:#0">
            </span><span style="color:#B040BE">return</span><span style="color:#0"> x
        }
    }

    </span><span style="color:#3186CD">let</span><span style="color:#0"> fct = </span><span style="color:#FF6A00">getInc</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fct</span><span style="color:#0">() == </span><span style="color:#74A35B">11</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fct</span><span style="color:#0">() == </span><span style="color:#74A35B">12</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">fct</span><span style="color:#0">() == </span><span style="color:#74A35B">13</span><span style="color:#0">)</span></code></pre><pre><code><span style="color:#0">}</span></code></pre>
<h3 id="104__mixin">Mixin</h3>
<p>

  A mixin is declared like a function, with the attribute <code>#[Swag.Mixin]</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">()
    {
    }
}</span></code></pre><p>

  A mixin function is inserted in the scope of the caller. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">()
    {
        a += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">()   </span><span style="color:#6A9955">// Equivalent to 'a += 1'</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">()   </span><span style="color:#6A9955">// Equivalent to 'a += 1'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  This behaves like a function, so you can add parameters. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">(increment: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">)
    {
        a += increment
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">()   </span><span style="color:#6A9955">// Equivalent to 'a += 1'</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMixin</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">)  </span><span style="color:#6A9955">// Equivalent to 'a += 2'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  A mixin accepts parameters of type <code>code</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">doItTwice</span><span style="color:#0">(what: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#6A9955">// You can then insert the code with '#mixin'</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> what
        </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> what
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">doItTwice</span><span style="color:#0">(</span><span style="color:#C3973B">#code</span><span style="color:#0"> {a += </span><span style="color:#74A35B">1</span><span style="color:#0">;})
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  When the last parameter of a mixin is of type <code>code</code>, the caller can declare that code in a statement just after the call. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">doItTwice</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, what: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> what
        </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> what
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">doItTwice</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">)
    {
        a += value
    }

    </span><span style="color:#FF6A00">doItTwice</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">)
    {
        a += value
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">12</span><span style="color:#0">)
}</span></code></pre><p>

  You can use the special name <code>@alias</code> to create a named alias for an identifier. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">inc10</span><span style="color:#0">()
    {
        </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> += </span><span style="color:#74A35B">10</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a, b = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">inc10</span><span style="color:#0">(|a|)  </span><span style="color:#6A9955">// Passing alias name 'a'</span><span style="color:#0">
    </span><span style="color:#FF6A00">inc10</span><span style="color:#0">(|b|)  </span><span style="color:#6A9955">// Passing alias name 'b'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == b </span><span style="color:#B040BE">and</span><span style="color:#0"> b == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}

{
    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">setVar</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
    {
        </span><span style="color:#3186CD">let</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = value
    }

    </span><span style="color:#FF6A00">setVar</span><span style="color:#0">(|a| </span><span style="color:#74A35B">10</span><span style="color:#0">)  </span><span style="color:#6A9955">// Passing alias name 'a'</span><span style="color:#0">
    </span><span style="color:#FF6A00">setVar</span><span style="color:#0">(|b| </span><span style="color:#74A35B">20</span><span style="color:#0">)  </span><span style="color:#6A9955">// Passing alias name 'b'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">20</span><span style="color:#0">)
    </span><span style="color:#FF6A00">setVar</span><span style="color:#0">(</span><span style="color:#74A35B">30</span><span style="color:#0">)      </span><span style="color:#6A9955">// No typealias, so name is @alias0</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> == </span><span style="color:#74A35B">30</span><span style="color:#0">)
}</span></code></pre><p>

  You can declare special variables named <code>@mixin?</code>. Those variables will have a unique name each time the mixin is used. So the same mixin, even if it declares local variables, can be used multiple time in the same scope. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> total: </span><span style="color:#ED9A11">s32</span><span style="color:#0">

    </span><span style="color:#7F7F7F">#[Swag.Mixin]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toScope</span><span style="color:#0">()
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@mixin0</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">
        total += </span><span style="color:#B4B44A">@mixin0</span><span style="color:#0">
    }

    </span><span style="color:#FF6A00">toScope</span><span style="color:#0">()
    </span><span style="color:#FF6A00">toScope</span><span style="color:#0">()
    </span><span style="color:#FF6A00">toScope</span><span style="color:#0">()

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(total == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre>
<h3 id="105__macro">Macro</h3>
<p>

  A macro, like a mixin, is declared like a function, but with the attribute <code>Swag.Macro</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()
    {
    }
}</span></code></pre><p>

  Unlike a mixin, a macro has its own scope, and cannot conflict with the function it is inserted inside. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">666</span><span style="color:#0">    </span><span style="color:#6A9955">// 'a' is declared in the scope of 'myMacro'</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">let</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()   </span><span style="color:#6A9955">// no conflict with the 'a' declared above</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">0</span><span style="color:#0">)
}</span></code></pre><p>

  But you can force an identifier to be found <b>outside</b> of the scope of the macro with <code>#up</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()
    {
        </span><span style="color:#C3973B">#up</span><span style="color:#0"> a += </span><span style="color:#74A35B">1</span><span style="color:#0"> </span><span style="color:#6A9955">// Add '#up' before the identifier to reference the 'a' of the caller</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()   </span><span style="color:#6A9955">// This will change the 'a' above</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()   </span><span style="color:#6A9955">// This will change the 'a' above</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  Like a mixin, macro accepts <code>code</code> parameters. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">(what: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> what
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">

    </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">(</span><span style="color:#C3973B">#code</span><span style="color:#0"> { </span><span style="color:#C3973B">#up</span><span style="color:#0"> a += </span><span style="color:#74A35B">1</span><span style="color:#0">; } )

    </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()
    {
        </span><span style="color:#C3973B">#up</span><span style="color:#0"> a += </span><span style="color:#74A35B">1</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  You can use <code>#macro</code> inside a macro to force the code after to be in the same scope of the caller. That is, no <code>#up</code> is necessary to reference variables of the caller. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">(what: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#6A9955">// No conflict, in its own scope</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">666</span><span style="color:#0">

        </span><span style="color:#6A9955">// Isolate the caller code, to avoid conflicts with the macro internals</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#macro</span><span style="color:#0">
        {
            </span><span style="color:#6A9955">// In the scope of the caller</span><span style="color:#0">
            </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> what
        }
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#FF6A00">myMacro</span><span style="color:#0">()
    {
        a += </span><span style="color:#74A35B">2</span><span style="color:#0">  </span><span style="color:#6A9955">// will reference the 'a' above because this code has been inserted inside '#macro'</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  You can extend the language with macros, without using pointers to functions (no lambda call cost). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">repeat</span><span style="color:#0">(count: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, what: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
        </span><span style="color:#B040BE">while</span><span style="color:#0"> a < count
        {
            </span><span style="color:#7F7F7F">#macro</span><span style="color:#0">
            {
                </span><span style="color:#3186CD">var</span><span style="color:#0"> index = </span><span style="color:#C3973B">#up</span><span style="color:#0"> a
                </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> what
            }

            a += </span><span style="color:#74A35B">1</span><span style="color:#0">
        }
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">repeat</span><span style="color:#0">(</span><span style="color:#74A35B">5</span><span style="color:#0">) { a += index; }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">0</span><span style="color:#0">+</span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#FF6A00">repeat</span><span style="color:#0">(</span><span style="color:#74A35B">3</span><span style="color:#0">) { a += index; }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">10</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  When you need <code>break</code> in the user code to break outside of a multi loop. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">repeatSquare</span><span style="color:#0">(count: </span><span style="color:#ED9A11">u32</span><span style="color:#0">, what: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#7F7F7F">#scope</span><span style="color:#0"> </span><span style="color:#3BC3A7">Up</span><span style="color:#0"> </span><span style="color:#B040BE">loop</span><span style="color:#0"> count
        {
            </span><span style="color:#B040BE">loop</span><span style="color:#0"> count
            {
                </span><span style="color:#7F7F7F">#macro</span><span style="color:#0">
                {
                    </span><span style="color:#6A9955">// 'break' in the user code will be replaced by 'break Up'</span><span style="color:#0">
                    </span><span style="color:#6A9955">// So it will break outside the outer loop</span><span style="color:#0">
                    </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> </span><span style="color:#C3973B">#up</span><span style="color:#0"> what { </span><span style="color:#B040BE">break</span><span style="color:#0"> = </span><span style="color:#B040BE">break</span><span style="color:#0"> </span><span style="color:#3BC3A7">Up</span><span style="color:#0">; }
                }
            }
        }
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> a = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#FF6A00">repeatSquare</span><span style="color:#0">(</span><span style="color:#74A35B">5</span><span style="color:#0">)
    {
        a += </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B040BE">if</span><span style="color:#0"> a == </span><span style="color:#74A35B">10</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(a == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}</span></code></pre><p>

  In a macro, you can use special variables named <code>@alias<num></code>. Note that this is also valid for mixins. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Macro]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">call</span><span style="color:#0">(v: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, stmt: </span><span style="color:#ED9A11">code</span><span style="color:#0">)
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> = v
        </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#B4B44A">@alias1</span><span style="color:#0"> = v * </span><span style="color:#74A35B">2</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#mixin</span><span style="color:#0"> stmt
    }

    </span><span style="color:#FF6A00">call</span><span style="color:#0">(</span><span style="color:#74A35B">20</span><span style="color:#0">)
    {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias0</span><span style="color:#0"> == </span><span style="color:#74A35B">20</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias1</span><span style="color:#0"> == </span><span style="color:#74A35B">40</span><span style="color:#0">)
    }

    </span><span style="color:#6A9955">// The caller can then name those special variables</span><span style="color:#0">
    </span><span style="color:#6A9955">// Use |name0, name1, ...| before the function call parameters</span><span style="color:#0">
    </span><span style="color:#FF6A00">call</span><span style="color:#0">(|x| </span><span style="color:#74A35B">20</span><span style="color:#0">)
    {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">20</span><span style="color:#0">)        </span><span style="color:#6A9955">// x is @alias0</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@alias1</span><span style="color:#0"> == </span><span style="color:#74A35B">40</span><span style="color:#0">)  </span><span style="color:#6A9955">// @alias1 is not renamed</span><span style="color:#0">
    }

    </span><span style="color:#FF6A00">call</span><span style="color:#0">(|x, y| </span><span style="color:#74A35B">20</span><span style="color:#0">)
    {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">20</span><span style="color:#0">)   </span><span style="color:#6A9955">// x is @alias0</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y == </span><span style="color:#74A35B">40</span><span style="color:#0">)   </span><span style="color:#6A9955">// y is @alias1</span><span style="color:#0">
    }
}</span></code></pre>
<h3 id="105__variadic_parameters">Variadic parameters</h3>
<p>

  A function can take a variable number of arguments with <code>...</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">(value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">, parameters: ...)
    {
    }

    </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#BB6643">"true"</span><span style="color:#0">, </span><span style="color:#74A35B">5.6</span><span style="color:#0">)
}</span></code></pre><p>

  In that case, <code>parameters</code> is a slice of <code>any</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">(parameters: ...)
    {
        </span><span style="color:#6A9955">// We can know the number of parameters at runtime with '@countof'</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(parameters) == </span><span style="color:#74A35B">3</span><span style="color:#0">)

        </span><span style="color:#6A9955">// Each parameter is a type 'any'</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">0</span><span style="color:#0">]) == </span><span style="color:#ED9A11">any</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">1</span><span style="color:#0">]) == </span><span style="color:#ED9A11">any</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">2</span><span style="color:#0">]) == </span><span style="color:#ED9A11">any</span><span style="color:#0">

        </span><span style="color:#6A9955">// But you can use '@kindof' to get the real type (at runtime) of the parameter</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">0</span><span style="color:#0">]) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">1</span><span style="color:#0">]) == </span><span style="color:#ED9A11">string</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">2</span><span style="color:#0">]) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">)
    }

    </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">, </span><span style="color:#BB6643">"true"</span><span style="color:#0">, </span><span style="color:#74A35B">5.6</span><span style="color:#0">)
}</span></code></pre><p>

  If all variadic parameters are of the same type, you can force it. Parameters then won't be of type <code>any</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">(value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">, parameters: </span><span style="color:#ED9A11">s32</span><span style="color:#0">...)
    {
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">0</span><span style="color:#0">]).name == </span><span style="color:#BB6643">"s32"</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">1</span><span style="color:#0">]).name == </span><span style="color:#BB6643">"s32"</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">2</span><span style="color:#0">]) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">3</span><span style="color:#0">]) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">

        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">10</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">20</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">30</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(parameters[</span><span style="color:#74A35B">3</span><span style="color:#0">] == </span><span style="color:#74A35B">40</span><span style="color:#0">)
    }

    </span><span style="color:#FF6A00">myFunction</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">, </span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">, </span><span style="color:#74A35B">40</span><span style="color:#0">)
}</span></code></pre><p>

  Variadic parameters can be passed from function to function. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0">(params: ...)
    {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(params) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(params[</span><span style="color:#74A35B">0</span><span style="color:#0">]) == </span><span style="color:#ED9A11">string</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">(params[</span><span style="color:#74A35B">1</span><span style="color:#0">]) == </span><span style="color:#ED9A11">bool</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">string</span><span style="color:#0">) params[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#BB6643">"value"</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">bool</span><span style="color:#0">) params[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    }

    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">B</span><span style="color:#0">(params: ...)
    {
        </span><span style="color:#3BC3A7">A</span><span style="color:#0">(params)
    }

    </span><span style="color:#3BC3A7">B</span><span style="color:#0">(</span><span style="color:#BB6643">"value"</span><span style="color:#0">, </span><span style="color:#3186CD">true</span><span style="color:#0">)
}</span></code></pre><p>
You can spread the content of an array or a slice to variadic parameters with <code>@spread</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(params: </span><span style="color:#ED9A11">s32</span><span style="color:#0">...)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// Note that variadic parameters can be visited, as this is a slice</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> total = </span><span style="color:#74A35B">0</span><span style="color:#0">
        </span><span style="color:#B040BE">visit</span><span style="color:#0"> v: params
            total += v
        </span><span style="color:#B040BE">return</span><span style="color:#0"> total
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">]
    </span><span style="color:#3186CD">let</span><span style="color:#0"> res = </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#B4B44A">@spread</span><span style="color:#0">(arr)) </span><span style="color:#6A9955">// is equivalent to sum(1, 2, 3, 4)</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res == </span><span style="color:#74A35B">1</span><span style="color:#0">+</span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">+</span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> res1 = </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#B4B44A">@spread</span><span style="color:#0">(arr[</span><span style="color:#74A35B">1</span><span style="color:#0">..</span><span style="color:#74A35B">2</span><span style="color:#0">])) </span><span style="color:#6A9955">// is equivalent to sum(2, 3)</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res1 == </span><span style="color:#74A35B">2</span><span style="color:#0">+</span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre>
<h3 id="106__ufcs">Ufcs</h3>
<p>
<i>ufcs</i> stands for <i>uniform function call syntax</i>. It allows every functions to be called with a <code>param.func()</code> form when the first parameter of <code>func()</code> is of the same type as <code>param</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(param: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) => param

    </span><span style="color:#3186CD">let</span><span style="color:#0"> b = </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(b) == b.</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">())
}</span></code></pre><p>

  This means that in Swag, there are only <i>static</i> functions, but which can be called like <i>methods</i>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point</span><span style="color:#0"> { x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; }
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> pt: *</span><span style="color:#3BC3A7">Point</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) { x, y = value; }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">

    </span><span style="color:#6A9955">// Ufcs call</span><span style="color:#0">
    pt.</span><span style="color:#FF6A00">set</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">10</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> pt.y == </span><span style="color:#74A35B">10</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Normal call</span><span style="color:#0">
    </span><span style="color:#FF6A00">set</span><span style="color:#0">(&pt, </span><span style="color:#74A35B">20</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">20</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> pt.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)
}</span></code></pre>
<h3 id="107__constexpr">Constexpr</h3>
<p>
A function marked with <code>Swag.ConstExpr</code> can be executed by the compiler if it can. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">) => x + y</span></code></pre><p>
Here <code>G</code> will be baked to 3 by the compiler. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> = </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> == </span><span style="color:#74A35B">3</span></code></pre><p>
If a function is not <code>ConstExpr</code>, you can force the compile time call with <code>#run</code>. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">mul</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">f32</span><span style="color:#0">) => x * y
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">G1</span><span style="color:#0"> = </span><span style="color:#FF6A00">#run</span><span style="color:#0"> </span><span style="color:#FF6A00">mul</span><span style="color:#0">(</span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">6</span><span style="color:#0">)
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">G1</span><span style="color:#0"> == </span><span style="color:#74A35B">18</span></code></pre>
<h3 id="108__function_overloading">Function overloading</h3>
<p>
Functions can have the same names as long as their parameters are different. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x + y
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x, y, z: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x + y + z
}

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">3</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">) == </span><span style="color:#74A35B">6</span></code></pre><p>
Note that in Swag, there is no implicit cast for function parameters. So you must always specify the right type. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">over</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x + y
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">over</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s64</span><span style="color:#0">) => x * y

</span><span style="color:#6A9955">// This would generate an error because it's ambiguous, as 1 and 2 are not strong types</span><span style="color:#0">
</span><span style="color:#6A9955">// var res0 = over(1, 2)</span><span style="color:#0">

</span><span style="color:#3186CD">let</span><span style="color:#0"> res0 = </span><span style="color:#FF6A00">over</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res0 == </span><span style="color:#74A35B">3</span><span style="color:#0">)
</span><span style="color:#3186CD">let</span><span style="color:#0"> res1 = </span><span style="color:#FF6A00">over</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s64</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s64</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res1 == </span><span style="color:#74A35B">2</span><span style="color:#0">)</span></code></pre>
<h3 id="109__discard">Discard</h3>
<p>
By default, you must always use the returned value of a function, otherwise the compiler will generate an error. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x + y

</span><span style="color:#6A9955">// This would generated an error, because the return value of 'sum' is not used</span><span style="color:#0">
</span><span style="color:#6A9955">// sum(2, 3)</span><span style="color:#0">

</span><span style="color:#6A9955">// To force the return value to be ignored, you can use 'discard' at the call site</span><span style="color:#0">
</span><span style="color:#3186CD">discard</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">)</span></code></pre><p>
If a function authorizes the caller to not use its return value, because it's not that important, it can be marked with <code>Swag.Discardable</code>. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Discardable]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">mul</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0"> => x * y

</span><span style="color:#6A9955">// This is fine to ignore the return value of 'mul' (even if strange)</span><span style="color:#0">
</span><span style="color:#FF6A00">mul</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">)</span></code></pre>
<h3 id="110__retval">Retval</h3>
<p>
Inside a function, you can use the <code>retval</code> type which is an alias to the function return type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">()-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> result: </span><span style="color:#3186CD">retval</span><span style="color:#0">
        result = </span><span style="color:#74A35B">10</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> result
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toto</span><span style="color:#0">() == </span><span style="color:#74A35B">10</span><span style="color:#0">)
}</span></code></pre><p>
But <code>retval</code> will also make a direct reference to the caller storage, to avoid an unnecessary copy (if possible). So this is mostly a hint for the compiler, and usefull when the function returns a complexe type like a struct, a tuple or an array. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0"> { x, y, z: </span><span style="color:#ED9A11">f64</span><span style="color:#0">; }

    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">getWhite</span><span style="color:#0">()-></span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// To avoid the clear of the returned struct, we use = undefined</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> result: </span><span style="color:#3186CD">retval</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span><span style="color:#0">
        result.x = </span><span style="color:#74A35B">0.5</span><span style="color:#0">
        result.y = </span><span style="color:#74A35B">0.1</span><span style="color:#0">
        result.z = </span><span style="color:#74A35B">1.0</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> result
    }

    </span><span style="color:#6A9955">// Here the 'getWhite' function can directly modify r, g and b without storing</span><span style="color:#0">
    </span><span style="color:#6A9955">// a temporary value on the stack.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> (r, g, b) = </span><span style="color:#FF6A00">getWhite</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(r == </span><span style="color:#74A35B">0.5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(g == </span><span style="color:#74A35B">0.1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(b == </span><span style="color:#74A35B">1.0</span><span style="color:#0">)
}</span></code></pre><p>

  This is the preferred way (because optimal) to return a struct or an array. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">()->[</span><span style="color:#74A35B">255</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// To avoid the clear of the array, we use = undefined</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> result: </span><span style="color:#3186CD">retval</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span><span style="color:#0">
        </span><span style="color:#B040BE">loop</span><span style="color:#0"> i: </span><span style="color:#74A35B">255</span><span style="color:#0">
            result[i] = i
        </span><span style="color:#B040BE">return</span><span style="color:#0"> result
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> arr = </span><span style="color:#FF6A00">toto</span><span style="color:#0">()
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr[</span><span style="color:#74A35B">100</span><span style="color:#0">] == </span><span style="color:#74A35B">100</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(arr[</span><span style="color:#74A35B">254</span><span style="color:#0">] == </span><span style="color:#74A35B">254</span><span style="color:#0">)
}</span></code></pre>
<h3 id="111__foreign">Foreign</h3>
<p>
Swag can interop with external "modules" (dlls under windows), which contain exported C functions. </p>
<p>
Put a special attribute <code>Swag.Foreign</code> before the function prototype, and specify the module name where the function is located. </p>
<p>
The module name can be a swag compiled module, or an external system module (where the location depends on the OS). </p>
<p>
In the case below, the function is located in <code>kernel32.dll</code> (under windows) </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Foreign("kernel32")]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">ExitProcess</span><span style="color:#0">(uExitCode: </span><span style="color:#ED9A11">u32</span><span style="color:#0">);

</span><span style="color:#6A9955">// Like for other attributes, you can use a block.</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Swag.Foreign("kernel32")]</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">ExitProcess</span><span style="color:#0">(uExitCode: </span><span style="color:#ED9A11">u32</span><span style="color:#0">);
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">Sleep</span><span style="color:#0">(dwMilliseconds: </span><span style="color:#ED9A11">u32</span><span style="color:#0">);
}</span></code></pre><p>
Note that in the case of an external module, you will have to declare somewhere the imported library too. </p>
<p>
<code>#foreignlib</code> is here to force a link to the given library (when generating executables). </p>
<pre><code><span style="color:#7F7F7F">#foreignlib</span><span style="color:#0"> </span><span style="color:#BB6643">"kernel32"</span></code></pre>
<h2 id="120_intrinsics">Intrinsics</h2>
<p>
This is the list of all intrinsics. All intrinsics start with <code>@</code>, which is reserved for them. </p>
<pre><code><span style="color:#7F7F7F">#global</span><span style="color:#0"> skip</span></code></pre><p>
<h3 id="Base">Base </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">);
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@breakpoint</span><span style="color:#0">();
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@getcontext</span><span style="color:#0">()->*</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">Context</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@setcontext</span><span style="color:#0">(context: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">Context</span><span style="color:#0">);
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@isbytecode</span><span style="color:#0">()-></span><span style="color:#ED9A11">bool</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@compiler</span><span style="color:#0">()-></span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">ICompiler</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#B4B44A">@args</span><span style="color:#0">()-></span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">string</span><span style="color:#0">;
</span><span style="color:#B4B44A">@panic</span><span style="color:#0">()
</span><span style="color:#B4B44A">@compilererror</span><span style="color:#0">()
</span><span style="color:#B4B44A">@compilerwarning</span><span style="color:#0">()</span></code></pre><p>
<h3 id="Buildin">Buildin </h3></p>
<p>
</p>
<pre><code><span style="color:#B4B44A">@spread</span><span style="color:#0">()
</span><span style="color:#B4B44A">@init</span><span style="color:#0">()
</span><span style="color:#B4B44A">@drop</span><span style="color:#0">()
</span><span style="color:#B4B44A">@postmove</span><span style="color:#0">()
</span><span style="color:#B4B44A">@postcopy</span><span style="color:#0">()
</span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@alignof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@offsetof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@kindof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@countof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@stringof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@dataof</span><span style="color:#0">()
</span><span style="color:#B4B44A">@mkslice</span><span style="color:#0">()
</span><span style="color:#B4B44A">@mkstring</span><span style="color:#0">()
</span><span style="color:#B4B44A">@mkany</span><span style="color:#0">()
</span><span style="color:#B4B44A">@mkinterface</span><span style="color:#0">()
</span><span style="color:#B4B44A">@mkcallback</span><span style="color:#0">()
</span><span style="color:#B4B44A">@getpinfos</span><span style="color:#0">()
</span><span style="color:#B4B44A">@isconstexpr</span><span style="color:#0">()
</span><span style="color:#B4B44A">@itftableof</span><span style="color:#0">()

</span><span style="color:#B4B44A">@index</span><span style="color:#0">
</span><span style="color:#B4B44A">@errmsg</span></code></pre><p>
<h3 id="Memory related">Memory related </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@alloc</span><span style="color:#0">(size: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)->*</span><span style="color:#ED9A11">void</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@realloc</span><span style="color:#0">(ptr: *</span><span style="color:#ED9A11">void</span><span style="color:#0">, size: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)->*</span><span style="color:#ED9A11">void</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@free</span><span style="color:#0">(ptr: *</span><span style="color:#ED9A11">void</span><span style="color:#0">);
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@memset</span><span style="color:#0">(dst: *</span><span style="color:#ED9A11">void</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">, size: </span><span style="color:#ED9A11">u64</span><span style="color:#0">);
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@memcpy</span><span style="color:#0">(dst: *</span><span style="color:#ED9A11">void</span><span style="color:#0">, src: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">void</span><span style="color:#0">, size: </span><span style="color:#ED9A11">u64</span><span style="color:#0">);
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@memmove</span><span style="color:#0">(dst: *</span><span style="color:#ED9A11">void</span><span style="color:#0">, src: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">void</span><span style="color:#0">, size: </span><span style="color:#ED9A11">u64</span><span style="color:#0">);
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@memcmp</span><span style="color:#0">(dst, src: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">void</span><span style="color:#0">, size: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@strlen</span><span style="color:#0">(value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;</span></code></pre><p>
<h3 id="Atomic operations">Atomic operations </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomadd</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomand</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u16</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u32</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxor</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u64</span><span style="color:#0">, value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s8</span><span style="color:#0">,  exchangeWith: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s16</span><span style="color:#0">, exchangeWith: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s32</span><span style="color:#0">, exchangeWith: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s64</span><span style="color:#0">, exchangeWith: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">,  exchangeWith: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u16</span><span style="color:#0">, exchangeWith: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u32</span><span style="color:#0">, exchangeWith: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u64</span><span style="color:#0">, exchangeWith: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s8</span><span style="color:#0">,  compareTo, exchangeWith: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s16</span><span style="color:#0">, compareTo, exchangeWith: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s32</span><span style="color:#0">, compareTo, exchangeWith: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">s64</span><span style="color:#0">, compareTo, exchangeWith: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u8</span><span style="color:#0">,  compareTo, exchangeWith: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u16</span><span style="color:#0">, compareTo, exchangeWith: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u32</span><span style="color:#0">, compareTo, exchangeWith: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atomcmpxchg</span><span style="color:#0">(addr: *</span><span style="color:#ED9A11">u64</span><span style="color:#0">, compareTo, exchangeWith: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;</span></code></pre><p>
<h3 id="Math">Math </h3></p>
<p>
</p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@sqrt</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@sqrt</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@sin</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@sin</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@cos</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@cos</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@tan</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@tan</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@sinh</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@sinh</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@cosh</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@cosh</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@tanh</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@tanh</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@asin</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@asin</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@acos</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@acos</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atan</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@atan</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@log</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@log</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@log2</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@log2</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@log10</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@log10</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@floor</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@floor</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@ceil</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@ceil</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@trunc</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@trunc</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@round</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@round</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@exp</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@exp</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@exp2</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@exp2</span><span style="color:#0">(value: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@pow</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@pow</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@min</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s8</span><span style="color:#0">)-></span><span style="color:#ED9A11">s8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s16</span><span style="color:#0">)-></span><span style="color:#ED9A11">s16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">s64</span><span style="color:#0">)-></span><span style="color:#ED9A11">s64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@max</span><span style="color:#0">(value1, value2: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountnz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountnz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountnz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountnz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcounttz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcounttz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcounttz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcounttz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountlz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)-></span><span style="color:#ED9A11">u8</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountlz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountlz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@bitcountlz</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@byteswap</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u16</span><span style="color:#0">)-></span><span style="color:#ED9A11">u16</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@byteswap</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u32</span><span style="color:#0">)-></span><span style="color:#ED9A11">u32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@byteswap</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u64</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0">;

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@muladd</span><span style="color:#0">(val1, val2, val3: </span><span style="color:#ED9A11">f32</span><span style="color:#0">)-></span><span style="color:#ED9A11">f32</span><span style="color:#0">;
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#F2470C">@muladd</span><span style="color:#0">(val1, val2, val3: </span><span style="color:#ED9A11">f64</span><span style="color:#0">)-></span><span style="color:#ED9A11">f64</span><span style="color:#0">;</span></code></pre>
<h2 id="121_init">Init</h2>
<p>
<h3 id="@init">@init </h3></p>
<p>
<code>@init</code> can be used to reinitialize a variable/array to the default value. </p>
<p>

  For a simple variable, the default value is 0. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">666</span><span style="color:#0">
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&x)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">0</span><span style="color:#0">)
}</span></code></pre><p>

  Work also for an array, as you can specify the number of elements you want to initialize. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&x, </span><span style="color:#74A35B">2</span><span style="color:#0">)    </span><span style="color:#6A9955">// Initialize 2 elements</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">0</span><span style="color:#0">)
}</span></code></pre><p>

  You can also specify a <i>value</i> to initialize, instead of the default one. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">666</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&x)(</span><span style="color:#74A35B">3.14</span><span style="color:#0">)  </span><span style="color:#6A9955">// Initialize to 3.14 instead of zero</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">3.14</span><span style="color:#0">)
}</span></code></pre><p>

  Same for an array. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x = [</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&x, </span><span style="color:#74A35B">2</span><span style="color:#0">)(</span><span style="color:#74A35B">555</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">555</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">555</span><span style="color:#0">)
}</span></code></pre><p>
When called on a struct, the struct will be restored to the values defined in it. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
{
    r = </span><span style="color:#74A35B">1</span><span style="color:#0">
    g = </span><span style="color:#74A35B">2</span><span style="color:#0">
    b = </span><span style="color:#74A35B">3</span><span style="color:#0">
}

{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> rgb: </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">{</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&rgb)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb.r == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb.g == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb.b == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  But you can also specified the values. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> rgb: </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">{</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">, </span><span style="color:#74A35B">30</span><span style="color:#0">}
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&rgb)(</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">6</span><span style="color:#0">, </span><span style="color:#74A35B">7</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb.r == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb.g == </span><span style="color:#74A35B">6</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb.b == </span><span style="color:#74A35B">7</span><span style="color:#0">)
}</span></code></pre><p>

  And this works also for array. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> rgb: [</span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&rgb, </span><span style="color:#74A35B">4</span><span style="color:#0">)(</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">6</span><span style="color:#0">, </span><span style="color:#74A35B">7</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb[</span><span style="color:#74A35B">3</span><span style="color:#0">].r == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb[</span><span style="color:#74A35B">3</span><span style="color:#0">].g == </span><span style="color:#74A35B">6</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb[</span><span style="color:#74A35B">3</span><span style="color:#0">].b == </span><span style="color:#74A35B">7</span><span style="color:#0">)
}</span></code></pre><p>
<h3 id="@drop">@drop </h3></p>
<p>
For a struct, <code>@init</code> will <b>not</b> call <code>opDrop</code>, so this is mostly useful to initialize a plain old data. </p>
<p>
But there is also <code>@drop</code> intrinsic, which works the same, except that it will <code>drop</code> all the content by calling <code>opDrop</code> if it is defined. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
{
    r = </span><span style="color:#74A35B">1</span><span style="color:#0">
    g = </span><span style="color:#74A35B">2</span><span style="color:#0">
    b = </span><span style="color:#74A35B">3</span><span style="color:#0">
}

{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> rgb: [</span><span style="color:#74A35B">4</span><span style="color:#0">] </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">

    </span><span style="color:#6A9955">// In fact this is a no op, as struct RGB is plain old data, without a defined 'opDrop'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@drop</span><span style="color:#0">(&rgb, </span><span style="color:#74A35B">4</span><span style="color:#0">)

    </span><span style="color:#B4B44A">@init</span><span style="color:#0">(&rgb, </span><span style="color:#74A35B">4</span><span style="color:#0">)(</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">6</span><span style="color:#0">, </span><span style="color:#74A35B">7</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb[</span><span style="color:#74A35B">3</span><span style="color:#0">].r == </span><span style="color:#74A35B">5</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb[</span><span style="color:#74A35B">3</span><span style="color:#0">].g == </span><span style="color:#74A35B">6</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(rgb[</span><span style="color:#74A35B">3</span><span style="color:#0">].b == </span><span style="color:#74A35B">7</span><span style="color:#0">)
}</span></code></pre>
<h2 id="130_generic">Generic</h2>

<h3 id="131__declaration">Declaration</h3>
<p>
A function can be generic by specifying some parameters after <code>func</code>. At the call site, you specify the generic parameters with <code>funcCall'(type1, type2, ...)(parameters)</code>. Note that parenthesis can be omitted if there's only one generic parameter. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// Here 'T' is a generic type.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(val: </span><span style="color:#3BC3A7">T</span><span style="color:#0">) => </span><span style="color:#74A35B">2</span><span style="color:#0"> * val

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'</span><span style="color:#ED9A11">s32</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'</span><span style="color:#ED9A11">f32</span><span style="color:#0">(</span><span style="color:#74A35B">2.0</span><span style="color:#0">) == </span><span style="color:#74A35B">4.0</span><span style="color:#0">)
}

{
    </span><span style="color:#6A9955">// You can omit 'var' to declare the generic type, because a single identifier</span><span style="color:#0">
    </span><span style="color:#6A9955">// is considered to be a type.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(val: </span><span style="color:#3BC3A7">T</span><span style="color:#0">) => </span><span style="color:#74A35B">2</span><span style="color:#0"> * val

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'</span><span style="color:#ED9A11">s32</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'</span><span style="color:#ED9A11">f32</span><span style="color:#0">(</span><span style="color:#74A35B">2.0</span><span style="color:#0">) == </span><span style="color:#74A35B">4.0</span><span style="color:#0">)
}

{
    </span><span style="color:#6A9955">// You can set a default value to the type.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0"> = </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(val: </span><span style="color:#3BC3A7">T</span><span style="color:#0">) => </span><span style="color:#74A35B">2</span><span style="color:#0"> * val

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'</span><span style="color:#ED9A11">f32</span><span style="color:#0">(</span><span style="color:#74A35B">2.0</span><span style="color:#0">) == </span><span style="color:#74A35B">4.0</span><span style="color:#0">)
}

{
    </span><span style="color:#6A9955">// Of course you can specify more than one generic parameter</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">K</span><span style="color:#0">, </span><span style="color:#3BC3A7">V</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(key: </span><span style="color:#3BC3A7">K</span><span style="color:#0">, value: </span><span style="color:#3BC3A7">V</span><span style="color:#0">) => value

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#BB6643">"value"</span><span style="color:#0">) == </span><span style="color:#BB6643">"value"</span><span style="color:#0">)              </span><span style="color:#6A9955">// K and V are deduced</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'(</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#ED9A11">string</span><span style="color:#0">)(</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#BB6643">"value"</span><span style="color:#0">) == </span><span style="color:#BB6643">"value"</span><span style="color:#0">)    </span><span style="color:#6A9955">// K and V are explicit</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#3186CD">true</span><span style="color:#0">) == </span><span style="color:#3186CD">true</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'(</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#ED9A11">bool</span><span style="color:#0">)(</span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#3186CD">true</span><span style="color:#0">) == </span><span style="color:#3186CD">true</span><span style="color:#0">)
}</span></code></pre><p>
Generic types can be deduced from parameters, so <code>func'type()</code> is not always necessary. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(val: </span><span style="color:#3BC3A7">T</span><span style="color:#0">) => </span><span style="color:#74A35B">2</span><span style="color:#0"> * val

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">) == </span><span style="color:#74A35B">4</span><span style="color:#0">)         </span><span style="color:#6A9955">// T is deduced to be s32</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">2.0</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">) == </span><span style="color:#74A35B">4.0</span><span style="color:#0">)     </span><span style="color:#6A9955">// T is deduced to be f32</span></code></pre><p>
You can also specify constants as generic parameters. </p>
<p>

  <code>N</code> is a constant a type <code>s32</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">N</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">() = </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">N</span><span style="color:#0"> == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    myFunc'</span><span style="color:#74A35B">10</span><span style="color:#0">()
}</span></code></pre><p>

  <code>const</code> can also be omitted, as an identifier followed by a type definition is considered to be a constant and not a type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">N</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">() = </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">N</span><span style="color:#0"> == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    myFunc'</span><span style="color:#74A35B">10</span><span style="color:#0">()
}</span></code></pre><p>

  You can also assign a default value to the constant. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">N</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">() = </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">N</span><span style="color:#0"> == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">()
}</span></code></pre><p>

  You can ommit the type if you declare the constant with <code>const</code>. It will be deduced from the assignment expression. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">N</span><span style="color:#0"> = </span><span style="color:#74A35B">10</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">() = </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">N</span><span style="color:#0"> == </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">()
}</span></code></pre><p>
You can mix types and constants. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#6A9955">// `T` is a type, `N` is a constant of type `s32`, because remember that an identifier</span><span style="color:#0">
    </span><span style="color:#6A9955">// alone is considered to be a generic type.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">, </span><span style="color:#3BC3A7">N</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">) => x * </span><span style="color:#3BC3A7">N</span><span style="color:#0">

    </span><span style="color:#3186CD">namealias</span><span style="color:#0"> call = myFunc'(</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">call</span><span style="color:#0">(</span><span style="color:#74A35B">2</span><span style="color:#0">) == </span><span style="color:#74A35B">20</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">call</span><span style="color:#0">(</span><span style="color:#74A35B">100</span><span style="color:#0">) == </span><span style="color:#74A35B">1000</span><span style="color:#0">)
}

{
    </span><span style="color:#6A9955">// So if you want to declare multiple constants, specify the type (or 'const') for each.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#3BC3A7">N</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">() => </span><span style="color:#3BC3A7">T</span><span style="color:#0"> * </span><span style="color:#3BC3A7">N</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myFunc'(</span><span style="color:#74A35B">5</span><span style="color:#0">, </span><span style="color:#74A35B">10</span><span style="color:#0">)() == </span><span style="color:#74A35B">50</span><span style="color:#0">)
}

{
    </span><span style="color:#6A9955">// And if you want to declare multiple types with default values, specify the value for each.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0"> = </span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#3BC3A7">V</span><span style="color:#0"> = </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">, y: </span><span style="color:#3BC3A7">V</span><span style="color:#0">) => x * y

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">) == </span><span style="color:#74A35B">2.0</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">) == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>
Like functions, a struct can also be generic. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
    {
        val: </span><span style="color:#3BC3A7">T</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x.val) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x1: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x1.val) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">)
}

{
    </span><span style="color:#3186CD">struct</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">, </span><span style="color:#3BC3A7">N</span><span style="color:#0">: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">
    {
        val: [</span><span style="color:#3BC3A7">N</span><span style="color:#0">] </span><span style="color:#3BC3A7">T</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#3BC3A7">Struct</span><span style="color:#0">'(</span><span style="color:#ED9A11">bool</span><span style="color:#0">, </span><span style="color:#74A35B">10</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x.val) == </span><span style="color:#C3973B">#type</span><span style="color:#0"> [</span><span style="color:#74A35B">10</span><span style="color:#0">] </span><span style="color:#ED9A11">bool</span><span style="color:#0">)
}</span></code></pre>
<h3 id="132__validif">Validif</h3>
<p>
<h4 id="One time evaluation">One time evaluation </h4></p>
<p>
On a function, you can use <code>#validif</code> to check if the usage of the function is correct. </p>
<p>
If the <code>#validif</code> expression returns false, then the function will not be considered for the call. If there's no other overload to match, then the compiler will raise an error. </p>
<p>
The <code>#validif</code> expression is evaluated <b>only once</b>, whatever the call, so it is typically used to check generic parameters. </p>
<pre><code><span style="color:#6A9955">// Here we validate the function only if the generic type is `s32` or `s64`.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">...)-></span><span style="color:#3BC3A7">T</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#validif</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s32</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s64</span><span style="color:#0">
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> total = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#3BC3A7">T</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> it: x
        total += it
    </span><span style="color:#B040BE">return</span><span style="color:#0"> total
}

</span><span style="color:#6A9955">// This is ok.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> res1 = sum'</span><span style="color:#ED9A11">s32</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res1 == </span><span style="color:#74A35B">3</span><span style="color:#0">)
</span><span style="color:#3186CD">let</span><span style="color:#0"> res2 = sum'</span><span style="color:#ED9A11">s64</span><span style="color:#0">(</span><span style="color:#74A35B">10</span><span style="color:#0">, </span><span style="color:#74A35B">20</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res2 == </span><span style="color:#74A35B">30</span><span style="color:#0">)

</span><span style="color:#6A9955">// But the following would generate an error because the type is `f32`.</span><span style="color:#0">
</span><span style="color:#6A9955">// So there's no match possible for that type.</span><span style="color:#0">

</span><span style="color:#6A9955">// var res1 = sum'f32(1, 2)</span></code></pre><p>
You can use <code>#validif</code> to make a kind of a generic specialisation. </p>
<pre><code><span style="color:#6A9955">// s32 version</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">isNull</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">)-></span><span style="color:#ED9A11">bool</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#validif</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#B040BE">return</span><span style="color:#0"> x == </span><span style="color:#74A35B">0</span><span style="color:#0">
}

</span><span style="color:#6A9955">// f32/f64 version</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">isNull</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">)-></span><span style="color:#ED9A11">bool</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#validif</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">f32</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">
{
    </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#F2470C">@abs</span><span style="color:#0">(x) < </span><span style="color:#74A35B">0.01</span><span style="color:#0">
}

</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">isNull</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">))
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">isNull</span><span style="color:#0">(</span><span style="color:#74A35B">0.001</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">))</span></code></pre><p>
Instead of a single expression, <code>#validif</code> can be followed by a block that returns a <code>bool</code>. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">...)-></span><span style="color:#3BC3A7">T</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#validif</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s32</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s64</span><span style="color:#0">
            </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">true</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">
    }
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> total = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#3BC3A7">T</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> it: x
        total += it
    </span><span style="color:#B040BE">return</span><span style="color:#0"> total
}</span></code></pre><p>
By using <code>@compilererror</code>, you can then trigger your own errors at compile time if the type is incorrect. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x, y: </span><span style="color:#3BC3A7">T</span><span style="color:#0">)-></span><span style="color:#3BC3A7">T</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#validif</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s32</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) == </span><span style="color:#ED9A11">s64</span><span style="color:#0">
            </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">true</span><span style="color:#0">
        </span><span style="color:#B4B44A">@compilererror</span><span style="color:#0">(</span><span style="color:#BB6643">"invalid type "</span><span style="color:#0"> ++ </span><span style="color:#B4B44A">@stringof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">), </span><span style="color:#B4B44A">@location</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">))
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">
    }
{
    </span><span style="color:#B040BE">return</span><span style="color:#0"> x + y
}

</span><span style="color:#6A9955">// This will trigger an error</span><span style="color:#0">

</span><span style="color:#6A9955">// var x = sum'f32(1, 2)</span></code></pre><p>
<code>#validif</code> can also be used on a generic struct. Unlike functions, if the expression failed, then you will have an error right away because there's no overload in the case of structures. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#validif</span><span style="color:#0"> </span><span style="color:#3BC3A7">T</span><span style="color:#0"> == </span><span style="color:#ED9A11">f32</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#3BC3A7">T</span><span style="color:#0"> == </span><span style="color:#ED9A11">f64</span><span style="color:#0">
{
    x, y: </span><span style="color:#3BC3A7">T</span><span style="color:#0">
}

</span><span style="color:#6A9955">// Fine.</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">

</span><span style="color:#6A9955">// Error.</span><span style="color:#0">
</span><span style="color:#6A9955">//var v: Point's32</span></code></pre><p>
<h4 id="Multiple evaluations">Multiple evaluations </h4></p>
<p>
Instead of <code>#validif</code>, you can use <code>#validifx</code>. <code>#validifx</code> is evaluated for <b>each</b> call, so it can be used to check parameters, as long as they can be <b>evaluated at compile time</b>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">div</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#validifx</span><span style="color:#0">
        {
            </span><span style="color:#6A9955">// Here we use '@isconstexpr'.</span><span style="color:#0">
            </span><span style="color:#6A9955">// If 'y' cannot be evaluated at compile time, then we can do nothing about it.</span><span style="color:#0">
            </span><span style="color:#B040BE">if</span><span style="color:#0"> !</span><span style="color:#B4B44A">@isconstexpr</span><span style="color:#0">(y)
                </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">true</span><span style="color:#0">
            </span><span style="color:#B040BE">if</span><span style="color:#0"> y == </span><span style="color:#74A35B">0</span><span style="color:#0">
                </span><span style="color:#B4B44A">@compilererror</span><span style="color:#0">(</span><span style="color:#BB6643">"division by zero"</span><span style="color:#0">, </span><span style="color:#B4B44A">@location</span><span style="color:#0">(y))
            </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">true</span><span style="color:#0">
        }
    {
        </span><span style="color:#B040BE">return</span><span style="color:#0"> x / y
    }

    </span><span style="color:#6A9955">// Fine</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x1 = </span><span style="color:#FF6A00">div</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Error at compile time, division by zero.</span><span style="color:#0">

    </span><span style="color:#6A9955">// var x2 = div(1, 0)</span><span style="color:#0">
}

{
    </span><span style="color:#6A9955">// A version of 'first' where 'x' is known at compile time.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">first</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#validifx</span><span style="color:#0"> </span><span style="color:#B4B44A">@isconstexpr</span><span style="color:#0">(x)
    {
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">555</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// A version of 'first' where 'x' is **not** known at compile time.</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">first</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
        </span><span style="color:#7F7F7F">#validifx</span><span style="color:#0"> !</span><span style="color:#B4B44A">@isconstexpr</span><span style="color:#0">(x)
    {
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">666</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// Will call version 1 because parameter is a literal.</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">first</span><span style="color:#0">(</span><span style="color:#74A35B">0</span><span style="color:#0">) == </span><span style="color:#74A35B">555</span><span style="color:#0">)

    </span><span style="color:#6A9955">// Will call version 2 because parameter is a variable.</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> a: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">first</span><span style="color:#0">(a) == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}</span></code></pre>
<h3 id="133__constraint">Constraint</h3>
<p>
Swag provides also a simple way of checking generic parameters, without the need of <code>#validif</code>. A type constraint can be added when declaring a generic type. If a function or a struct is instantiated with a type that does not conform to the constraint, then an error will be raised. </p>
<pre><code><span style="color:#6A9955">// The type constraint is a compile time function (with #[Swag.ConstExpr]) that should return a bool.</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isS32</span><span style="color:#0">(t: </span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">) => t == </span><span style="color:#ED9A11">s32</span><span style="color:#0">

</span><span style="color:#6A9955">// Here we check that the function generic type is 's32' by calling 'isS32'.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">T</span><span style="color:#0">: </span><span style="color:#FF6A00">isS32</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">)) </span><span style="color:#FF6A00">sum</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">...)-></span><span style="color:#3BC3A7">T</span><span style="color:#0">
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> total = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#3BC3A7">T</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> it: x
        total += it
    </span><span style="color:#B040BE">return</span><span style="color:#0"> total
}

</span><span style="color:#6A9955">// This is ok.</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> res1 = sum'</span><span style="color:#ED9A11">s32</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(res1 == </span><span style="color:#74A35B">3</span><span style="color:#0">)

</span><span style="color:#6A9955">// But the following would generate an error because the type is 'f32'.</span><span style="color:#0">
</span><span style="color:#6A9955">// let res1 = sum'f32(1, 2)</span></code></pre><p>
The type constraint can be any compile time expression, as long as the resulting type is <code>bool</code>. So you could do something like this. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isS32</span><span style="color:#0">(t: </span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">)  => t == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isBool</span><span style="color:#0">(t: </span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">) => t == </span><span style="color:#ED9A11">bool</span><span style="color:#0">

</span><span style="color:#6A9955">// Here we check that the function generic type is 's32' or 'bool'.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0">(</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">T</span><span style="color:#0">: </span><span style="color:#FF6A00">isS32</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#B040BE">or</span><span style="color:#0"> </span><span style="color:#FF6A00">isBool</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">)) </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(x: </span><span style="color:#3BC3A7">T</span><span style="color:#0">) => x

</span><span style="color:#6A9955">// This is ok.</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">5</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">) == </span><span style="color:#74A35B">5</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)  == </span><span style="color:#3186CD">true</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">) == </span><span style="color:#3186CD">false</span><span style="color:#0">)</span></code></pre><p>
Works also for structs. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isFloat</span><span style="color:#0">(t: </span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">) => t == </span><span style="color:#ED9A11">f32</span><span style="color:#0"> </span><span style="color:#B040BE">or</span><span style="color:#0"> t == </span><span style="color:#ED9A11">f64</span><span style="color:#0">

</span><span style="color:#3186CD">struct</span><span style="color:#0">(</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">T</span><span style="color:#0">: </span><span style="color:#FF6A00">isFloat</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">)) </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
{
    x, y: </span><span style="color:#3BC3A7">T</span><span style="color:#0">
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> pt:  </span><span style="color:#3BC3A7">Point</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> pt1: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">

</span><span style="color:#6A9955">// This will generate a type constraint error.</span><span style="color:#0">
</span><span style="color:#6A9955">//var pt: Point's32</span></code></pre>
<h2 id="160_scoping">Scoping</h2>

<h3 id="161__defer">Defer</h3>
<p>
<code>defer</code> is used to call an expression when the current scope is left. It's purely compile time, so it does not evaluate until the block is left. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> v = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">defer</span><span style="color:#0"> </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    v += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#6A9955">// defer expression will be executed here</span><span style="color:#0">
}</span></code></pre><p>

  <code>defer</code> can also be used with a block. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> v = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">defer</span><span style="color:#0">
    {
        v += </span><span style="color:#74A35B">10</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v == </span><span style="color:#74A35B">15</span><span style="color:#0">)
    }

    v += </span><span style="color:#74A35B">5</span><span style="color:#0">
    </span><span style="color:#6A9955">// defer block will be executed here</span><span style="color:#0">
}</span></code></pre><p>

  <code>defer</code> expressions are called when leaving the corresponding scope, even with <code>return</code>, <code>break</code>, <code>continue</code> etc., and even inside a <code>loop/while/for</code> etc. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">defer</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> += </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> == </span><span style="color:#74A35B">2</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">   </span><span style="color:#6A9955">// will be called here, before breaking the loop</span><span style="color:#0">
        </span><span style="color:#6A9955">// will be called here also</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0"> == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>

  <code>defer</code> are executed in reverse order of their declaration. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B040BE">defer</span><span style="color:#0"> </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">2</span><span style="color:#0">)   </span><span style="color:#6A9955">// Will be executed second</span><span style="color:#0">
    </span><span style="color:#B040BE">defer</span><span style="color:#0"> x *= </span><span style="color:#74A35B">2</span><span style="color:#0">            </span><span style="color:#6A9955">// Will be executed first</span><span style="color:#0">
}</span></code></pre><p>

  It's typically used to unregister/destroy a resource, by putting the release code just after the creation one. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">createResource</span><span style="color:#0">() => </span><span style="color:#3186CD">true</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">releaseResource</span><span style="color:#0">(resource: *</span><span style="color:#ED9A11">bool</span><span style="color:#0">) = </span><span style="color:#3186CD">dref</span><span style="color:#0"> resource = </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isResourceCreated</span><span style="color:#0">(b: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) => b

    </span><span style="color:#3186CD">var</span><span style="color:#0"> resource = </span><span style="color:#3186CD">false</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
    {
        resource = </span><span style="color:#FF6A00">createResource</span><span style="color:#0">()
        </span><span style="color:#B040BE">defer</span><span style="color:#0">
        {
            </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(resource.</span><span style="color:#FF6A00">isResourceCreated</span><span style="color:#0">())
            </span><span style="color:#FF6A00">releaseResource</span><span style="color:#0">(&resource)
        }

        </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@index</span><span style="color:#0"> == </span><span style="color:#74A35B">2</span><span style="color:#0">
            </span><span style="color:#B040BE">break</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(!resource.</span><span style="color:#FF6A00">isResourceCreated</span><span style="color:#0">())
}</span></code></pre>
<h3 id="162__using">Using</h3>
<p>

  <code>using</code> brings the scope of a namespace, a struct or an enum in the current one. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0"> { </span><span style="color:#3BC3A7">R</span><span style="color:#0">; </span><span style="color:#3BC3A7">G</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#3BC3A7">R</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">)

    </span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">)
}</span></code></pre><p>

  <code>using</code> can also be used with a variable </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point</span><span style="color:#0"> { x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">

    </span><span style="color:#3186CD">using</span><span style="color:#0"> pt
    x = </span><span style="color:#74A35B">1</span><span style="color:#0"> </span><span style="color:#6A9955">// no need to specify 'pt'</span><span style="color:#0">
    y = </span><span style="color:#74A35B">2</span><span style="color:#0"> </span><span style="color:#6A9955">// no need to specify 'pt'</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  You can declare a variable with <code>using</code> just before. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point</span><span style="color:#0"> { x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; }

    </span><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
    x = </span><span style="color:#74A35B">1</span><span style="color:#0"> </span><span style="color:#6A9955">// no need to specify 'pt'</span><span style="color:#0">
    y = </span><span style="color:#74A35B">2</span><span style="color:#0"> </span><span style="color:#6A9955">// no need to specify 'pt'</span><span style="color:#0">

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>
<h4 id="For a function parameter">For a function parameter </h4></p>
<p>
<code>using</code> applied to a function parameter can be seen as the equivalent of the hidden <code>this</code> in C++. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point</span><span style="color:#0"> { x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; }

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">setOne</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> point: *</span><span style="color:#3BC3A7">Point</span><span style="color:#0">)
{
    </span><span style="color:#6A9955">// Here there's no need to specify 'point'</span><span style="color:#0">
    x, y = </span><span style="color:#74A35B">1</span><span style="color:#0">
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
</span><span style="color:#FF6A00">setOne</span><span style="color:#0">(&pt)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.y == </span><span style="color:#74A35B">1</span><span style="color:#0">)

</span><span style="color:#6A9955">// ufcs</span><span style="color:#0">
pt.</span><span style="color:#FF6A00">setOne</span><span style="color:#0">()
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.y == </span><span style="color:#74A35B">1</span><span style="color:#0">)</span></code></pre><p>
<h4 id="For a field">For a field </h4></p>
<p>
<code>using</code> can also be used with a field inside a struct. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
{
    x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}

</span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">
{
    </span><span style="color:#3186CD">using</span><span style="color:#0"> base: </span><span style="color:#3BC3A7">Point2</span><span style="color:#0">
    z: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}

</span><span style="color:#6A9955">// That way the content of the field can be referenced directly</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> value: </span><span style="color:#3BC3A7">Point3</span><span style="color:#0">
value.x = </span><span style="color:#74A35B">0</span><span style="color:#0"> </span><span style="color:#6A9955">// Equivalent to value.base.x = 0</span><span style="color:#0">
value.y = </span><span style="color:#74A35B">0</span><span style="color:#0"> </span><span style="color:#6A9955">// Equivalent to value.base.y = 0</span><span style="color:#0">
value.z = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(&value.x == &value.base.x)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(&value.y == &value.base.y)

</span><span style="color:#6A9955">// The compiler can then cast automatically 'Point3' to 'Point2'</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">set1</span><span style="color:#0">(</span><span style="color:#3186CD">using</span><span style="color:#0"> ptr: *</span><span style="color:#3BC3A7">Point2</span><span style="color:#0">)
{
    x, y = </span><span style="color:#74A35B">1</span><span style="color:#0">
}

</span><span style="color:#FF6A00">set1</span><span style="color:#0">(&value) </span><span style="color:#6A9955">// Here the cast is automatic thanks to the using</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value.y == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value.base.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(value.base.y == </span><span style="color:#74A35B">1</span><span style="color:#0">)</span></code></pre>
<h3 id="163__with">With</h3>
<p>
You can use <code>with</code> to avoid repeating the same variable again and again. You can then access fields with a simple <code>.</code>. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point</span><span style="color:#0"> { x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">; }

</span><span style="color:#3186CD">impl</span><span style="color:#0"> </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
{
    </span><span style="color:#3186CD">mtd</span><span style="color:#0"> </span><span style="color:#FF6A00">setOne</span><span style="color:#0">()
    {
        x, y = </span><span style="color:#74A35B">1</span><span style="color:#0">
    }
}</span></code></pre><p>

  <code>with</code> on a variable. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
    </span><span style="color:#3186CD">with</span><span style="color:#0"> pt
    {
        .x = </span><span style="color:#74A35B">1</span><span style="color:#0">   </span><span style="color:#6A9955">// equivalent to pt.x</span><span style="color:#0">
        .y = </span><span style="color:#74A35B">2</span><span style="color:#0">   </span><span style="color:#6A9955">// equivalent to pt.y</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre><p>

  Works for function calls to. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
    </span><span style="color:#3186CD">with</span><span style="color:#0"> pt
    {
        .</span><span style="color:#FF6A00">setOne</span><span style="color:#0">()           </span><span style="color:#6A9955">// equivalent to pt.setOne() or setOne(pt)</span><span style="color:#0">
        .y = </span><span style="color:#74A35B">2</span><span style="color:#0">              </span><span style="color:#6A9955">// equivalent to pt.y</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)    </span><span style="color:#6A9955">// equivalent to pt.x</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)    </span><span style="color:#6A9955">// equivalent to pt.y</span><span style="color:#0">
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    }
}</span></code></pre><p>

  Works also with a namespace. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">with</span><span style="color:#0"> </span><span style="color:#3BC3A7">NameSpace</span><span style="color:#0">
    {
        .</span><span style="color:#FF6A00">inside0</span><span style="color:#0">()
        .</span><span style="color:#FF6A00">inside1</span><span style="color:#0">()
    }
}</span></code></pre><p>

  Instead of an identifier name, <code>with</code> also accepts a variable declaration. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">with</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> pt = </span><span style="color:#3BC3A7">Point</span><span style="color:#0">{</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}
    {
        .x = </span><span style="color:#74A35B">10</span><span style="color:#0">
        .y = </span><span style="color:#74A35B">20</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">10</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> pt.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)
}

{
    </span><span style="color:#3186CD">with</span><span style="color:#0"> </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
    {
        .x = </span><span style="color:#74A35B">10</span><span style="color:#0">
        .y = </span><span style="color:#74A35B">20</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">10</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> pt.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)
}</span></code></pre><p>

  Or an affectation statement. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> pt: </span><span style="color:#3BC3A7">Point</span><span style="color:#0">
    </span><span style="color:#3186CD">with</span><span style="color:#0"> pt = </span><span style="color:#3BC3A7">Point</span><span style="color:#0">{</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">}
    {
        .x = </span><span style="color:#74A35B">10</span><span style="color:#0">
        .y = </span><span style="color:#74A35B">20</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(pt.x == </span><span style="color:#74A35B">10</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> pt.y == </span><span style="color:#74A35B">20</span><span style="color:#0">)
}</span></code></pre><pre><code><span style="color:#3186CD">namespace</span><span style="color:#0"> </span><span style="color:#3BC3A7">NameSpace</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">inside0</span><span style="color:#0">() {}
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">inside1</span><span style="color:#0">() {}
}</span></code></pre>
<h2 id="164_alias">Alias</h2>
<p>
<h3 id="Type alias">Type alias </h3></p>
<p>
</p>
<p>

  <code>typealias</code> is used to make a shortcut to another type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0"> { </span><span style="color:#3BC3A7">R</span><span style="color:#0">; </span><span style="color:#3BC3A7">G</span><span style="color:#0">; </span><span style="color:#3BC3A7">B</span><span style="color:#0">; }
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">RGB</span><span style="color:#0">.</span><span style="color:#3BC3A7">R</span><span style="color:#0"> == </span><span style="color:#74A35B">0</span><span style="color:#0">)

    </span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">Color</span><span style="color:#0"> = </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">   </span><span style="color:#6A9955">// 'Color' is now equivalent to 'RGB'</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">Color</span><span style="color:#0">.</span><span style="color:#3BC3A7">G</span><span style="color:#0"> == </span><span style="color:#74A35B">1</span><span style="color:#0">)
}</span></code></pre><p>

  You can then use the new name in place of the original type. This does not create a new type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">Float32</span><span style="color:#0"> = </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">Float64</span><span style="color:#0"> = </span><span style="color:#ED9A11">f64</span><span style="color:#0">

    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#3BC3A7">Float32</span><span style="color:#0"> = </span><span style="color:#74A35B">1.0</span><span style="color:#0">    </span><span style="color:#6A9955">// Same as 'f32'</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> y: </span><span style="color:#3BC3A7">Float64</span><span style="color:#0"> = </span><span style="color:#74A35B">1.0</span><span style="color:#0">    </span><span style="color:#6A9955">// Same as 'f64'</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Float32</span><span style="color:#0">) == </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">Float64</span><span style="color:#0">) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">
}</span></code></pre><p>

  But to create a new type, a <code>typealias</code> can also be marked with the <code>Swag.Strict</code> attribute. In that case, all implicit casts won't be done. Explicit cast are still possible. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.Strict]</span><span style="color:#0">
    </span><span style="color:#3186CD">typealias</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyType</span><span style="color:#0"> = </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyType</span><span style="color:#0">) != </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#3BC3A7">MyType</span><span style="color:#0"> = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3BC3A7">MyType</span><span style="color:#0">) </span><span style="color:#74A35B">0</span><span style="color:#0">
}</span></code></pre><p>
<h3 id="Name alias">Name alias </h3></p>
<p>
</p>
<p>

  You can alias a function name with <code>namealias</code> </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">thisIsABigFunctionName</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">) => x * x
    </span><span style="color:#3186CD">namealias</span><span style="color:#0"> myFunc = thisIsABigFunctionName
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">myFunc</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">) == </span><span style="color:#74A35B">16</span><span style="color:#0">)
}</span></code></pre><p>

  You can also alias variables and namespaces </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> myLongVariableName: </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#3186CD">namealias</span><span style="color:#0"> short = myLongVariableName
    short += </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(myLongVariableName == </span><span style="color:#74A35B">2</span><span style="color:#0">)
}</span></code></pre>
<h2 id="170_error_management">Error management</h2>
<p>
Swag contains a <b>very</b> simple error system used to deal with function returning errors. It will probably be changed/improved at some point. </p>
<p>
</p>
<blockquote><p>
These are <b>not</b> exceptions ! </p></blockquote>
<p>
</p>
<p>
A function that can return an error must be marked with <code>throw</code>. It can then raise an error with the <code>throw</code> keyword, passing an error message. </p>
<p>
</p>
<blockquote><p>
When an error is raised by a function, the return value is always equal to the <b>default value</b>, depending on the return type. </p></blockquote>
<p>
</p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">count</span><span style="color:#0">(name: </span><span style="color:#ED9A11">string</span><span style="color:#0">)-></span><span style="color:#ED9A11">u64</span><span style="color:#0"> </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#B040BE">if</span><span style="color:#0"> name == </span><span style="color:#3186CD">null</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// This function will return 0 in case of an error, because this is the default</span><span style="color:#0">
        </span><span style="color:#6A9955">// value for 'u64'.</span><span style="color:#0">
        </span><span style="color:#3186CD">throw</span><span style="color:#0"> </span><span style="color:#BB6643">"null pointer"</span><span style="color:#0">
    }

    </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(name)
}</span></code></pre><p>
The caller will then have to deal with the error in some way. It can <code>catch</code> it, and test (or not) its value with the <code>@errmsg</code> intrinsic. The execution will continue at the call site. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">()
{
    </span><span style="color:#6A9955">// Dismiss the eventual error with 'catch' and continue execution</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> cpt = </span><span style="color:#3186CD">catch</span><span style="color:#0"> </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"fileName"</span><span style="color:#0">)

    </span><span style="color:#6A9955">// And test it with @errmsg, which returns the 'throw' corresponding string</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@errmsg</span><span style="color:#0">
    {
        </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">0</span><span style="color:#0">)
        </span><span style="color:#B4B44A">@print</span><span style="color:#0">(</span><span style="color:#B4B44A">@errmsg</span><span style="color:#0">)
        </span><span style="color:#B040BE">return</span><span style="color:#0">
    }

    </span><span style="color:#6A9955">// You can also use 'trycatch', which will exit the current function in case</span><span style="color:#0">
    </span><span style="color:#6A9955">// an error has been raised (returning the default value if necessary)</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt1 = </span><span style="color:#3186CD">trycatch</span><span style="color:#0"> </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"fileName"</span><span style="color:#0">)
}</span></code></pre><p>
The caller can stop the execution with <code>try</code>, and return to its own caller with the same error raised. The function must then also be marked with <code>throw</code>. </p>
<p>
Here, the caller of <code>myFunc1</code> will have to deal with the error at its turn. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunc1</span><span style="color:#0">() </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// If 'count()' raises an error, 'myFunc1' will return with the same error</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#3186CD">try</span><span style="color:#0"> </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"filename"</span><span style="color:#0">)
}</span></code></pre><p>
The caller can also panic if an error is raised, with <code>assume</code>. </p>
<p>
</p>
<blockquote><p>
This can be disabled in release builds (in that case the behaviour is undefined). </p></blockquote>
<p>
</p>
<p>
</p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunc2</span><span style="color:#0">()
{
    </span><span style="color:#6A9955">// Here the program will stop with a panic message if 'count()' throws an error</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#3186CD">assume</span><span style="color:#0"> </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"filename"</span><span style="color:#0">)
}</span></code></pre><p>
Note that you can use a block instead of one single statement (this does not create a scope). </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunc3</span><span style="color:#0">() </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// This is not really necessary, see below, but this is just to show 'try' with a block</span><span style="color:#0">
    </span><span style="color:#6A9955">// instead of one single call</span><span style="color:#0">
    </span><span style="color:#3186CD">try</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt0 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"filename"</span><span style="color:#0">)
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt1 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"other filename"</span><span style="color:#0">)
    }

    </span><span style="color:#3186CD">assume</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt2 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"filename"</span><span style="color:#0">)
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt3 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"other filename"</span><span style="color:#0">)
    }

    </span><span style="color:#6A9955">// Works also for 'catch' if you do not want to deal with the error message.</span><span style="color:#0">
    </span><span style="color:#6A9955">// '@errmsg' in that case is not really relevant.</span><span style="color:#0">
    </span><span style="color:#3186CD">catch</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt4 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"filename"</span><span style="color:#0">)
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt5 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"other filename"</span><span style="color:#0">)
    }

    </span><span style="color:#6A9955">// Works also for 'trycatch' if you do not want to deal with the error message</span><span style="color:#0">
    </span><span style="color:#6A9955">// and you want to return as soon as an error is raised.</span><span style="color:#0">
    </span><span style="color:#3186CD">trycatch</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt6 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"filename"</span><span style="color:#0">)
        </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt7 = </span><span style="color:#FF6A00">count</span><span style="color:#0">(</span><span style="color:#BB6643">"other filename"</span><span style="color:#0">)
    }
}</span></code></pre><p>
When a function is marked with <code>throw</code>, the <code>try</code> for a function call is automatic if not specified. That means that most of the time it's not necessary to specify it. </p>
<pre><code><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">mySubFunc2</span><span style="color:#0">() </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#3186CD">throw</span><span style="color:#0"> </span><span style="color:#BB6643">"error"</span><span style="color:#0">
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">mySubFunc1</span><span style="color:#0">() </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// In fact there's no need to add a 'try' before the call because 'mySubFunc1' is</span><span style="color:#0">
    </span><span style="color:#6A9955">// marked with 'throw'</span><span style="color:#0">
    </span><span style="color:#6A9955">// This is less verbose when you do not want to do something special in case</span><span style="color:#0">
    </span><span style="color:#6A9955">// of errors (with 'assume', 'catch' or 'trycatch')</span><span style="color:#0">
    </span><span style="color:#FF6A00">mySubFunc2</span><span style="color:#0">()
}

</span><span style="color:#3186CD">catch</span><span style="color:#0"> </span><span style="color:#FF6A00">mySubFunc1</span><span style="color:#0">()
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@errmsg</span><span style="color:#0"> == </span><span style="color:#BB6643">"error"</span><span style="color:#0">)</span></code></pre><p>
<h3 id="defer">defer </h3></p>
<p>
<code>defer</code> can have parameters like <code>defer(err)</code> or <code>defer(noerr)</code> to control if it should be executed depending on the error status. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> g_Defer = </span><span style="color:#74A35B">0</span><span style="color:#0">

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">raiseError</span><span style="color:#0">() </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#3186CD">throw</span><span style="color:#0"> </span><span style="color:#BB6643">"error"</span><span style="color:#0">
}

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">testDefer</span><span style="color:#0">(err: </span><span style="color:#ED9A11">bool</span><span style="color:#0">) </span><span style="color:#3186CD">throw</span><span style="color:#0">
{
    </span><span style="color:#B040BE">defer</span><span style="color:#0">(err)      g_Defer += </span><span style="color:#74A35B">1</span><span style="color:#0">    </span><span style="color:#6A9955">// This will be called in case an error is raised, before exiting</span><span style="color:#0">
    </span><span style="color:#B040BE">defer</span><span style="color:#0">(noerr)    g_Defer += </span><span style="color:#74A35B">2</span><span style="color:#0">    </span><span style="color:#6A9955">// This will only be called in case an error is not raised</span><span style="color:#0">
    </span><span style="color:#B040BE">defer</span><span style="color:#0">           g_Defer += </span><span style="color:#74A35B">3</span><span style="color:#0">    </span><span style="color:#6A9955">// This will be called in both cases</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> err
        </span><span style="color:#FF6A00">raiseError</span><span style="color:#0">()
}</span></code></pre><pre><code><span style="color:#0">g_Defer = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#3186CD">catch</span><span style="color:#0"> </span><span style="color:#FF6A00">testDefer</span><span style="color:#0">(</span><span style="color:#3186CD">true</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(g_Defer == </span><span style="color:#74A35B">4</span><span style="color:#0">)   </span><span style="color:#6A9955">// Will call only defer(err) and the normal defer</span><span style="color:#0">

g_Defer = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#3186CD">catch</span><span style="color:#0"> </span><span style="color:#FF6A00">testDefer</span><span style="color:#0">(</span><span style="color:#3186CD">false</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(g_Defer == </span><span style="color:#74A35B">5</span><span style="color:#0">)   </span><span style="color:#6A9955">// Will call only defer(noerr) and the normal defer</span></code></pre>
<h2 id="175_safety">Safety</h2>
<p>
Swag comes with a bunch of safety checks which can be activated by module, function or even instruction with the <code>#[Swag.Safety]</code> attribute. </p>
<p>
Safety checks can also be changed for a specific build configuration (<code>--cfg:<config></code>) with <code>buildCfg.safetyGuards</code>. </p>
<p>
</p>
<blockquote><p>
Swag comes with four predefined configurations : <code>debug</code>, <code>fast-debug</code>, <code>fast-compile</code> and <code>release</code>. Safety checks are disabled in <code>fast-compile</code> and <code>release</code>. </p></blockquote>
<p>
</p>
<p>
<h3 id="overflow">overflow </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("overflow", true)]</span></code></pre><p>
Swag will panic if some operators overflow and if we lose some bits during an integer conversion. </p>
<p>
Operators that can overflow are : <code>+ - * << >></code> and their equivalent <code>+= -= *= <<= >>=</code>. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
</span><span style="color:#6A9955">// x += 1      // This would overflow, and panic, because we lose informations</span></code></pre><p>
But if you know what your are doing, you can use a special version of those operators, which will not panic. Add the <code>,over</code> modifier after the operation. This will disable safety checks. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
x +=,over </span><span style="color:#74A35B">1</span><span style="color:#0">     </span><span style="color:#6A9955">// Overflow is expected, so this will wrap around</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
You can also use <code>#[Swag.Overflow(true)]</code> to authorize overflow on a more global scale. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Overflow(true)]</span></code></pre><pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
x += </span><span style="color:#74A35B">1</span><span style="color:#0">     </span><span style="color:#6A9955">// No need for operator modifier ',over'</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">0</span><span style="color:#0">)</span></code></pre><p>
For 8 or 16 bits, you can promote an operation to 32 bits by using ',up'. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> x = </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> +,up </span><span style="color:#74A35B">1</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">256</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x) == </span><span style="color:#ED9A11">u32</span><span style="color:#0">)</span></code></pre><p>
Swag will also check that a cast does not lose information. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> x1 = </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">

</span><span style="color:#6A9955">//var y0 = cast(s8) x1     // This would lose information and panic, as 255 cannot be encoded in 's8'</span><span style="color:#0">
</span><span style="color:#6A9955">//@print(y0)</span><span style="color:#0">

</span><span style="color:#3186CD">let</span><span style="color:#0"> y1 = </span><span style="color:#3186CD">cast</span><span style="color:#0">,</span><span style="color:#FF6A00">over</span><span style="color:#0">(</span><span style="color:#ED9A11">s8</span><span style="color:#0">) x1  </span><span style="color:#6A9955">// But ',over' can be used on the cast operation too</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y1 == -</span><span style="color:#74A35B">1</span><span style="color:#0">)

</span><span style="color:#3186CD">let</span><span style="color:#0"> x2 = -</span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">s8</span><span style="color:#0">
</span><span style="color:#6A9955">//var y2 = cast(u8) x2     // This cast also is not possible, because 'x2' is negative and 'y' is 'u8'</span><span style="color:#0">
</span><span style="color:#6A9955">//@print(y)</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y2 = </span><span style="color:#3186CD">cast</span><span style="color:#0">,</span><span style="color:#FF6A00">over</span><span style="color:#0">(</span><span style="color:#ED9A11">u8</span><span style="color:#0">) x2
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(y2 == </span><span style="color:#74A35B">255</span><span style="color:#0">)</span></code></pre><p>
Rember that you can disable these safety checks with the corresponding attribute. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Overflow(true)]</span></code></pre><pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">255</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
x += </span><span style="color:#74A35B">255</span><span style="color:#0">    </span><span style="color:#6A9955">// 254</span><span style="color:#0">
x += </span><span style="color:#74A35B">1</span><span style="color:#0">      </span><span style="color:#6A9955">// 255</span><span style="color:#0">
x >>= </span><span style="color:#74A35B">1</span><span style="color:#0">     </span><span style="color:#6A9955">// 127</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">127</span><span style="color:#0">)</span></code></pre><p>
<h3 id="any">any </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("any", true)]</span></code></pre><p>
Swag will panic if a bad cast from <code>any</code> is performed. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#ED9A11">any</span><span style="color:#0"> = </span><span style="color:#BB6643">"1"</span><span style="color:#0">
</span><span style="color:#3186CD">let</span><span style="color:#0"> y = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">string</span><span style="color:#0">) x     </span><span style="color:#6A9955">// This is valid, because this is the correct underlying type</span><span style="color:#0">
</span><span style="color:#6A9955">//var z = cast(s32) x      // This is not valid, and will panic</span><span style="color:#0">
</span><span style="color:#6A9955">//@assert(z == 0)</span></code></pre><p>
<h3 id="boundcheck">boundcheck </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("boundcheck", true)]</span></code></pre><p>
Swag will panic if an index is out of range when dereferencing a sized value like an array, a slice, a string... </p>
<p>
Safety for fixed size arrays. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
</span><span style="color:#3186CD">var</span><span style="color:#0"> idx = </span><span style="color:#74A35B">10</span><span style="color:#0">
</span><span style="color:#6A9955">//@assert(x[idx] == 1)     // '10' is out of range, will panic</span></code></pre><p>
Safety when indexing a slice. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> x: </span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
</span><span style="color:#3186CD">var</span><span style="color:#0"> idx = </span><span style="color:#74A35B">1</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x[idx] == </span><span style="color:#74A35B">1</span><span style="color:#0">)        </span><span style="color:#6A9955">// '1' is in range, ok</span><span style="color:#0">
idx += </span><span style="color:#74A35B">9</span><span style="color:#0">
</span><span style="color:#6A9955">//@assert(x[idx] == 1)      // '10' is out of range, will panic</span></code></pre><p>
Safety when slicing a sized value. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#3186CD">const</span><span style="color:#0"> [..] </span><span style="color:#ED9A11">s32</span><span style="color:#0"> = [</span><span style="color:#74A35B">0</span><span style="color:#0">, </span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">]
</span><span style="color:#6A9955">//var slice = x[1..4]              // '4' is out of range, will panic</span><span style="color:#0">
</span><span style="color:#6A9955">//@assert(slice[0] == 1)</span></code></pre><pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#BB6643">"string"</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> idx = </span><span style="color:#74A35B">10</span><span style="color:#0">
</span><span style="color:#6A9955">//var slice = x[0..idx]            // '10' is out of range, will panic</span><span style="color:#0">
</span><span style="color:#6A9955">//@assert(slice[0] == "s"'u8)</span></code></pre><p>
<h3 id="math">math </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("math", true)]</span></code></pre><p>
Swag will panic if some math operations are invalid. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> x = </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> y = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">
</span><span style="color:#6A9955">//var z = x / y        // Division by zero, panic</span><span style="color:#0">
</span><span style="color:#6A9955">//@print(z)</span></code></pre><p>
Swag will also check for invalid arguments on some math intrinsics. </p>
<pre><code><span style="color:#6A9955">// All of this will panic if the arguments are unsupported.</span><span style="color:#0">

</span><span style="color:#6A9955">//@abs(-128)</span><span style="color:#0">
</span><span style="color:#6A9955">//@log(-2'f32)</span><span style="color:#0">
</span><span style="color:#6A9955">//@log2(-2'f32)</span><span style="color:#0">
</span><span style="color:#6A9955">//@log10(2'f64)</span><span style="color:#0">
</span><span style="color:#6A9955">//@sqrt(-2'f32)</span><span style="color:#0">
</span><span style="color:#6A9955">//@asin(-2'f32)</span><span style="color:#0">
</span><span style="color:#6A9955">//@acos(2'f32)</span></code></pre><p>
<h3 id="switch">switch </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("switch", true)]</span></code></pre><p>
Swag will panic if a switch is marked with <code>#[Swag.Complete]</code>, but the value is not covered by a 'case'. </p>
<p>
<h3 id="bool">bool </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("bool", true)]</span></code></pre><p>
Swag will panic if a boolean value is not <code>true</code> (1) or <code>false</code> (0). </p>
<p>
<h3 id="nan">nan </h3></p>
<p>
</p>
<pre><code><span style="color:#7F7F7F">#[Swag.Safety("nan", true)]</span></code></pre><p>
Swag will panic if a floating point <code>NaN</code> is used in an operation. </p>

<h2 id="180_compiler_declarations">Compiler declarations</h2>

<h3 id="181__compile_time_evaluation">Compile time evaluation</h3>
<p>
<code>#assert</code> is a static assert (at compile time). </p>
<pre><code><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3186CD">true</span></code></pre><p>
<code>@defined(SYMBOL)</code> returns true, at compile time, if the given symbol exists in the current context. </p>
<pre><code><span style="color:#7F7F7F">#assert</span><span style="color:#0"> !</span><span style="color:#B4B44A">@defined</span><span style="color:#0">(</span><span style="color:#3BC3A7">DOES_NOT_EXISTS</span><span style="color:#0">)
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@defined</span><span style="color:#0">(</span><span style="color:#3BC3A7">Global</span><span style="color:#0">)
</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">Global</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span></code></pre><p>
A static <code>#if/#elif/#else</code>, with an expression that can be evaluated at compile time. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">DEBUG</span><span style="color:#0"> = </span><span style="color:#74A35B">1</span><span style="color:#0">
</span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">RELEASE</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">
</span><span style="color:#7F7F7F">#if</span><span style="color:#0"> </span><span style="color:#3BC3A7">DEBUG</span><span style="color:#0">
{
}
</span><span style="color:#7F7F7F">#elif</span><span style="color:#0"> </span><span style="color:#3BC3A7">RELEASE</span><span style="color:#0">
{
}
</span><span style="color:#7F7F7F">#else</span><span style="color:#0">
{
}</span></code></pre><p>
<code>#error</code> to raise a compile time error, and <code>#warning</code> to raise a compile time warning. </p>
<pre><code><span style="color:#7F7F7F">#if</span><span style="color:#0"> </span><span style="color:#3186CD">false</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#error</span><span style="color:#0">   </span><span style="color:#BB6643">"this is an error"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#warning</span><span style="color:#0"> </span><span style="color:#BB6643">"this is a warning"</span><span style="color:#0">
}

</span><span style="color:#6A9955">// 'isThisDebug' is marked with 'Swag.ConstExpr', so it can be automatically evaluated</span><span style="color:#0">
</span><span style="color:#6A9955">// at compile time</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isThisDebug</span><span style="color:#0">() => </span><span style="color:#3186CD">true</span><span style="color:#0">

</span><span style="color:#6A9955">// This call is valid</span><span style="color:#0">
</span><span style="color:#7F7F7F">#if</span><span style="color:#0"> </span><span style="color:#FF6A00">isThisDebug</span><span style="color:#0">() == </span><span style="color:#3186CD">false</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#error</span><span style="color:#0"> </span><span style="color:#BB6643">"this should not be called !"</span><span style="color:#0">
}

</span><span style="color:#6A9955">// This time 'isThisRelease' is not marked with 'Swag.ConstExpr'</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">isThisRelease</span><span style="color:#0">() => </span><span style="color:#3186CD">true</span><span style="color:#0">

</span><span style="color:#6A9955">// But this call is still valid because we force the compile time execution with '#run'</span><span style="color:#0">
</span><span style="color:#7F7F7F">#if</span><span style="color:#0"> </span><span style="color:#FF6A00">#run</span><span style="color:#0"> </span><span style="color:#FF6A00">isThisRelease</span><span style="color:#0">() == </span><span style="color:#3186CD">false</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#error</span><span style="color:#0"> </span><span style="color:#BB6643">"this should not be called !"</span><span style="color:#0">
}</span></code></pre><p>
A more complicated <code>#assert</code>. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">factorial</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#B040BE">if</span><span style="color:#0"> x == </span><span style="color:#74A35B">1</span><span style="color:#0"> </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B040BE">return</span><span style="color:#0"> x * </span><span style="color:#FF6A00">factorial</span><span style="color:#0">(x - </span><span style="color:#74A35B">1</span><span style="color:#0">)
}

</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#FF6A00">factorial</span><span style="color:#0">(</span><span style="color:#74A35B">4</span><span style="color:#0">) == </span><span style="color:#74A35B">24</span><span style="color:#0"> </span><span style="color:#6A9955">// Evaluated at compile time</span></code></pre>
<h3 id="182__special_functions">Special functions</h3>
<pre><code><span style="color:#7F7F7F">#global</span><span style="color:#0"> skip</span></code></pre><p>
<h4 id="#test">#test </h4></p>
<p>
<code>#test</code> is a special function than can be used in the <code>tests/</code> folder of the workspace. All <code>#test</code> will be executed only if swag is running in test mode. </p>
<p>
<h4 id="#main">#main </h4></p>
<p>
<code>#main</code> is the program entry point. It can only be defined <b>once</b> per module, and has meaning only for an executable. </p>
<pre><code><span style="color:#FF6A00">#main</span><span style="color:#0">
{
}</span></code></pre><p>
Unlike the C function <code>main()</code>, there's no argument, but you can use the intrinsic <code>@args</code> to get a slice of all the parameters. </p>
<pre><code><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> myArgs = </span><span style="color:#B4B44A">@args</span><span style="color:#0">()
    </span><span style="color:#3186CD">var</span><span style="color:#0"> count = </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(myArgs)
    </span><span style="color:#B040BE">if</span><span style="color:#0"> myArgs[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#BB6643">"fullscreen"</span><span style="color:#0">
    {
        ...
    }
}</span></code></pre><p>
<h4 id="#init">#init </h4></p>
<p>
<code>#init</code> will be called at runtime, during the module initialization. You can have as many <code>#init</code> as you want, but the execution order in the same module is undefined. </p>
<pre><code><span style="color:#FF6A00">#init</span><span style="color:#0">
{
}</span></code></pre><p>
<h4 id="#drop">#drop </h4></p>
<p>
<code>#drop</code> will be called at runtime, when module is unloaded. You can have as many <code>#drop</code> as you want. The execution order in the same module is undefined, but is always the inverse order of <code>#init</code>. </p>
<pre><code><span style="color:#FF6A00">#drop</span><span style="color:#0">
{
}</span></code></pre><p>
<h4 id="#premain">#premain </h4></p>
<p>
<code>#premain</code> will be called after all the modules have done their <code>#init</code> code, but before the <code>#main</code> function is called. </p>
<pre><code><span style="color:#FF6A00">#premain</span><span style="color:#0">
{
}</span></code></pre>
<h3 id="183__run">Run</h3>
<p>
<code>#run</code> is a special function that will be called at <b>compile time</b>. It can be used to precompute some global values for example. </p>
<pre><code><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0">: [</span><span style="color:#74A35B">5</span><span style="color:#0">] </span><span style="color:#ED9A11">f32</span><span style="color:#0"> = </span><span style="color:#3186CD">undefined</span></code></pre><p>
Initialize <code>G</code> with <code>[1,2,4,8,16]</code> at compile time. </p>
<pre><code><span style="color:#FF6A00">#run</span><span style="color:#0">
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> value = </span><span style="color:#74A35B">1</span><span style="color:#0">'</span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> i: </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0">)
    {
        </span><span style="color:#3BC3A7">G</span><span style="color:#0">[i] = value
        value *= </span><span style="color:#74A35B">2</span><span style="color:#0">
    }
}</span></code></pre><p>
<code>#test</code> are executed after <code>#run</code>, even at compile time (during testing). So we can test the values of <code>G</code> here. </p>
<pre><code><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0">[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0">[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0">[</span><span style="color:#74A35B">2</span><span style="color:#0">] == </span><span style="color:#74A35B">4</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0">[</span><span style="color:#74A35B">3</span><span style="color:#0">] == </span><span style="color:#74A35B">8</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0">[</span><span style="color:#74A35B">4</span><span style="color:#0">] == </span><span style="color:#74A35B">16</span><span style="color:#0">)</span></code></pre><p>
<code>#run</code> can also be used as an expression, to call for example a function not marked with <code>#[Swag.ConstExpr]</code>. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">SumValue</span><span style="color:#0"> = </span><span style="color:#FF6A00">#run</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(</span><span style="color:#74A35B">1</span><span style="color:#0">, </span><span style="color:#74A35B">2</span><span style="color:#0">, </span><span style="color:#74A35B">3</span><span style="color:#0">, </span><span style="color:#74A35B">4</span><span style="color:#0">) + </span><span style="color:#74A35B">10</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">SumValue</span><span style="color:#0"> == </span><span style="color:#74A35B">20</span><span style="color:#0">

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">sum</span><span style="color:#0">(values: </span><span style="color:#ED9A11">s32</span><span style="color:#0">...)-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> result = </span><span style="color:#74A35B">0</span><span style="color:#0">'</span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> v: values
        result += v
    </span><span style="color:#B040BE">return</span><span style="color:#0"> result
}</span></code></pre><p>
<code>#run</code> can also be used as an expression block. The return type is deduced from the <code>return</code> statement. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0"> = </span><span style="color:#FF6A00">#run</span><span style="color:#0"> {
    </span><span style="color:#3186CD">var</span><span style="color:#0"> result: </span><span style="color:#ED9A11">f32</span><span style="color:#0">
    </span><span style="color:#B040BE">loop</span><span style="color:#0"> </span><span style="color:#74A35B">10</span><span style="color:#0">
        result += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#B040BE">return</span><span style="color:#0"> result   </span><span style="color:#6A9955">// 'Value' will be of type 'f32'</span><span style="color:#0">
}
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#3BC3A7">Value</span><span style="color:#0"> == </span><span style="color:#74A35B">10.0</span></code></pre>
<h3 id="184__global">Global</h3>
<p>
A bunch of <code>#global</code> can start a source file. </p>
<pre><code><span style="color:#6A9955">// Skip the content of the file, like this one (but must be a valid swag file)</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> skip

</span><span style="color:#6A9955">// All symbols in the file will be public/internal</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> </span><span style="color:#3186CD">public</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> </span><span style="color:#3186CD">internal</span><span style="color:#0">

</span><span style="color:#6A9955">// All symbols in the file will go in the namespace 'Toto'</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> </span><span style="color:#3186CD">namespace</span><span style="color:#0"> </span><span style="color:#3BC3A7">Toto</span><span style="color:#0">

</span><span style="color:#6A9955">// A #if for the whole file</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#3BC3A7">DEBUG</span><span style="color:#0"> == </span><span style="color:#3186CD">true</span><span style="color:#0">

</span><span style="color:#6A9955">// Some attributes can be assigned to the full file</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> </span><span style="color:#7F7F7F">#[Swag.Safety("", true)]</span><span style="color:#0">

</span><span style="color:#6A9955">// The file will be exported for external usage</span><span style="color:#0">
</span><span style="color:#6A9955">// It's like putting everything in public, except that the file will</span><span style="color:#0">
</span><span style="color:#6A9955">// be copied in its totality in the public folder</span><span style="color:#0">
</span><span style="color:#7F7F7F">#global</span><span style="color:#0"> export

</span><span style="color:#6A9955">// Link with a given external library</span><span style="color:#0">
</span><span style="color:#7F7F7F">#foreignlib</span><span style="color:#0"> </span><span style="color:#BB6643">"windows.lib"</span></code></pre>
<h3 id="185__var">Var</h3>
<p>
A global variable can be tagged with <code>#[Swag.Tls]</code> to store it in the thread local storage (one copy per thread). </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Tls]</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span></code></pre><p>

  A local variable can be tagged with <code>#[Swag.Global]</code> to make it global (aka <code>static</code> in C/C++). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">toto</span><span style="color:#0">()-></span><span style="color:#ED9A11">s32</span><span style="color:#0">
    {
        </span><span style="color:#7F7F7F">#[Swag.Global]</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">G1</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">

        </span><span style="color:#3BC3A7">G1</span><span style="color:#0"> += </span><span style="color:#74A35B">1</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3BC3A7">G1</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toto</span><span style="color:#0">() == </span><span style="color:#74A35B">1</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toto</span><span style="color:#0">() == </span><span style="color:#74A35B">2</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#FF6A00">toto</span><span style="color:#0">() == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>
A global variable can also be marked as <code>#[Swag.Compiler]</code>. That kind of variable will not be exported to the runtime and can only be used in compile time code. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Compiler]</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">G2</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">

</span><span style="color:#FF6A00">#run</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">G2</span><span style="color:#0"> += </span><span style="color:#74A35B">5</span><span style="color:#0">
}</span></code></pre>
<h2 id="190_attributes">Attributes</h2>
<p>
Attributes are tags associated with functions, structures etc... </p>

<h3 id="191__user_attributes">User attributes</h3>
<p>
User attributes are declared like functions, but with the <code>attr</code> keyword before instead of <code>func</code>. </p>
<pre><code><span style="color:#3186CD">using</span><span style="color:#0"> </span><span style="color:#3BC3A7">Swag</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">AttributeA</span><span style="color:#0">()</span></code></pre><p>
Like functions, attributes can have parameters. </p>
<pre><code><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">AttributeB</span><span style="color:#0">(x, y: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, z: </span><span style="color:#ED9A11">string</span><span style="color:#0">)</span></code></pre><p>
So attributes can also have default values. </p>
<pre><code><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">AttributeBA</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">, y: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">"string"</span><span style="color:#0">)</span></code></pre><p>
You can define a usage before the attribute definition to restrict its usage. </p>
<pre><code><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">AttributeC</span><span style="color:#0">()</span></code></pre><p>
To use an attribute, the syntax is <code>#[attribute, attribute...]</code>. It should be placed <b>before</b> the thing you want to tag. </p>
<pre><code><span style="color:#7F7F7F">#[AttributeA, AttributeB(0, 0, "string")]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">function1</span><span style="color:#0">()
{
}</span></code></pre><p>
You can declare multiple usages. </p>
<pre><code><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">AttributeD</span><span style="color:#0">(x: </span><span style="color:#ED9A11">s32</span><span style="color:#0">);

</span><span style="color:#7F7F7F">#[AttributeD(6)]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">function2</span><span style="color:#0">()
{
}

</span><span style="color:#7F7F7F">#[AttributeD(150)]</span><span style="color:#0">
</span><span style="color:#3186CD">struct</span><span style="color:#0"> struct1
{
}</span></code></pre><p>
Finaly, attributes can be retrieved at runtime thanks to <b>type reflection</b>. </p>
<pre><code><span style="color:#3186CD">let</span><span style="color:#0"> type = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(function2)                  </span><span style="color:#6A9955">// Get the type of the function</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@countof</span><span style="color:#0">(type.attributes) == </span><span style="color:#74A35B">1</span><span style="color:#0">)     </span><span style="color:#6A9955">// Check that the function has one attribute associated with it</span></code></pre>
<h3 id="192__predefined_attributes">Predefined attributes</h3>
<p>
This is the list of predefined attributes. All are located in the reserved <code>Swag</code> namespace. </p>
<pre><code><span style="color:#7F7F7F">#global</span><span style="color:#0"> skip

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">ConstExpr</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">PrintBc</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Compiler</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Inline</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Macro</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Mixin</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Test</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Implicit</span><span style="color:#0">()

</span><span style="color:#6A9955">// Hardcoded also for switch</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Complete</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">CalleeReturn</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Foreign</span><span style="color:#0">(module: </span><span style="color:#ED9A11">string</span><span style="color:#0">, function: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#BB6643">""</span><span style="color:#0">);

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Callback</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function|AttributeUsage.Variable)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Discardable</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Deprecated</span><span style="color:#0">(msg: </span><span style="color:#ED9A11">string</span><span style="color:#0"> = </span><span style="color:#3186CD">null</span><span style="color:#0">)

</span><span style="color:#6A9955">// Hardcoded for type typealias</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Strict</span><span style="color:#0">()

</span><span style="color:#6A9955">// Hardcoded for local variables</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Global</span><span style="color:#0">()

</span><span style="color:#6A9955">// Hardcoded for struct and variables</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Align</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Struct)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Pack</span><span style="color:#0">(value: </span><span style="color:#ED9A11">u8</span><span style="color:#0">)

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Struct)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">NoCopy</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.StructVariable)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Offset</span><span style="color:#0">(name: </span><span style="color:#ED9A11">string</span><span style="color:#0">)

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Enum)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">EnumFlags</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Enum)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">EnumIndex</span><span style="color:#0">()

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.All|AttributeUsage.File)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Safety</span><span style="color:#0">(what: </span><span style="color:#ED9A11">string</span><span style="color:#0">, value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">)

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.All)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">SelectIf</span><span style="color:#0">(value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">)

</span><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function|AttributeUsage.File)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Optim</span><span style="color:#0">(what: </span><span style="color:#ED9A11">string</span><span style="color:#0">, value: </span><span style="color:#ED9A11">bool</span><span style="color:#0">)</span></code></pre>
<h2 id="200_type_reflection">Type reflection</h2>
<p>
In Swag, types are also values that can be inspected at compile time or at runtime. The two main intrinsics for this are <code>@typeof</code> and <code>@kindof</code>. </p>
<p>

  You can get the type of an expression with <code>@typeof</code>, or just with the type itself (<b>types are also values</b>). </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr1 = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">s8</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr1.name == </span><span style="color:#BB6643">"s8"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr1 == </span><span style="color:#ED9A11">s8</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr2 = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">s16</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr2.name == </span><span style="color:#BB6643">"s16"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr2 == </span><span style="color:#ED9A11">s16</span><span style="color:#0">)

    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr3 = </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr3.name == </span><span style="color:#BB6643">"s32"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr3 == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">))

    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr4 = </span><span style="color:#ED9A11">s64</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr4.name == </span><span style="color:#BB6643">"s64"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr4 == </span><span style="color:#ED9A11">s64</span><span style="color:#0">)
}</span></code></pre><p>

  The return result of <code>@typeof</code> is a const pointer to a <code>Swag.TypeInfo</code> kind of structure. This is an typealias for the <code>typeinfo</code> type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = </span><span style="color:#ED9A11">bool</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(ptr) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">TypeInfoNative</span><span style="color:#0">))

    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr1 = </span><span style="color:#C3973B">#type</span><span style="color:#0"> [</span><span style="color:#74A35B">2</span><span style="color:#0">] </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(ptr1) == </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">TypeInfoArray</span><span style="color:#0">))
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr1.name == </span><span style="color:#BB6643">"[2] s32"</span><span style="color:#0">)
}</span></code></pre><p>

  The <code>TypeInfo</code> structure contains a different enum value for each type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> ptr = </span><span style="color:#ED9A11">f64</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(ptr.kind).fullname == </span><span style="color:#BB6643">"Swag.TypeInfoKind"</span><span style="color:#0">)
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(ptr.sizeof == </span><span style="color:#B4B44A">@sizeof</span><span style="color:#0">(</span><span style="color:#ED9A11">f64</span><span style="color:#0">))
}</span></code></pre><p>
<h3 id="@decltype">@decltype </h3></p>
<p>
</p>
<p>

  <code>@decltype</code> can be used to transform a <code>typeinfo</code> to a real compiler type. This is the opposite of <code>@typeof</code> or <code>@kindof</code>. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#B4B44A">@decltype</span><span style="color:#0">(</span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#ED9A11">s32</span><span style="color:#0">))
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>

  <code>@decltype</code> can evaluate a constexpr expression that returns a <code>typeinfo</code> to determine the real type. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#7F7F7F">#[Swag.ConstExpr]</span><span style="color:#0">
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">getType</span><span style="color:#0">(needAString: </span><span style="color:#ED9A11">bool</span><span style="color:#0">)-></span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">if</span><span style="color:#0"> needAString
            </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#ED9A11">string</span><span style="color:#0">
        </span><span style="color:#B040BE">else</span><span style="color:#0">
            </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    }

    </span><span style="color:#3186CD">var</span><span style="color:#0"> x: </span><span style="color:#B4B44A">@decltype</span><span style="color:#0">(</span><span style="color:#FF6A00">getType</span><span style="color:#0">(needAString = </span><span style="color:#3186CD">false</span><span style="color:#0">))
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x) == </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    x = </span><span style="color:#74A35B">0</span><span style="color:#0">

    </span><span style="color:#3186CD">var</span><span style="color:#0"> x1: </span><span style="color:#B4B44A">@decltype</span><span style="color:#0">(</span><span style="color:#FF6A00">getType</span><span style="color:#0">(needAString = </span><span style="color:#3186CD">true</span><span style="color:#0">))
    </span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(x1) == </span><span style="color:#ED9A11">string</span><span style="color:#0">
    x1 = </span><span style="color:#BB6643">"0"</span><span style="color:#0">
}</span></code></pre>
<h2 id="210_code_inspection">Code inspection</h2>
<p>
<code>#message</code> is a special function that will be called by the compiler when something specific occurs during the build. The parameter of <code>#message</code> is a mask that tells the compiler when to call the function. </p>
<p>
With the <code>Swag.CompilerMsgMask.SemFunctions</code> flag, for example, <code>#message</code> will be called each time a function of the module <b>has been typed</b>. You can then use <code>getMessage()</code> in the <code>@compiler()</code> interface to retrieve some informations about the reason of the call. </p>
<pre><code><span style="color:#FF6A00">#message</span><span style="color:#0">(</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">CompilerMsgMask</span><span style="color:#0">.</span><span style="color:#3BC3A7">SemFunctions</span><span style="color:#0">)
{
    </span><span style="color:#6A9955">// Get the interface to communicate with the compiler</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> itf = </span><span style="color:#B4B44A">@compiler</span><span style="color:#0">()

    </span><span style="color:#6A9955">// Get the current message</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> msg = itf.</span><span style="color:#FF6A00">getMessage</span><span style="color:#0">()

    </span><span style="color:#6A9955">// We know that the type in the message is a function because of the '#message' mask.</span><span style="color:#0">
    </span><span style="color:#6A9955">// So we can safely cast.</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> typeFunc = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">TypeInfoFunc</span><span style="color:#0">) msg.type

    </span><span style="color:#6A9955">// The message name, for `Swag.CompilerMsgMask.SemFunctions`, is the name of the</span><span style="color:#0">
    </span><span style="color:#6A9955">// function being compiled.</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> nameFunc = msg.name

    </span><span style="color:#6A9955">// As an example, we count that name if it starts with "XX".</span><span style="color:#0">
    </span><span style="color:#6A9955">// But we could use custom function attributes instead...</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(nameFunc) > </span><span style="color:#74A35B">2</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> nameFunc[</span><span style="color:#74A35B">0</span><span style="color:#0">] == </span><span style="color:#BB6643">"X"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0"> </span><span style="color:#B040BE">and</span><span style="color:#0"> nameFunc[</span><span style="color:#74A35B">1</span><span style="color:#0">] == </span><span style="color:#BB6643">"X"</span><span style="color:#0">'</span><span style="color:#ED9A11">u8</span><span style="color:#0">
        </span><span style="color:#3BC3A7">G</span><span style="color:#0"> += </span><span style="color:#74A35B">1</span><span style="color:#0">
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> </span><span style="color:#3BC3A7">G</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">

</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">XXTestFunc1</span><span style="color:#0">() {}
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">XXTestFunc2</span><span style="color:#0">() {}
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#3BC3A7">XXTestFunc3</span><span style="color:#0">() {}</span></code></pre><p>
The compiler will call the following function after the semantic pass. So after <b>all the functions</b> of the module have been parsed. </p>
<pre><code><span style="color:#FF6A00">#message</span><span style="color:#0">(</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">CompilerMsgMask</span><span style="color:#0">.</span><span style="color:#3BC3A7">PassAfterSemantic</span><span style="color:#0">)
{
    </span><span style="color:#6A9955">// We should have found 3 functions starting with "XX"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(</span><span style="color:#3BC3A7">G</span><span style="color:#0"> == </span><span style="color:#74A35B">3</span><span style="color:#0">)
}</span></code></pre><p>
This will be called for every global variables of the module. </p>
<pre><code><span style="color:#FF6A00">#message</span><span style="color:#0">(</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">CompilerMsgMask</span><span style="color:#0">.</span><span style="color:#3BC3A7">SemGlobals</span><span style="color:#0">)
{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> itf = </span><span style="color:#B4B44A">@compiler</span><span style="color:#0">()
    </span><span style="color:#3186CD">var</span><span style="color:#0"> msg = itf.</span><span style="color:#FF6A00">getMessage</span><span style="color:#0">()
}</span></code></pre><p>
This will be called for every global types of the module (structs, enums, interfaces...). </p>
<pre><code><span style="color:#FF6A00">#message</span><span style="color:#0">(</span><span style="color:#3BC3A7">Swag</span><span style="color:#0">.</span><span style="color:#3BC3A7">CompilerMsgMask</span><span style="color:#0">.</span><span style="color:#3BC3A7">SemTypes</span><span style="color:#0">)
{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> itf = </span><span style="color:#B4B44A">@compiler</span><span style="color:#0">()
    </span><span style="color:#3186CD">var</span><span style="color:#0"> msg = itf.</span><span style="color:#FF6A00">getMessage</span><span style="color:#0">()
}</span></code></pre>
<h2 id="220_meta_programmation">Meta programmation</h2>
<p>
In Swag you can construct some source code at compile time, which will then be compiled. The source code you provide in the form of a <b>string</b> must be a valid Swag program. </p>

<h3 id="221__ast">Ast</h3>
<p>
The most simple way to produce a string which contains the Swag code to compile is with an <code>#ast</code> block. An <code>#ast</code> block is executed at compile time and the string it returns will be compiled <b>inplace</b>. </p>
<p>

  The <code>#ast</code> can be a simple expression with the string to compile. </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0"> </span><span style="color:#BB6643">"var x = 666"</span><span style="color:#0">
    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(x == </span><span style="color:#74A35B">666</span><span style="color:#0">)
}</span></code></pre><p>

  Or it can be a block, with an explicit <code>return</code> </p>
<pre><code><span style="color:#0">{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> cpt = </span><span style="color:#74A35B">2</span><span style="color:#0">
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">INC</span><span style="color:#0"> = </span><span style="color:#74A35B">5</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#BB6643">"cpt += "</span><span style="color:#0"> ++ </span><span style="color:#3BC3A7">INC</span><span style="color:#0">   </span><span style="color:#6A9955">// Equivalent to 'cpt += 5'</span><span style="color:#0">
    }

    </span><span style="color:#B4B44A">@assert</span><span style="color:#0">(cpt == </span><span style="color:#74A35B">7</span><span style="color:#0">)
}</span></code></pre><p>
<h4 id="Struct and enums">Struct and enums </h4></p>
<p>
<code>#ast</code> can for example be used to generate the content of a <code>struct</code> or <code>enum</code>. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#BB6643">"x, y: s32 = 666"</span><span style="color:#0">
    }
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">666</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">666</span><span style="color:#0">)</span></code></pre><p>
It works with generics too, and can be mixed with static declarations. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">
{
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#BB6643">"x, y: "</span><span style="color:#0"> ++ </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">).name
    }

    z: </span><span style="color:#ED9A11">string</span><span style="color:#0">
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">'</span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(v.x) == </span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(v.y) == </span><span style="color:#ED9A11">bool</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(v.z) == </span><span style="color:#ED9A11">string</span><span style="color:#0">

</span><span style="color:#3186CD">var</span><span style="color:#0"> v1: </span><span style="color:#3BC3A7">MyStruct</span><span style="color:#0">'</span><span style="color:#ED9A11">f64</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(v1.x) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(v1.y) == </span><span style="color:#ED9A11">f64</span><span style="color:#0">
</span><span style="color:#7F7F7F">#assert</span><span style="color:#0"> </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(v1.z) == </span><span style="color:#ED9A11">string</span></code></pre><p>
<code>#ast</code> needs to return a <i>string like</i> value, which can of course be dynamically constructed. </p>
<pre><code><span style="color:#7F7F7F">#[Swag.Compiler]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">append</span><span style="color:#0">(buf: ^</span><span style="color:#ED9A11">u8</span><span style="color:#0">, val: </span><span style="color:#ED9A11">string</span><span style="color:#0">)
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> len = </span><span style="color:#74A35B">0</span><span style="color:#0">
    </span><span style="color:#B040BE">while</span><span style="color:#0"> buf[len] len += </span><span style="color:#74A35B">1</span><span style="color:#0">
    </span><span style="color:#F2470C">@memcpy</span><span style="color:#0">(buf + len, </span><span style="color:#B4B44A">@dataof</span><span style="color:#0">(val), </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">u64</span><span style="color:#0">) </span><span style="color:#B4B44A">@countof</span><span style="color:#0">(val) + </span><span style="color:#74A35B">1</span><span style="color:#0">)
}

</span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">
{
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// We construct the code to compile in this local array</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> buf: [</span><span style="color:#74A35B">256</span><span style="color:#0">] </span><span style="color:#ED9A11">u8</span><span style="color:#0">
        </span><span style="color:#FF6A00">append</span><span style="color:#0">(buf, </span><span style="color:#BB6643">"x: f32 = 1\n"</span><span style="color:#0">)
        </span><span style="color:#FF6A00">append</span><span style="color:#0">(buf, </span><span style="color:#BB6643">"y: f32 = 2\n"</span><span style="color:#0">)
        </span><span style="color:#FF6A00">append</span><span style="color:#0">(buf, </span><span style="color:#BB6643">"z: f32 = 3\n"</span><span style="color:#0">)

        </span><span style="color:#6A9955">// And returns to the compiler the corresponding *code*</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#ED9A11">string</span><span style="color:#0">) buf
    }
}

</span><span style="color:#3186CD">var</span><span style="color:#0"> v: </span><span style="color:#3BC3A7">Vector3</span><span style="color:#0">
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.x == </span><span style="color:#74A35B">1</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.y == </span><span style="color:#74A35B">2</span><span style="color:#0">)
</span><span style="color:#B4B44A">@assert</span><span style="color:#0">(v.z == </span><span style="color:#74A35B">3</span><span style="color:#0">)</span></code></pre><p>
<h4 id="For example">For example </h4></p>
<p>
This is a real life example of an <code>#ast</code> usage from the <code>Std.Core</code> module. Here we generate a structure which contains all the fields of an original other structure, but where the types are forced to be <code>bool</code>. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">) </span><span style="color:#3BC3A7">IsSet</span><span style="color:#0">
{
    </span><span style="color:#FF6A00">#ast</span><span style="color:#0">
    {
        </span><span style="color:#6A9955">// A `StringBuilder` is used to manipulate dynamic strings.</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> str = </span><span style="color:#3BC3A7">StrConv</span><span style="color:#0">.</span><span style="color:#3BC3A7">StringBuilder</span><span style="color:#0">{}

        </span><span style="color:#6A9955">// We get the type of the generic parameter 'T'</span><span style="color:#0">
        </span><span style="color:#3186CD">var</span><span style="color:#0"> typeof = </span><span style="color:#B4B44A">@typeof</span><span style="color:#0">(</span><span style="color:#3BC3A7">T</span><span style="color:#0">)

        </span><span style="color:#6A9955">// Then we visit all the fields, assuming the type is a struct (or this will not compile).</span><span style="color:#0">
        </span><span style="color:#6A9955">// For each original field, we create one with the same name, but with a `bool` type.</span><span style="color:#0">
        </span><span style="color:#B040BE">visit</span><span style="color:#0"> f: typeof.fields
            str.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"%: bool\n"</span><span style="color:#0">, f.name)

        </span><span style="color:#6A9955">// Then we return the constructed source code.</span><span style="color:#0">
        </span><span style="color:#6A9955">// It will be used by the compiler to generate the content of the `IsSet` struct.</span><span style="color:#0">
        </span><span style="color:#B040BE">return</span><span style="color:#0"> str.</span><span style="color:#FF6A00">toString</span><span style="color:#0">()
    }
}</span></code></pre><p>
</p>
<p>
<h4 id="At global scope">At global scope </h4></p>
<p>
<code>#ast</code> can also be called at the global scope. </p>
<pre><code><span style="color:#FF6A00">#ast</span><span style="color:#0">
{
    </span><span style="color:#3186CD">const</span><span style="color:#0"> value = </span><span style="color:#74A35B">666</span><span style="color:#0">
    </span><span style="color:#B040BE">return</span><span style="color:#0"> </span><span style="color:#BB6643">"const myGeneratedConst = "</span><span style="color:#0"> ++ value
}</span></code></pre><p>
But be aware that you must use <code>#placeholder</code> in case you are generating global symbols that can be used by something else in the code. This will tell Swag that <i>this symbol</i> will exist at some point, so please wait for it to <i>exist</i> before complaining. </p>
<pre><code><span style="color:#7F7F7F">#placeholder</span><span style="color:#0"> myGeneratedConst   </span><span style="color:#6A9955">// Symbol `myGeneratedConst` will be generated</span></code></pre><p>
Here for example, thanks to the <code>#placeholder</code>, the <code>#assert</code> will wait for the symbol <code>myGeneratedConst</code> to be replaced with its real content. </p>
<pre><code><span style="color:#7F7F7F">#assert</span><span style="color:#0"> myGeneratedConst == </span><span style="color:#74A35B">666</span></code></pre>
<h3 id="222__compiler_interface">Compiler interface</h3>
<p>
The other method to compile generated code is to use the function <code>compileString()</code> in the <code>@compiler()</code> interface. Of course this should be called at compile time, and mostly during a <code>#message</code> call. </p>
<p>
Here is a real life example from the <code>Std.Ogl</code> module (opengl wrapper), which uses <code>#message</code> to track functions marked with a specific <b>user attribute</b> <code>Ogl.Extension</code>, and generates some code for each function that has been found. </p>
<p>
First we declare a new specific attribute, which can then be associated with a function. </p>
<pre><code><span style="color:#7F7F7F">#[AttrUsage(AttributeUsage.Function)]</span><span style="color:#0">
</span><span style="color:#3186CD">attr</span><span style="color:#0"> </span><span style="color:#3BC3A7">Extension</span><span style="color:#0">()

</span><span style="color:#6A9955">// Here is an example of usage of that attribute.</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Extension]</span><span style="color:#0">
{
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">glUniformMatrix2x3fv</span><span style="color:#0">(location: </span><span style="color:#3BC3A7">GLint</span><span style="color:#0">, count: </span><span style="color:#3BC3A7">GLsizei</span><span style="color:#0">, transpose: </span><span style="color:#3BC3A7">GLboolean</span><span style="color:#0">, value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">GLfloat</span><span style="color:#0">);
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">glUniformMatrix2x4fv</span><span style="color:#0">(location: </span><span style="color:#3BC3A7">GLint</span><span style="color:#0">, count: </span><span style="color:#3BC3A7">GLsizei</span><span style="color:#0">, transpose: </span><span style="color:#3BC3A7">GLboolean</span><span style="color:#0">, value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">GLfloat</span><span style="color:#0">);
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">glUniformMatrix3x2fv</span><span style="color:#0">(location: </span><span style="color:#3BC3A7">GLint</span><span style="color:#0">, count: </span><span style="color:#3BC3A7">GLsizei</span><span style="color:#0">, transpose: </span><span style="color:#3BC3A7">GLboolean</span><span style="color:#0">, value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">GLfloat</span><span style="color:#0">);
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">glUniformMatrix3x4fv</span><span style="color:#0">(location: </span><span style="color:#3BC3A7">GLint</span><span style="color:#0">, count: </span><span style="color:#3BC3A7">GLsizei</span><span style="color:#0">, transpose: </span><span style="color:#3BC3A7">GLboolean</span><span style="color:#0">, value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">GLfloat</span><span style="color:#0">);
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">glUniformMatrix4x2fv</span><span style="color:#0">(location: </span><span style="color:#3BC3A7">GLint</span><span style="color:#0">, count: </span><span style="color:#3BC3A7">GLsizei</span><span style="color:#0">, transpose: </span><span style="color:#3BC3A7">GLboolean</span><span style="color:#0">, value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">GLfloat</span><span style="color:#0">);
    </span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">glUniformMatrix4x3fv</span><span style="color:#0">(location: </span><span style="color:#3BC3A7">GLint</span><span style="color:#0">, count: </span><span style="color:#3BC3A7">GLsizei</span><span style="color:#0">, transpose: </span><span style="color:#3BC3A7">GLboolean</span><span style="color:#0">, value: </span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">GLfloat</span><span style="color:#0">);
}</span></code></pre><p>
The following will be used to track the functions with that specific attribute. </p>
<pre><code><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">OneFunc</span><span style="color:#0">
{
    type: </span><span style="color:#ED9A11">typeinfo</span><span style="color:#0">
    name: </span><span style="color:#ED9A11">string</span><span style="color:#0">
}

</span><span style="color:#7F7F7F">#[Compiler]</span><span style="color:#0">
</span><span style="color:#3186CD">var</span><span style="color:#0"> g_Functions: </span><span style="color:#3BC3A7">Array</span><span style="color:#0">'</span><span style="color:#3BC3A7">OneFunc</span></code></pre><p>
This <code>#message</code> will be called for each function of the <code>Ogl</code> module. </p>
<pre><code><span style="color:#FF6A00">#message</span><span style="color:#0">(</span><span style="color:#3BC3A7">CompilerMsgMask</span><span style="color:#0">.</span><span style="color:#3BC3A7">SemFunctions</span><span style="color:#0">)
{
    </span><span style="color:#3186CD">let</span><span style="color:#0"> itf = </span><span style="color:#B4B44A">@compiler</span><span style="color:#0">()
    </span><span style="color:#3186CD">var</span><span style="color:#0"> msg = itf.</span><span style="color:#FF6A00">getMessage</span><span style="color:#0">()

    </span><span style="color:#6A9955">// If the function does not have our attribute, forget it</span><span style="color:#0">
    </span><span style="color:#B040BE">if</span><span style="color:#0"> !</span><span style="color:#3BC3A7">Reflection</span><span style="color:#0">.</span><span style="color:#FF6A00">hasAttribute</span><span style="color:#0">(msg.type, </span><span style="color:#3BC3A7">Extension</span><span style="color:#0">)
        </span><span style="color:#B040BE">return</span><span style="color:#0">

    </span><span style="color:#6A9955">// We just track all the functions with the given attribute</span><span style="color:#0">
    g_Functions.</span><span style="color:#FF6A00">add</span><span style="color:#0">({msg.type, msg.name})
}</span></code></pre><p>
We will generate a <code>glInitExtensions</code> global function, so we register it as a place holder. </p>
<pre><code><span style="color:#7F7F7F">#placeholder</span><span style="color:#0"> glInitExtensions</span></code></pre><p>
This is called once all functions of the module have been typed, and this is the main code generation. </p>
<pre><code><span style="color:#FF6A00">#message</span><span style="color:#0">(</span><span style="color:#3BC3A7">CompilerMsgMask</span><span style="color:#0">.</span><span style="color:#3BC3A7">PassAfterSemantic</span><span style="color:#0">)
{
    </span><span style="color:#3186CD">var</span><span style="color:#0"> builderVars: </span><span style="color:#3BC3A7">StringBuilder</span><span style="color:#0">
    </span><span style="color:#3186CD">var</span><span style="color:#0"> builderInit: </span><span style="color:#3BC3A7">StringBuilder</span><span style="color:#0">

    </span><span style="color:#6A9955">// Generate the `glInitExtensions` function</span><span style="color:#0">
    builderInit.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">"public func glInitExtensions()\n{\n"</span><span style="color:#0">);

    </span><span style="color:#6A9955">// Visit all functions we have registered, i.e. all functions with the `Ogl.Extension` attribute.</span><span style="color:#0">
    </span><span style="color:#B040BE">visit</span><span style="color:#0"> e: g_Functions
    {
        </span><span style="color:#3186CD">var</span><span style="color:#0"> typeFunc = </span><span style="color:#3186CD">cast</span><span style="color:#0">(</span><span style="color:#3186CD">const</span><span style="color:#0"> *</span><span style="color:#3BC3A7">TypeInfoFunc</span><span style="color:#0">) e.type

        </span><span style="color:#6A9955">// Declare a lambda variable for that extension</span><span style="color:#0">
        builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"var ext_%: %\n"</span><span style="color:#0">, e.name, typeFunc.name)

        </span><span style="color:#6A9955">// Make a wrapper function</span><span style="color:#0">
        builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"public func %("</span><span style="color:#0">, e.name)
        </span><span style="color:#B040BE">visit</span><span style="color:#0"> p, i: typeFunc.parameters
        {
            </span><span style="color:#B040BE">if</span><span style="color:#0"> i != </span><span style="color:#74A35B">0</span><span style="color:#0"> builderVars.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">", "</span><span style="color:#0">)
            builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"p%: %"</span><span style="color:#0">, i, p.pointedType.name)
        }

        </span><span style="color:#B040BE">if</span><span style="color:#0"> typeFunc.returnType == </span><span style="color:#ED9A11">void</span><span style="color:#0">
            builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">")\n{\n"</span><span style="color:#0">)
        </span><span style="color:#B040BE">else</span><span style="color:#0">
            builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">")->%\n{\n"</span><span style="color:#0">, typeFunc.returnType.name)
        builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"\treturn ext_%("</span><span style="color:#0">, e.name)
        </span><span style="color:#B040BE">visit</span><span style="color:#0"> p, i: typeFunc.parameters
        {
            </span><span style="color:#B040BE">if</span><span style="color:#0"> i != </span><span style="color:#74A35B">0</span><span style="color:#0"> builderVars.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">", "</span><span style="color:#0">)
            builderVars.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"p%"</span><span style="color:#0">, i)
        }

        builderVars.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">");\n}\n\n"</span><span style="color:#0">)

        </span><span style="color:#6A9955">// Initialize the variable with the getExtensionAddress</span><span style="color:#0">
        builderInit.</span><span style="color:#FF6A00">appendFormat</span><span style="color:#0">(</span><span style="color:#BB6643">"\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n"</span><span style="color:#0">, e.name, typeFunc.name, e.name);
    }

    </span><span style="color:#6A9955">// Compile !!</span><span style="color:#0">
    </span><span style="color:#3186CD">let</span><span style="color:#0"> itf = </span><span style="color:#B4B44A">@compiler</span><span style="color:#0">()
    </span><span style="color:#3186CD">var</span><span style="color:#0"> str = builderVars.</span><span style="color:#FF6A00">toString</span><span style="color:#0">()
    itf.</span><span style="color:#FF6A00">compileString</span><span style="color:#0">(str.</span><span style="color:#FF6A00">toString</span><span style="color:#0">())

    builderInit.</span><span style="color:#FF6A00">appendString</span><span style="color:#0">(</span><span style="color:#BB6643">"}\n"</span><span style="color:#0">);
    str = builderInit.</span><span style="color:#FF6A00">toString</span><span style="color:#0">()
    itf.</span><span style="color:#FF6A00">compileString</span><span style="color:#0">(str.</span><span style="color:#FF6A00">toString</span><span style="color:#0">())
}</span></code></pre><p>
</p>

<h2 id="230_documentation">Documentation</h2>
<p>
The Swag compiler can generate documentation for all the modules of a given workspace. </p>
<pre><code><span style="color:#0">swag doc -w:myWorkspaceFolder</span></code></pre><p>
The main module documentation should be placed at the top of the corresponding <code>module.swg</code> file. The rest is placed in various source files. </p>
<p>
The documentation comment needs to be placed just before a function, struct or enum. </p>
<pre><code><span style="color:#6A9955">// Everything between empty lines is considered to be a simple paragraph. Which</span><span style="color:#0">
</span><span style="color:#6A9955">// means that if you put several comments on several lines like this, they all</span><span style="color:#0">
</span><span style="color:#6A9955">// will be part of the same paragraph.</span><span style="color:#0">
</span><span style="color:#6A9955">//</span><span style="color:#0">
</span><span style="color:#6A9955">// This is another paragraph because there's an empty line before.</span><span style="color:#0">
</span><span style="color:#6A9955">//</span><span style="color:#0">
</span><span style="color:#6A9955">// This is yet another paragraph.</span></code></pre><p>
The first paragraph is considered to be the 'short description' which can appear on specific parts of the documentation. So make it short. </p>
<p>
If the first line ends with a dot <code>.</code>, then this marks the end of the paragraph, i.e. the end of the short description. </p>
<pre><code><span style="color:#6A9955">// This is the short description.</span><span style="color:#0">
</span><span style="color:#6A9955">// As the previous first line ends with '.', this is another paragraph, so this should be</span><span style="color:#0">
</span><span style="color:#6A9955">// the long description. No need for an empty line before.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">test</span><span style="color:#0">()
{
}</span></code></pre><p>
A paragraph that starts with <code>---</code> is a paragraph where every blanks and end of lines are respected. </p>
<pre><code><span style="color:#6A9955">// ---</span><span style="color:#0">
</span><span style="color:#6A9955">// Even...</span><span style="color:#0">
</span><span style="color:#6A9955">//</span><span style="color:#0">
</span><span style="color:#6A9955">// ...empty lines are preserved.</span><span style="color:#0">
</span><span style="color:#6A9955">//</span><span style="color:#0">
</span><span style="color:#6A9955">// You end that kind of paragraph with another '---' alone on its line.</span><span style="color:#0">
</span><span style="color:#6A9955">// ---</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">test</span><span style="color:#0">()
{
}</span></code></pre><p>
You can create a list of bullet points with <code>*</code>. </p>
<pre><code><span style="color:#0"> </span><span style="color:#FF6A00">#test</span><span style="color:#0">
 {
    </span><span style="color:#6A9955">// * This is a bullet point</span><span style="color:#0">
    </span><span style="color:#6A9955">// * This is a bullet point</span><span style="color:#0">
    </span><span style="color:#6A9955">// * This is a bullet point</span><span style="color:#0">
    </span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
    {
        r, g, b: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
    }
}</span></code></pre><p>
You can create a quote with <code>></code> </p>
<pre><code><span style="color:#6A9955">// This is the short description.</span><span style="color:#0">
</span><span style="color:#6A9955">// > This is a block quote on multiple</span><span style="color:#0">
</span><span style="color:#6A9955">// > lines.</span><span style="color:#0">
</span><span style="color:#6A9955">// ></span><span style="color:#0">
</span><span style="color:#6A9955">// > End of the quote.</span><span style="color:#0">
</span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
{
    r, g, b: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>
You can create a table with <code>|</code>. </p>
<pre><code><span style="color:#6A9955">// A table with 4 lines of 2 columns:</span><span style="color:#0">
</span><span style="color:#6A9955">// | 'boundcheck'   | Check out of bound access</span><span style="color:#0">
</span><span style="color:#6A9955">// | 'overflow'     | Check type conversion lost of bits or precision</span><span style="color:#0">
</span><span style="color:#6A9955">// | 'math'         | Various math checks (like a negative '@sqrt')</span><span style="color:#0">
</span><span style="color:#6A9955">// | 'switch'       | Check an invalid case in a '#[Swag.Complete]' switch</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">myFunc</span><span style="color:#0">()
{
}</span></code></pre><p>
You can create a code paragraph with three backticks. </p>
<pre><code><span style="color:#6A9955">// For example:</span><span style="color:#0">
</span><span style="color:#6A9955">// ```</span><span style="color:#0">
</span><span style="color:#6A9955">// if a == true</span><span style="color:#0">
</span><span style="color:#6A9955">//   @print("true")</span><span style="color:#0">
</span><span style="color:#6A9955">// ```</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">test</span><span style="color:#0">()
{
}</span></code></pre><p>
For constants or enum values, the document comment is the one declared at the end of the line. </p>
<pre><code><span style="color:#3186CD">const</span><span style="color:#0"> </span><span style="color:#3BC3A7">A</span><span style="color:#0"> = </span><span style="color:#74A35B">0</span><span style="color:#0">     </span><span style="color:#6A9955">// This is a documentation comment</span><span style="color:#0">
</span><span style="color:#3186CD">enum</span><span style="color:#0"> </span><span style="color:#3BC3A7">Color</span><span style="color:#0">
{
    </span><span style="color:#3BC3A7">Red</span><span style="color:#0">         </span><span style="color:#6A9955">// This is a documentation comment</span><span style="color:#0">
}</span></code></pre><p>
Some other markdown markers are also supported inside paragraphs. </p>
<pre><code><span style="color:#6A9955">// `this is code` (backtick) for 'inline' code.</span><span style="color:#0">
</span><span style="color:#6A9955">// 'single_word'  (tick) for 'inline' code.</span><span style="color:#0">
</span><span style="color:#6A9955">// **bold**</span><span style="color:#0">
</span><span style="color:#6A9955">// # Title</span><span style="color:#0">
</span><span style="color:#6A9955">// ## Title</span><span style="color:#0">
</span><span style="color:#3186CD">struct</span><span style="color:#0"> </span><span style="color:#3BC3A7">RGB</span><span style="color:#0">
{
    r, g, b: </span><span style="color:#ED9A11">s32</span><span style="color:#0">
}</span></code></pre><p>
You can create a reference to something in the current module with <code>[name]</code> or <code>[name1.name2 etc.]</code> </p>
<pre><code><span style="color:#6A9955">// This is a function with a 'value' parameter.</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">one</span><span style="color:#0">(value: </span><span style="color:#ED9A11">s32</span><span style="color:#0">)
{
}

</span><span style="color:#6A9955">// This is a reference to [one]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">two</span><span style="color:#0">()
{
}</span></code></pre><p>
The attribute <code>#[Swag.NoDoc]</code> can be used to avoid a given element to appear in the documentation. </p>
<pre><code><span style="color:#6A9955">// This function will be ignored when generating documentation.</span><span style="color:#0">
</span><span style="color:#7F7F7F">#[Swag.NoDoc]</span><span style="color:#0">
</span><span style="color:#3186CD">func</span><span style="color:#0"> </span><span style="color:#FF6A00">one</span><span style="color:#0">()
{
}</span></code></pre></div>
</div>
</div>
<?php include('html_end.php'); ?>
