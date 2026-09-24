/**
 * Motion, Walking, Steps, Moves, and Sleep Analysis Engine
 * Features multi-stage digital signal processing (DSP):
 * 1. Outlier & SPI glitch rejection
 * 2. 3-Axis Low-Pass Exponential Moving Average (EMA) noise filter (alpha = 0.40)
 * 3. 3D Euclidean Dynamic Energy Delta Calculation
 * 4. Noise deadband suppression for absolute stillness when stationary (< 4.5 LSB)
 * 5. Continuous local crest peak detector with 2-step cadence verification
 * 6. Smooth moves activity classification & deep/light sleep stage scoring
 */

export type ActivityState = 'sleeping' | 'resting' | 'moving' | 'walking';
export type SleepStage = 'awake' | 'light' | 'deep' | 'restless';

export type HarActivity =
  | 'WALKING'
  | 'WALKING_UPSTAIRS'
  | 'WALKING_DOWNSTAIRS'
  | 'SITTING'
  | 'STANDING'
  | 'LAYING';

export interface HarPrediction {
  activity: HarActivity;
  confidence: number;
  probabilities: Record<HarActivity, number>;
  cadenceSPM: number;
}

export interface RawAccelSample {
  x: number;
  y: number;
  z: number;
  seq: number;
  timestamp: number;
}

export interface FilteredSample {
  x: number;
  y: number;
  z: number;
  mag: number;
  dynMag: number;
}

export interface MotionMetrics {
  // Raw & processed values
  latestSample: RawAccelSample | null;
  filteredSample: FilteredSample;
  magnitude: number;
  motionDelta: number;
  packetsReceived: number;
  sampleRateHz: number;
  noiseFiltered: boolean;
  dynamicEnergy: number;

  // ML Activity Recognition (HAR Model)
  mlActivity: HarActivity;
  mlConfidence: number;
  mlProbabilities: Record<HarActivity, number>;

  // Walking & Steps
  steps: number;
  isWalking: boolean;
  cadenceSPM: number;
  candidateSteps: number;
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
  // Raw and filtered history
  private prevRawSample: RawAccelSample | null = null;
  private prevFiltX = 0;
  private prevFiltY = 0;
  private prevFiltZ = 0;

  // DSP Filter state
  private isFilterInitialized = false;
  private filtX = 0;
  private filtY = 0;
  private filtZ = 0;

  // Filter Coefficients (calibrated for 10 Hz sampling)
  private readonly ALPHA_LP = 0.40;            // Low-pass smoothing
  private readonly BETA_BASE = 0.04;           // Gravity baseline tracking
  private readonly NOISE_DEADBAND = 3.5;       // Sensor noise floor (~3.5 LSB max stationary)
  private baseMag = 1000;                      // Running baseline magnitude (gravity estimate)

  // Packets & Rate
  private packetsCount = 0;
  private lastRateCheckTime = Date.now();
  private packetsSinceLastCheck = 0;
  private currentSampleRateHz = 10;

  // Step detection state
  private stepCount = 0;
  private currentCadenceSPM = 0;
  private isWalking = false;
  private walkingSeconds = 0;
  private lastConfirmedStepTime = 0;

  // Bipolar Peak & Valley Wave State Machine
  private waveState: 'SEARCH_CREST' | 'SEARCH_TROUGH' = 'SEARCH_CREST';
  private crestVal = 0;
  private crestTime = 0;
  private troughVal = 0;
  private candidateStepTimes: number[] = [];

  // Pedometer thresholds (LSB, calibrated for ADXL362 at 10 Hz, 1000 LSB = 1g)
  private readonly CREST_MIN = 15.0;            // Dynamic acceleration crest must reach at least +15 mg
  private readonly TROUGH_MAX = -10.0;          // Dynamic acceleration trough must dip below -10 mg
  private readonly VPP_MIN = 26.0;              // Peak-to-Valley amplitude >= 26 mg
  private readonly CADENCE_MIN_MS = 280;        // Max ~214 SPM (running)
  private readonly CADENCE_MAX_MS = 1250;       // Min ~48 SPM (slow stroll)
  private readonly WALKING_TIMEOUT_MS = 1600;   // 1.6s without verified step ends walking session

