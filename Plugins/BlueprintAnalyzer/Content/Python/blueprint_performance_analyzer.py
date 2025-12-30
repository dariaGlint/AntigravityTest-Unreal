"""
Blueprint Performance Analyzer - Phase 3
Advanced performance analysis for Unreal Engine Blueprints

This module provides detailed analysis of Blueprint assets including:
- Performance metrics (Tick usage, heavy operations, Cast optimization)
- Memory usage analysis
- Best practices checks
- Document auto-generation (Mermaid diagrams, reports)
"""

import os
import re
import json
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, field
from enum import Enum


class IssueLevel(Enum):
    """Issue severity levels"""
    INFO = "info"
    WARNING = "warning"
    CRITICAL = "critical"


@dataclass
class PerformanceIssue:
    """Represents a performance issue found in Blueprint analysis"""
    level: IssueLevel
    blueprint_name: str
    issue_type: str
    description: str
    location: str = ""
    suggestion: str = ""
    score_impact: int = 0


@dataclass
class BlueprintMetrics:
    """Metrics collected for a Blueprint"""
    name: str
    path: str
    tick_event_count: int = 0
    foreach_loop_count: int = 0
    cast_count: int = 0
    string_operation_count: int = 0
    array_operation_count: int = 0
    construction_script_complexity: int = 0
    total_node_count: int = 0
    function_count: int = 0
    event_count: int = 0
    variable_count: int = 0
    complexity_score: int = 0
    issues: List[PerformanceIssue] = field(default_factory=list)


