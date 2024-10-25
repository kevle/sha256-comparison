import json
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

file_path = "output.json"
with open(file_path) as f:
    data = json.load(f)

benchmark_data = data["benchmarks"]
df = pd.json_normalize(benchmark_data)

df["MB_per_second"] = df["bytes_per_second"] / 1e6
df["implementation"] = df["name"].apply(lambda x: x.split("/")[0])
df["batchsize"] = df["name"].apply(lambda x: int(x.split("/")[1]))

implementations = df["implementation"].unique()
print(implementations)
batchsizes = df["batchsize"].unique()

num_batchsizes = len(batchsizes)
num_implementations = len(implementations)
width = 1.0 / num_implementations

thread_to_pos = {
    thread_count: (id*(1+width) + 1 - width / 2. * num_implementations) for id, (thread_count, _) in enumerate(df.groupby("threads"))
}
mapping = np.vectorize(thread_to_pos.get)

for num_bytes in batchsizes:
    plt.clf()
    plt.figure(figsize=(12, 8))
    for implementation, group in df.loc[df["batchsize"] == num_bytes].groupby("implementation"):
        group = group.sort_values("threads")
        offset = width * (list(df["implementation"].unique()).index(implementation))
        base_position = mapping(group["threads"].values)
        plt.bar(
            base_position + offset,
            group["MB_per_second"],
            width=width,
            label=implementation,
        )

    plt.xlabel("Thread count")
    plt.ylabel("Throughput (MB/s)")
    plt.title(f"SHA256 throughput by thread count for {num_bytes} byte chunks")

    all_thread_counts = np.array(sorted(df["threads"].unique()))
    x_center_pos = mapping(all_thread_counts) + (num_implementations / 2.) * width - width / 2

    plt.xticks(
        x_center_pos, all_thread_counts
    )
    plt.legend(title="Implementation")
    plt.tight_layout()

    plt.savefig(f"output_{num_bytes}.png")
