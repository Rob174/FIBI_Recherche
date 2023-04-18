import argparse
def linspace(start,end,num_points):
    return [int(start + i*(end-start)/(num_points-1)) for i in range(num_points)]
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', help='start seed')
    parser.add_argument('-e', help='end seed')
    parser.add_argument('-n', help='number of processes')
    args = parser.parse_args()
    if args.s is None or args.e is None or args.n is None:
        start = int(input("Start seed? :"))
        end = int(input("End seed? :"))
        n = int(input("Number of processes? :"))
    else:
        start = int(args.s)
        end = int(args.e)
        n = int(args.n)
    v = linspace(start,end,n+1)
    flags = input("Flags? :")
    for i,(s,e) in enumerate(zip(v[:-1],v[1:])):
        if i>0:
            s += 1
        print(rf'{i+1} .\Max_SAT.exe -s {s} -e {e} {flags}')
