from pathlib import Path
import numpy as np
import tensorflow as tf
from sklearn.metrics import accuracy_score

DATA_DIR = Path("data/UCI HAR Dataset")
MODEL_DIR = Path("models")

SIGNALS = [
    "body_acc_x",
    "body_acc_y",
    "body_acc_z",
    "body_gyro_x",
    "body_gyro_y",
    "body_gyro_z",
]

def load_split(split: str):
    signal_dir = DATA_DIR / split / "Inertial Signals"

    channels = []
    for signal in SIGNALS:
        path = signal_dir / f"{signal}_{split}.txt"
        data = np.loadtxt(path)
        channels.append(data)

    X = np.stack(channels, axis=-1).astype(np.float32)
    y = np.loadtxt(DATA_DIR / split / f"y_{split}.txt", dtype=int) - 1
    return X, y

X_test, y_test = load_split("test")

mean = np.load(MODEL_DIR / "raw_window_mean.npy")
std = np.load(MODEL_DIR / "raw_window_std.npy")
X_test = ((X_test - mean) / std).astype(np.float32)

for model_name in ["har_raw_window.tflite", "har_raw_window_dynamic_quant.tflite"]:
    interpreter = tf.lite.Interpreter(model_path=str(MODEL_DIR / model_name))
    interpreter.allocate_tensors()

    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    predictions = []

    for x in X_test:
        x = np.expand_dims(x, axis=0).astype(np.float32)

        interpreter.set_tensor(input_details[0]["index"], x)
        interpreter.invoke()

        output = interpreter.get_tensor(output_details[0]["index"])
        predictions.append(np.argmax(output[0]))

    acc = accuracy_score(y_test, predictions)
    print(model_name, "accuracy:", acc)