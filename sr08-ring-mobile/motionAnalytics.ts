/**
 * Motion, Walking, Steps, Moves, and Sleep Analysis Engine
 * Processes raw 3-axis accelerometer data streamed from DA14585 Smart Ring via BLE.
 */

export type ActivityState = 'sleeping' | 'resting' | 'moving' | 'walking';
export type SleepStage = 'awake' | 'light' | 'deep' | 'restless';

export interface RawAccelSample {
  x: number;
  y: number;
  z: number;
  seq: number;
  timestamp: number;
}

export interface MotionMetrics {
  // Raw & processed values
  latestSample: RawAccelSample | null;
  magnitude: number;
  motionDelta: number;
  packetsReceived: number;
  sampleRateHz: number;

  // Walking & Steps
  steps: number;
  isWalking: boolean;
  cadenceSPM: number;
  walkingPace: 'idle' | 'slow' | 'moderate' | 'brisk';
  distanceKm: number;
  caloriesKcal: number;
  walkingDurationSeconds: number;

  // Moves & Activity
  activityState: ActivityState;
  movesCount: number;
  activeMinutes: number;
  restingMinutes: number;
  motionIntensityPercent: number;

  // Sleep Time & Tracking
  isSleeping: boolean;
  sleepMinutes: number;
  sleepStage: SleepStage;
  sleepScore: number;
  restlessEvents: number;
  deepSleepMinutes: number;
  lightSleepMinutes: number;
  sleepSessionStart: Date | null;
}

export class MotionAnalyticsEngine {
  private prevSample: RawAccelSample | null = null;
  private sampleHistory: RawAccelSample[] = [];
  private deltaHistory: number[] = [];
  private maxHistoryLen = 20;

  // Packets & Rate
  private packetsCount = 0;
  private lastRateCheckTime = Date.now();
  private packetsSinceLastCheck = 0;
  private currentSampleRateHz = 10;

  // Steps detection
  private stepCount = 0;
  private lastStepTimestamp = 0;
  private stepPeakDetected = false;
  private stepCandidateCount = 0;
  private stepIntervals: number[] = [];
  private currentCadenceSPM = 0;
  private isWalking = false;
  private walkingSeconds = 0;
  private lastWalkingActiveTime = 0;

  // Move tracking
  private movesCount = 0;
  private activeSamples = 0;
  private restingSamples = 0;
  private currentActivityState: ActivityState = 'resting';
  private wasActiveLastSample = false;

  // Sleep tracking
  private inactivityDurationMs = 0;
  private continuousStillnessMs = 0;
  private isSleeping = false;
  private sleepStartTimestamp = 0;
  private totalSleepMs = 0;
  private deepSleepMs = 0;
  private lightSleepMs = 0;
  private restlessCount = 0;
  private currentSleepStage: SleepStage = 'awake';
  private restlessStartTime = 0;

  // Threshold constants (calibrated for ADXL362 raw LSBs at 100ms / 10Hz)
  private readonly STILLNESS_THRESHOLD = 20;       // Extreme stillness (sleep candidate)
  private readonly MOTION_THRESHOLD = 35;          // Distinguishes still vs hand move
  private readonly STEP_PEAK_THRESHOLD = 160;      // Step acceleration peak
  private readonly STEP_RELEASE_THRESHOLD = 60;    // Step release threshold
  private readonly STEP_MIN_INTERVAL_MS = 260;     // Max ~230 SPM (sprint)
  private readonly STEP_MAX_INTERVAL_MS = 1400;    // Min ~42 SPM (slow stroll)
  private readonly SLEEP_ONSET_DELAY_MS = 180000;  // 3 minutes of stillness -> enter sleep

