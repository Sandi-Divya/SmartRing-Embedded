from pathlib import Path
import numpy as np
import tensorflow as tf
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import classification_report, accuracy_score
import joblib

DATA_DIR = Path("data/UCI HAR Dataset")
MODEL_DIR = Path("models")
MODEL_DIR.mkdir(exist_ok=True)

# Load data
X_train = np.loadtxt(DATA_DIR / "train/X_train.txt")
y_train = np.loadtxt(DATA_DIR / "train/y_train.txt", dtype=int) - 1

X_test = np.loadtxt(DATA_DIR / "test/X_test.txt")
y_test = np.loadtxt(DATA_DIR / "test/y_test.txt", dtype=int) - 1

# Normalize features
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

joblib.dump(scaler, MODEL_DIR / "scaler.joblib")

num_features = X_train.shape[1]
num_classes = 6

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(num_features,)),
    tf.keras.layers.Dense(128, activation="relu"),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(64, activation="relu"),
    tf.keras.layers.Dense(num_classes, activation="softmax"),
])

model.compile(
    optimizer="adam",
    loss="sparse_categorical_crossentropy",
    metrics=["accuracy"],
)

history = model.fit(
    X_train,
    y_train,
    validation_split=0.2,
    epochs=20,
    batch_size=64,
    verbose=1,
)

test_probs = model.predict(X_test)
y_pred = np.argmax(test_probs, axis=1)

print("\nTest accuracy:", accuracy_score(y_test, y_pred))
print("\nClassification report:")
print(classification_report(
    y_test,
    y_pred,
    target_names=[
        "WALKING",
        "WALKING_UPSTAIRS",
        "WALKING_DOWNSTAIRS",
        "SITTING",
        "STANDING",
        "LAYING",
    ],
))

model.save(MODEL_DIR / "har_baseline.keras")
print("\nSaved model to:", MODEL_DIR / "har_baseline.keras")