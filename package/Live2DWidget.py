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
