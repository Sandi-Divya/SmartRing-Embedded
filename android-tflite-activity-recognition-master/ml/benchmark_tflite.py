from pathlib import Path
import time
import numpy as np
import tensorflow as tf
import joblib

DATA_DIR = Path("data/UCI HAR Dataset")
MODEL_DIR = Path("models")

X_test = np.loadtxt(DATA_DIR / "test/X_test.txt")
scaler = joblib.load(MODEL_DIR / "scaler.joblib")
X_test = scaler.transform(X_test).astype(np.float32)

interpreter = tf.lite.Interpreter(model_path=str(MODEL_DIR / "har_baseline.tflite"))
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

latencies_ms = []

# warmup
for i in range(20):
    x = np.expand_dims(X_test[i], axis=0).astype(np.float32)
    interpreter.set_tensor(input_details[0]["index"], x)
    interpreter.invoke()

for x in X_test:
    x = np.expand_dims(x, axis=0).astype(np.float32)

    start = time.perf_counter()
    interpreter.set_tensor(input_details[0]["index"], x)
    interpreter.invoke()
    _ = interpreter.get_tensor(output_details[0]["index"])
    end = time.perf_counter()

    latencies_ms.append((end - start) * 1000)

latencies_ms = np.array(latencies_ms)

print("Samples:", len(latencies_ms))
print(f"Mean latency: {latencies_ms.mean():.4f} ms")
print(f"Median latency: {np.percentile(latencies_ms, 50):.4f} ms")
print(f"P95 latency: {np.percentile(latencies_ms, 95):.4f} ms")
print(f"P99 latency: {np.percentile(latencies_ms, 99):.4f} ms")