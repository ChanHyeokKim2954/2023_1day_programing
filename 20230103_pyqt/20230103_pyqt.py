import sys
from PyQt5.QtWidgets import QApplication, QWidget, QCheckBox, QRadioButton
from PyQt5.QtCore import Qt


class MyApp(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        cb = QCheckBox('Show title', self)
        cb.move(20, 20)
        cb.toggle()
        cb.stateChanged.connect(self.changeTitle)

        rbtn1 = QRadioButton('Position Controller', self)
        rbtn1.move(20, 50)
        rbtn1.setChecked(True)

        rbtn2 = QRadioButton(self)
        rbtn2.move(20, 80)
        rbtn2.setText('Angular Speed Controller')

        self.setWindowTitle('20230103_pyqt')
        self.setGeometry(300, 300, 280, 120)
        self.show()

    def changeTitle(self, state):
        if state == Qt.Checked:
            self.setWindowTitle('20230103_pyqt')
        else:
            self.setWindowTitle(' ')


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MyApp()
    sys.exit(app.exec_())