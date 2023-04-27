import argparse
import json
import os

parser = argparse.ArgumentParser(description='Check for missing and duplicate SEED_GLOB values.')
parser.add_argument('--folder', type=str, required=True, help='Path to folder containing missing and merged files.')
args = parser.parse_args()

missing_file = os.path.join(args.folder, 'missing.txt')
merged_file = os.path.join(args.folder, 'dataset.txt')

# Read in missing seeds from file and cast to integers
# with open(missing_file, 'r') as f:
#     data = f.read().split(',')
#     try:
#         missing_seeds = [int(seed) for seed in data]
#     except Exception:
#         print('Warning conversion missing seeds not possible for : ', data)
#         missing_seeds = []

# Read in data from merged file
with open(merged_file, 'r') as f:
    data = f.read()

# Remove last two characters and [ before ] from data and parse as JSON
json_data = json.loads('[' + data[:-2] + ']')

# Extract SEED_GLOB values from dicts in JSON data and cast to integers
seeds = [int(d['SEED_GLOB']) for d in json_data]

# Check for duplicates in seeds and print any duplicates found
duplicates = set([x for x in seeds if seeds.count(x) > 1])
if duplicates:
    print("Duplicate SEED_GLOB values found:", ', '.join(str(x) for x in duplicates))
    last_duplicate = list(duplicates)[-1]  # get the last duplicated SEED_GLOB value
    merged_dicts = [d for d in json_data if d['SEED_GLOB'] == last_duplicate]
    print(f"All merged dicts containing {last_duplicate}:")
    for d in merged_dicts:
        print('-'*100)
        print(d)
else:
    print('All good, no duplicates')