from pathlib import Path
import tensorflow as tf

MODEL_DIR = Path("models")
keras_model_path = MODEL_DIR / "har_baseline.keras"
quant_model_path = MODEL_DIR / "har_baseline_dynamic_quant.tflite"

model = tf.keras.models.load_model(keras_model_path)

converter = tf.lite.TFLiteConverter.from_keras_model(model)

# Dynamic range quantization
converter.optimizations = [tf.lite.Optimize.DEFAULT]

quant_tflite_model = converter.convert()
quant_model_path.write_bytes(quant_tflite_model)

print("Saved quantized TFLite model to:", quant_model_path)
print("Quantized model size:", quant_model_path.stat().st_size / 1024, "KB")