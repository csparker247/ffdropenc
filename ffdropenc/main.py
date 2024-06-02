import sys

from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QIcon

# noinspection PyUnresolvedReferences
import ffdropenc.resources
from ffdropenc.gui.windows import MainWindow


def main():
    app = QApplication(sys.argv)
    app.setOrganizationName('plenoptic')
    app.setApplicationName('ffdropenc')
    app.setApplicationVersion('2.0.0')
    app.setWindowIcon(QIcon(':img/icon.icns'))

    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec())


if __name__ == '__main__':
    main()