  public resetAll(): void {
    this.prevSample = null;
    this.sampleHistory = [];
    this.deltaHistory = [];
    this.packetsCount = 0;
    this.stepCount = 0;
    this.lastStepTimestamp = 0;
    this.stepPeakDetected = false;
    this.stepCandidateCount = 0;
    this.stepIntervals = [];
    this.currentCadenceSPM = 0;
    this.isWalking = false;
    this.walkingSeconds = 0;
    this.movesCount = 0;
    this.activeSamples = 0;
    this.restingSamples = 0;
    this.currentActivityState = 'resting';
    this.inactivityDurationMs = 0;
    this.continuousStillnessMs = 0;
    this.isSleeping = false;
    this.sleepStartTimestamp = 0;
    this.totalSleepMs = 0;
    this.deepSleepMs = 0;
    this.lightSleepMs = 0;
    this.restlessCount = 0;
    this.currentSleepStage = 'awake';
  }

  public resetSteps(): void {
    this.stepCount = 0;
    this.walkingSeconds = 0;
    this.stepCandidateCount = 0;
    this.stepIntervals = [];
    this.currentCadenceSPM = 0;
    this.isWalking = false;
  }

  public resetSleep(): void {
    this.inactivityDurationMs = 0;
    this.continuousStillnessMs = 0;
    this.isSleeping = false;
    this.sleepStartTimestamp = 0;
    this.totalSleepMs = 0;
    this.deepSleepMs = 0;
    this.lightSleepMs = 0;
    this.restlessCount = 0;
    this.currentSleepStage = 'awake';
  }

  public resetMoves(): void {
    this.movesCount = 0;
    this.activeSamples = 0;
    this.restingSamples = 0;
  }

