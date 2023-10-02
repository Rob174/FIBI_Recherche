import argparse
import pandas as pd
import json
import datetime
import os


def main():
    # Step 1: Create an argument parser to accept the filename as input
    parser = argparse.ArgumentParser(
        description="Find missing seed intervals in a file."
    )
    parser.add_argument("metadata_path", type=str, help="Metadata path")
    parser.add_argument(
        "dataset_path", type=str, help="Path to the dataset where the cpp program wrote"
    )
    args = parser.parse_args()
    with open(args.metadata_path, "r") as f:
        metadata = json.load(f)
    root_path: str  = metadata["root"]
    # Step 0: check if generated less than 1 minute ago
    # Get the current time as a datetime object
    current_time = datetime.datetime.now()
    try:
        # Convert the time string to a datetime object (replace 'time_string' with your actual time string)
        time_string = metadata["last_check"]
        parsed_time = datetime.datetime.strptime(time_string, "%Y-%m-%d %H:%M:%S")

        # Calculate the time difference
        time_difference = current_time - parsed_time

        # Check if the time difference is greater than 1 minute (60 seconds)
        if time_difference.total_seconds() <= 60:
            return
        # If not aggregate the datasets
        os.system(
            f"cat {root_path}out/dataset*.txt >> {root_path}out/merged_dataset.txt"
        )
        os.system(f"rm {root_path}out/dataset*.txt")
    except Exception as e:
        print(e)
    with open(f"{root_path}last_check.json", "w") as f:
        metadata["last_check"] = current_time.strftime("%Y-%m-%d %H:%M:%S")
        json.dump(metadata, f)

    # Step 2: Open the file as text
    try:
        with open(args.dataset_path, "r") as file:
            data_text = file.read()

        # Step 3: Convert the text data to a DataFrame (assuming it's in JSON format)
        data_list_of_dicts = eval(
            "[" + data_text[:-2] + "]"
        )  # Use eval cautiously with untrusted data
        df = pd.DataFrame(data_list_of_dicts)
        df = df.drop_duplicates(subset=["SEED_GLOB"])
        df.sort_values(by="SEED_GLOB", inplace=True)
        with open(args.dataset_path, "w") as file:
            d = df.to_dict(orient="record")  # type: ignore
            s = ",\n".join([str(e) for e in d]) + ",\n"
            file.write(s)

        # Step 4: Get unique and sorted SEED_GLOB values
        unique_sorted_seeds = sorted(df["SEED_GLOB"].unique())

        # Step 5: Create a function to convert a list of integers to intervals
        def create_intervals(lst):
            intervals = []
            start = lst[0]
            end = lst[0]
            for num in lst[1:]:
                if num == end + 1:
                    end = num
                else:
                    intervals.append([int(start), int(end)])
                    start = end = num
            intervals.append([int(start), int(end)])
            return intervals

        # Step 6: Extract intervals that are inside seed_glob
        seed_intervals = create_intervals(unique_sorted_seeds)

        # Step 7: Deduce the missing intervals considering seeds from 0 to 52000
        missing_intervals = []
        if seed_intervals[0][0] > 0:
            missing_intervals.append([0, seed_intervals[0][0] - 1])

        for i in range(1, len(seed_intervals)):
            prev_end = seed_intervals[i - 1][1]
            current_start = seed_intervals[i][0]
            if current_start > prev_end + 1:
                missing_intervals.append([prev_end + 1, current_start - 1])

        if seed_intervals[-1][1] < 52000:
            missing_intervals.append([seed_intervals[-1][1] + 1, 52000])

        # Step 8: Print the intervals of missing seeds
        missing = []
        for interval in missing_intervals:
            # if interval[1]-interval[0]+1 < 10:
            for i in range(int(interval[0]), int(interval[1]) + 1):
                missing.append(i)
            #   continue
            # print(f"{interval[0]:d} -> {interval[1]:d}")
        print(f"{len(missing)} missing seeds")
        with open(f"{root_path}missing.txt", "w") as f:
            f.write("\n".join(list(map(str, missing))))

    except FileNotFoundError:
        print(f"File '{args.filename}' not found.")
    except Exception as e:
        print(f"An error occurred: {str(e)}")


main()
