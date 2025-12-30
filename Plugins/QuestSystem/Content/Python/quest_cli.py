"""
Command-line interface for quest generation
Used by Claude Code skill
"""

import argparse
import os
import sys
from quest_generator import generate_quest_from_file


def main():
    """Main entry point for quest generation CLI"""
    parser = argparse.ArgumentParser(
        description='Generate Unreal Engine quest assets from YAML/JSON definitions'
    )

    parser.add_argument(
        '--from',
        dest='input_file',
        required=True,
        help='Path to quest definition file (YAML or JSON)'
    )

    parser.add_argument(
        '--output',
        dest='output_path',
        default='/Game/Quests/',
        help='Output path for generated assets (default: /Game/Quests/)'
    )

    args = parser.parse_args()

    # Validate input file exists
    if not os.path.exists(args.input_file):
        print(f"Error: Input file not found: {args.input_file}")
        sys.exit(1)

    # Generate quest
    print(f"Generating quest from: {args.input_file}")
    print(f"Output path: {args.output_path}")

    try:
        quest_asset = generate_quest_from_file(args.input_file, args.output_path)

        if quest_asset:
            print(f"✓ Successfully generated quest: {quest_asset.get_name()}")
            print(f"  Asset path: {quest_asset.get_path_name()}")
            return 0
        else:
            print("✗ Failed to generate quest")
            return 1

    except Exception as e:
        print(f"✗ Error generating quest: {str(e)}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
