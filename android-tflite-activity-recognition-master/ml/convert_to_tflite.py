from pathlib import Path
import tensorflow as tf

MODEL_DIR = Path("models")
keras_model_path = MODEL_DIR / "har_baseline.keras"
tflite_model_path = MODEL_DIR / "har_baseline.tflite"

model = tf.keras.models.load_model(keras_model_path)

converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

tflite_model_path.write_bytes(tflite_model)

print("Saved TFLite model to:", tflite_model_path)
print("TFLite model size:", tflite_model_path.stat().st_size / 1024, "KB")