class ConfigLoader:
    """Loads configuration from BlueprintAnalyzer.ini"""

    def __init__(self, config_path: str):
        self.config_path = config_path
        self.config = self._load_config()

    def _load_config(self) -> Dict:
        """Load and parse INI configuration file"""
        config = {
            'Performance': {},
            'Memory': {},
            'BestPractices': {},
            'Documentation': {},
            'Scoring': {},
            'CI': {}
        }

        if not os.path.exists(self.config_path):
            print(f"Warning: Config file not found at {self.config_path}, using defaults")
            return self._get_default_config()

        current_section = None
        with open(self.config_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith(';'):
                    continue

                if line.startswith('[') and line.endswith(']'):
                    current_section = line[1:-1]
                    continue

                if '=' in line and current_section:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()

                    # Convert to appropriate type
                    if value.lower() in ('true', 'false'):
                        value = value.lower() == 'true'
                    elif value.isdigit():
                        value = int(value)

                    config[current_section][key] = value

        return config

    def _get_default_config(self) -> Dict:
        """Return default configuration values"""
        return {
            'Performance': {
                'MaxTickUsage': 5,
                'MaxCastPerBlueprint': 10,
                'MaxLoopDepth': 3,
                'WarnOnConstructionScriptComplexity': True,
                'ConstructionScriptComplexityThreshold': 50,
                'MaxForEachLoops': 5,
                'MaxStringOperations': 10
            },
            'Memory': {
                'MaxArrayMapCount': 20,
                'WarnOnLargeAssetReferences': True,
                'MaxObjectReferences': 50
            },
            'BestPractices': {
                'CheckEventDispatcherUsage': True,
                'CheckFunctionDesign': True,
                'RequireComments': True,
                'DetectMagicNumbers': True,
                'CheckNamingConsistency': True
            },
            'Documentation': {
                'GenerateMermaid': True,
                'IncludePrivateFunctions': False,
                'OutputFormat': 'markdown',
                'GenerateClassHierarchy': True,
                'GenerateDependencyMap': True,
                'GenerateImprovementReport': True
            },
            'Scoring': {
                'TickEventWeight': 10,
                'ForEachLoopWeight': 5,
                'CastWeight': 2,
                'StringOperationWeight': 3,
                'ArrayOperationWeight': 2,
                'LowComplexityThreshold': 50,
                'MediumComplexityThreshold': 100,
                'HighComplexityThreshold': 150
            },
            'CI': {
                'FailOnCritical': False,
                'FailOnWarnings': False,
                'MaxCriticalIssues': 0,
                'MaxWarnings': 10
            }
        }

    def get(self, section: str, key: str, default=None):
        """Get configuration value"""
        return self.config.get(section, {}).get(key, default)


class BlueprintPerformanceAnalyzer:
    """Main analyzer class for Blueprint performance analysis"""

    def __init__(self, config_path: str = "Config/BlueprintAnalyzer.ini"):
        self.config = ConfigLoader(config_path)
        self.metrics: List[BlueprintMetrics] = []

    def calculate_complexity_score(self, metrics: BlueprintMetrics) -> int:
        """Calculate complexity score based on various metrics"""
        score = 0
        score += metrics.tick_event_count * self.config.get('Scoring', 'TickEventWeight', 10)
        score += metrics.foreach_loop_count * self.config.get('Scoring', 'ForEachLoopWeight', 5)
        score += metrics.cast_count * self.config.get('Scoring', 'CastWeight', 2)
        score += metrics.string_operation_count * self.config.get('Scoring', 'StringOperationWeight', 3)
        score += metrics.array_operation_count * self.config.get('Scoring', 'ArrayOperationWeight', 2)
        return score

    def analyze_tick_usage(self, metrics: BlueprintMetrics):
        """Analyze EventTick usage"""
        max_tick = self.config.get('Performance', 'MaxTickUsage', 5)

        if metrics.tick_event_count > 0:
            level = IssueLevel.WARNING if metrics.tick_event_count <= max_tick else IssueLevel.CRITICAL
            issue = PerformanceIssue(
                level=level,
                blueprint_name=metrics.name,
                issue_type="Tick Usage",
                description=f"EventTick used {metrics.tick_event_count} times",
                suggestion="Consider using Timers instead of EventTick for better performance",
                score_impact=metrics.tick_event_count * self.config.get('Scoring', 'TickEventWeight', 10)
            )
            metrics.issues.append(issue)

    def analyze_cast_usage(self, metrics: BlueprintMetrics):
        """Analyze Cast operation usage"""
        max_cast = self.config.get('Performance', 'MaxCastPerBlueprint', 10)

        if metrics.cast_count > max_cast:
            issue = PerformanceIssue(
                level=IssueLevel.CRITICAL if metrics.cast_count > max_cast * 1.5 else IssueLevel.WARNING,
                blueprint_name=metrics.name,
                issue_type="Cast Optimization",
                description=f"{metrics.cast_count} Cast operations detected",
                suggestion="Consider implementing interfaces instead of using Cast operations",
                score_impact=metrics.cast_count * self.config.get('Scoring', 'CastWeight', 2)
            )
            metrics.issues.append(issue)

    def analyze_loop_usage(self, metrics: BlueprintMetrics):
        """Analyze ForEach loop usage"""
        max_loops = self.config.get('Performance', 'MaxForEachLoops', 5)

        if metrics.foreach_loop_count > max_loops:
            issue = PerformanceIssue(
                level=IssueLevel.WARNING,
                blueprint_name=metrics.name,
                issue_type="Heavy Operations",
                description=f"ForEach loop used {metrics.foreach_loop_count} times",
                suggestion="Review loop usage, especially in frequently called functions",
                score_impact=metrics.foreach_loop_count * self.config.get('Scoring', 'ForEachLoopWeight', 5)
            )
            metrics.issues.append(issue)

    def analyze_string_operations(self, metrics: BlueprintMetrics):
        """Analyze string operation usage"""
        max_string_ops = self.config.get('Performance', 'MaxStringOperations', 10)

        if metrics.string_operation_count > max_string_ops:
            issue = PerformanceIssue(
                level=IssueLevel.WARNING,
                blueprint_name=metrics.name,
                issue_type="String Operations",
                description=f"{metrics.string_operation_count} string operations detected",
                suggestion="Cache string operations, especially in Tick events",
                score_impact=metrics.string_operation_count * self.config.get('Scoring', 'StringOperationWeight', 3)
            )
            metrics.issues.append(issue)

    def analyze_construction_script(self, metrics: BlueprintMetrics):
        """Analyze ConstructionScript complexity"""
        if not self.config.get('Performance', 'WarnOnConstructionScriptComplexity', True):
            return

        threshold = self.config.get('Performance', 'ConstructionScriptComplexityThreshold', 50)

        if metrics.construction_script_complexity > threshold:
            issue = PerformanceIssue(
                level=IssueLevel.CRITICAL,
                blueprint_name=metrics.name,
                issue_type="ConstructionScript",
                description=f"ConstructionScript has high complexity ({metrics.construction_script_complexity})",
                suggestion="Move heavy operations from ConstructionScript to BeginPlay for better editor performance",
                score_impact=metrics.construction_script_complexity
            )
            metrics.issues.append(issue)

    def analyze_blueprint(self, metrics: BlueprintMetrics) -> BlueprintMetrics:
        """Run all analysis checks on a Blueprint"""
        # Calculate complexity score
        metrics.complexity_score = self.calculate_complexity_score(metrics)

        # Run individual analyzers
        self.analyze_tick_usage(metrics)
        self.analyze_cast_usage(metrics)
        self.analyze_loop_usage(metrics)
        self.analyze_string_operations(metrics)
        self.analyze_construction_script(metrics)

        return metrics

    def generate_performance_report(self, output_file: str = None) -> str:
        """Generate performance analysis report in markdown format"""
        report = []
        report.append("## Performance Analysis Report\n")

        # Separate issues by level
        critical_issues = []
        warnings = []
        info_items = []

        for metrics in self.metrics:
            for issue in metrics.issues:
                if issue.level == IssueLevel.CRITICAL:
                    critical_issues.append(issue)
                elif issue.level == IssueLevel.WARNING:
                    warnings.append(issue)
                else:
                    info_items.append(issue)

        # Critical Issues
        if critical_issues:
            report.append("### Critical Issues\n")
            for issue in critical_issues:
                report.append(f"- ❌ **{issue.blueprint_name}**: {issue.description}")
                if issue.suggestion:
                    report.append(f"  - 💡 {issue.suggestion}")
                report.append("")

        # Warnings
        if warnings:
            report.append("### Warnings\n")
            for issue in warnings:
                report.append(f"- ⚠️ **{issue.blueprint_name}**: {issue.description}")
                if issue.suggestion:
                    report.append(f"  - 💡 {issue.suggestion}")
                report.append("")

        # Recommendations
        if critical_issues or warnings:
            report.append("### Recommendations\n")

            # Group recommendations by type
            recommendations = {}
            for issue in critical_issues + warnings:
                if issue.issue_type not in recommendations:
                    recommendations[issue.issue_type] = []
                recommendations[issue.issue_type].append(issue)

            rec_num = 1
            for issue_type, issues in recommendations.items():
                bp_names = ", ".join(set(i.blueprint_name for i in issues[:3]))
                if len(issues) > 3:
                    bp_names += f" and {len(issues) - 3} more"

                # Get first suggestion
                suggestion = issues[0].suggestion if issues[0].suggestion else "Review and optimize"
                report.append(f"{rec_num}. **{issue_type}** in {bp_names}")
                report.append(f"   - {suggestion}")
                rec_num += 1

        # Summary statistics
        report.append("\n### Summary\n")
        report.append(f"- Total Blueprints Analyzed: {len(self.metrics)}")
        report.append(f"- Critical Issues: {len(critical_issues)}")
        report.append(f"- Warnings: {len(warnings)}")

        # Top complexity offenders
        if self.metrics:
            sorted_metrics = sorted(self.metrics, key=lambda m: m.complexity_score, reverse=True)
            report.append("\n### Top Complexity Offenders\n")
            for i, metrics in enumerate(sorted_metrics[:5], 1):
                report.append(f"{i}. **{metrics.name}** (Score: {metrics.complexity_score})")

        report_text = "\n".join(report)

        if output_file:
            with open(output_file, 'w') as f:
                f.write(report_text)

        return report_text

    def generate_mermaid_diagram(self, diagram_type: str = "hierarchy") -> str:
        """Generate Mermaid diagram for Blueprint relationships"""
        if diagram_type == "hierarchy":
            return self._generate_hierarchy_diagram()
        elif diagram_type == "complexity":
            return self._generate_complexity_diagram()
        else:
            return ""

    def _generate_hierarchy_diagram(self) -> str:
        """Generate class hierarchy Mermaid diagram"""
        diagram = ["```mermaid", "classDiagram"]

        # For now, generate a sample diagram
        # In Phase 2 with Unreal Python API, we can get actual class relationships
        diagram.append("    class Blueprint {")
        diagram.append("        +EventGraph")
        diagram.append("        +ConstructionScript")
        diagram.append("        +Functions")
        diagram.append("    }")

        for metrics in self.metrics[:10]:  # Limit to top 10
            safe_name = re.sub(r'[^a-zA-Z0-9_]', '_', metrics.name)
            diagram.append(f"    class {safe_name} {{")
            diagram.append(f"        +Functions: {metrics.function_count}")
            diagram.append(f"        +Events: {metrics.event_count}")
            diagram.append(f"        +Complexity: {metrics.complexity_score}")
            diagram.append("    }")
            diagram.append(f"    Blueprint <|-- {safe_name}")

        diagram.append("```")
        return "\n".join(diagram)

    def _generate_complexity_diagram(self) -> str:
        """Generate complexity visualization diagram"""
        diagram = ["```mermaid", "graph TD"]

        # Group by complexity level
        low = [m for m in self.metrics if m.complexity_score < self.config.get('Scoring', 'LowComplexityThreshold', 50)]
        medium = [m for m in self.metrics if self.config.get('Scoring', 'LowComplexityThreshold', 50) <= m.complexity_score < self.config.get('Scoring', 'MediumComplexityThreshold', 100)]
        high = [m for m in self.metrics if self.config.get('Scoring', 'MediumComplexityThreshold', 100) <= m.complexity_score < self.config.get('Scoring', 'HighComplexityThreshold', 150)]
        critical = [m for m in self.metrics if m.complexity_score >= self.config.get('Scoring', 'HighComplexityThreshold', 150)]

        diagram.append("    Root[Blueprint Complexity Analysis]")

        if low:
            diagram.append(f"    Root --> Low[Low Complexity: {len(low)} BPs]")
            diagram.append("    style Low fill:#90EE90")

        if medium:
            diagram.append(f"    Root --> Medium[Medium Complexity: {len(medium)} BPs]")
            diagram.append("    style Medium fill:#FFD700")

        if high:
            diagram.append(f"    Root --> High[High Complexity: {len(high)} BPs]")
            diagram.append("    style High fill:#FFA500")

        if critical:
            diagram.append(f"    Root --> Critical[Critical Complexity: {len(critical)} BPs]")
            diagram.append("    style Critical fill:#FF6347")
            for metrics in critical[:5]:
                safe_name = re.sub(r'[^a-zA-Z0-9_]', '_', metrics.name)
                diagram.append(f"    Critical --> {safe_name}[{metrics.name}: {metrics.complexity_score}]")

        diagram.append("```")
        return "\n".join(diagram)


def main():
    """Main entry point for standalone execution"""
    print("Blueprint Performance Analyzer - Phase 3")
    print("=" * 60)

    # Initialize analyzer
    analyzer = BlueprintPerformanceAnalyzer()

    # For demonstration, create sample metrics
    # In actual implementation with Unreal Python API, this would scan actual Blueprints
    sample_metrics = BlueprintMetrics(
        name="BP_CombatCharacter",
        path="/Game/Variant_Combat/Blueprints/BP_CombatCharacter",
        tick_event_count=2,
        foreach_loop_count=3,
        cast_count=15,
        string_operation_count=8,
        construction_script_complexity=60,
        total_node_count=150,
        function_count=12,
        event_count=8
    )

    sample_metrics2 = BlueprintMetrics(
        name="BP_Enemy",
        path="/Game/Variant_Combat/Blueprints/BP_Enemy",
        tick_event_count=1,
        foreach_loop_count=1,
        cast_count=5,
        string_operation_count=2,
        construction_script_complexity=20,
        total_node_count=80,
        function_count=6,
        event_count=5
    )

    # Analyze
    analyzer.metrics.append(analyzer.analyze_blueprint(sample_metrics))
    analyzer.metrics.append(analyzer.analyze_blueprint(sample_metrics2))

    # Generate report
    report = analyzer.generate_performance_report()
    print(report)

    # Generate diagrams
    print("\n" + analyzer.generate_mermaid_diagram("hierarchy"))
    print("\n" + analyzer.generate_mermaid_diagram("complexity"))


if __name__ == "__main__":
    main()
