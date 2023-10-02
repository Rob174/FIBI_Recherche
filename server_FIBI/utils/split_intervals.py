from pathlib import Path
from string import Template
import json
def split_interval(start, end, n):
    interval_size = (end - start) / n
    intervals = []

    for i in range(n):
        interval_start = start + i * interval_size
        interval_end = start + (i + 1) * interval_size
        if i == 0:
            interval_end += 1
        intervals.append((int(interval_start), int(interval_end)))

    return intervals


def run():
    # Define the interval and the number of intervals
    start = 0
    end = 52000
    n = 10
    n_threads_choice = 64
    impr = 0
    duration = "3-00:00:00"
    name = "launch_FIBI"
    do_missings = 2
    dataset = 0
    root_dir = Path(__file__).parent.parent
    with open(root_dir / "metadata.json") as f:
        f = json.load(f)
    
    dorsal = f['server'] == "dorsal"
    root_path = f['root_path']
    templates_dir = root_dir / "templates"
    with open(templates_dir / "template_launch.txt") as f:
        template = f.read()
    template = Template(template)
    
    installs = ""
    if dorsal:
        with open(templates_dir / "installs_dorsal.txt") as f:
            installs = f.read()
    else:
        with open(templates_dir / "installs_cc.txt") as f:
            installs = f.read()
    intervals = split_interval(start, end, n)

    # Print the resulting intervals
    for i, (interval_start, interval_end) in enumerate(intervals):
        content = template.substitute(
            n_threads_choice=n_threads_choice,
            seed_start_choice=interval_start,
            seed_end_choice=interval_end,
            duration=duration,
            installs=installs,
            impr=impr,
            do_missings=do_missings,
            dataset=dataset,
            root_path=root_path
        )
        with open(root_dir / "launches" / f"{name}{i}","w") as f:
            f.write(content)


run()