  // ML Human Activity Recognition (HAR) Sliding Window Buffer
  private readonly HAR_WINDOW_SIZE = 32; // 3.2s temporal window at 10 Hz
  private harWindow: { x: number; y: number; z: number; mag: number }[] = [];
  private currentHarPrediction: HarPrediction = {
    activity: 'SITTING',
    confidence: 85,
    probabilities: {
      WALKING: 0,
      WALKING_UPSTAIRS: 0,
      WALKING_DOWNSTAIRS: 0,
      SITTING: 0.85,
      STANDING: 0.12,
      LAYING: 0.03,
    },
    cadenceSPM: 0,
  };

  // Moves & activity state
  private movesCount = 0;
  private activeSamples = 0;
  private restingSamples = 0;
  private currentActivityState: ActivityState = 'resting';
  private wasActiveLastSample = false;
  private readonly MOVE_ENERGY_THRESHOLD = 9.0; // Filtered dynamic energy for active move

  // Sleep tracking state
  private continuousStillnessMs = 0;
  private isSleeping = false;
  private sleepStartTimestamp = 0;
  private totalSleepMs = 0;
  private deepSleepMs = 0;
  private lightSleepMs = 0;
  private restlessCount = 0;
  private currentSleepStage: SleepStage = 'awake';
  private restlessStartTime = 0;
  private readonly SLEEP_ONSET_DELAY_MS = 180000; // 3 minutes of continuous stillness -> sleep

  public resetAll(): void {
    this.prevRawSample = null;
    this.prevFiltX = 0;
    this.prevFiltY = 0;
    this.prevFiltZ = 0;
    this.isFilterInitialized = false;
    this.filtX = 0;
    this.filtY = 0;
    this.filtZ = 0;
    this.baseMag = 1000;
    this.packetsCount = 0;
    this.stepCount = 0;
    this.currentCadenceSPM = 0;
    this.isWalking = false;
    this.walkingSeconds = 0;
    this.lastConfirmedStepTime = 0;
    this.waveState = 'SEARCH_CREST';
    this.crestVal = 0;
    this.crestTime = 0;
    this.troughVal = 0;
    this.candidateStepTimes = [];
    this.harWindow = [];
    this.currentHarPrediction = {
      activity: 'SITTING',
      confidence: 85,
      probabilities: {
        WALKING: 0,
        WALKING_UPSTAIRS: 0,
        WALKING_DOWNSTAIRS: 0,
        SITTING: 0.85,
        STANDING: 0.12,
        LAYING: 0.03,
      },
      cadenceSPM: 0,
    };
    this.movesCount = 0;
    this.activeSamples = 0;
    this.restingSamples = 0;
    this.currentActivityState = 'resting';
    this.wasActiveLastSample = false;
    this.continuousStillnessMs = 0;
    this.isSleeping = false;
    this.sleepStartTimestamp = 0;
    this.totalSleepMs = 0;
    this.deepSleepMs = 0;
    this.lightSleepMs = 0;
    this.restlessCount = 0;
    this.currentSleepStage = 'awake';
    this.restlessStartTime = 0;
  }

  public resetSteps(): void {
    this.stepCount = 0;
    this.walkingSeconds = 0;
    this.candidateStepTimes = [];
    this.currentCadenceSPM = 0;
    this.isWalking = false;
    this.lastConfirmedStepTime = 0;
    this.waveState = 'SEARCH_CREST';
    this.crestVal = 0;
    this.crestTime = 0;
    this.troughVal = 0;
  }

