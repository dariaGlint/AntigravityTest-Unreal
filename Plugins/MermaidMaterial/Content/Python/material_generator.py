import unreal
import mermaid_parser

def create_material_from_mermaid(mermaid_text, material_name="M_MermaidGenerated"):
    # 1. Parse Mermaid
    parser = mermaid_parser.MermaidParser()
    nodes_data, edges_data = parser.parse(mermaid_text)
    
    # 2. Create Asset
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    package_path = "/Game/MermaidMaterials"
    
    # Check if directory exists, if not, creating asset might fail or create folder? 
    # AssetTools creates folder automatically usually.
    
    # Make a unique name
    unique_name = asset_tools.create_unique_asset_name(package_path + "/", material_name)[0]
    asset_name = unique_name.split('/')[-1]
    
    material_factory = unreal.MaterialFactoryNew()
    new_material = asset_tools.create_asset(asset_name, package_path, unreal.Material, material_factory)
    
    if not new_material:
        unreal.log_error("Failed to create material")
        return None
        
    # 3. Create Nodes
    # Map friendly names to actual classes or functions
    # This is a simple mapping, can be expanded
    class_map = {
        "TextureSample": unreal.MaterialExpressionTextureSample,
        "Add": unreal.MaterialExpressionAdd,
        "Multiply": unreal.MaterialExpressionMultiply,
        "Constant": unreal.MaterialExpressionConstant,
        "Constant3Vector": unreal.MaterialExpressionConstant3Vector,
        "Constant4Vector": unreal.MaterialExpressionConstant4Vector,
        "ScalarParameter": unreal.MaterialExpressionScalarParameter,
        "VectorParameter": unreal.MaterialExpressionVectorParameter,
        "Sine": unreal.MaterialExpressionSine,
        "Time": unreal.MaterialExpressionTime,
        "Panner": unreal.MaterialExpressionPanner,
        "TexCoord": unreal.MaterialExpressionTextureCoordinate,
    }
    
    created_expressions = {} # {node_id: expression_object}
    
    # Layout config
    start_x = -600
    start_y = 0
    x_step = 250
    y_step = 100
    
    # Traverse nodes to create them
    idx = 0
    import re # Ensure re is imported
    
    for node_id, raw_node_type in nodes_data.items():
        if raw_node_type == "BaseColor":
            continue
            
        # Parse Value: Type(Value)
        node_type = raw_node_type
        node_value = None
        
        # Regex to find Type(Value)
        # Handles cases like Constant3Vector(1,0,0) or ScalarParameter ( 2.0 )
        match = re.match(r'([^\(]+)\s*\(\s*(.+)\s*\)', raw_node_type)
        if match:
            node_type = match.group(1).strip()
            node_value = match.group(2).strip()
            unreal.log(f"Debug: Parsed Value for {node_id}: Type={node_type}, Value={node_value}")
            
        ue_class = class_map.get(node_type)
        if not ue_class:
            unreal.log_warning(f"Unknown node type: {node_type} (Raw: {raw_node_type})")
            continue
            
        expression = unreal.MaterialEditingLibrary.create_material_expression(new_material, ue_class)
        created_expressions[node_id] = expression
        
        # Set Parameter Name
        if isinstance(expression, unreal.MaterialExpressionParameter):
            expression.set_editor_property("ParameterName", node_id)
        
        # Apply Values
        if node_value:
            try:
                # Constant (Scalar)
                if isinstance(expression, unreal.MaterialExpressionConstant):
                    val = float(node_value)
                    expression.r = val
                    
                # Constant3Vector / 4Vector
                elif isinstance(expression, (unreal.MaterialExpressionConstant3Vector, unreal.MaterialExpressionConstant4Vector)):
                    # Expect "R,G,B" or "R,G,B,A"
                    vals = [float(v.strip()) for v in node_value.split(',')]
                    if len(vals) >= 3:
                        a = vals[3] if len(vals) > 3 else 1.0
                        color = unreal.LinearColor(vals[0], vals[1], vals[2], a)
                        expression.constant = color
                        
                # Scalar Parameter
                elif isinstance(expression, unreal.MaterialExpressionScalarParameter):
                    val = float(node_value)
                    expression.set_editor_property("DefaultValue", val)

                    
                # Vector Parameter
                elif isinstance(expression, unreal.MaterialExpressionVectorParameter):
                    vals = [float(v.strip()) for v in node_value.split(',')]
                    if len(vals) >= 3:
                        a = vals[3] if len(vals) > 3 else 1.0
                        color = unreal.LinearColor(vals[0], vals[1], vals[2], a)
                        expression.default_value = color
                        
            except Exception as e:
                unreal.log_warning(f"Failed to parse value for {node_id}: {node_value} - {e}")

        # Simple positioning
        expression.material_expression_editor_x = start_x + (idx * x_step) % 1000
        expression.material_expression_editor_y = start_y + (idx // 4) * y_step
        idx += 1

    # 4. Connect Edges
    unreal.log(f"Debug: Nodes: {nodes_data}")
    unreal.log(f"Debug: Edges: {edges_data}")

    for source_id, target_id in edges_data:
        target_type = nodes_data.get(target_id)
        unreal.log(f"Debug: Processing Edge {source_id} -> {target_id} (TargetType: {target_type})")
        
        # Define supported Material Properties
        property_map = {
            "BaseColor": unreal.MaterialProperty.MP_BASE_COLOR,
            "Metallic": unreal.MaterialProperty.MP_METALLIC,
            "Specular": unreal.MaterialProperty.MP_SPECULAR,
            "Roughness": unreal.MaterialProperty.MP_ROUGHNESS,
            "EmissiveColor": unreal.MaterialProperty.MP_EMISSIVE_COLOR,
            "Opacity": unreal.MaterialProperty.MP_OPACITY,
            "OpacityMask": unreal.MaterialProperty.MP_OPACITY_MASK,
            "Normal": unreal.MaterialProperty.MP_NORMAL,
            "WorldPositionOffset": unreal.MaterialProperty.MP_WORLD_POSITION_OFFSET,
        }

        # Check if target is a Material Property Root Node
        target_prop = property_map.get(target_type) or property_map.get(target_id)
        
        if target_prop is not None:
            source_expr = created_expressions.get(source_id)
            unreal.log(f"Debug: Connecting to Main Node Property: {target_type or target_id}. Source: {source_id}")
            
            if source_expr:
                try:
                    source_pin = ""
                    # Special default pins for specific source types
                    if isinstance(source_expr, unreal.MaterialExpressionTextureSample):
                        source_pin = "RGB"
                    
                    # For Opacity/Roughness/Metallic, usually we want a single channel if it comes from a Texture
                    # But if it sends RGB, it converts to float (grayscale) automatically often, or we might want specific channels.
                    # For now, let's stick to simple logic: TextureSample -> RGB unless specified otherwise.
                    # (Refactoring point: Allow A[R] --> Roughness in parser)
                    
                    unreal.MaterialEditingLibrary.connect_material_property(
                        source_expr, 
                        source_pin, 
                        target_prop
                    )
                except Exception as e:
                    unreal.log_error(f"Error connecting to {target_type or target_id}: {e}")
            continue
            
        source_expr = created_expressions.get(source_id)
        target_expr = created_expressions.get(target_id)
        
        if source_expr and target_expr:
            # Auto-connect first outputs to first inputs (naive approach)
            # You might want to specify pins in mermaid: A[RGB] --> B[A]
            # For now, default "" connects Main Output to Main Input (usually A)
            
            # Track connection counts to assign A/B inputs
            # This requires us to process edges sequentially and track usage
            if 'node_input_counts' not in locals():
                node_input_counts = {}
            
            current_count = node_input_counts.get(target_id, 0)
            node_input_counts[target_id] = current_count + 1
            
            source_pin = ""
            target_pin = ""
            
            # Simple heuristic for common nodes
            if isinstance(source_expr, unreal.MaterialExpressionTextureSample):
                source_pin = "RGB" 
                
            if isinstance(target_expr, (unreal.MaterialExpressionAdd, unreal.MaterialExpressionMultiply)):
                # Assign A or B based on how many times we've connected to this node
                if current_count == 0:
                    target_pin = "A"
                elif current_count == 1:
                    target_pin = "B"
                else:
                    target_pin = "B" # Fallback/Overwrite
            
            unreal.log(f"Connecting {source_id}.{source_pin} -> {target_id}.{target_pin}")
            unreal.MaterialEditingLibrary.connect_material_expressions(source_expr, source_pin, target_expr, target_pin)

    # 5. Recompile
    unreal.MaterialEditingLibrary.recompile_material(new_material)
    
    return asset_name
