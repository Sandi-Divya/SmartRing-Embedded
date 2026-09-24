from pathlib import Path
import numpy as np
import tensorflow as tf
import joblib
from sklearn.metrics import accuracy_score

DATA_DIR = Path("data/UCI HAR Dataset")
MODEL_DIR = Path("models")

X_test = np.loadtxt(DATA_DIR / "test/X_test.txt")
y_test = np.loadtxt(DATA_DIR / "test/y_test.txt", dtype=int) - 1

scaler = joblib.load(MODEL_DIR / "scaler.joblib")
X_test = scaler.transform(X_test).astype(np.float32)

interpreter = tf.lite.Interpreter(model_path=str(MODEL_DIR / "har_baseline.tflite"))
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

print("Input details:", input_details)
print("Output details:", output_details)

predictions = []

for x in X_test:
    x = np.expand_dims(x, axis=0).astype(np.float32)

    interpreter.set_tensor(input_details[0]["index"], x)
    interpreter.invoke()

    output = interpreter.get_tensor(output_details[0]["index"])
    predictions.append(np.argmax(output[0]))

accuracy = accuracy_score(y_test, predictions)
print("TFLite accuracy:", accuracy)