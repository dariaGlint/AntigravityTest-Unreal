#!/usr/bin/env python3
"""
Class Analyzer for UE5 C++ Code

Parses C++ header files to extract class information for Mermaid diagram generation.
"""

import re
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from dataclasses import dataclass, field


@dataclass
class ClassInfo:
    """Information about a C++ class."""
    name: str
    file_path: str
    base_classes: List[str] = field(default_factory=list)
    interfaces: List[str] = field(default_factory=list)
    methods: List[str] = field(default_factory=list)
    properties: List[str] = field(default_factory=list)
    is_abstract: bool = False
    is_interface: bool = False
    category: str = ""  # Actor, Component, GameMode, etc.


class UE5ClassAnalyzer:
    """Analyzes UE5 C++ classes and generates structure information."""

    # UE5 class prefixes
    PREFIX_MAP = {
        'A': 'Actor',
        'U': 'Object',
        'F': 'Struct',
        'I': 'Interface',
        'E': 'Enum',
        'T': 'Template'
    }

    def __init__(self):
        self.classes: Dict[str, ClassInfo] = {}
        self.inheritance_graph: Dict[str, List[str]] = {}

    def analyze_file(self, file_path: Path) -> Optional[ClassInfo]:
        """Analyze a single header file and extract class information."""
        try:
            content = file_path.read_text(encoding='utf-8')
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return None

        # Find UCLASS declarations
        class_pattern = r'UCLASS\((.*?)\)\s*class\s+(\w+_API\s+)?(\w+)\s*:\s*public\s+(.*?)\s*{'
        matches = re.finditer(class_pattern, content, re.MULTILINE | re.DOTALL)

        for match in matches:
            uclass_specifiers = match.group(1)
            class_name = match.group(3)
            inheritance = match.group(4)

            # Parse base classes
            base_classes = []
            interfaces = []
            for base in inheritance.split(','):
                base = base.strip()
                if not base:
                    continue
                # Remove public/protected/private keywords
                base = re.sub(r'^(public|protected|private)\s+', '', base)

                if base.startswith('I') and len(base) > 1 and base[1].isupper():
                    # Interface
                    interfaces.append(base)
                else:
                    base_classes.append(base)

            # Check if abstract
            is_abstract = 'abstract' in uclass_specifiers.lower()

            # Determine category from prefix
            category = self._get_category(class_name)

            class_info = ClassInfo(
                name=class_name,
                file_path=str(file_path),
                base_classes=base_classes,
                interfaces=interfaces,
                is_abstract=is_abstract,
                category=category
            )

            # Extract public methods (simplified)
            self._extract_methods(content, class_info)

            self.classes[class_name] = class_info
            return class_info

        # Also check for UINTERFACE
        interface_pattern = r'UINTERFACE\((.*?)\)\s*class\s+(\w+_API\s+)?(\w+)\s*:\s*public\s+UInterface'
        matches = re.finditer(interface_pattern, content, re.MULTILINE)

        for match in matches:
            class_name = match.group(3)
            class_info = ClassInfo(
                name=class_name,
                file_path=str(file_path),
                is_interface=True,
                category='Interface'
            )
            self.classes[class_name] = class_info
            return class_info

        return None

    def _get_category(self, class_name: str) -> str:
        """Determine class category from name prefix."""
        if not class_name:
            return "Unknown"

        prefix = class_name[0]
        base_category = self.PREFIX_MAP.get(prefix, "Unknown")

        # More specific categorization
        if 'GameMode' in class_name:
            return 'GameMode'
        elif 'PlayerController' in class_name:
            return 'PlayerController'
        elif 'Character' in class_name:
            return 'Character'
        elif 'AIController' in class_name:
            return 'AIController'
        elif 'Component' in class_name:
            return 'Component'
        elif 'Widget' in class_name or 'UI' in class_name:
            return 'UI'
        elif 'Volume' in class_name:
            return 'Volume'
        elif 'AnimNotify' in class_name:
            return 'Animation'

        return base_category

    def _extract_methods(self, content: str, class_info: ClassInfo):
        """Extract UFUNCTION declarations (simplified)."""
        # Look for UFUNCTION declarations
        func_pattern = r'UFUNCTION\([^)]*\)\s*(?:virtual\s+)?(\w+)\s+(\w+)\s*\('
        matches = re.finditer(func_pattern, content)

        for match in matches:
            return_type = match.group(1)
            func_name = match.group(2)
            class_info.methods.append(f"{func_name}()")

    def analyze_directory(self, directory: Path, pattern: str = "**/*.h") -> List[ClassInfo]:
        """Analyze all header files in a directory."""
        analyzed = []
        for file_path in directory.glob(pattern):
            if file_path.is_file():
                result = self.analyze_file(file_path)
                if result:
                    analyzed.append(result)
        return analyzed

    def build_inheritance_graph(self):
        """Build inheritance relationship graph."""
        self.inheritance_graph = {}
        for class_name, class_info in self.classes.items():
            if class_info.base_classes:
                for base in class_info.base_classes:
                    if base not in self.inheritance_graph:
                        self.inheritance_graph[base] = []
                    self.inheritance_graph[base].append(class_name)

    def get_class_hierarchy(self, root_class: Optional[str] = None, max_depth: int = -1) -> Dict[str, List[str]]:
        """Get class hierarchy starting from root_class."""
        if not self.inheritance_graph:
            self.build_inheritance_graph()

        if root_class is None:
            return self.inheritance_graph

        # BFS to get hierarchy up to max_depth
        hierarchy = {}
        visited = set()
        queue = [(root_class, 0)]

        while queue:
            current, depth = queue.pop(0)
            if current in visited:
                continue
            if max_depth >= 0 and depth > max_depth:
                continue

            visited.add(current)
            children = self.inheritance_graph.get(current, [])
            if children:
                hierarchy[current] = children
                for child in children:
                    queue.append((child, depth + 1))

        return hierarchy

    def filter_by_variant(self, variant: str) -> Dict[str, ClassInfo]:
        """Filter classes by variant (Combat, SideScrolling, Platforming)."""
        filtered = {}
        search_path = f"Variant_{variant}"

        for class_name, class_info in self.classes.items():
            if search_path in class_info.file_path or variant.lower() in class_name.lower():
                filtered[class_name] = class_info

        return filtered

    def get_interface_implementations(self, interface_name: Optional[str] = None) -> Dict[str, List[str]]:
        """Get mapping of interfaces to implementing classes."""
        implementations = {}

        for class_name, class_info in self.classes.items():
            for interface in class_info.interfaces:
                if interface_name and interface != interface_name:
                    continue

                if interface not in implementations:
                    implementations[interface] = []
                implementations[interface].append(class_name)

        return implementations


