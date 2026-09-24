from pathlib import Path
import numpy as np

DATA_DIR = Path("data/UCI HAR Dataset")

activity_labels = DATA_DIR / "activity_labels.txt"
y_train_path = DATA_DIR / "train/y_train.txt"
x_train_path = DATA_DIR / "train/X_train.txt"

print("Dataset exists:", DATA_DIR.exists())

print("\nActivity labels:")
print(activity_labels.read_text())

y_train = np.loadtxt(y_train_path, dtype=int)
x_train = np.loadtxt(x_train_path)

print("X_train shape:", x_train.shape)
print("y_train shape:", y_train.shape)
print("First label:", y_train[0])
print("First sample feature count:", len(x_train[0]))