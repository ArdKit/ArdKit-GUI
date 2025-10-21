# ArdKit-GUI 版本管理

## 当前版本

**版本号：2.0.3**

## 版本格式

版本号采用 `x.y.z` 格式：

- **x** - 主架构版本（Major）：重大架构变更时递增，y和z归零
- **y** - 特性版本（Minor）：新增功能特性时递增，z归零
- **z** - 构建次数（Build）：每次构建时自动递增

## 起始版本

项目从 **2.0.1** 开始计数。

## 版本管理脚本

### 位置
```
scripts/version.sh
scripts/version.txt
```

### 使用方法

#### 查看当前版本
```bash
./scripts/version.sh get
```

#### 递增构建号（自动）
```bash
./scripts/version.sh build
```
> 注意：每次执行 `cmake` 配置时会自动递增构建号

#### 递增特性版本
```bash
./scripts/version.sh minor
```

#### 递增主版本
```bash
./scripts/version.sh major
```

#### 手动设置版本
```bash
./scripts/version.sh set 2.1.0
```

## 构建流程中的版本管理

### CMake 配置阶段

1. 读取当前版本：`scripts/version.txt`
2. 自动递增构建号
3. 生成版本头文件：`build/version.h`
4. 显示配置信息：
   ```
   ========================================
   ArdKit-GUI Configuration
   ========================================
   Version: 2.0.3
   Build date: 2025-10-21 10:55:50
   Qt version: 6.x.x
   ========================================
   ```

### 编译阶段

版本信息被编译到应用程序中：

```cpp
#include "version.h"

// 版本宏定义
ARDKIT_VERSION          // "2.0.3"
ARDKIT_VERSION_MAJOR    // 2
ARDKIT_VERSION_MINOR    // 0
ARDKIT_VERSION_BUILD    // 3
ARDKIT_BUILD_DATE       // "2025-10-21 10:55:50"
```

### 运行时显示

应用程序启动时会显示版本信息：

```
========================================
ArdKit-GUI v 2.0.3
Build date: 2025-10-21 10:55:50
========================================
Starting ArdKit-GUI...
```

## 版本历史

- **2.0.1** - 初始版本，添加版本管理系统
- **2.0.2** - CMake配置测试
- **2.0.3** - 首次成功编译带版本号的应用

## 发布流程建议

### 发布新特性版本

```bash
# 1. 开发完成后，递增特性版本
./scripts/version.sh minor

# 2. 提交代码
git add scripts/version.txt
git commit -m "Bump version to $(./scripts/version.sh get)"

# 3. 创建标签
git tag -a v$(./scripts/version.sh get) -m "Release version $(./scripts/version.sh get)"

# 4. 推送
git push origin main --tags
```

### 发布主版本

```bash
# 主版本变更（重大架构改变）
./scripts/version.sh major
git add scripts/version.txt
git commit -m "Bump major version to $(./scripts/version.sh get)"
git tag -a v$(./scripts/version.sh get) -m "Major release $(./scripts/version.sh get)"
git push origin main --tags
```

## 注意事项

1. ⚠️ **不要手动编辑 `scripts/version.txt`**，使用脚本管理版本
2. ✅ 每次 `cmake` 配置都会自动递增构建号
3. ✅ 版本号会自动嵌入到应用程序中
4. ✅ Qt 应用程序的版本通过 `app.setApplicationVersion()` 设置
5. ✅ 版本信息在启动时显示在调试日志中

## 与子项目版本同步

本项目包含子项目 MediaKit，它有独立的版本管理：

- **ArdKit-GUI**: `scripts/version.sh`（主项目）
- **MediaKit**: `tools/mediakit/scripts/version.sh`（子项目）

两者版本可以独立管理，互不影响。
