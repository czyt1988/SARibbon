# SARibbon开发要求

## Qt 集成方案

### 1. 信号槽设计

充分发挥Qt的信号槽机制，工具类使用信号和槽进行事件通讯。

### 2. 属性暴露方式

为了贴合Qt框架，类的属性使用`Q_PROPERTY`暴露

如：

```cpp
// 数据属性
Q_PROPERTY(QImAbstractDataSeries* data READ data WRITE setData NOTIFY dataChanged)

// 样式属性
Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
Q_PROPERTY(float size READ size WRITE setSize NOTIFY sizeChanged)
Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

// 可见性属性
Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
```

## 注释与文档规范

### 1️. 代码风格

- 严格保持与现有代码一致（命名规范、缩进、头文件组织等）
- 遵循 Qt 开发最佳实践（使用 `Q_PROPERTY`、`Q_SIGNALS`、`Q_SLOT` 等宏，禁止使用`slot`、`signal`等小写命名的宏）
- 行内注释请不要修改

### 2️. 注释规范（强制）

#### 2.1 源文件（.cpp）注释规范

所有新增代码必须使用 **Doxygen 格式**，并区分中英文：

```cpp
/**
 * \if ENGLISH
 * @brief [English brief description]
 * @param[in] param_name [English parameter description]
 * @return [English return value description]
 * @details [English detailed explanation]
 * \endif
 * 
 * \if CHINESE
 * @brief [中文简要说明]
 * @param[in] param_name [中文参数描述]
 * @return [中文返回值描述]
 * @details [中文详细说明]
 * \endif
 */
```

#### 2.2 头文件（.h）注释规范

- 头文件中的 `public` 函数声明旁，仅添加**单行英文简要注释**（使用 `//`  或简洁的 `/** */`）。
- **不要**在头文件中写入详细的双语 Doxygen 块（类的doxygen注释除外、信号的doxygen注释除外），详细内容应保留在对应的 `.cpp` 文件中，以保持头文件整洁。
- 示例：

```cpp
// 类的注释规范建见下一节
class MyClass {
public:
    // Constructor for MyClass (English only)
    MyClass(); 
};
```

- 但有些特例，例如qt的信号（头文件中Q_SIGNALS关键字下面的函数），它没有在cpp中的定义，这些函数的doxygen注释需要在头文件中按上面中英文要求添加，你需要把信号的doxygen注释转换为中英双语。
- 另外类的doxygen注释也需要在头文件中按上面中英文要求添加。

#### 2.3 类的doxygen注释规范

- 类的doxygen注释需要在头文件中按上面中英文要求添加。
- 示例：

```cpp
/**
 * \if ENGLISH
 * @brief [English description]
 * \endif
 *
 * \if CHINESE
 * @brief [中文说明]
 * \endif
 */
class MyClass {
public:
    // Constructor for MyClass (English only)
    MyClass(); 
};
```

- 对于功能性较强的类，类的注释中应该加入使用示例，以便使用者了解如何使用

## Git 提交

在完成当前任务后，需提交所有更改到 Git 仓库。
创建有意义的提交信息保证下次任务能清楚了解这次任务的实现情况
提交信息最好包含以下信息：

- 任务类型（例如：实现、修复、文档更新）
- 实现内容的简要描述
- 相关文件列表
- 关联到计划书（如果适用）
