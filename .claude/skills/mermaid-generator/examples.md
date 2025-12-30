# Mermaid Diagram Generator - Examples

This file contains example Mermaid diagrams that can be generated from the AntigravityTest project.

## Example 1: Combat Variant Class Hierarchy

```mermaid
classDiagram
    class ACharacter {
        <<Unreal Base>>
    }

    class ACombatCharacter {
        <<abstract>>
        +DoMove(Right, Forward)
        +DoLook(Right, Up)
        +DoAttack()
        +DoJump()
        -float MaxHP
        -float CurrentHP
    }

    class ACombatEnemy {
        +OnDeath()
        +SetPatrolRoute()
        -float AggroRange
    }

    class ACombatDummy {
        +OnHit()
        -int HitCount
    }

    class AGameMode {
        <<Unreal Base>>
    }

    class ACombatGameMode {
        +OnEnemyDefeated()
        +OnPlayerDeath()
        +CheckWinCondition()
    }

    class ICombatAttacker {
        <<interface>>
        +PerformAttack()
        +GetAttackDamage()
    }

    class ICombatDamageable {
        <<interface>>
        +ApplyDamage(Damage, Causer, Location, Impulse)
        +GetCurrentHealth()
    }

    class ICombatActivatable {
        <<interface>>
        +Activate()
        +Deactivate()
    }

    ACharacter <|-- ACombatCharacter
    ACombatCharacter <|-- ACombatEnemy
    ACharacter <|-- ACombatDummy
    AGameMode <|-- ACombatGameMode

    ACombatCharacter ..|> ICombatAttacker
    ACombatCharacter ..|> ICombatDamageable
    ACombatEnemy ..|> ICombatAttacker
    ACombatEnemy ..|> ICombatDamageable
    ACombatDummy ..|> ICombatDamageable
```

## Example 2: Project Folder Structure

```mermaid
graph TD
    Root[AntigravityTest/]

    Root --> Source[Source/]
    Root --> Content[Content/]
    Root --> Plugins[Plugins/]
    Root --> Config[Config/]
    Root --> Scripts[scripts/]

    Source --> AT[AntigravityTest/]
    AT --> AI[AI/]
    AT --> Component[Component/]
    AT --> Items[Items/]
    AT --> Tests[Tests/]
    AT --> UI[UI/]
    AT --> Combat[Variant_Combat/]
    AT --> SideScroll[Variant_SideScrolling/]
    AT --> Platform[Variant_Platforming/]

    Combat --> CombatAI[AI/]
    Combat --> CombatAnim[Animation/]
    Combat --> CombatGameplay[Gameplay/]
    Combat --> CombatInterface[Interfaces/]
    Combat --> CombatUI[UI/]

    Content --> InputContent[Input/]
    Content --> ThirdPerson[ThirdPerson/]
    Content --> CombatContent[Variant_Combat/]
    Content --> SideScrollContent[Variant_SideScrolling/]
    Content --> MermaidMaterials[MermaidMaterials/]

    Plugins --> ScreenTransition[ScreenTransitionSystem/]
    Plugins --> MermaidMaterial[MermaidMaterial/]

    style Root fill:#f9f,stroke:#333,stroke-width:4px
    style Source fill:#bbf,stroke:#333,stroke-width:2px
    style Content fill:#bfb,stroke:#333,stroke-width:2px
    style Plugins fill:#fbb,stroke:#333,stroke-width:2px
    style Combat fill:#ffa,stroke:#333,stroke-width:2px
    style SideScroll fill:#ffa,stroke:#333,stroke-width:2px
    style Platform fill:#ffa,stroke:#333,stroke-width:2px
```

## Example 3: Combat Character Components

```mermaid
graph LR
    Character[ACombatCharacter]

    Character --> SpringArm[USpringArmComponent]
    Character --> Camera[UCameraComponent]
    Character --> LifeBar[UCombatLifeBar Widget]
    Character --> Movement[UCharacterMovementComponent]
    Character --> Mesh[USkeletalMeshComponent]
    Character --> Input[UInputComponent]

    SpringArm --> Camera

    Input --> MoveAction[IA_Move]
    Input --> LookAction[IA_Look]
    Input --> JumpAction[IA_Jump]
    Input --> AttackAction[IA_Attack]

    style Character fill:#f96,stroke:#333,stroke-width:3px
    style SpringArm fill:#9cf,stroke:#333,stroke-width:2px
    style Camera fill:#9cf,stroke:#333,stroke-width:2px
    style LifeBar fill:#fc9,stroke:#333,stroke-width:2px
    style Movement fill:#9cf,stroke:#333,stroke-width:2px
```

## Example 4: Game Mode Flow

