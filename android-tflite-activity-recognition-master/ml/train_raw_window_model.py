from pathlib import Path
import numpy as np
import tensorflow as tf
from sklearn.metrics import accuracy_score, classification_report

DATA_DIR = Path("data/UCI HAR Dataset")
MODEL_DIR = Path("models")
MODEL_DIR.mkdir(exist_ok=True)

SIGNALS = [
    "body_acc_x",
    "body_acc_y",
    "body_acc_z",
    "body_gyro_x",
    "body_gyro_y",
    "body_gyro_z",
]

ACTIVITY_LABELS = [
    "WALKING",
    "WALKING_UPSTAIRS",
    "WALKING_DOWNSTAIRS",
    "SITTING",
    "STANDING",
    "LAYING",
]


def load_split(split: str):
    signal_dir = DATA_DIR / split / "Inertial Signals"

    channels = []
    for signal in SIGNALS:
        path = signal_dir / f"{signal}_{split}.txt"
        data = np.loadtxt(path)
        channels.append(data)

    # channels: 6 arrays of shape [samples, 128]
    # stacked: [samples, 128, 6]
    X = np.stack(channels, axis=-1).astype(np.float32)

    y = np.loadtxt(DATA_DIR / split / f"y_{split}.txt", dtype=int) - 1

    return X, y


X_train, y_train = load_split("train")
X_test, y_test = load_split("test")

print("X_train:", X_train.shape)
print("y_train:", y_train.shape)
print("X_test:", X_test.shape)
print("y_test:", y_test.shape)

# Normalize using train statistics
mean = X_train.mean(axis=(0, 1), keepdims=True)
std = X_train.std(axis=(0, 1), keepdims=True) + 1e-6

X_train = (X_train - mean) / std
X_test = (X_test - mean) / std

np.save(MODEL_DIR / "raw_window_mean.npy", mean)
np.save(MODEL_DIR / "raw_window_std.npy", std)

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(128, 6)),

    tf.keras.layers.Conv1D(32, kernel_size=5, activation="relu", padding="same"),
    tf.keras.layers.MaxPooling1D(pool_size=2),

    tf.keras.layers.Conv1D(64, kernel_size=5, activation="relu", padding="same"),
    tf.keras.layers.MaxPooling1D(pool_size=2),

    tf.keras.layers.Flatten(),

    tf.keras.layers.Dense(64, activation="relu"),
    tf.keras.layers.Dropout(0.3),
    tf.keras.layers.Dense(6, activation="softmax"),
])

model.compile(
    optimizer="adam",
    loss="sparse_categorical_crossentropy",
    metrics=["accuracy"],
)

model.summary()

history = model.fit(
    X_train,
    y_train,
    validation_split=0.2,
    epochs=20,
    batch_size=64,
    verbose=1,
)

probs = model.predict(X_test)
y_pred = np.argmax(probs, axis=1)

print("\nRaw-window test accuracy:", accuracy_score(y_test, y_pred))
print("\nClassification report:")
print(classification_report(y_test, y_pred, target_names=ACTIVITY_LABELS))

model.save(MODEL_DIR / "har_raw_window.keras")
print("\nSaved raw-window model to:", MODEL_DIR / "har_raw_window.keras")