  /**
   * Process a single accelerometer sample (called for every BLE notification).
   */
  public processSample(sample: RawAccelSample): MotionMetrics {
    this.packetsCount++;
    this.packetsSinceLastCheck++;

    const now = sample.timestamp || Date.now();

    // Sample rate calculation (updated once per second)
    const timeSinceRateCheck = now - this.lastRateCheckTime;
    if (timeSinceRateCheck >= 1000) {
      this.currentSampleRateHz = Math.round(
        (this.packetsSinceLastCheck * 1000) / timeSinceRateCheck
      );
      this.packetsSinceLastCheck = 0;
      this.lastRateCheckTime = now;
    }

    // 1. Magnitude: sqrt(x^2 + y^2 + z^2)
    const mag = Math.round(
      Math.sqrt(sample.x * sample.x + sample.y * sample.y + sample.z * sample.z)
    );

    // 2. Motion Delta: sum of absolute axis differences from previous sample
    let delta = 0;
    if (this.prevSample) {
      delta =
        Math.abs(sample.x - this.prevSample.x) +
        Math.abs(sample.y - this.prevSample.y) +
        Math.abs(sample.z - this.prevSample.z);
    }

    // Update history
    this.sampleHistory.push(sample);
    if (this.sampleHistory.length > this.maxHistoryLen) {
      this.sampleHistory.shift();
    }

    this.deltaHistory.push(delta);
    if (this.deltaHistory.length > this.maxHistoryLen) {
      this.deltaHistory.shift();
    }

    // Calculate rolling average delta over last 5 samples (~500 ms)
    const recentDeltas = this.deltaHistory.slice(-5);
    const avgDelta =
      recentDeltas.reduce((acc, d) => acc + d, 0) / (recentDeltas.length || 1);

    // 3. STEP DETECTION ALGORITHM
    const timeSinceLastStep = now - this.lastStepTimestamp;

    if (!this.stepPeakDetected) {
      if (
        delta >= this.STEP_PEAK_THRESHOLD &&
        timeSinceLastStep >= this.STEP_MIN_INTERVAL_MS
      ) {
        this.stepPeakDetected = true;

        if (
          this.lastStepTimestamp > 0 &&
          timeSinceLastStep <= this.STEP_MAX_INTERVAL_MS
        ) {
          // Valid human walking interval
          this.stepIntervals.push(timeSinceLastStep);
          if (this.stepIntervals.length > 5) {
            this.stepIntervals.shift();
          }

          const avgIntervalMs =
            this.stepIntervals.reduce((a, b) => a + b, 0) /
            this.stepIntervals.length;
          this.currentCadenceSPM = Math.round(60000 / avgIntervalMs);

          // Rhythmic verification: require at least 2 steps within cadence window
          if (this.stepCandidateCount < 2) {
            this.stepCandidateCount++;
            if (this.stepCandidateCount === 2) {
              // Confirmed walking! Count the candidate steps
              this.stepCount += 2;
              this.isWalking = true;
              this.lastWalkingActiveTime = now;
            }
          } else {
            // Consecutive confirmed steps
            this.stepCount++;
            this.isWalking = true;
            this.lastWalkingActiveTime = now;
          }
        } else {
          // First step or broken cadence
          this.stepCandidateCount = 1;
          this.stepIntervals = [];
        }

        this.lastStepTimestamp = now;
      }
    } else {
      // Waiting for signal to fall below release threshold before next step
      if (delta <= this.STEP_RELEASE_THRESHOLD) {
        this.stepPeakDetected = false;
      }
    }

    // Walking timeout: if no step detected for 2 seconds, stop walking status
    if (this.isWalking && now - this.lastWalkingActiveTime > 2000) {
      this.isWalking = false;
      this.currentCadenceSPM = 0;
      this.stepCandidateCount = 0;
    } else if (this.isWalking) {
      this.walkingSeconds += 0.1; // ~100ms per sample
    }

    // 4. MOVES & ACTIVITY TRACKING
    const isMotionActive = delta >= this.MOTION_THRESHOLD;

    if (isMotionActive) {
      this.activeSamples++;
      if (!this.wasActiveLastSample) {
        this.movesCount++;
        this.wasActiveLastSample = true;
      }
    } else {
      this.restingSamples++;
      if (delta < this.STILLNESS_THRESHOLD) {
        this.wasActiveLastSample = false;
      }
    }

    // 5. SLEEP TIME & STATE ANALYSIS
    const isStill = avgDelta < this.STILLNESS_THRESHOLD;

    if (isStill) {
      this.inactivityDurationMs += 100; // 100 ms per sample
      this.continuousStillnessMs += 100;

      if (!this.isSleeping) {
        if (this.inactivityDurationMs >= this.SLEEP_ONSET_DELAY_MS) {
          this.isSleeping = true;
          this.sleepStartTimestamp = now - this.SLEEP_ONSET_DELAY_MS;
          this.totalSleepMs += this.SLEEP_ONSET_DELAY_MS;
          this.currentSleepStage = 'light';
        }
      } else {
        // While sleeping
        this.totalSleepMs += 100;

        // Classify deep vs light sleep: sustained stillness > 10 minutes = deep
        if (this.continuousStillnessMs > 600000 && avgDelta < 12) {
          this.deepSleepMs += 100;
          this.currentSleepStage = 'deep';
        } else {
          this.lightSleepMs += 100;
          this.currentSleepStage = 'light';
        }
      }
    } else {
      // Movement detected
      this.continuousStillnessMs = 0;

      if (this.isSleeping) {
        if (delta >= this.STEP_PEAK_THRESHOLD || avgDelta >= this.MOTION_THRESHOLD * 2) {
          // Significant active movement while in sleep session
          if (this.restlessStartTime === 0) {
            this.restlessStartTime = now;
            this.restlessCount++;
            this.currentSleepStage = 'restless';
          } else if (now - this.restlessStartTime > 60000) {
            // Sustained active movement for over 1 minute -> Wake up!
            this.isSleeping = false;
            this.currentSleepStage = 'awake';
            this.inactivityDurationMs = 0;
            this.restlessStartTime = 0;
          }
        } else {
          // Minor micro-twitch, remains sleeping
          this.totalSleepMs += 100;
          this.lightSleepMs += 100;
          this.restlessStartTime = 0;
        }
      } else {
        this.inactivityDurationMs = 0;
        this.currentSleepStage = 'awake';
      }
    }

    // Determine overall activity state
    if (this.isSleeping) {
      this.currentActivityState = 'sleeping';
    } else if (this.isWalking) {
      this.currentActivityState = 'walking';
    } else if (isMotionActive) {
      this.currentActivityState = 'moving';
    } else {
      this.currentActivityState = 'resting';
    }

    // Walking pace classification
    let walkingPace: 'idle' | 'slow' | 'moderate' | 'brisk' = 'idle';
    if (this.isWalking) {
      if (this.currentCadenceSPM > 115) {
        walkingPace = 'brisk';
      } else if (this.currentCadenceSPM >= 85) {
        walkingPace = 'moderate';
      } else {
        walkingPace = 'slow';
      }
    }

    // Motion intensity (0 to 100%)
    const intensity = Math.min(100, Math.round((delta / 300) * 100));

    // Distance: stride length approx 0.762m
    const distanceKm = Number(((this.stepCount * 0.762) / 1000).toFixed(2));

    // Calories: approx 0.04 kcal per step
    const caloriesKcal = Math.round(this.stepCount * 0.04);

    // Active minutes (at 10 Hz, 600 samples = 1 minute)
    const activeMinutes = Math.round((this.activeSamples / 600) * 10) / 10;
    const restingMinutes = Math.round((this.restingSamples / 600) * 10) / 10;

    // Sleep minutes
    const sleepMinutes = Math.round(this.totalSleepMs / 60000);
    const deepSleepMins = Math.round(this.deepSleepMs / 60000);
    const lightSleepMins = Math.round(this.lightSleepMs / 60000);

    // Sleep quality score (0-100) based on duration and restlessness
    let sleepScore = 0;
    if (sleepMinutes > 0) {
      // 8 hours = 480 mins (optimal)
      const durationScore = Math.min(60, (sleepMinutes / 480) * 60);
      const deepRatio = sleepMinutes > 0 ? (deepSleepMins / sleepMinutes) : 0;
      const deepScore = Math.min(25, deepRatio * 50);
      const penalty = Math.min(15, this.restlessCount * 2);
      sleepScore = Math.max(10, Math.min(100, Math.round(durationScore + deepScore + 15 - penalty)));
    }

    this.prevSample = sample;

    return {
      latestSample: sample,
      magnitude: mag,
      motionDelta: delta,
      packetsReceived: this.packetsCount,
      sampleRateHz: this.currentSampleRateHz,

      steps: this.stepCount,
      isWalking: this.isWalking,
      cadenceSPM: this.currentCadenceSPM,
      walkingPace,
      distanceKm,
      caloriesKcal,
      walkingDurationSeconds: Math.round(this.walkingSeconds),

      activityState: this.currentActivityState,
      movesCount: this.movesCount,
      activeMinutes,
      restingMinutes,
      motionIntensityPercent: intensity,

      isSleeping: this.isSleeping,
      sleepMinutes,
      sleepStage: this.currentSleepStage,
      sleepScore,
      restlessEvents: this.restlessCount,
      deepSleepMinutes: deepSleepMins,
      lightSleepMinutes: lightSleepMins,
      sleepSessionStart:
        this.sleepStartTimestamp > 0
          ? new Date(this.sleepStartTimestamp)
          : null,
    };
  }
}

/**
 * Decode base64 BLE characteristic value into raw X, Y, Z, Sequence sample.
 */
export function decodeSensorData(base64Str: string): RawAccelSample | null {
  try {
    const binary = atob(base64Str);
    const bytes: number[] = [];
    for (let i = 0; i < binary.length; i++) {
      bytes.push(binary.charCodeAt(i));
    }

    if (bytes.length < 6) {
      return null;
    }

    // Little-endian 16-bit signed integers
    let x = bytes[0] | (bytes[1] << 8);
    if (x >= 0x8000) x -= 0x10000;

    let y = bytes[2] | (bytes[3] << 8);
    if (y >= 0x8000) y -= 0x10000;

    let z = bytes[4] | (bytes[5] << 8);
    if (z >= 0x8000) z -= 0x10000;

    let seq = 0;
    if (bytes.length >= 8) {
      seq = bytes[6] | (bytes[7] << 8);
    }

    return {
      x,
      y,
      z,
      seq,
      timestamp: Date.now(),
    };
  } catch {
    return null;
  }
}
