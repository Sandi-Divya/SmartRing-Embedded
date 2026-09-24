import numpy as np
from pathlib import Path

MODEL_DIR = Path("models")

mean = np.load(MODEL_DIR / "raw_window_mean.npy").reshape(-1)
std = np.load(MODEL_DIR / "raw_window_std.npy").reshape(-1)

print("MEAN:")
print(", ".join(f"{x:.8f}f" for x in mean))

print("\nSTD:")
print(", ".join(f"{x:.8f}f" for x in std))