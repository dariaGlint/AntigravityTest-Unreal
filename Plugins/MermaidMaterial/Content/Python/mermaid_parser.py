import re

class MermaidParser:
    def __init__(self):
        self.nodes = {} # {id: type}
        self.edges = [] # [(from_id, to_id)]

    def parse(self, text):
        self.nodes = {}
        self.edges = []
        
        lines = text.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('graph') or line.startswith('%%'):
                continue
            
            # Simple parsing for Node Definition: A[Type]
            # And Connection: A --> B
            
            # Extract nodes first to ensure we know types
            # Regex for ID[Type]
            # Potential patterns:
            # A[TextureSample] --> B[Multiply]
            # A --> B
            # C[Constant]
            
            # Find all node definitions in the line
            # Pattern: Identifier followed by [Type]
            # We assume ID is alphanumeric, Type can be anything inside []
            node_matches = re.finditer(r'([a-zA-Z0-9_]+)\[([^\]]+)\]', line)
            for match in node_matches:
                node_id = match.group(1)
                node_type = match.group(2)
                self.nodes[node_id] = node_type
            
            # Find connections
            if '-->' in line:
                parts = line.split('-->')
                if len(parts) >= 2:
                    source_part = parts[0].strip()
                    target_part = parts[1].strip()
                    
                    # Extract IDs from parts (ignore [Type] if present for connection logic)
                    source_id = self._extract_id(source_part)
                    target_id = self._extract_id(target_part)
                    
                    if source_id and target_id:
                        self.edges.append((source_id, target_id))
                        
        return self.nodes, self.edges

    def _extract_id(self, part):
        # Extract ID from strings like "A[Type]" or "A"
        match = re.search(r'^([a-zA-Z0-9_]+)', part)
        if match:
            return match.group(1)
        return None
