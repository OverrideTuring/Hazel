# Hazel

Hazel 是一个从零开始编写的 C++ 游戏引擎项目，开发过程参考 The Cherno 的 Hazel Engine 系列教程。

项目目前仍处于早期阶段，现有代码主要搭建了引擎动态库、客户端程序以及由引擎统一管理的应用启动流程。

## 构建环境

- Windows 10 或更高版本
- Visual Studio（需安装“使用 C++ 的桌面开发”工作负载）
- Windows 10 SDK
- 支持项目当前所用 `v145` 平台工具集的 MSVC
- C++17
- x64 平台

解决方案当前提供以下构建配置：

- `Debug | x64`
- `Release | x64`

## 构建与运行

1. 使用 Visual Studio 打开根目录下的 `Hazel.sln`。
2. 选择 `Debug | x64` 或 `Release | x64` 配置。
3. 将 `Sandbox` 设置为启动项目。
4. 构建并运行解决方案。

构建中间文件会输出到：

```text
bin-int/<配置>-<平台>/<项目名>/
```

最终产物会输出到：

```text
bin/<配置>-<平台>/<项目名>/
```

其中，`Hazel` 会生成动态库，`Sandbox` 会生成可执行程序。`Sandbox` 通过项目引用依赖 `Hazel`，构建时会先构建引擎项目，并将运行所需的动态库复制到 Sandbox 的输出目录。

## 项目结构

```text
Hazel/
├── Hazel.sln                  # Visual Studio 解决方案
├── Hazel/                     # 引擎项目，构建为动态库
│   ├── Hazel.vcxproj
│   └── src/
│       ├── Hazel.h            # 提供给客户端的统一包含文件
│       └── Hazel/
│           ├── Application.h  # 应用基类与应用工厂声明
│           ├── Application.cpp
│           ├── Core.h         # 平台检查和 DLL 导入、导出宏
│           └── EntryPoint.h   # Windows 程序入口与启动流程
├── Sandbox/                   # 引擎客户端与功能验证项目
│   ├── Sandbox.vcxproj
│   └── src/
│       └── SandboxApp.cpp     # Sandbox 应用及工厂函数实现
├── bin/                       # 最终构建产物
└── bin-int/                   # 构建中间文件
```

### Hazel

`Hazel` 是引擎核心项目，以动态库形式构建。`Core.h` 根据 `HZ_BUILD_DLL` 宏控制引擎 API 的导出与导入，目前仅支持 Windows 平台。

`Hazel.h` 是客户端使用引擎时的统一入口头文件，当前公开 `Application`，并包含引擎提供的程序入口。

### Sandbox

`Sandbox` 是使用 Hazel 的客户端程序，也是当前验证引擎功能的测试场所。它继承 `Hazel::Application` 并实现 `Hazel::CreateApplication()`，但不直接定义 `main` 函数。

## 启动流程

当前程序入口由 Hazel 统一管理，调用关系如下：

```text
EntryPoint.h 中的 main()
    └── Hazel::CreateApplication()
            └── 创建 Sandbox 实例
                    └── Application::Run()
                            └── 销毁应用实例并退出
```

具体流程为：

1. `SandboxApp.cpp` 包含 `Hazel.h`。
2. `Hazel.h` 间接包含 `EntryPoint.h`，由引擎侧提供 `main` 函数。
3. `main` 调用客户端实现的 `Hazel::CreateApplication()`。
4. 工厂函数创建并返回 `Sandbox` 实例。
5. 引擎调用应用的 `Run()`，并在运行结束后释放应用对象。

这种结构让客户端只需定义自己的应用类型和创建方式，程序入口与生命周期则由引擎统一控制。

## 当前进度

目前已完成：

- Hazel 动态库与 Sandbox 客户端的工程划分
- Debug、Release 两套 x64 构建配置
- Windows DLL API 导入与导出
- 基础 `Application` 类型
- 引擎侧统一入口和客户端应用工厂

窗口、事件、输入、渲染等引擎模块尚未加入。当前 `Application::Run()` 仍是一个空的无限循环，运行 Sandbox 后需要手动终止程序。

## 后续开发

随着功能增加，README 将继续补充各模块的职责、依赖关系、构建方式与使用方法；内容较多时再按模块分层管理。

（未完待续......）
