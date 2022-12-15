import sys

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QCoreApplication

from ffdropenc.gui.windows import MainWindow


def main():
    app = QApplication(sys.argv)
    QCoreApplication.setOrganizationName('plenopt')
    QCoreApplication.setApplicationName('ffdropenc')
    QCoreApplication.setApplicationVersion('2.0.0')

    main_window = MainWindow()
    main_window.setWindowTitle('ffdropenc')
    main_window.show()
    sys.exit(app.exec())


if __name__ == '__main__':
    main()
