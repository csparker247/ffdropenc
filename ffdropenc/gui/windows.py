from PySide6.QtWidgets import (QMainWindow, QLabel)
from PySide6.QtCore import Qt


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setCentralWidget(QLabel('ffdropenc'))
