"""
Unreal Blueprint API Integration - Phase 2
Provides integration with Unreal Engine Python API for detailed Blueprint analysis

This module extracts real Blueprint data using the Unreal Engine Python API:
- Internal structure (nodes, functions, events, variables)
- Dependencies (Blueprint references, parent classes, interfaces)
- Asset metadata and relationships
"""

import os
import json
from typing import Dict, List, Optional, Tuple, Any
from dataclasses import dataclass, asdict
from pathlib import Path

# Unreal Engine imports - only available when running inside UE Editor
try:
    import unreal
    UNREAL_AVAILABLE = True
except ImportError:
    UNREAL_AVAILABLE = False
    print("Warning: Unreal Engine Python API not available. Running in standalone mode.")


@dataclass
class BlueprintNodeInfo:
    """Information about a Blueprint node"""
    node_type: str
    node_title: str
    count: int = 1


@dataclass
class BlueprintFunctionInfo:
    """Information about a Blueprint function"""
    name: str
    is_pure: bool
    is_const: bool
    is_blueprint_callable: bool
    input_count: int
    output_count: int
    node_count: int = 0


@dataclass
class BlueprintVariableInfo:
    """Information about a Blueprint variable"""
    name: str
    variable_type: str
    is_array: bool
    is_exposed: bool
    category: str


@dataclass
class BlueprintDependencyInfo:
    """Information about Blueprint dependencies"""
    blueprint_path: str
    parent_class: Optional[str] = None
    implemented_interfaces: List[str] = None
    referenced_blueprints: List[str] = None
    referenced_assets: List[str] = None

    def __post_init__(self):
        if self.implemented_interfaces is None:
            self.implemented_interfaces = []
        if self.referenced_blueprints is None:
            self.referenced_blueprints = []
        if self.referenced_assets is None:
            self.referenced_assets = []


@dataclass
class DetailedBlueprintMetrics:
    """Detailed metrics extracted from Blueprint using Unreal Python API"""
    name: str
    path: str
    asset_path: str

    # Internal structure
    total_node_count: int = 0
    function_count: int = 0
    event_count: int = 0
    variable_count: int = 0
    macro_count: int = 0

    # Node type breakdown
    tick_event_count: int = 0
    foreach_loop_count: int = 0
    cast_count: int = 0
    string_operation_count: int = 0
    array_operation_count: int = 0
    construction_script_node_count: int = 0

    # Functions detail
    functions: List[BlueprintFunctionInfo] = None

    # Variables detail
    variables: List[BlueprintVariableInfo] = None

    # Node types
    node_types: Dict[str, int] = None

    # Dependencies
    dependencies: Optional[BlueprintDependencyInfo] = None

    # Metadata
    file_size: int = 0
    last_modified: str = ""

    def __post_init__(self):
        if self.functions is None:
            self.functions = []
        if self.variables is None:
            self.variables = []
        if self.node_types is None:
            self.node_types = {}


