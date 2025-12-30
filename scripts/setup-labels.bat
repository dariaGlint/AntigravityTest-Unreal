@echo off
REM GitHub labels creation script

echo Creating GitHub labels...

REM Technology Area
gh label create "UE5" --description "UE5 C++/Blueprint implementation" --color "0075ca" --force
gh label create "Plugin" --description "Plugin development" --color "1d76db" --force
gh label create "AI" --description "AI (Behavior Tree, EQS, State Tree)" --color "5319e7" --force
gh label create "UI" --description "UI/UX (UMG, Slate)" --color "0e8a16" --force
gh label create "Gameplay" --description "Gameplay logic" --color "006b75" --force
gh label create "Python" --description "Python scripts" --color "fbca04" --force
gh label create "CI/CD" --description "GitHub Actions, workflows" --color "d4c5f9" --force

REM Work Type
gh label create "documentation" --description "Documentation improvements" --color "0075ca" --force
gh label create "enhancement" --description "Enhancement to existing features" --color "a2eeef" --force
gh label create "bug" --description "Bug fix" --color "d73a4a" --force
gh label create "feature" --description "New feature" --color "0e8a16" --force
gh label create "refactor" --description "Refactoring" --color "fbca04" --force
gh label create "test" --description "Testing related" --color "d876e3" --force

REM Priority
gh label create "priority: high" --description "High priority" --color "d93f0b" --force
gh label create "priority: medium" --description "Medium priority" --color "fbca04" --force
gh label create "priority: low" --description "Low priority" --color "c2e0c6" --force

REM Status
gh label create "help wanted" --description "Help wanted" --color "008672" --force
gh label create "good first issue" --description "Good for newcomers" --color "7057ff" --force
gh label create "blocked" --description "Blocked" --color "e99695" --force
gh label create "in progress" --description "In progress" --color "d4c5f9" --force
gh label create "claude-auto" --description "Claude auto-implementation target" --color "bfdadc" --force

echo.
echo Labels created successfully!
pause
