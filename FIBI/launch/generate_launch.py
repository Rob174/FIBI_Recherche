import argparse
from string import Template

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate the launch scripts')
    parser.add_argument('--impr', type=int, nargs='+', required=True, help='List of improvement values possible')
    parser.add_argument('--datasets', type=int, nargs='+', required=True, help='List of datasets values possible')
    parser.add_argument('--n_threads', type=int, required=False, default=96, help='Number of threads to use for each script')
    args = parser.parse_args()
    with open('template_slurm.txt') as f:
        t = Template(template=f.read())
    i = 0
    for dataset in args.datasets:
        for impr in args.impr:
            with open(f'launch{i}.sh','w') as f:
                f.write(t.substitute(
                    id_folder=i,
                    n_threads=96,
                    impr=impr,
                    dataset=dataset
                ))
            i+=1