class UnrealBlueprintAnalyzer:
    """Analyzer using Unreal Engine Python API"""

    def __init__(self):
        self.unreal_available = UNREAL_AVAILABLE
        if UNREAL_AVAILABLE:
            self.asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
            self.editor_asset_library = unreal.EditorAssetLibrary
        else:
            self.asset_registry = None
            self.editor_asset_library = None

    def is_available(self) -> bool:
        """Check if Unreal Python API is available"""
        return self.unreal_available

    def get_all_blueprints(self, content_path: str = "/Game/") -> List[str]:
        """Get all Blueprint assets in the project"""
        if not self.unreal_available:
            return []

        try:
            # Get all Blueprint assets
            filter_options = unreal.ARFilter(
                class_names=["Blueprint"],
                package_paths=[content_path],
                recursive_paths=True
            )

            assets = self.asset_registry.get_assets(filter_options)
            blueprint_paths = [str(asset.package_name) for asset in assets]

            return blueprint_paths
        except Exception as e:
            print(f"Error getting Blueprints: {e}")
            return []

    def analyze_blueprint(self, blueprint_path: str) -> Optional[DetailedBlueprintMetrics]:
        """Analyze a single Blueprint and extract detailed metrics"""
        if not self.unreal_available:
            print(f"Cannot analyze {blueprint_path}: Unreal Python API not available")
            return None

        try:
            # Load the Blueprint asset
            blueprint = self.editor_asset_library.load_asset(blueprint_path)
            if not blueprint:
                print(f"Failed to load Blueprint: {blueprint_path}")
                return None

            # Get Blueprint class
            generated_class = blueprint.get_editor_property('generated_class')
            if not generated_class:
                print(f"No generated class for: {blueprint_path}")
                return None

            # Initialize metrics
            metrics = DetailedBlueprintMetrics(
                name=blueprint.get_name(),
                path=blueprint_path,
                asset_path=blueprint_path
            )

            # Analyze structure
            self._analyze_structure(blueprint, generated_class, metrics)

            # Analyze dependencies
            self._analyze_dependencies(blueprint, blueprint_path, metrics)

            # Get file metadata
            self._get_file_metadata(blueprint_path, metrics)

            return metrics

        except Exception as e:
            print(f"Error analyzing Blueprint {blueprint_path}: {e}")
            return None

    def _analyze_structure(self, blueprint, generated_class, metrics: DetailedBlueprintMetrics):
        """Analyze Blueprint internal structure"""
        try:
            # Get all graphs
            graphs = blueprint.get_editor_property('ubergraph_pages')
            if graphs:
                for graph in graphs:
                    self._analyze_graph(graph, metrics)

            # Analyze functions
            function_graphs = blueprint.get_editor_property('function_graphs')
            if function_graphs:
                metrics.function_count = len(function_graphs)
                for func_graph in function_graphs:
                    func_info = self._analyze_function(func_graph)
                    if func_info:
                        metrics.functions.append(func_info)

            # Analyze variables
            new_variables = blueprint.get_editor_property('new_variables')
            if new_variables:
                metrics.variable_count = len(new_variables)
                for var in new_variables:
                    var_info = self._analyze_variable(var)
                    if var_info:
                        metrics.variables.append(var_info)

        except Exception as e:
            print(f"Error analyzing structure: {e}")

    def _analyze_graph(self, graph, metrics: DetailedBlueprintMetrics):
        """Analyze a Blueprint graph"""
        try:
            nodes = graph.get_editor_property('nodes')
            if not nodes:
                return

            for node in nodes:
                metrics.total_node_count += 1

                # Get node class name
                node_class = node.get_class().get_name()

                # Count node types
                if node_class not in metrics.node_types:
                    metrics.node_types[node_class] = 0
                metrics.node_types[node_class] += 1

                # Detect specific node types
                node_title = str(node.get_editor_property('node_title') or '')

                # Event Tick
                if 'K2Node_Event' in node_class:
                    metrics.event_count += 1
                    if 'Tick' in node_title or 'ReceiveTick' in str(node):
                        metrics.tick_event_count += 1

                # Loops
                if 'ForEach' in node_class or 'ForEachLoop' in node_title:
                    metrics.foreach_loop_count += 1

                # Casts
                if 'Cast' in node_class or 'DynamicCast' in node_class:
                    metrics.cast_count += 1

                # String operations
                if 'String' in node_class:
                    metrics.string_operation_count += 1

                # Array operations
                if 'Array' in node_class:
                    metrics.array_operation_count += 1

                # Construction Script
                if graph.get_name() == 'ConstructionScript':
                    metrics.construction_script_node_count += 1

        except Exception as e:
            print(f"Error analyzing graph: {e}")

    def _analyze_function(self, function_graph) -> Optional[BlueprintFunctionInfo]:
        """Analyze a Blueprint function"""
        try:
            func_name = function_graph.get_name()

            # Count nodes in function
            nodes = function_graph.get_editor_property('nodes')
            node_count = len(nodes) if nodes else 0

            # Get function flags
            # Note: These properties may vary based on UE version
            func_info = BlueprintFunctionInfo(
                name=func_name,
                is_pure=False,  # Would need to check function flags
                is_const=False,
                is_blueprint_callable=False,
                input_count=0,
                output_count=0,
                node_count=node_count
            )

            return func_info

        except Exception as e:
            print(f"Error analyzing function: {e}")
            return None

    def _analyze_variable(self, variable) -> Optional[BlueprintVariableInfo]:
        """Analyze a Blueprint variable"""
        try:
            var_name = variable.get_editor_property('var_name')
            var_type = variable.get_editor_property('var_type')

            var_info = BlueprintVariableInfo(
                name=str(var_name),
                variable_type=str(var_type) if var_type else "Unknown",
                is_array=False,  # Would need to check type details
                is_exposed=variable.get_editor_property('property_flags') is not None,
                category=variable.get_editor_property('category') or "Default"
            )

            return var_info

        except Exception as e:
            print(f"Error analyzing variable: {e}")
            return None

    def _analyze_dependencies(self, blueprint, blueprint_path: str, metrics: DetailedBlueprintMetrics):
        """Analyze Blueprint dependencies"""
        try:
            dependencies = BlueprintDependencyInfo(blueprint_path=blueprint_path)

            # Get parent class
            generated_class = blueprint.get_editor_property('generated_class')
            if generated_class:
                parent_class = generated_class.get_super_class()
                if parent_class:
                    dependencies.parent_class = parent_class.get_name()

            # Get implemented interfaces
            implemented_interfaces = blueprint.get_editor_property('implemented_interfaces')
            if implemented_interfaces:
                for interface in implemented_interfaces:
                    interface_class = interface.get_editor_property('interface')
                    if interface_class:
                        dependencies.implemented_interfaces.append(interface_class.get_name())

            # Get asset dependencies using Asset Registry
            if self.asset_registry:
                asset_data = self.asset_registry.get_asset_by_object_path(blueprint_path)
                if asset_data:
                    # Get dependencies
                    dep_options = unreal.AssetRegistryDependencyOptions(
                        include_soft_package_references=True,
                        include_hard_package_references=True,
                        include_searchable_names=False,
                        include_soft_management_references=False,
                        include_hard_management_references=False
                    )

                    deps = self.asset_registry.get_dependencies(
                        asset_data.package_name,
                        dep_options
                    )

                    for dep in deps:
                        dep_str = str(dep)
                        # Categorize dependencies
                        if '/Game/' in dep_str:
                            if 'Blueprint' in dep_str or 'BP_' in dep_str:
                                dependencies.referenced_blueprints.append(dep_str)
                            else:
                                dependencies.referenced_assets.append(dep_str)

            metrics.dependencies = dependencies

        except Exception as e:
            print(f"Error analyzing dependencies: {e}")

    def _get_file_metadata(self, blueprint_path: str, metrics: DetailedBlueprintMetrics):
        """Get file metadata for the Blueprint"""
        try:
            # Convert package path to file path
            # Package paths are like /Game/Path/To/Asset
            # Need to find corresponding .uasset file

            # This is approximate - actual file path resolution may vary
            if blueprint_path.startswith('/Game/'):
                relative_path = blueprint_path[6:]  # Remove '/Game/'
                # Would need project content directory
                # For now, just store the path
                metrics.last_modified = ""
                metrics.file_size = 0

        except Exception as e:
            print(f"Error getting file metadata: {e}")

    def export_to_json(self, metrics_list: List[DetailedBlueprintMetrics], output_path: str):
        """Export metrics to JSON format"""
        try:
            data = {
                'blueprints': [],
                'summary': {
                    'total_blueprints': len(metrics_list),
                    'total_nodes': sum(m.total_node_count for m in metrics_list),
                    'total_functions': sum(m.function_count for m in metrics_list),
                    'total_variables': sum(m.variable_count for m in metrics_list)
                }
            }

            for metrics in metrics_list:
                bp_data = {
                    'name': metrics.name,
                    'path': metrics.path,
                    'structure': {
                        'total_nodes': metrics.total_node_count,
                        'functions': metrics.function_count,
                        'events': metrics.event_count,
                        'variables': metrics.variable_count,
                        'macros': metrics.macro_count
                    },
                    'performance': {
                        'tick_events': metrics.tick_event_count,
                        'foreach_loops': metrics.foreach_loop_count,
                        'casts': metrics.cast_count,
                        'string_operations': metrics.string_operation_count,
                        'array_operations': metrics.array_operation_count,
                        'construction_script_nodes': metrics.construction_script_node_count
                    },
                    'node_types': metrics.node_types,
                    'functions': [asdict(f) for f in metrics.functions],
                    'variables': [asdict(v) for v in metrics.variables]
                }

                if metrics.dependencies:
                    bp_data['dependencies'] = asdict(metrics.dependencies)

                data['blueprints'].append(bp_data)

            with open(output_path, 'w') as f:
                json.dump(data, f, indent=2)

            print(f"Exported metrics to {output_path}")

        except Exception as e:
            print(f"Error exporting to JSON: {e}")

    def export_to_csv(self, metrics_list: List[DetailedBlueprintMetrics], output_path: str):
        """Export metrics to CSV format"""
        try:
            import csv

            with open(output_path, 'w', newline='') as f:
                writer = csv.writer(f)

                # Write header
                writer.writerow([
                    'Name', 'Path', 'Total Nodes', 'Functions', 'Events', 'Variables',
                    'Tick Events', 'ForEach Loops', 'Casts', 'String Ops', 'Array Ops',
                    'Construction Script Nodes', 'Parent Class', 'Interfaces'
                ])

                # Write data
                for metrics in metrics_list:
                    parent_class = metrics.dependencies.parent_class if metrics.dependencies else ''
                    interfaces = ','.join(metrics.dependencies.implemented_interfaces) if metrics.dependencies else ''

                    writer.writerow([
                        metrics.name,
                        metrics.path,
                        metrics.total_node_count,
                        metrics.function_count,
                        metrics.event_count,
                        metrics.variable_count,
                        metrics.tick_event_count,
                        metrics.foreach_loop_count,
                        metrics.cast_count,
                        metrics.string_operation_count,
                        metrics.array_operation_count,
                        metrics.construction_script_node_count,
                        parent_class,
                        interfaces
                    ])

            print(f"Exported metrics to {output_path}")

        except Exception as e:
            print(f"Error exporting to CSV: {e}")

    def generate_dependency_graph(self, metrics_list: List[DetailedBlueprintMetrics]) -> str:
        """Generate Mermaid dependency graph"""
        try:
            lines = ['```mermaid', 'graph TD']

            # Create nodes and edges
            for metrics in metrics_list:
                if not metrics.dependencies:
                    continue

                safe_name = metrics.name.replace('-', '_').replace(' ', '_')

                # Parent class relationship
                if metrics.dependencies.parent_class:
                    parent_safe = metrics.dependencies.parent_class.replace('-', '_').replace(' ', '_')
                    lines.append(f'    {parent_safe}[{metrics.dependencies.parent_class}] --> {safe_name}[{metrics.name}]')

                # Interface implementations
                for interface in metrics.dependencies.implemented_interfaces:
                    interface_safe = interface.replace('-', '_').replace(' ', '_')
                    lines.append(f'    {interface_safe}{{{{interface: {interface}}}}} -.-> {safe_name}')

                # Blueprint references (limit to avoid clutter)
                for ref in metrics.dependencies.referenced_blueprints[:3]:
                    ref_name = ref.split('/')[-1]
                    ref_safe = ref_name.replace('-', '_').replace(' ', '_')
                    lines.append(f'    {safe_name} --> {ref_safe}[{ref_name}]')

            lines.append('```')
            return '\n'.join(lines)

        except Exception as e:
            print(f"Error generating dependency graph: {e}")
            return ""


def main():
    """Main entry point for testing"""
    print("Unreal Blueprint API Integration - Phase 2")
    print("=" * 60)

    analyzer = UnrealBlueprintAnalyzer()

    if not analyzer.is_available():
        print("Unreal Python API not available.")
        print("This module must be run from within Unreal Editor.")
        return

    # Get all Blueprints
    blueprints = analyzer.get_all_blueprints()
    print(f"Found {len(blueprints)} Blueprints")

    # Analyze first few Blueprints
    metrics_list = []
    for bp_path in blueprints[:5]:
        print(f"\nAnalyzing: {bp_path}")
        metrics = analyzer.analyze_blueprint(bp_path)
        if metrics:
            metrics_list.append(metrics)
            print(f"  Nodes: {metrics.total_node_count}")
            print(f"  Functions: {metrics.function_count}")
            print(f"  Variables: {metrics.variable_count}")

    # Export results
    if metrics_list:
        analyzer.export_to_json(metrics_list, "blueprint_analysis.json")
        analyzer.export_to_csv(metrics_list, "blueprint_analysis.csv")

        print("\n" + analyzer.generate_dependency_graph(metrics_list))


if __name__ == "__main__":
    main()
