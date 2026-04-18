# 构建过程常见错误

- ✅ **VS2017路径问题**：中文路径和长路径限制的解决方案
- ✅ **MOC编译异常**：首次构建批量MOC操作失败的简单修复
- ✅ **QWindowKit依赖缺失**：找不到第三方库时的配置排查

| 错误现象 | 原因 | 解决方案 |
|----------|------|----------|
| `error D8050: 无法执行 c1xx.dll` | 构建路径含中文或超过260字符限制 | 设置英文构建路径，启用Win32长路径 |
| MOC相关编译错误 | 首次构建批量MOC操作异常 | 保留build目录，重新构建1-2次 |
| `Could not find QWindowKit` | 未编译或安装QWindowKit库 | 先编译QWindowKit，或指定 `-DQWindowKit_DIR` 路径 |

!!! bug "VS2017编译器构建出错"

> error D8050: 无法执行 xxx/c1xx.dll  未能将命令行放入调试记录中

如果你的vs正确安装，但出现这个错误，有两种情况：你的构建目录可能存在中文，例如你的用户名就是中文，早期版本的vs，例如vs2017，会把构建目录放到用户的临时文件夹下面，这时就会导致构建出错，解决方法是定义

```json
{
  "configurations": [
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "inheritEnvironments": [ "msvc_x64" ],
      "buildRoot": "${workspaceRoot}\\build\\x64-Debug",
      "cmakeCommandArgs": "",
      "ctestCommandArgs": ""
    },
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "inheritEnvironments": [ "msvc_x64" ],
      "buildRoot": "${workspaceRoot}\\build\\x64-Release"
    }
  ]
}
```

另外还有一种情况就是你的操作系统最大路径没有放开，只支持255长度，这样非常容易出现问题，你可以通过修改组策略编辑器，把路径的最大长度设置为8192

```txt
按 Win + R 输入 gpedit.msc。

导航到：
计算机配置 > 管理模板 > 系统 > 文件系统
启用 启用 Win32 长路径。

重启系统。
```

如果还是不行，把项目移动到其他目录下，比如D盘，或者C盘，这样路径长度就变短了，这样问题就解决啦

!!! bug "构建过程中出现MOC错误"

在编译输出中看到moc相关的错误时，只需要再多几次构建即可。这个问题尤其发生在第一次构建的时候，大批量的moc操作有时会出现异常，只要保留build目录，继续构建即可。

!!! bug "QWindowKit 找不到"

如果构建时启用了 `SARIBBON_USE_FRAMELESS_LIB=ON`，但出现类似 `Could not find a package configuration file provided by "QWindowKit"` 的错误，说明 QWindowKit 库未正确编译或安装。

解决方法：

1. 先按照 [第三方库编译](./build-3rdparty.md) 文档编译安装 QWindowKit
2. 或者在 CMake 配置时手动指定路径：`-DQWindowKit_DIR=<安装路径>/lib/cmake/QWindowKit`
3. 如果不需要 QWindowKit，将 `SARIBBON_USE_FRAMELESS_LIB` 设为 `OFF` 即可
