@echo off
REM Issue #46-53 に適切なラベルを適用するスクリプト

echo Applying labels to issues #46-53...

REM Issue #46: CLAUDE.md作成
echo Applying labels to #46...
gh issue edit 46 --add-label "documentation,priority: high"

REM Issue #47: AGENTS.md作成
echo Applying labels to #47...
gh issue edit 47 --add-label "documentation,priority: high"

REM Issue #48: .agent/workflows/README作成
echo Applying labels to #48...
gh issue edit 48 --add-label "documentation,CI/CD,priority: medium"

REM Issue #49: SETUP.md作成
echo Applying labels to #49...
gh issue edit 49 --add-label "documentation,UE5,priority: high"

REM Issue #50: PLUGINS.md作成
echo Applying labels to #50...
gh issue edit 50 --add-label "documentation,Plugin,priority: medium"

REM Issue #51: ARCHITECTURE.md作成
echo Applying labels to #51...
gh issue edit 51 --add-label "documentation,UE5,priority: high"

REM Issue #52: TROUBLESHOOTING.md作成
echo Applying labels to #52...
gh issue edit 52 --add-label "documentation,UE5,priority: medium"

REM Issue #53: CI/CD文書化
echo Applying labels to #53...
gh issue edit 53 --add-label "documentation,CI/CD,priority: low"

echo.
echo ✅ Labels applied successfully to all issues!
echo.
echo Summary:
echo   #46: documentation, priority: high
echo   #47: documentation, priority: high
echo   #48: documentation, CI/CD, priority: medium
echo   #49: documentation, UE5, priority: high
echo   #50: documentation, Plugin, priority: medium
echo   #51: documentation, UE5, priority: high
echo   #52: documentation, UE5, priority: medium
echo   #53: documentation, CI/CD, priority: low
pause