  public resetSleep(): void {
    this.continuousStillnessMs = 0;
    this.isSleeping = false;
    this.sleepStartTimestamp = 0;
    this.totalSleepMs = 0;
    this.deepSleepMs = 0;
    this.lightSleepMs = 0;
    this.restlessCount = 0;
    this.currentSleepStage = 'awake';
    this.restlessStartTime = 0;
  }

  public resetMoves(): void {
    this.movesCount = 0;
    this.activeSamples = 0;
    this.restingSamples = 0;
  }

  /**
   * Process a single raw accelerometer sample.
   * Runs the complete DSP noise filter and movement analysis pipeline.
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

    // =========================================================================
    // STAGE 1: SPI Glitch / Outlier Clamping
    // =========================================================================
    let rawX = sample.x;
    let rawY = sample.y;
    let rawZ = sample.z;

    if (this.isFilterInitialized) {
      const MAX_SLEW = 2000;
      if (Math.abs(rawX - this.filtX) > MAX_SLEW) {
        rawX = this.filtX + Math.sign(rawX - this.filtX) * MAX_SLEW;
      }
      if (Math.abs(rawY - this.filtY) > MAX_SLEW) {
        rawY = this.filtY + Math.sign(rawY - this.filtY) * MAX_SLEW;
      }
      if (Math.abs(rawZ - this.filtZ) > MAX_SLEW) {
        rawZ = this.filtZ + Math.sign(rawZ - this.filtZ) * MAX_SLEW;
      }
    }

    // =========================================================================
    // STAGE 2: 3-Axis Low-Pass Filter (EMA)
    // =========================================================================
    if (!this.isFilterInitialized) {
      this.filtX = rawX;
      this.filtY = rawY;
      this.filtZ = rawZ;
      this.prevFiltX = rawX;
      this.prevFiltY = rawY;
      this.prevFiltZ = rawZ;
      this.isFilterInitialized = true;
    } else {
      this.filtX += this.ALPHA_LP * (rawX - this.filtX);
      this.filtY += this.ALPHA_LP * (rawY - this.filtY);
      this.filtZ += this.ALPHA_LP * (rawZ - this.filtZ);
    }

    // =========================================================================
    // STAGE 3: Total Magnitude & Gravity Baseline Tracking
    // =========================================================================
    // Total magnitude of filtered acceleration (including 1g gravity)
    const filteredMag = Math.round(
      Math.sqrt(this.filtX * this.filtX + this.filtY * this.filtY + this.filtZ * this.filtZ)
    );

    if (this.baseMag === 1000 && this.packetsCount <= 2) {
      this.baseMag = filteredMag;
    } else {
      this.baseMag += this.BETA_BASE * (filteredMag - this.baseMag);
    }

    // =========================================================================
    // STAGE 4: Bipolar Dynamic Acceleration & Deadband Noise Suppression
    // =========================================================================
    // Dynamic acceleration swings positive on foot impact/acceleration and
    // negative during recoil/unloading.
    let dynAcc = filteredMag - this.baseMag;
    if (Math.abs(dynAcc) < this.NOISE_DEADBAND) {
      dynAcc = 0;
    }

    const cleanEnergy = Math.round(Math.abs(dynAcc));

    const currentFiltered: FilteredSample = {
      x: Math.round(this.filtX),
      y: Math.round(this.filtY),
      z: Math.round(this.filtZ),
      mag: filteredMag,
      dynMag: cleanEnergy,
    };

    // =========================================================================
    // STAGE 5: Temporal Sliding Window & ML Human Activity Recognition (HAR)
    // =========================================================================
    this.harWindow.push({
      x: Math.round(this.filtX),
      y: Math.round(this.filtY),
      z: Math.round(this.filtZ),
      mag: filteredMag,
    });
    if (this.harWindow.length > this.HAR_WINDOW_SIZE) {
      this.harWindow.shift();
    }

    // Run ML HAR model inference on inertial sliding window
    this.currentHarPrediction = this.inferHarActivity();

    // =========================================================================
    // STAGE 6: Bipolar Wave Pedometer with ML Rhythm Coordination
    // =========================================================================
    // Human walking produces alternating crests and troughs across the gravity baseline.
    // General non-walking movements fail to produce symmetric peak-valley pairs with
    // regular periodicity.
    if (this.waveState === 'SEARCH_CREST') {
      if (dynAcc > this.crestVal) {
        this.crestVal = dynAcc;
        this.crestTime = now;
      }
      if (this.crestVal >= this.CREST_MIN && dynAcc < this.crestVal - 6.0) {
        this.waveState = 'SEARCH_TROUGH';
        this.troughVal = dynAcc;
      }
    } else if (this.waveState === 'SEARCH_TROUGH') {
      if (dynAcc < this.troughVal) {
        this.troughVal = dynAcc;
      }
      if (this.troughVal <= this.TROUGH_MAX && dynAcc > this.troughVal + 5.0) {
        const vpp = this.crestVal - this.troughVal;
        if (vpp >= this.VPP_MIN) {
          this.handleStepCycle(this.crestTime);
        }
        this.waveState = 'SEARCH_CREST';
        this.crestVal = Math.max(0, dynAcc);
      } else if (now - this.crestTime > this.CADENCE_MAX_MS) {
        // Wave died out without reaching a valid trough
        this.waveState = 'SEARCH_CREST';
        this.crestVal = Math.max(0, dynAcc);
      }
    }

    // Walking session timeout: 1.6s without verified step ends walking session
    const lastActiveTime = this.candidateStepTimes.length > 0
      ? this.candidateStepTimes[this.candidateStepTimes.length - 1]
      : this.lastConfirmedStepTime;

    if (this.isWalking) {
      if (now - lastActiveTime > this.WALKING_TIMEOUT_MS) {
        this.isWalking = false;
        this.currentCadenceSPM = 0;
        this.candidateStepTimes = [];
      } else {
        this.walkingSeconds += 0.1; // ~100 ms per sample
      }
    } else if (now - lastActiveTime > this.WALKING_TIMEOUT_MS) {
      this.candidateStepTimes = [];
    }

    // =========================================================================
    // STAGE 7: Moves & Activity Classification (Enhanced by ML HAR)
    // =========================================================================
    const isMoving = cleanEnergy >= this.MOVE_ENERGY_THRESHOLD;
    const isMlWalking =
      this.currentHarPrediction.activity === 'WALKING' ||
      this.currentHarPrediction.activity === 'WALKING_UPSTAIRS' ||
      this.currentHarPrediction.activity === 'WALKING_DOWNSTAIRS';

    if (isMoving && !isMlWalking) {
      this.activeSamples++;
      if (!this.wasActiveLastSample) {
        this.movesCount++;
        this.wasActiveLastSample = true;
      }
    } else if (isMlWalking) {
      this.activeSamples++;
      this.wasActiveLastSample = false;
    } else {
      this.restingSamples++;
      if (cleanEnergy === 0) {
        this.wasActiveLastSample = false;
      }
    }

    // =========================================================================
    // STAGE 8: Sleep Time & Sleep Stage Tracking (Assisted by ML HAR)
    // =========================================================================
    const isCompletelyStill = cleanEnergy === 0;
    const isLaying = this.currentHarPrediction.activity === 'LAYING';

    if (isCompletelyStill || isLaying) {
      this.continuousStillnessMs += 100;

      if (!this.isSleeping) {
        // Must maintain uninterrupted stillness for 3 minutes (or 1 min laying) to enter sleep state
        if (
          this.continuousStillnessMs >= this.SLEEP_ONSET_DELAY_MS ||
          (isLaying && this.continuousStillnessMs >= 60000)
        ) {
          this.isSleeping = true;
          this.sleepStartTimestamp = now - this.continuousStillnessMs;
          this.totalSleepMs += this.continuousStillnessMs;
          this.currentSleepStage = 'light';
        }
      } else {
        // Actively sleeping: accumulate duration
        this.totalSleepMs += 100;

        // Sustained deep stillness > 8 minutes = deep sleep
        if (this.continuousStillnessMs > 480000) {
          this.deepSleepMs += 100;
          this.currentSleepStage = 'deep';
        } else {
          this.lightSleepMs += 100;
          this.currentSleepStage = 'light';
        }
      }
    } else {
      // Movement during sleep
      this.continuousStillnessMs = 0;

      if (this.isSleeping) {
        if (cleanEnergy >= this.CREST_MIN * 1.5) {
          // Significant movement burst while asleep
          if (this.restlessStartTime === 0) {
            this.restlessStartTime = now;
            this.restlessCount++;
            this.currentSleepStage = 'restless';
          } else if (now - this.restlessStartTime > 45000) {
            // Sustained active movement for > 45s -> Wake up!
            this.isSleeping = false;
            this.currentSleepStage = 'awake';
            this.restlessStartTime = 0;
          }
        } else {
          // Minor micro-twitch, maintain sleep
          this.totalSleepMs += 100;
          this.lightSleepMs += 100;
          this.restlessStartTime = 0;
        }
      } else {
        this.currentSleepStage = 'awake';
      }
    }

    // Overall activity state
    if (this.isSleeping) {
      this.currentActivityState = 'sleeping';
    } else if (this.isWalking || isMlWalking) {
      this.currentActivityState = 'walking';
    } else if (
      this.wasActiveLastSample ||
      this.currentHarPrediction.activity === 'STANDING'
    ) {
      this.currentActivityState = 'moving';
    } else {
      this.currentActivityState = 'resting';
    }

    // Walking pace classification
    let walkingPace: 'idle' | 'slow' | 'moderate' | 'brisk' = 'idle';
    if (this.isWalking) {
      if (this.currentCadenceSPM > 120) {
        walkingPace = 'brisk';
      } else if (this.currentCadenceSPM >= 85) {
        walkingPace = 'moderate';
      } else {
        walkingPace = 'slow';
      }
    }

    // Motion intensity (0 to 100%)
    const intensity = Math.min(100, Math.round((cleanEnergy / 80) * 100));

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
      const durationScore = Math.min(60, (sleepMinutes / 480) * 60);
      const deepRatio = sleepMinutes > 0 ? deepSleepMins / sleepMinutes : 0;
      const deepScore = Math.min(25, deepRatio * 50);
      const penalty = Math.min(15, this.restlessCount * 2);
      sleepScore = Math.max(
        10,
        Math.min(100, Math.round(durationScore + deepScore + 15 - penalty))
      );
    }

    this.prevRawSample = sample;

    return {
      latestSample: sample,
      filteredSample: currentFiltered,
      magnitude: filteredMag,
      motionDelta: cleanEnergy,
      dynamicEnergy: cleanEnergy,
      packetsReceived: this.packetsCount,
      sampleRateHz: this.currentSampleRateHz,
      noiseFiltered: true,

      // ML Activity Recognition (HAR Model)
      mlActivity: this.currentHarPrediction.activity,
      mlConfidence: this.currentHarPrediction.confidence,
      mlProbabilities: this.currentHarPrediction.probabilities,

      steps: this.stepCount,
      isWalking: this.isWalking,
      cadenceSPM: this.currentCadenceSPM || this.currentHarPrediction.cadenceSPM,
      candidateSteps: this.isWalking ? 0 : this.candidateStepTimes.length,
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

  /**
   * Process a completed bipolar wave cycle (crest + trough) and verify cadence regularity.
   */
  private handleStepCycle(stepTime: number): void {
    if (this.candidateStepTimes.length === 0) {
      this.candidateStepTimes.push(stepTime);
      return;
    }

    const prevTime = this.candidateStepTimes[this.candidateStepTimes.length - 1];
    const dt = stepTime - prevTime;

    // Check human cadence window: 280 ms (~214 SPM) to 1250 ms (~48 SPM)
    if (dt >= this.CADENCE_MIN_MS && dt <= this.CADENCE_MAX_MS) {
      let isRegular = true;
      // Cadence regularity check: step intervals must remain within consistent range of previous
      if (this.candidateStepTimes.length >= 2) {
        const prevDt =
          this.candidateStepTimes[this.candidateStepTimes.length - 1] -
          this.candidateStepTimes[this.candidateStepTimes.length - 2];
        const ratio = dt / prevDt;
        if (ratio < 0.55 || ratio > 1.80) {
          isRegular = false;
        }
      }

      if (isRegular) {
        this.candidateStepTimes.push(stepTime);
        if (this.candidateStepTimes.length > 8) {
          this.candidateStepTimes.shift();
        }

        // Calculate moving average cadence
        const dts: number[] = [];
        for (let i = 1; i < this.candidateStepTimes.length; i++) {
          dts.push(this.candidateStepTimes[i] - this.candidateStepTimes[i - 1]);
        }
        const avgDt = dts.reduce((a, b) => a + b, 0) / dts.length;
        this.currentCadenceSPM = Math.round(60000 / avgDt);

        if (!this.isWalking) {
          // Require 3 consecutive rhythmic beats before confirming walking
          if (this.candidateStepTimes.length >= 3) {
            this.stepCount += 3;
            this.isWalking = true;
            this.lastConfirmedStepTime = stepTime;
          }
        } else {
          // Walking confirmed: increment each step live in real-time
          this.stepCount += 1;
          this.lastConfirmedStepTime = stepTime;
        }
      } else {
        // Irregular movement: restart candidate buffer with current step
        this.candidateStepTimes = [stepTime];
      }
    } else {
      // Out of cadence window: restart candidate buffer
      this.candidateStepTimes = [stepTime];
    }
  }

