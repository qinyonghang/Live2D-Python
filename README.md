# PyLive2D

## 简介

PyLive2D 是对 Live2D Cubism SDK 的 Python 封装，旨在为 Python 开发者提供简单易用的 Live2D 模型渲染和交互功能。

## 安装

### 前提条件

- Python 3.12
- CMake 3.22 或更高版本
- C++ 编译器（如 Visual Studio 或 GCC）

### 安装步骤

1. **克隆仓库**
   ```bash
   git clone https://github.com/qinyonghang/Live2D-Python.git

2. **安装依赖**

    本项目没有额外的 Python 依赖项，但需要确保系统中已安装 CMake 和 C++ 编译器。

3. **构建和安装**

使用 pip 安装：

    ```bash
    pip install whl/PyLive2D-1.0-cp312-cp312-win_amd64.whl
    ```

如果你希望从源代码构建，请确保已经安装了必要的编译工具链，并运行以下命令：

    ```bash
    python setup.py build_ext --inplace
    ```

需要注意的是，由于boost和VS 2022的兼容性问题，boost的构建需要单独进行。

    ```bash
    python scripts\\replace.py ${boost_root}\\tools\\build\\src\\tools\\msvc.jam
    bootstrap.bat
    .\b2.exe --with-python link=shared address-model=64 variant=release
    ```

## 使用说明

### 导入模块

    ```python
    import PyLive2D
    model = PyLive2D.Model("path_to_model")
    ```

### 示例代码

    ```python
    import os
    from PyQt5.QtCore import QTimerEvent, Qt
    from PyQt5.QtGui import QMouseEvent, QCursor
    from PyQt5.QtWidgets import QOpenGLWidget
    import random
    import PyLive2D


    class Live2DWidget(QOpenGLWidget):
        def __init__(self, *args, path: str, parent=None, **kwargs) -> None:
            self.path = path
            if not os.path.exists(path):
                raise FileNotFoundError(f"path({path}) not found...")

            super().__init__(parent)
            self.setMouseTracking(True)

        def initializeGL(self) -> None:
            self.makeCurrent()
            self.model = PyLive2D.Model(self.path, self.width(), self.height())
            self.expression_ids = self.model.expression_ids()
            self.motion_ids = self.model.motion_ids()
            self.refresh = self.startTimer(33)

        def paintGL(self) -> None:
            self.model.draw(self.width(), self.height())

        def timerEvent(self, event: QTimerEvent | None):
            if event.timerId() == self.refresh:
                self.update()

        def mouseReleaseEvent(self, a0: QMouseEvent | None):
            if a0 and a0.button() == Qt.LeftButton:
                pos = a0.pos()
                area = self.model.hit_area(pos.x(), pos.y())
                if area == "Head":
                    idx = random.randrange(0, len(self.expression_ids))
                    self.model.set_expression(self.expression_ids[idx])
                elif area == "Body":
                    idx = random.randrange(0, len(self.motion_ids))
                    self.model.set_motion(self.motion_ids[idx])
            super().mouseReleaseEvent(a0)

        def mouseMoveEvent(self, event: QMouseEvent | None):
            self.model.set_dragging(event.x(), event.y())
            super().mouseMoveEvent(event)


    if __name__ == "__main__":
        from PyQt5.QtWidgets import QApplication
        import sys

        app = QApplication(sys.argv)
        w = Live2DWidget(path="resources/Haru/Haru.model3.json")
        w.resize(550, 500)
        w.show()
        sys.exit(app.exec())
    ```

## 三方库引用说明

本项目引用了多个第三方库。使用这些库时，需遵守其各自的许可证条款，并向相关作者获取授权。以下是本项目所使用的第三方库列表及联系方式：

- **common**：由 qinyonghang 维护，[GitHub](https://github.com/qinyonghang/common)，联系邮箱：yonghang.qin@gmail.com
- **spdlog**：轻量级 C++ 日志库，[GitHub](https://github.com/gabime/spdlog)，遵循 MIT 许可证
- **glfw**：跨平台窗口和 OpenGL 上下文创建库，[GitHub](https://github.com/glfw/glfw)，遵循 zlib/libpng 许可证
- **glew**：OpenGL 扩展加载库，[GLEW 官网](http://glew.sourceforge.net/)，遵循 BSD 许可证
- **boost**：C++ 标准库扩展，[Boost 官网](https://www.boost.org/)，遵循 Boost Software License
- **CubismSdkForNative**：Live2D 提供的原生 SDK，[Live2D 官网](https://www.live2d.com/en/)，遵循 Live2D 的许可协议

请确保在使用这些库时，遵守其各自的许可证要求，并在必要时与库的维护者取得联系以获取授权。

## 许可证

本项目采用 Apache License 2.0 许可证。

## 联系方式

如果有任何问题或建议，请联系作者：yonghang.qin@google.com

## 致谢
感谢您对 PyLive2D 的关注和支持！
