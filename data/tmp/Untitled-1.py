if __name__ == "__main__":
    with open("data/tmp/FI.txt") as f:
        fi = f.read().split(",")
    with open("data/tmp/BI.txt") as f:
        bi = f.read().split(",")
    with open(
        "data/tmp/FI_BI_50_points_init_RAND_16_clusters_1000_samples.txt", "w"
    ) as f:
        f.write(f"F/init\t\tBI/init\n")
        for eFI, eBI in zip(fi, bi):
            f.write(f"{eFI}\t\t{eBI}\n")
