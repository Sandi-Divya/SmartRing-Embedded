# Real-Time On-Device Human Activity Recognition with TensorFlow Lite

This project explores on-device human activity recognition using smartphone sensor data and TensorFlow Lite.

## Dataset

Initial experiments use the UCI HAR (Human Activity Recognition) dataset.

### Activities
- WALKING
- WALKING_UPSTAIRS
- WALKING_DOWNSTAIRS
- SITTING
- STANDING
- LAYING

### Dataset Statistics

| Metric | Value |
|---|---:|
| Training samples | 7,352 |
| Test samples | 2,947 |
| Features per sample | 561 |

The dataset contains engineered accelerometer and gyroscope features extracted from smartphone motion signals.

---

## Baseline Model

Model:
- Dense neural network
- Input: 561 engineered sensor features
- Output: 6 activity classes

### Baseline Results

| Metric | Value |
|---|---:|
| Test accuracy | 93.4% |
| FP32 TFLite model size | 316.9 KB |
| Mean latency | 0.0072 ms |
| P95 latency | 0.0080 ms |
| P99 latency | 0.0088 ms |

Note: CPU benchmarks were measured on laptop hardware using TensorFlow Lite runtime.

---

## Quantization

Dynamic range quantization was applied to reduce model size and improve inference speed for edge deployment.

### Quantization Results

| Model | Accuracy | Size | Mean Latency | P95 | P99 |
|---|---:|---:|---:|---:|---:|
| FP32 TFLite | 93.4% | 316.9 KB | 0.0072 ms | 0.0080 ms | 0.0088 ms |
| Dynamic Quantized TFLite | 93.4% | 84.9 KB | 0.0026 ms | 0.0028 ms | 0.0037 ms |

Quantization reduced model size by approximately 73% while preserving accuracy.

---

## Android Deployment

The quantized TensorFlow Lite model was deployed into a native Android application using the TensorFlow Lite Android runtime.

### Android Inference

| Device | Inference Latency |
|---|---:|
| Pixel 5 Android Emulator | ~1.27 ms |

The Android application successfully:
- Loaded the quantized `.tflite` model
- Executed on-device inference
- Returned predicted activities and confidence scores
- Measured inference latency directly on Android

---

## Real-Time Embedded Inference Pipeline

```text
Phone Accelerometer + Gyroscope
                ↓
Live Sensor Streaming
                ↓
128×6 Sliding Temporal Window
                ↓
Normalization
                ↓
Conv1D Temporal Activity Model
                ↓
TensorFlow Lite Conversion
                ↓
Dynamic Quantization
                ↓
On-Device Android Inference
                ↓
Real-Time Activity Prediction
```
---

## Raw Sensor Window Model

To support real-time mobile inference directly from phone sensors, a second model was trained using raw inertial windows instead of handcrafted features.

### Model Architecture
- Input: 128×6 temporal sensor window
- Channels:
  - Accelerometer x/y/z
  - Gyroscope x/y/z
- Conv1D temporal neural network
- Output: 6 activity classes

### Raw-Window Results

| Model | Accuracy | Size |
|---|---:|---:|
| Raw Conv1D FP32 | 91.6% | ~565 KB |
| Raw Conv1D Quantized | 91.5% | ~152 KB |

This model is directly compatible with live Android sensor streaming and supports fully on-device real-time inference.

---

## Live Android Sensor Inference

The Android application was extended to support:

- Real-time accelerometer and gyroscope streaming
- Sliding-window buffering
- Live TensorFlow Lite inference
- Real-time activity prediction directly on device

The system performs fully on-device inference without requiring a server connection.

---

## Future Work
- Personalized activity adaptation
- Battery and memory profiling
- Edge AI optimization experiments
- Comparison between FP32 and INT8 quantization