```mermaid
stateDiagram-v2
    [*] --> MainMenu
    MainMenu --> SelectVariant: Start Game

    SelectVariant --> CombatMode: Choose Combat
    SelectVariant --> SideScrollMode: Choose Side-Scrolling
    SelectVariant --> PlatformMode: Choose Platforming

    CombatMode --> Playing: Load Level
    SideScrollMode --> Playing: Load Level
    PlatformMode --> Playing: Load Level

    Playing --> Paused: Pause
    Paused --> Playing: Resume

    Playing --> GameOver: Player Death
    Playing --> Victory: Win Condition Met

    GameOver --> SelectVariant: Retry
    Victory --> SelectVariant: Next Level

    SelectVariant --> MainMenu: Back
    Paused --> MainMenu: Quit
```

## Example 5: Combat AI System

```mermaid
graph TD
    AIController[ACombatAIController]
    Enemy[ACombatEnemy]
    BehaviorTree[UBehaviorTree]
    Blackboard[UBlackboardComponent]

    AIController --> Enemy
    AIController --> BehaviorTree
    AIController --> Blackboard

    BehaviorTree --> Patrol[Patrol Task]
    BehaviorTree --> Chase[Chase Player Task]
    BehaviorTree --> Attack[Attack Task]
    BehaviorTree --> Flee[Flee Task]

    Blackboard --> TargetPlayer[Target Player]
    Blackboard --> PatrolPoint[Patrol Point]
    Blackboard --> HealthLow[Health Low?]

    Patrol --> Chase: Player Detected
    Chase --> Attack: In Range
    Attack --> Flee: Health Low
    Flee --> Patrol: Health Recovered
    Chase --> Patrol: Lost Player

    style AIController fill:#f96,stroke:#333,stroke-width:3px
    style Enemy fill:#6f9,stroke:#333,stroke-width:3px
    style BehaviorTree fill:#96f,stroke:#333,stroke-width:2px
```

## Example 6: Interface Implementation Map

```mermaid
graph TD
    ICombatAttacker[ICombatAttacker Interface]
    ICombatDamageable[ICombatDamageable Interface]
    ICombatActivatable[ICombatActivatable Interface]

    ICombatAttacker --> CombatChar[ACombatCharacter]
    ICombatAttacker --> Enemy[ACombatEnemy]

    ICombatDamageable --> CombatChar
    ICombatDamageable --> Enemy
    ICombatDamageable --> Dummy[ACombatDummy]
    ICombatDamageable --> Box[ACombatDamageableBox]

    ICombatActivatable --> Volume[ACombatActivationVolume]
    ICombatActivatable --> Spawner[ACombatEnemySpawner]

    style ICombatAttacker fill:#f66,stroke:#333,stroke-width:3px
    style ICombatDamageable fill:#66f,stroke:#333,stroke-width:3px
    style ICombatActivatable fill:#6f6,stroke:#333,stroke-width:3px
```

## Example 7: Plugin Architecture

```mermaid
graph TD
    Project[AntigravityTest Project]

    Project --> ScreenTransition[ScreenTransitionSystem Plugin]
    Project --> MermaidMaterial[MermaidMaterial Plugin]

    ScreenTransition --> TransitionManager[UScreenTransitionManager]
    ScreenTransition --> TransitionWidget[UScreenTransitionWidget]
    ScreenTransition --> AsyncLoader[Async Level Loading]

    MermaidMaterial --> Parser[Mermaid Parser Python]
    MermaidMaterial --> MaterialGen[Material Generator Python]
    MermaidMaterial --> UIWidget[EUW_Mermaid]

    Parser --> MaterialGen
    MaterialGen --> UMaterial[UMaterial Assets]

    style Project fill:#f9f,stroke:#333,stroke-width:4px
    style ScreenTransition fill:#9cf,stroke:#333,stroke-width:3px
    style MermaidMaterial fill:#fc9,stroke:#333,stroke-width:3px
```

## Example 8: Input System Flow

```mermaid
sequenceDiagram
    participant Player
    participant InputComponent
    participant MappingContext
    participant InputAction
    participant Character
    participant CharacterMovement

    Player->>InputComponent: Press W Key
    InputComponent->>MappingContext: Map to Enhanced Input
    MappingContext->>InputAction: IA_Move (Forward=1.0)
    InputAction->>Character: DoMove(0.0, 1.0)
    Character->>CharacterMovement: AddMovementInput(Forward)
    CharacterMovement-->>Character: Update Position
    Character-->>Player: Character Moves Forward
```

## How to Use These Examples

1. **Copy to clipboard**: Select and copy any diagram above
2. **Test in Mermaid Live Editor**: Paste at https://mermaid.live to visualize
3. **Save to file**: Create a .md or .mmd file with the diagram
4. **Integrate with MermaidMaterial**: Use the plugin to generate UE5 material graphs
5. **Include in documentation**: Embed in project README or design docs

## Customization Tips

- **Colors**: Use `style NodeName fill:#color` to customize node colors
- **Shapes**: Different shapes for different node types (rectangles, circles, diamonds)
- **Subgraphs**: Group related items with `subgraph` blocks
- **Icons**: Add icons or emojis for visual appeal (use sparingly in UE projects)
- **Links**: Add clickable links to documentation or source files
