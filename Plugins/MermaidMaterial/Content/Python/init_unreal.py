import unreal
import mermaid_window

def menu_entry_callback():
    mermaid_window.show_window()

def init_menus():
    menus = unreal.ToolMenus.get()
    
    # Add to "Tools" menu
    main_menu = menus.find_menu("LevelEditor.MainMenu.Tools")
    if not main_menu:
        return
        
    entry = unreal.ToolMenuEntry(
        name="MermaidMaterial",
        type=unreal.MultiBlockType.MENU_ENTRY,
        insert_position=unreal.ToolMenuInsert("", unreal.ToolMenuInsertType.DEFAULT)
    )
    
    entry.set_label("Mermaid to Material")
    entry.set_tool_tip("Generate Material from Mermaid Graph")
    
    # Set command handler
    # Note: simple python callback binding for ToolMenus can be tricky in some UE versions.
    # The robust way is to register a Python command or use add_menu_entry_object if available.
    # But for a script execution, we can use a script string command.
    
    entry.set_string_command(
        type=unreal.ToolMenuStringCommandType.PYTHON,
        custom_type="Python",
        string="import mermaid_window; mermaid_window.show_window()"
    )
    
    main_menu.add_menu_entry("Tools", entry)
    menus.refresh_all_widgets()

# Run initialization
if __name__ == "__main__":
    init_menus()
