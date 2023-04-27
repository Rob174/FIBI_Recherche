import os
import shutil

folder_path = "." # Replace with the path to your folder
num_folders = 4

# List all files in the folder
files = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]

# Create the new folders
for i in range(num_folders):
    new_folder = os.path.join(folder_path, f"folder_{i+1}")
    os.makedirs(new_folder, exist_ok=True)

# Split the files into the folders
for i, file in enumerate(files):
    dest_folder = os.path.join(folder_path, f"folder_{i % num_folders + 1}")
    shutil.move(os.path.join(folder_path, file), os.path.join(dest_folder, file))
