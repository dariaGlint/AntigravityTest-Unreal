# Link Plugin to UE5 Project

Create a symbolic link from a plugin in this worktree to a UE5 project.

## Instructions

1. Ask the user for the plugin name (default: ScreenTransitionSystem)
2. Ask the user for the UE5 project path
3. Verify the plugin exists in the Plugins directory
4. Check if the target UE5 project path exists
5. Create the Plugins directory in the UE5 project if it doesn't exist
6. Check if a symlink or directory already exists at the target location
7. If it exists, ask the user if they want to overwrite it
8. Create the symbolic link using the appropriate command for the platform:
   - Windows: Use `cmd /c mklink /D "target" "source"` (note: requires administrator privileges)
   - Linux/Mac: Use `ln -s "source" "target"`
9. Verify the link was created successfully
10. Inform the user of the result and next steps

## Important Notes

- On Windows, symbolic links require administrator privileges
- The source path should be the absolute path to Plugins/{PluginName} in this worktree
- The target path should be {UE5ProjectPath}/Plugins/{PluginName}
- If the command fails due to permissions, suggest running the bat/sh script instead with admin privileges

## Example Interaction

```
Plugin name (default: ScreenTransitionSystem): [wait for input]
UE5 Project path: C:\MyUE5Project

Checking plugin exists...
✓ Found: C:\Users\daria\.claude-worktrees\CodingAgentSandbox\vigorous-tharp\Plugins\ScreenTransitionSystem

Creating symlink...
✓ Success!

The plugin is now linked to your UE5 project.
Any changes in this worktree will be immediately reflected in the UE5 project.
```
