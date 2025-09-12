# conv2d (C + OpenMP) — Running on **Kaya** (UWA HPC)

This README shows how to build and run the provided 2‑D convolution program (`src/conv_test_2d.c`) on the **Kaya** cluster using **Slurm**.

> TL;DR: SSH to Kaya, put your project in `/group/cits3402/<username>` or `/scratch/cits3402/<username>`, build with `make`, and submit a Slurm job using the `cits3402` partition.

---

## 1) Prerequisites

- You have a **Kaya** account and can SSH to the login node:
  ```bash
  ssh <username>@kaya01.hpc.uwa.edu.au
  ```
- **Do not** run heavy computations on login nodes. Use **Slurm** to run on compute nodes.
- Recommended working locations (fast storage):
  - `/group/cits3402/<username>` for shared course files
  - `/scratch/cits3402/<username>` for temporary data and job outputs

> Note: HPC file systems are not for long‑term storage; they are regularly purged.

---

## 2) Getting the code onto Kaya

From your laptop/desktop:
```bash
scp -r <local_project_dir> <username>@kaya01.hpc.uwa.edu.au:/group/cits3402/<username>/conv2d
# or
scp -r <local_project_dir> <username>@kaya01.hpc.uwa.edu.au:/scratch/cits3402/<username>/conv2d
```

Then on Kaya:
```bash
ssh <username>@kaya01.hpc.uwa.edu.au
cd /group/cits3402/<username>/conv2d   # or /scratch/cits3402/<username>/conv2d
```

---

## 3) Build

The assignment requires that we can build with a simple `make` on Kaya. Put a `Makefile` at the project root. A minimal one is below:

```makefile
# Makefile (minimal)
CC      := gcc
CFLAGS  := -O3 -march=native -fopenmp
SRC     := src/conv_test_2d.c
TARGET  := conv_test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
```

Build:
```bash
make           # produces ./conv_test
```

> If you prefer to compile manually:  
> `gcc -O3 -march=native -fopenmp -o conv_test src/conv_test_2d.c`

---

## 4) Run via Slurm (batch script)

Create a batch script `conv2d.slurm` in your project root (this compiles and runs the program on a compute node). Make sure the partition is **cits3402**.

```bash
cat > conv2d.slurm <<'SLURM'
#!/bin/bash
#SBATCH --job-name=conv2d
#SBATCH --output=conv2d.%j.out
#SBATCH --error=conv2d.%j.err
#SBATCH --cpus-per-task=16
#SBATCH --time=00:10:00
#SBATCH --mem=8G
#SBATCH --partition=cits3402

# Build (expects Makefile)
make

# Example 1: generate 10k x 10k with 3x3 kernel (parallel)
./conv_test -H 10000 -W 10000 -kH 3 -kW 3 -p

# Example 2: read existing inputs and write output (serial)
# ./conv_test -f f.txt -g g.txt -o o.txt
SLURM
```

Submit the job:
```bash
sbatch conv2d.slurm
```

Check queue/status:
```bash
squeue -u <username>
```

Outputs will appear as `conv2d.<jobid>.out` and `conv2d.<jobid>.err` in your working directory.

---

## 5) Program usage

Two modes are supported by `conv_test`:

- **Generate mode** (random input/kernel), optional save of inputs/outputs:
  ```bash
  ./conv_test -H H -W W -kH kH -kW kW [-f f.txt] [-g g.txt] [-o o.txt] [-s SEED] [-p]
  ```
- **Read mode** (read `f.txt` and `g.txt`, optional `-o`):
  ```bash
  ./conv_test -f f.txt -g g.txt [-o o.txt] [-p]
  ```

Notes:
- Default is **serial**; add `-p` for **OpenMP** parallel.
- Text format for arrays: first line `H W`, followed by rows of space‑separated values.

---

## 6) Timing policy (for your report)

The program prints three timings: `prep`, `conv`, and `io_out` (seconds). **Use only the `conv` time** when reporting performance; exclude I/O and random generation from the timing as required by the assignment.

---

## 7) Troubleshooting

- **Job won’t start**: ensure the script requests the **cits3402** partition and a reasonable wall time (e.g., 10–30 minutes for large runs).  
- **Build fails**: confirm a working `Makefile` and GCC is available; try compiling manually as shown above.  
- **Where are the files?**: prefer working under `/group/cits3402/<username>` or `/scratch/cits3402/<username>`; avoid putting large files in `/home/<username>`.

---

## 8) Useful Slurm commands

```bash
# Show your jobs
squeue -u <username>

# Show partition/node info (what's available)
sinfo | grep cits3402
```

---

## 9) Clean up

Delete large temporary files from `/scratch` and keep your workspace tidy. Remember the HPC storage is **not** for long‑term backup.

---

## 10) Repro examples

```bash
# Large parallel run (10k x 10k, 3x3)
./conv_test -H 10000 -W 10000 -kH 3 -kW 3 -p

# Small serial run (1k x 1k, 3x3)
./conv_test -H 1000 -W 1000 -kH 3 -kW 3
```
