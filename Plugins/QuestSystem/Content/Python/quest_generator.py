"""
Quest Generator for Unreal Engine 5
Generates Quest DataAssets from YAML/JSON definitions
"""

import unreal
import json
import os
from typing import Dict, List, Any, Optional


class QuestGenerator:
    """Main class for generating quest assets from definitions"""

    def __init__(self):
        self.asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        self.editor_asset_lib = unreal.EditorAssetLibrary()

    def generate_from_yaml(self, yaml_path: str, output_path: str = "/Game/Quests/") -> Optional[unreal.QuestDataAsset]:
        """
        Generate quest assets from a YAML file

        Args:
            yaml_path: Path to the YAML quest definition file
            output_path: UE asset path where quest will be created

        Returns:
            Created QuestDataAsset or None if failed
        """
        try:
            import yaml
            with open(yaml_path, 'r', encoding='utf-8') as f:
                quest_data = yaml.safe_load(f)
        except ImportError:
            unreal.log_warning("PyYAML not available, falling back to JSON")
            # Try as JSON instead
            with open(yaml_path, 'r', encoding='utf-8') as f:
                quest_data = json.load(f)

        return self.generate_from_dict(quest_data, output_path)

    def generate_from_json(self, json_path: str, output_path: str = "/Game/Quests/") -> Optional[unreal.QuestDataAsset]:
        """
        Generate quest assets from a JSON file

        Args:
            json_path: Path to the JSON quest definition file
            output_path: UE asset path where quest will be created

        Returns:
            Created QuestDataAsset or None if failed
        """
        with open(json_path, 'r', encoding='utf-8') as f:
            quest_data = json.load(f)

        return self.generate_from_dict(quest_data, output_path)

    def generate_from_dict(self, quest_data: Dict[str, Any], output_path: str = "/Game/Quests/") -> Optional[unreal.QuestDataAsset]:
        """
        Generate quest assets from a dictionary

        Args:
            quest_data: Dictionary containing quest definition
            output_path: UE asset path where quest will be created

        Returns:
            Created QuestDataAsset or None if failed
        """
        if 'quest' not in quest_data:
            unreal.log_error("Invalid quest data: missing 'quest' key")
            return None

        quest_info = quest_data['quest']

        # Validate required fields
        if not self._validate_quest_data(quest_info):
            return None

        quest_id = quest_info.get('id', 'UnnamedQuest')
        asset_name = f"DA_{quest_id}"
        full_path = f"{output_path}{asset_name}"

        unreal.log(f"Generating quest: {quest_id} at {full_path}")

        # Create objectives first
        objectives = []
        for i, obj_data in enumerate(quest_info.get('objectives', [])):
            obj_asset = self._create_objective(obj_data, quest_id, i, output_path)
            if obj_asset:
                objectives.append(obj_asset)

        # Create main quest asset
        quest_asset = self._create_quest_asset(quest_info, objectives, full_path)

        if quest_asset:
            unreal.log(f"Successfully created quest: {full_path}")
            # Save the asset
            self.editor_asset_lib.save_asset(full_path)

        return quest_asset

    def _validate_quest_data(self, quest_info: Dict[str, Any]) -> bool:
        """Validate quest data has required fields"""
        required_fields = ['id', 'title', 'description']

        for field in required_fields:
            if field not in quest_info:
                unreal.log_error(f"Missing required field: {field}")
                return False

        # Validate objectives
        if 'objectives' not in quest_info or not quest_info['objectives']:
            unreal.log_error("Quest must have at least one objective")
            return False

        for obj in quest_info['objectives']:
            if 'type' not in obj or 'description' not in obj:
                unreal.log_error("Objective missing required fields (type, description)")
                return False

        return True

    def _create_objective(self, obj_data: Dict[str, Any], quest_id: str, index: int,
                         output_path: str) -> Optional[unreal.QuestObjectiveDataAsset]:
        """Create a quest objective DataAsset"""

        obj_id = obj_data.get('id', f"{quest_id}_Obj{index:02d}")
        asset_name = f"DA_{obj_id}"
        full_path = f"{output_path}Objectives/{asset_name}"

        # Ensure the directory exists
        obj_dir = f"{output_path}Objectives/"
        if not self.editor_asset_lib.does_directory_exist(obj_dir):
            self.editor_asset_lib.make_directory(obj_dir)

        # Create the objective asset
        factory = unreal.DataAssetFactory()
        obj_asset = self.asset_tools.create_asset(
            asset_name,
            f"{output_path}Objectives/",
            unreal.QuestObjectiveDataAsset,
            factory
        )

        if not obj_asset:
            unreal.log_error(f"Failed to create objective asset: {full_path}")
            return None

        # Set objective properties
        obj_asset.set_editor_property('objective_id', obj_id)
        obj_asset.set_editor_property('description', unreal.Text(obj_data['description']))

        # Map objective type
        obj_type_str = obj_data.get('type', 'Custom')
        obj_type = self._get_objective_type(obj_type_str)
        obj_asset.set_editor_property('objective_type', obj_type)

        # Set type-specific properties
        if 'target' in obj_data:
            obj_asset.set_editor_property('target_id', obj_data['target'])

        if 'count' in obj_data:
            obj_asset.set_editor_property('required_count', obj_data['count'])

        if 'location' in obj_data:
            obj_asset.set_editor_property('target_id', obj_data['location'])

        obj_asset.set_editor_property('optional', obj_data.get('optional', False))

        # Save the asset
        self.editor_asset_lib.save_asset(full_path)

        return obj_asset

    def _create_quest_asset(self, quest_info: Dict[str, Any], objectives: List,
                           full_path: str) -> Optional[unreal.QuestDataAsset]:
        """Create the main quest DataAsset"""

        # Extract path and name
        asset_path = '/'.join(full_path.split('/')[:-1]) + '/'
        asset_name = full_path.split('/')[-1]

        # Ensure directory exists
        if not self.editor_asset_lib.does_directory_exist(asset_path):
            self.editor_asset_lib.make_directory(asset_path)

        # Create the quest asset
        factory = unreal.DataAssetFactory()
        quest_asset = self.asset_tools.create_asset(
            asset_name,
            asset_path,
            unreal.QuestDataAsset,
            factory
        )

        if not quest_asset:
            unreal.log_error(f"Failed to create quest asset: {full_path}")
            return None

        # Set quest properties
        quest_asset.set_editor_property('quest_id', quest_info['id'])
        quest_asset.set_editor_property('title', unreal.Text(quest_info['title']))
        quest_asset.set_editor_property('description', unreal.Text(quest_info['description']))
        quest_asset.set_editor_property('objectives', objectives)

        # Set rewards
        rewards = []
        for reward_data in quest_info.get('rewards', []):
            reward = self._create_reward(reward_data)
            if reward:
                rewards.append(reward)
        quest_asset.set_editor_property('rewards', rewards)

        # Set prerequisites
        prerequisites = quest_info.get('prerequisites', [])
        quest_asset.set_editor_property('prerequisites', prerequisites)

        # Set repeatable flag
        quest_asset.set_editor_property('repeatable', quest_info.get('repeatable', False))

        return quest_asset

    def _create_reward(self, reward_data: Dict[str, Any]) -> unreal.QuestReward:
        """Create a quest reward struct"""
        reward = unreal.QuestReward()

        # Map reward type
        reward_type_str = reward_data.get('type', 'Custom')
        reward_type = self._get_reward_type(reward_type_str)
        reward.set_editor_property('reward_type', reward_type)

        # Set reward properties
        if 'item' in reward_data:
            reward.set_editor_property('item_id', reward_data['item'])

        if 'amount' in reward_data:
            reward.set_editor_property('amount', reward_data['amount'])

        description = reward_data.get('description', f"{reward_data.get('type')} reward")
        reward.set_editor_property('description', unreal.Text(description))

        return reward

    def _get_objective_type(self, type_str: str) -> unreal.QuestObjectiveType:
        """Map string to objective type enum"""
        type_map = {
            'DefeatEnemies': unreal.QuestObjectiveType.DEFEAT_ENEMIES,
            'ReachLocation': unreal.QuestObjectiveType.REACH_LOCATION,
            'CollectItems': unreal.QuestObjectiveType.COLLECT_ITEMS,
            'InteractWithObject': unreal.QuestObjectiveType.INTERACT_WITH_OBJECT,
            'Custom': unreal.QuestObjectiveType.CUSTOM
        }
        return type_map.get(type_str, unreal.QuestObjectiveType.CUSTOM)

    def _get_reward_type(self, type_str: str) -> unreal.QuestRewardType:
        """Map string to reward type enum"""
        type_map = {
            'Experience': unreal.QuestRewardType.EXPERIENCE,
            'Currency': unreal.QuestRewardType.CURRENCY,
            'Item': unreal.QuestRewardType.ITEM,
            'Custom': unreal.QuestRewardType.CUSTOM
        }
        return type_map.get(type_str, unreal.QuestRewardType.CUSTOM)


def generate_quest_from_file(file_path: str, output_path: str = "/Game/Quests/") -> Optional[unreal.QuestDataAsset]:
    """
    Convenience function to generate a quest from a file

    Args:
        file_path: Path to YAML or JSON quest definition
        output_path: UE asset path for output

    Returns:
        Created QuestDataAsset or None
    """
    generator = QuestGenerator()

    if file_path.endswith('.yaml') or file_path.endswith('.yml'):
        return generator.generate_from_yaml(file_path, output_path)
    elif file_path.endswith('.json'):
        return generator.generate_from_json(file_path, output_path)
    else:
        unreal.log_error(f"Unsupported file format: {file_path}")
        return None


if __name__ == "__main__":
    # Example usage
    unreal.log("Quest Generator loaded. Use generate_quest_from_file() to create quests.")
