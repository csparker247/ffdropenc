import datetime as dt

from PySide6.QtWidgets import (QMainWindow, QMessageBox, QVBoxLayout, QWidget)
from PySide6.QtCore import (QCoreApplication, Qt)
from PySide6.QtGui import (QAction, QIcon)

# noinspection PyUnresolvedReferences
import ffdropenc.resources


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setAcceptDrops(True)
        self.setWindowTitle('ffdropenc')
        self.setMinimumWidth(480)
        self.setWindowIcon(QIcon(':img/icon.png'))

        central_widget = QWidget()
        layout = QVBoxLayout(central_widget)
        alignment = Qt.AlignmentFlag.AlignTop | Qt.AlignmentFlag.AlignHCenter
        layout.setAlignment(alignment)
        self.setCentralWidget(central_widget)

        file_menu = self.menuBar().addMenu(' &File')
        about_msg = f'ffdropenc v{QCoreApplication.applicationVersion()}\n\n' \
                    f'\u00a9 {dt.datetime.now().year} C. Seth Parker'
        about_action = QAction('&About', self)
        about_action.triggered.connect(lambda: QMessageBox.about(self, 'ffdropenc', about_msg))
        file_menu.addAction(about_action)

