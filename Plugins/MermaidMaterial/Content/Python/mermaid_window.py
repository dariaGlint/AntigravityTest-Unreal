import sys
import unreal
import material_generator
try:
    from PySide2 import QtWidgets, QtCore, QtGui
except ImportError:
    # Fallback or try PySide6 if UE version changes, but UE5 uses PySide2 usually
    try:
        from PySide6 import QtWidgets, QtCore, QtGui
    except ImportError:
        unreal.log_error("PySide2/6 not available")

class MermaidMaterialWindow(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super(MermaidMaterialWindow, self).__init__(parent)
        self.setWindowTitle("Mermaid to Material")
        self.resize(600, 400)
        
        # Layout
        self.layout = QtWidgets.QVBoxLayout(self)
        
        # Instructions
        self.label = QtWidgets.QLabel("Paste Mermaid Graph Code Here:")
        self.layout.addWidget(self.label)
        
        # Text Edit
        self.text_edit = QtWidgets.QTextEdit()
        self.text_edit.setPlaceholderText("graph LR\n  A[TextureSample] --> B[BaseColor]")
        self.layout.addWidget(self.text_edit)
        
        # Button
        self.generate_btn = QtWidgets.QPushButton("Generate Material")
        self.generate_btn.clicked.connect(self.on_generate)
        self.layout.addWidget(self.generate_btn)
        
        # Status
        self.status_label = QtWidgets.QLabel("")
        self.layout.addWidget(self.status_label)

    def on_generate(self):
        text = self.text_edit.toPlainText()
        if not text.strip():
            self.status_label.setText("Error: Empty input")
            return
            
        try:
            asset_name = material_generator.create_material_from_mermaid(text)
            if asset_name:
                self.status_label.setText(f"Success! Created: {asset_name}")
                self.status_label.setStyleSheet("color: green")
            else:
                self.status_label.setText("Error: Failed to create material")
                self.status_label.setStyleSheet("color: red")
        except Exception as e:
            self.status_label.setText(f"Exception: {str(e)}")
            self.status_label.setStyleSheet("color: red")
            unreal.log_error(f"Mermaid Error: {e}")

# Keep reference to window to prevent GC
_window = None

def show_window():
    global _window
    if _window:
        _window.close()
        _window = None
        
    app = QtWidgets.QApplication.instance()
    if not app:
        app = QtWidgets.QApplication(sys.argv)
    
    # Parent to Unreal Main Window
    # unreal.parent_external_window_to_slate_handle is useful but 
    # for simplicity we just show it. 
    # To parent correctly:
    parent_hwnd = unreal.get_main_window_handle()
    
    _window = MermaidMaterialWindow()
    # _window.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint) # Optional
    _window.show()

if __name__ == "__main__":
    show_window()
