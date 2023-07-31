# amalgamate（把工程整合为一个h+一个cpp）

amalgamate工具可以把一个工程所有的cpp文件合并为一个超大的cpp文件，把所有h文件合并为一个头文件，这样整个工程只有1个h文件和1个cpp文件，直接引入工程即可使用，对于那种无需编译为库的情况非常方便

SARibbon借助[https://github.com/vinniefalco/Amalgamate.git](https://github.com/vinniefalco/Amalgamate.git) 这个工具，实现工程文件的合并

使用说明：

```
NAME

   amalgamate - produce an amalgamation of C/C++ source files.

  SYNOPSIS

   amalgamate [-s]
     [-w {wildcards}]
     [-f {file|macro}]...
     [-p {file|macro}]...
     [-d {name}={file}]...
     [-i {dir}]...
     {inputFile} {outputFile}

  DESCRIPTION

   Produces an amalgamation of {inputFile} by replacing #include statements with
   the contents of the file they refer to. This replacement will only occur if
   the file was located in the same directory, or one of the additional include
   paths added with the -i option.

   Files included in angle brackets (system includes) are only inlined if the
   -s option is specified.

   If an #include line contains a macro instead of a string literal, the list
   of definitions provided through the -d option is consulted to convert the
   macro into a string.

   A file will only be inlined once, with subsequent #include lines for the same
   file silently ignored, unless the -f option is specified for the file.

  OPTIONS

    -s                Process #include lines containing angle brackets (i.e.
                      system includes). Normally these are not inlined.

    -w {wildcards}    Specify a comma separated list of file name patterns to
                      match when deciding to inline (assuming the file can be
                      located). The default setting is "*.cpp;*.c;*.h;*.mm;*.m".

    -f {file|macro}   Force reinclusion of the specified file or macro on
                      all appearances in #include lines.

    -p {file|macro}   Prevent reinclusion of the specified file or macro on
                      subsequent appearances in #include lines.

    -d {name}={file}  Use {file} for macro {name} if it appears in an #include
                      line.

    -i {dir}          Additionally look in the specified directory for files when
                      processing #include lines.

    -v                Verbose output mode
```

# 编译amalgamate

从[https://github.com/vinniefalco/Amalgamate.git](https://github.com/vinniefalco/Amalgamate.git)把工程克隆下来，直接用vs进行编译即可，编译完会生成`Amalgamate.exe`

# 定义合并模板

合并需要定义模板，模板有h和cpp两个，合并模板的定义需要简单说一下，理论上只要h和cpp两个模板，但实际定义了4个文件

SARibbon项目定义的头文件合并模板位于：[SARibbonAmalgamTemplate.h](./amalgamate/SARibbonAmalgamTemplate.h)

模板里面要把所有的头文件都包含，合并的过程中会逐个查找头文件并把它合并到一个头文件中

头文件相对简单，cpp文件的合并比较复杂，cpp文件也包含头文件，如果不做特殊处理，会把cpp文件的头文件也合并进来，这样得到的cpp文件并不是我们想要的，因此amalgamate提供了`@remap`标记，被这个标记过的头文件，相当于加入了一个忽略库中，不会在合并cpp文件过程中重复查找

因此SARibbon项目的合并cpp模板[SARibbonAmalgamTemplate.cpp](./amalgamate/SARibbonAmalgamTemplate.cpp)有如下一句话

```
/*@remap "SARibbonAmalgamTemplatePublicHeaders.h" "SARibbon.h" */
#include "SARibbonAmalgamTemplateHeaderGlue.h"
```

这句话代表`SARibbonAmalgamTemplatePublicHeaders.h`里面的内容已经加入了`SARibbon.h`里面了，在合并cpp文件遇到include这里面的内容不用理会

为此，头文件模板并不是把所有相关的头文件列举，而是

```cpp
#include "SARibbonAmalgamTemplatePublicHeaders.h"
```

`SARibbonAmalgamTemplatePublicHeaders.h`这里定义了所有的SARibbon的头文件

另外还有一个关键的头文件是`SARibbonAmalgamTemplateHeaderGlue.h`,这个头文件只有如下两行：

```cpp
// This file provides an extra level of indirection for the @remap in the template
#include "SARibbonAmalgamTemplatePublicHeaders.h"
```

目的就是为了给cpp文件的`@remap`标记用

# 把SARibbon项目合并为一个h文件和一个cpp文件

windows下直接运行`Amalgamate.sh`即可