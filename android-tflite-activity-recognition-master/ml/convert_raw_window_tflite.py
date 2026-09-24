from pathlib import Path
import tensorflow as tf

MODEL_DIR = Path("models")

keras_model_path = MODEL_DIR / "har_raw_window.keras"
tflite_model_path = MODEL_DIR / "har_raw_window.tflite"
quant_model_path = MODEL_DIR / "har_raw_window_dynamic_quant.tflite"

model = tf.keras.models.load_model(keras_model_path)

# FP32 TFLite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()
tflite_model_path.write_bytes(tflite_model)

print("Saved FP32 raw-window TFLite model to:", tflite_model_path)
print("FP32 size:", tflite_model_path.stat().st_size / 1024, "KB")

# Dynamic quantized TFLite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
quant_model = converter.convert()
quant_model_path.write_bytes(quant_model)

print("Saved quantized raw-window TFLite model to:", quant_model_path)
print("Quantized size:", quant_model_path.stat().st_size / 1024, "KB")