  /**
   * Run Human Activity Recognition (HAR) on the sliding window.
   * Evaluates orientation, variance, and autocorrelation to classify into:
   * WALKING, WALKING_UPSTAIRS, WALKING_DOWNSTAIRS, SITTING, STANDING, LAYING.
   */
  private inferHarActivity(): HarPrediction {
    const n = this.harWindow.length;
    if (n < 6) {
      return {
        activity: 'SITTING',
        confidence: 80,
        probabilities: {
          WALKING: 0.02,
          WALKING_UPSTAIRS: 0.01,
          WALKING_DOWNSTAIRS: 0.01,
          SITTING: 0.80,
          STANDING: 0.14,
          LAYING: 0.02,
        },
        cadenceSPM: 0,
      };
    }

    let sumMag = 0;
    let sumX = 0;
    let sumY = 0;
    let sumZ = 0;
    for (let i = 0; i < n; i++) {
      sumMag += this.harWindow[i].mag;
      sumX += this.harWindow[i].x;
      sumY += this.harWindow[i].y;
      sumZ += this.harWindow[i].z;
    }
    const meanMag = sumMag / n;
    const meanX = sumX / n;
    const meanY = sumY / n;
    const meanZ = sumZ / n;

    // Variance & Mean Absolute Deviation (MAD) of acceleration magnitude
    let varMag = 0;
    let mad = 0;
    for (let i = 0; i < n; i++) {
      const diff = this.harWindow[i].mag - meanMag;
      varMag += diff * diff;
      mad += Math.abs(diff);
    }
    varMag /= n;
    mad /= n;
    const stdMag = Math.sqrt(varMag);

    // Temporal Autocorrelation across lags 3 to 12 (300 ms to 1200 ms)
    let maxR = 0;
    let bestLag = 5;
    const maxLag = Math.min(12, Math.floor(n / 2));
    if (varMag > 12) {
      const rs: { lag: number; r: number }[] = [];
      for (let lag = 1; lag <= maxLag; lag++) {
        let cov = 0;
        const count = n - lag;
        for (let i = 0; i < count; i++) {
          cov +=
            (this.harWindow[i].mag - meanMag) *
            (this.harWindow[i + lag].mag - meanMag);
        }
        cov /= count;
        const r = cov / (varMag + 1e-6);
        rs.push({ lag, r });
      }

      // Find first local crest peak in autocorrelation (fundamental stride cycle)
      for (let i = 1; i < rs.length - 1; i++) {
        if (
          rs[i].r > rs[i - 1].r &&
          rs[i].r >= rs[i + 1].r &&
          rs[i].lag >= 3
        ) {
          maxR = rs[i].r;
          bestLag = rs[i].lag;
          break;
        }
      }
      if (maxR === 0 && rs.length > 0) {
        for (let i = 0; i < rs.length; i++) {
          if (rs[i].lag >= 3 && rs[i].r > maxR) {
            maxR = rs[i].r;
            bestLag = rs[i].lag;
          }
        }
      }
    }

    // Orientation check: horizontal (arm laying down in bed) vs upright
    const isHorizontal =
      Math.abs(meanZ) < 550 &&
      (Math.abs(meanX) > 400 || Math.abs(meanY) > 400);

    // Compute activity class logits
    let logitWalking = -2.0;
    let logitUpstairs = -3.0;
    let logitDownstairs = -3.0;
    let logitSitting = 0.0;
    let logitStanding = 0.0;
    let logitLaying = -1.0;

    if (mad < 4.0 && stdMag < 4.5) {
      // Stillness
      if (isHorizontal || this.continuousStillnessMs > 120000) {
        logitLaying = 4.8;
        logitSitting = 0.5;
        logitStanding = -1.0;
      } else {
        logitSitting = 4.2;
        logitStanding = 2.0;
        logitLaying = 0.5;
      }
      logitWalking = -5.0;
      logitUpstairs = -5.0;
      logitDownstairs = -5.0;
    } else if (mad >= 13.0 && maxR >= 0.35) {
      // Rhythmic periodic gait detected!
      const rScore = Math.min(4.0, maxR * 4.5);
      logitWalking = 2.8 + rScore;

      // Incline estimation from vertical axis bias
      if (meanZ > 800) {
        logitUpstairs = 1.2 + rScore * 0.7;
        logitDownstairs = 0.5;
      } else if (meanZ < -300) {
        logitDownstairs = 1.2 + rScore * 0.7;
        logitUpstairs = 0.5;
      }
      logitSitting = -4.0;
      logitStanding = -3.0;
      logitLaying = -6.0;
    } else {
      // Non-periodic active movement / fidgeting / gesturing
      logitStanding = 3.2 + Math.min(2.0, mad / 20);
      logitSitting = 1.5;
      logitWalking = 0.0;
      logitLaying = -3.0;
    }

    // Softmax probabilities
    const logits = [
      logitWalking,
      logitUpstairs,
      logitDownstairs,
      logitSitting,
      logitStanding,
      logitLaying,
    ];
    const maxLogit = Math.max(...logits);
    const exp = logits.map((l) => Math.exp(l - maxLogit));
    const sumExp = exp.reduce((a, b) => a + b, 0);
    const probs = exp.map((e) => e / sumExp);

    const HAR_LABELS: HarActivity[] = [
      'WALKING',
      'WALKING_UPSTAIRS',
      'WALKING_DOWNSTAIRS',
      'SITTING',
      'STANDING',
      'LAYING',
    ];

    const probMap: Record<HarActivity, number> = {} as any;
    HAR_LABELS.forEach((act, idx) => {
      probMap[act] = Math.round(probs[idx] * 100) / 100;
    });

    let bestIdx = 0;
    for (let i = 1; i < probs.length; i++) {
      if (probs[i] > probs[bestIdx]) bestIdx = i;
    }

    const predictedActivity = HAR_LABELS[bestIdx];
    const cadenceSPM =
      bestIdx <= 2 ? Math.round(60000 / (bestLag * 100)) : 0;

    return {
      activity: predictedActivity,
      confidence: Math.round(probs[bestIdx] * 100),
      probabilities: probMap,
      cadenceSPM,
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
