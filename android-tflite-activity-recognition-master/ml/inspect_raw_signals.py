from pathlib import Path
import numpy as np

DATA_DIR = Path("data/UCI HAR Dataset")

signal_dir = DATA_DIR / "train/Inertial Signals"

files = [
    "body_acc_x_train.txt",
    "body_acc_y_train.txt",
    "body_acc_z_train.txt",
    "body_gyro_x_train.txt",
    "body_gyro_y_train.txt",
    "body_gyro_z_train.txt",
]

for filename in files:
    path = signal_dir / filename
    data = np.loadtxt(path)
    print(filename, data.shape)

y_train = np.loadtxt(DATA_DIR / "train/y_train.txt", dtype=int)
print("y_train:", y_train.shape)