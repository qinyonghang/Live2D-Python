import os
from PyQt5.QtCore import QTimerEvent, Qt
from PyQt5.QtGui import QMouseEvent, QCursor
from PyQt5.QtWidgets import QOpenGLWidget
import random
import pylive2d


class Live2DWidget(QOpenGLWidget):
    def __init__(self, *args, path: str, parent=None, **kwargs) -> None:
        self.path = path
        if not os.path.exists(path):
            raise FileNotFoundError(f"path({path}) not found...")

        super().__init__(parent)
        self.setWindowFlags(Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint)
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground)
        self.setAttribute(Qt.WidgetAttribute.WA_NoSystemBackground, True)
        self.setMouseTracking(True)

    def initializeGL(self) -> None:
        print("initializeGL: Start:")
        self.makeCurrent()
        self.model = pylive2d.Model(self.path, self.width(), self.height())
        self.expression_ids = self.model.expression_ids()
        self.motion_ids = self.model.motion_ids()
        self.refresh = self.startTimer(33)
        print("initializeGL: End!")

    def paintGL(self) -> None:
        if hasattr(self, "model"):
            self.model.draw(self.width(), self.height())

    def timerEvent(self, event: QTimerEvent | None):
        if event.timerId() == self.refresh:
            self.update()

    def mousePressEvent(self, a0: QMouseEvent | None):
        if a0 and a0.button() == Qt.LeftButton:
            self.drag_position = a0.globalPos() - self.frameGeometry().topLeft()
        a0.accept()

    def mouseReleaseEvent(self, a0: QMouseEvent | None):
        if a0 and a0.button() == Qt.LeftButton:
            del self.drag_position

            pos = a0.pos()
            area = self.model.hit_area(pos.x(), pos.y())
            if area == "Head":
                idx = random.randrange(0, len(self.expression_ids))
                self.model.set_expression(self.expression_ids[idx])
            elif area == "Body":
                idx = random.randrange(0, len(self.motion_ids))
                self.model.set_motion(self.motion_ids[idx])
        a0.accept()

    def mouseMoveEvent(self, a0: QMouseEvent | None):
        if a0 and hasattr(self, "drag_position"):
            self.move(a0.globalPos() - self.drag_position)
        self.model.set_dragging(a0.x(), a0.y())
        a0.accept()


if __name__ == "__main__":
    from PyQt5.QtWidgets import QApplication
    import sys

    app = QApplication(sys.argv)
    w = Live2DWidget(path="resources/Haru/Haru.model3.json")
    w.resize(550, 500)
    w.show()
    sys.exit(app.exec())
