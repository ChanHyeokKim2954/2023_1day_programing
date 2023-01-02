import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QLineEdit

class MyApp(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        #button
        btn1 = QPushButton(self)
        btn1.setText('INPUT')
        btn1.move(280, 15)

        btn2 = QPushButton(self)
        btn2.setText('INPUT')
        btn2.move(280, 55)

        btn3 = QPushButton(self)
        btn3.setText('INPUT')
        btn3.move(280, 95)

        #label
        label1 = QLabel('P gain :', self)
        font1 = label1.font()
        font1.setPointSize(10)
        label1.setFont(font1)
        label1.move(20, 20)

        label2 = QLabel('PD gain :', self)
        font2 = label2.font()
        font2.setPointSize(10)
        label2.setFont(font2)
        label2.move(20, 60)

        label3 = QLabel('PI gain :', self)
        font3 = label2.font()
        font3.setPointSize(10)
        label3.setFont(font2)
        label3.move(20, 100)

        #LineEdit
        self.le1 = QLabel(self)
        self.le1.move(80, 20)
        self.le2 = QLabel(self)
        self.le2.move(80, 60)
        self.le3 = QLabel(self)
        self.le3.move(80, 100)

        qle1 = QLineEdit(self)
        qle1.move(120, 16)
        qle1.textChanged[str].connect(self.onChanged1)

        qle2 = QLineEdit(self)
        qle2.move(120, 56)
        qle2.textChanged[str].connect(self.onChanged2)

        qle3 = QLineEdit(self)
        qle3.move(120, 96)
        qle3.textChanged[str].connect(self.onChanged3)
        self.setWindowTitle('MotorControl')
        self.setGeometry(300, 300, 370, 130)
        self.show()

    def onChanged1(self, text):
        self.le1.setText(text)
        self.le1.adjustSize()

    def onChanged2(self, text):
        self.le2.setText(text)
        self.le2.adjustSize()

    def onChanged3(self, text):
        self.le3.setText(text)
        self.le3.adjustSize()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MyApp()
    sys.exit(app.exec_())