def generate_mermaid_class_diagram(analyzer: UE5ClassAnalyzer,
                                   variant: Optional[str] = None,
                                   show_interfaces: bool = True,
                                   max_depth: int = -1) -> str:
    """Generate Mermaid class diagram from analyzer results."""

    classes_to_show = analyzer.classes
    if variant:
        classes_to_show = analyzer.filter_by_variant(variant)

    lines = ["classDiagram"]

    # Define classes
    for class_name, class_info in classes_to_show.items():
        if class_info.is_interface:
            lines.append(f"    class {class_name} {{")
            lines.append(f"        <<interface>>")
            for method in class_info.methods[:5]:  # Limit to 5 methods
                lines.append(f"        +{method}")
            lines.append(f"    }}")
        else:
            lines.append(f"    class {class_name} {{")
            if class_info.is_abstract:
                lines.append(f"        <<abstract>>")
            for method in class_info.methods[:3]:  # Limit to 3 methods
                lines.append(f"        +{method}")
            lines.append(f"    }}")

    # Add inheritance relationships
    for class_name, class_info in classes_to_show.items():
        for base_class in class_info.base_classes:
            # Only show relationship if base is also in our set or is a known UE class
            if base_class in classes_to_show or base_class.startswith(('A', 'U')) and len(base_class) > 1:
                lines.append(f"    {base_class} <|-- {class_name}")

    # Add interface implementations
    if show_interfaces:
        for class_name, class_info in classes_to_show.items():
            for interface in class_info.interfaces:
                lines.append(f"    {class_name} ..|> {interface}")

    return "\n".join(lines)


if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python class_analyzer.py <source_directory> [variant]")
        sys.exit(1)

    source_dir = Path(sys.argv[1])
    variant = sys.argv[2] if len(sys.argv) > 2 else None

    analyzer = UE5ClassAnalyzer()
    analyzer.analyze_directory(source_dir)

    print(f"Found {len(analyzer.classes)} classes")

    diagram = generate_mermaid_class_diagram(analyzer, variant=variant)
    print("\n" + diagram)
