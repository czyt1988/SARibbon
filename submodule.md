# submodule

本项目的第三方库使用submodule进行管理

## submdule的拉取

首次拉取项目之后，需要执行：

```shell
git submodule update --init --recursive
```

对所有的submodule进行拉取

## submodule的添加

目前添加的submodule有如下：

```shell
git submodule add https://github.com/stdware/qwindowkit ./src/SARibbonBar/3rdparty/qwindowkit
```


## submodule的更新

如果submodule更新了，使用`git submodule update --remote {submodule}`进行更新，如：

```shell
git submodule update --remote src/SARibbonBar/3rdparty/qwindowkit
```

关于如何编译，请看readme文档