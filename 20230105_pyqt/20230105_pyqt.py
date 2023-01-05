import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QLineEdit, QCheckBox, QRadioButton, QSpinBox,  QTabWidget
from PyQt5.QtCore import Qt

class MyApp(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        #button
        btn1 = QPushButton(self)
        btn1.setText('INPUT')
        btn1.move(280, 35)

        btn2 = QPushButton(self)
        btn2.setText('INPUT')
        btn2.move(280, 75)

        btn3 = QPushButton(self)
        btn3.setText('INPUT')
        btn3.move(280, 115)

        #label
        label1 = QLabel('P gain :', self)
        font1 = label1.font()
        font1.setPointSize(10)
        label1.setFont(font1)
        label1.move(20, 40)

        label2 = QLabel('PD gain :', self)
        font2 = label2.font()
        font2.setPointSize(10)
        label2.setFont(font2)
        label2.move(20, 80)

        label3 = QLabel('PI gain :', self)
        font3 = label2.font()
        font3.setPointSize(10)
        label3.setFont(font2)
        label3.move(20, 120)

        #LineEdit
        self.le1 = QLabel(self)
        self.le1.move(80, 40)
        self.le2 = QLabel(self)
        self.le2.move(80, 80)
        self.le3 = QLabel(self)
        self.le3.move(80, 120)

        qle1 = QLineEdit(self)
        qle1.move(120, 36)
        qle1.textChanged[str].connect(self.onChanged1)

        qle2 = QLineEdit(self)
        qle2.move(120, 76)
        qle2.textChanged[str].connect(self.onChanged2)

        qle3 = QLineEdit(self)
        qle3.move(120, 116)
        qle3.textChanged[str].connect(self.onChanged3)

        #CheckBox
        cb = QCheckBox('Show title', self)
        cb.move(20, 150)
        cb.toggle()
        cb.stateChanged.connect(self.changeTitle)

        #RadioButton
        rbtn1 = QRadioButton('Position Controller', self)
        rbtn1.move(120, 150)
        rbtn1.setChecked(True)

        rbtn2 = QRadioButton(self)
        rbtn2.move(120, 180)
        rbtn2.setText('Angular Speed Controller')

        #SpinBox
        spinbox = QSpinBox(self)
        spinbox.setRange(-50, 50)
        spinbox.setSingleStep(2)
        spinbox.move(20, 180)

        #TableWidget
        tab1 = QWidget(self)
        tab2 = QWidget(self)

        tabs = QTabWidget(self)
        tabs.addTab(tab1, 'Tab1')
        tabs.addTab(tab2, 'Tab2')
        tabs.move(0, 0)
        self.setWindowTitle('MotorControl')
        self.setGeometry(300, 300, 370, 230)
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

    def changeTitle(self, state):
        if state == Qt.Checked:
            self.setWindowTitle('MotorControl')
        else:
            self.setWindowTitle(' ')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MyApp()
    sys.exit(app.exec_())