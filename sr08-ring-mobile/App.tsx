import { StatusBar } from 'expo-status-bar';
import { useEffect, useMemo, useRef, useState } from 'react';
import {
  Alert,
  PermissionsAndroid,
  Platform,
  Pressable,
  SafeAreaView,
  ScrollView,
  StyleSheet,
  Text,
  View,
} from 'react-native';
import { BleManager, Device, State } from 'react-native-ble-plx';

const RING_NAME = 'DLG-PRPH';
const RING_MAC_PREFIX = '48:23:35';

const DATA_SERVICE =
  '18424398-7cbc-11e9-8f9e-2a86e4085a59';

const TELEMETRY_CHARACTERISTIC =
  '15005991-b131-3396-014c-664c9867b917';

const HEART_RATE_SERVICE =
  '184247d0-7cbc-11e9-089e-2a86e4085a59';

const HEART_RATE_CHARACTERISTIC =
  '16005991-b131-3396-014c-664c9867b919';

type HeartRateReading = {
  value: number;
  timestamp: number;
};

const MAX_HR_READINGS = 240;

function decodeBase64(value: string): number[] {
  try {
    const binary = atob(value);
    const bytes: number[] = [];

    for (let i = 0; i < binary.length; i++) {
      bytes.push(binary.charCodeAt(i));
    }

    return bytes;
  } catch {
    return [];
  }
}

function decodeTelemetry(value: string): number | null {
  const bytes = decodeBase64(value);

  if (bytes.length === 0) {
    return null;
  }

  const text = String.fromCharCode(...bytes).trim();

  if (/^\d{1,3}$/.test(text)) {
    const number = parseInt(text, 10);

    if (number >= 0 && number <= 100) {
      return number;
    }
  }

  const raw = bytes[0];

  if (raw >= 0 && raw <= 100) {
    return raw;
  }

  return null;
}

function decodeBattery(value: string): number | null {
  const bytes = decodeBase64(value);

  if (bytes.length === 0) {
    return null;
  }

  const text = String.fromCharCode(...bytes).trim();

  if (/^\d{1,3}$/.test(text)) {
    const number = parseInt(text, 10);

    if (number >= 0 && number <= 100) {
      return number;
    }
  }

  const raw = bytes[0];

  if (raw >= 0 && raw <= 100) {
    return raw;
  }

  return null;
}

function decodeHeartRate(value: string): number | null {
  const bytes = decodeBase64(value);

  if (bytes.length === 0) {
    return null;
  }

  const text = String.fromCharCode(...bytes).trim();

  if (/^\d{1,3}$/.test(text)) {
    const number = parseInt(text, 10);

    if (number > 0 && number <= 250) {
      return number;
    }
  }

  const raw = bytes[0];

  if (raw > 0 && raw <= 250) {
    return raw;
  }

  return null;
}

function getHeartRateStatus(
  value: number | null
): {
  label: string;
  description: string;
  level:
    | 'normal'
    | 'elevated'
    | 'high'
    | 'veryHigh'
    | 'low'
    | 'veryLow';
} {
  if (value === null) {
    return {
      label: 'Waiting',
      description: 'Waiting for heart rate data.',
      level: 'normal',
    };
  }

  if (value < 40) {
    return {
      label: 'Very low',
      description:
        'Heart rate is below the low range. If this is persistent or accompanied by symptoms, seek medical attention.',
      level: 'veryLow',
    };
  }

  if (value < 50) {
    return {
      label: 'Low',
      description:
        'Heart rate is below the typical resting range.',
      level: 'low',
    };
  }

  if (value <= 100) {
    return {
      label: 'Normal',
      description:
        'Heart rate is within the typical resting range.',
      level: 'normal',
    };
  }

  if (value <= 120) {
    return {
      label: 'Elevated',
      description:
        'Heart rate is above the typical resting range.',
      level: 'elevated',
    };
  }

  if (value <= 150) {
    return {
      label: 'High',
      description:
        'Heart rate is considerably elevated.',
      level: 'high',
    };
  }

  return {
    label: 'Very high',
    description:
      'Heart rate is very high. If this occurs at rest or with concerning symptoms, seek medical attention.',
    level: 'veryHigh',
  };
}

export default function App() {
  const managerRef = useRef(new BleManager());

  const [device, setDevice] = useState<Device | null>(null);
  const [status, setStatus] = useState('Disconnected');

  const [battery, setBattery] = useState<number | null>(null);
  const [heartRate, setHeartRate] = useState<number | null>(null);
  const [telemetry, setTelemetry] = useState<string | null>(null);

  const [isDarkMode, setIsDarkMode] = useState(false);

  const [activeTab, setActiveTab] =
    useState<'home' | 'heartRate'>('home');

  /*
   * IMPORTANT:
   * This history is NOT cleared when the ring disconnects.
   */
  const [heartRateReadings, setHeartRateReadings] =
    useState<HeartRateReading[]>([]);

  const [graphWidth, setGraphWidth] = useState(0);

  const scanningRef = useRef(false);
  const connectingRef = useRef(false);
  const seenDevicesRef = useRef<Set<string>>(new Set());

  const batterySubscriptionRef = useRef<any>(null);
  const heartRateSubscriptionRef = useRef<any>(null);
  const disconnectSubscriptionRef = useRef<any>(null);

  const styles = isDarkMode ? darkStyles : lightStyles;

  const hrStatus = getHeartRateStatus(heartRate);

  /*
   * ============================================================
   * RECORD HEART RATE
   * ============================================================
   */

  const recordHeartRate = (value: number) => {
    if (value <= 0 || value > 250) {
      return;
    }

    setHeartRateReadings((previous) => [
      ...previous.slice(-(MAX_HR_READINGS - 1)),
      {
        value,
        timestamp: Date.now(),
      },
    ]);
  };

  /*
   * ============================================================
   * SUBSCRIPTIONS
   * ============================================================
   */

  const clearSubscriptions = () => {
    batterySubscriptionRef.current?.remove();
    heartRateSubscriptionRef.current?.remove();
    disconnectSubscriptionRef.current?.remove();

    batterySubscriptionRef.current = null;
    heartRateSubscriptionRef.current = null;
    disconnectSubscriptionRef.current = null;
  };

  /*
   * ============================================================
   * BLUETOOTH STATE
   * ============================================================
   */

  useEffect(() => {
    const manager = managerRef.current;

    const subscription = manager.onStateChange(
      (state) => {
        if (state !== State.PoweredOn) {
          clearSubscriptions();

          setDevice(null);
          setBattery(null);
          setHeartRate(null);
          setTelemetry(null);

          /*
           * DO NOT CLEAR heartRateReadings HERE.
           *
           * Historical heart-rate data stays in the app.
           */

          setStatus('Bluetooth off');
        } else {
          setStatus('Disconnected');
        }
      },
      true
    );

    return () => {
      subscription.remove();
      clearSubscriptions();
      manager.destroy();
    };
  }, []);

  /*
   * ============================================================
   * DISCONNECT
   * ============================================================
   */

  const disconnectFromRing = async () => {
    scanningRef.current = false;
    connectingRef.current = false;

    clearSubscriptions();

    const currentDevice = device;

    try {
      if (currentDevice) {
        const connected =
          await currentDevice.isConnected();

        if (connected) {
          await currentDevice.cancelConnection();
        }
      }
    } catch (error) {
      console.log('Disconnect error:', error);
    } finally {
      setDevice(null);
      setBattery(null);
      setHeartRate(null);
      setTelemetry(null);

      /*
       * IMPORTANT:
       * Historical HR readings are intentionally preserved.
       */

      setStatus('Disconnected');
      seenDevicesRef.current.clear();
    }
  };

  /*
   * ============================================================
   * CONNECT
   * ============================================================
   */

  const connectToRing = async (
    foundDevice: Device
  ) => {
    if (connectingRef.current) {
      return;
    }

    connectingRef.current = true;

    try {
      setStatus('Connecting...');

      const manager = managerRef.current;

      const connectedDevice =
        await manager.connectToDevice(
          foundDevice.id,
          {
            autoConnect: false,
          }
        );

      const isConnected =
        await connectedDevice.isConnected();

      if (!isConnected) {
        throw new Error(
          'Device is not connected'
        );
      }

      setDevice(connectedDevice);
      setStatus('Connected');

      disconnectSubscriptionRef.current?.remove();

      disconnectSubscriptionRef.current =
        connectedDevice.onDisconnected(
          (error) => {
            console.log(
              'Disconnected:',
              error
            );

            batterySubscriptionRef.current?.remove();
            heartRateSubscriptionRef.current?.remove();

            batterySubscriptionRef.current = null;
            heartRateSubscriptionRef.current = null;

            setDevice(null);
            setBattery(null);
            setHeartRate(null);
            setTelemetry(null);

            /*
             * IMPORTANT:
             * DO NOT clear heartRateReadings.
             */

            setStatus('Disconnected');
          }
        );

      const discoveredDevice =
        await connectedDevice
          .discoverAllServicesAndCharacteristics();

      const services =
        await discoveredDevice.services();

      console.log(
        'Services:',
        services.map(
          (service) => service.uuid
        )
      );

      for (const service of services) {
        const characteristics =
          await service.characteristics();

        console.log(
          'Service:',
          service.uuid,
          'Characteristics:',
          characteristics.map(
            (characteristic) =>
              characteristic.uuid
          )
        );
      }

      /*
       * ========================================================
       * HEART RATE
       * ========================================================
       */

      const heartRateService =
        services.find(
          (service) =>
            service.uuid.toLowerCase() ===
            HEART_RATE_SERVICE.toLowerCase()
        );

      if (!heartRateService) {
        console.log(
          'Heart rate service not found'
        );
      } else {
        const characteristics =
          await heartRateService.characteristics();

        const heartRateCharacteristic =
          characteristics.find(
            (characteristic) =>
              characteristic.uuid.toLowerCase() ===
              HEART_RATE_CHARACTERISTIC.toLowerCase()
          );

        if (!heartRateCharacteristic) {
          console.log(
            'Heart rate characteristic not found'
          );
        } else {
          console.log(
            'Heart rate characteristic found:',
            heartRateCharacteristic.uuid
          );

          /*
           * INITIAL HR READ
           */

          if (
            heartRateCharacteristic.isReadable
          ) {
            try {
              const readValue =
                await discoveredDevice
                  .readCharacteristicForService(
                    HEART_RATE_SERVICE,
                    HEART_RATE_CHARACTERISTIC
                  );

              if (readValue.value) {
                const hr =
                  decodeHeartRate(
                    readValue.value
                  );

                if (hr !== null) {
                  console.log(
                    'Initial heart rate:',
                    hr
                  );

                  setHeartRate(hr);
                  recordHeartRate(hr);
                }
              }
            } catch (error) {
              console.log(
                'Heart rate read error:',
                error
              );
            }
          }

          /*
           * HR NOTIFICATIONS
           */

          heartRateSubscriptionRef.current?.remove();

          heartRateSubscriptionRef.current =
            discoveredDevice
              .monitorCharacteristicForService(
                HEART_RATE_SERVICE,
                HEART_RATE_CHARACTERISTIC,
                (error, characteristic) => {
                  if (error) {
                    console.log(
                      'Heart rate notification error:',
                      error
                    );

                    return;
                  }

                  if (
                    !characteristic?.value
                  ) {
                    return;
                  }

                  console.log(
                    'Heart rate notification:',
                    characteristic.value
                  );

                  const hr =
                    decodeHeartRate(
                      characteristic.value
                    );

                  if (hr !== null) {
                    setHeartRate(hr);
                    recordHeartRate(hr);
                  }
                }
              );
        }
      }

      /*
       * ========================================================
       * BATTERY / TELEMETRY
       * ========================================================
       */

      batterySubscriptionRef.current?.remove();

      batterySubscriptionRef.current =
        discoveredDevice
          .monitorCharacteristicForService(
            DATA_SERVICE,
            TELEMETRY_CHARACTERISTIC,
            (error, characteristic) => {
              if (error) {
                console.log(
                  'Battery notification error:',
                  error
                );

                return;
              }

              if (!characteristic?.value) {
                return;
              }

              console.log(
                'Battery telemetry:',
                characteristic.value
              );

              const decoded =
                decodeBattery(
                  characteristic.value
                );

              if (decoded !== null) {
                setBattery(decoded);
              }

              setTelemetry(
                characteristic.value
              );

              const telemetryValue =
                decodeTelemetry(
                  characteristic.value
                );

              if (
                telemetryValue !== null
              ) {
                setBattery(
                  telemetryValue
                );
              }
            }
          );

      const finalConnection =
        await discoveredDevice.isConnected();

      if (!finalConnection) {
        throw new Error(
          'Connection lost after service discovery'
        );
      }

      setStatus('Connected');
    } catch (error) {
      console.log(
        'Connection failed:',
        error
      );

      clearSubscriptions();

      setDevice(null);
      setBattery(null);
      setHeartRate(null);
      setTelemetry(null);

      /*
       * IMPORTANT:
       * Keep previous HR history even if
       * a new connection attempt fails.
       */

      setStatus('Connection failed');

      Alert.alert(
        'Connection failed',
        'Could not connect to the SR08 ring.'
      );
    } finally {
      connectingRef.current = false;
    }
  };

  /*
   * ============================================================
   * SCAN
   * ============================================================
   */

  const scanForRing = async () => {
    if (
      scanningRef.current ||
      connectingRef.current
    ) {
      return;
    }

    try {
      if (Platform.OS === 'android') {
        const granted =
          await PermissionsAndroid.requestMultiple(
            [
              PermissionsAndroid.PERMISSIONS
                .BLUETOOTH_SCAN,
              PermissionsAndroid.PERMISSIONS
                .BLUETOOTH_CONNECT,
            ]
          );

        const scanGranted =
          granted[
            PermissionsAndroid.PERMISSIONS
              .BLUETOOTH_SCAN
          ] ===
          PermissionsAndroid.RESULTS
            .GRANTED;

        const connectGranted =
          granted[
            PermissionsAndroid.PERMISSIONS
              .BLUETOOTH_CONNECT
          ] ===
          PermissionsAndroid.RESULTS
            .GRANTED;

        if (
          !scanGranted ||
          !connectGranted
        ) {
          Alert.alert(
            'Bluetooth permission required',
            'Please allow Bluetooth permissions to connect to the ring.'
          );

          return;
        }
      }

      const manager = managerRef.current;

      const state =
        await manager.state();

      if (state !== State.PoweredOn) {
        Alert.alert(
          'Bluetooth unavailable',
          'Please turn on Bluetooth and try again.'
        );

        return;
      }

      scanningRef.current = true;
      seenDevicesRef.current.clear();

      setStatus('Scanning...');

      manager.startDeviceScan(
        null,
        {
          allowDuplicates: false,
        },
        (error, scannedDevice) => {
          if (error) {
            console.log(
              'Scan error:',
              error
            );

            scanningRef.current = false;
            manager.stopDeviceScan();

            setStatus('Scan failed');

            return;
          }

          if (!scannedDevice) {
            return;
          }

          if (
            seenDevicesRef.current.has(
              scannedDevice.id
            )
          ) {
            return;
          }

          seenDevicesRef.current.add(
            scannedDevice.id
          );

          console.log(
            'Found:',
            scannedDevice.name,
            scannedDevice.id
          );

          const nameMatches =
            scannedDevice.name ===
              RING_NAME ||
            scannedDevice.localName ===
              RING_NAME;

          const macMatches =
            scannedDevice.id
              .toUpperCase()
              .startsWith(
                RING_MAC_PREFIX.toUpperCase()
              );

          if (
            nameMatches ||
            macMatches
          ) {
            scanningRef.current = false;

            manager.stopDeviceScan();

            connectToRing(
              scannedDevice
            );
          }
        }
      );

      setTimeout(() => {
        if (scanningRef.current) {
          scanningRef.current = false;

          manager.stopDeviceScan();

          setStatus('Disconnected');
        }
      }, 10000);
    } catch (error) {
      console.log(
        'Scan failed:',
        error
      );

      scanningRef.current = false;

      managerRef.current.stopDeviceScan();

      setStatus('Scan failed');
    }
  };

  /*
   * ============================================================
   * HEART RATE STATISTICS
   * ============================================================
   */

  const heartRateStats = useMemo(() => {
    if (
      heartRateReadings.length === 0
    ) {
      return {
        average: null,
        minimum: null,
        maximum: null,
      };
    }

    const values =
      heartRateReadings.map(
        (reading) => reading.value
      );

    const total =
      values.reduce(
        (sum, value) =>
          sum + value,
        0
      );

    return {
      average: Math.round(
        total / values.length
      ),
      minimum: Math.min(
        ...values
      ),
      maximum: Math.max(
        ...values
      ),
    };
  }, [heartRateReadings]);

  /*
   * ============================================================
   * GRAPH
   * ============================================================
   */

  const graphData = useMemo(() => {
    const readings =
      heartRateReadings.slice(-30);

    if (
      readings.length === 0 ||
      graphWidth <= 0
    ) {
      return null;
    }

    const chartHeight = 190;

    const leftPadding = 38;
    const rightPadding = 12;
    const topPadding = 18;
    const bottomPadding = 24;

    const plotWidth =
      graphWidth -
      leftPadding -
      rightPadding;

    const plotHeight =
      chartHeight -
      topPadding -
      bottomPadding;

    const values =
      readings.map(
        (reading) =>
          reading.value
      );

    let minValue =
      Math.min(...values);

    let maxValue =
      Math.max(...values);

    if (
      minValue === maxValue
    ) {
      minValue -= 10;
      maxValue += 10;
    }

    const range =
      maxValue - minValue;

    const points =
      readings.map(
        (reading, index) => {
          const x =
            readings.length === 1
              ? leftPadding +
                plotWidth / 2
              : leftPadding +
                (index /
                  (readings.length -
                    1)) *
                  plotWidth;

          const normalized =
            (reading.value -
              minValue) /
            range;

          const y =
            topPadding +
            (1 - normalized) *
              plotHeight;

          return {
            x,
            y,
            value: reading.value,
          };
        }
      );

    return {
      points,
      minValue,
      maxValue,
      chartHeight,
      leftPadding,
      topPadding,
      plotWidth,
      plotHeight,
    };
  }, [
    heartRateReadings,
    graphWidth,
  ]);

  const formatTime = (
    timestamp: number
  ) => {
    return new Date(
      timestamp
    ).toLocaleTimeString([], {
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit',
    });
  };

  /*
   * ============================================================
   * UI
   * ============================================================
   */

  return (
    <SafeAreaView
      style={styles.safeArea}
    >
      <StatusBar
        style={
          isDarkMode
            ? 'light'
            : 'dark'
        }
      />

      <View
        style={styles.container}
      >
        <ScrollView
          contentContainerStyle={
            styles.scrollContent
          }
          showsVerticalScrollIndicator={
            false
          }
        >
          {/* HEADER */}

          <View
            style={styles.header}
          >
            <View>
              <Text
                style={styles.brand}
              >
                SR08 / COMPANION
              </Text>

              <Text
                style={styles.tagline}
              >
                Your ring, in rhythm.
              </Text>
            </View>

            <Pressable
              style={
                styles.themeButton
              }
              onPress={() =>
                setIsDarkMode(
                  (previous) =>
                    !previous
                )
              }
            >
              <Text
                style={
                  styles.themeButtonText
                }
              >
                {isDarkMode
                  ? '☀'
                  : '☾'}
              </Text>
            </Pressable>
          </View>

          {/* TABS */}

          <View
            style={
              styles.tabContainer
            }
          >
            <Pressable
              style={[
                styles.tab,
                activeTab ===
                  'home' &&
                  styles.activeTab,
              ]}
              onPress={() =>
                setActiveTab(
                  'home'
                )
              }
            >
              <Text
                style={[
                  styles.tabText,
                  activeTab ===
                    'home' &&
                    styles.activeTabText,
                ]}
              >
                Home
              </Text>
            </Pressable>

            <Pressable
              style={[
                styles.tab,
                activeTab ===
                  'heartRate' &&
                  styles.activeTab,
              ]}
              onPress={() =>
                setActiveTab(
                  'heartRate'
                )
              }
            >
              <Text
                style={[
                  styles.tabText,
                  activeTab ===
                    'heartRate' &&
                    styles.activeTabText,
                ]}
              >
                Heart Rate
              </Text>
            </Pressable>
          </View>

          {/* ==================================================
              HOME
              ================================================== */}

          {activeTab === 'home' && (
            <>
              {/* DEVICE */}

              <View
                style={styles.card}
              >
                <View
                  style={
                    styles.cardHeader
                  }
                >
                  <View>
                    <Text
                      style={
                        styles.sectionLabel
                      }
                    >
                      DEVICE
                    </Text>

                    <Text
                      style={
                        styles.deviceName
                      }
                    >
                      {device?.name ||
                        RING_NAME}
                    </Text>
                  </View>

                  <View
                    style={[
                      styles.statusDot,
                      status ===
                        'Connected' &&
                        styles.statusDotConnected,
                    ]}
                  />
                </View>

                <Text
                  style={
                    styles.statusText
                  }
                >
                  {status}
                </Text>

                <Pressable
                  style={[
                    styles.primaryButton,
                    status ===
                      'Connected' &&
                      styles.disconnectButton,
                  ]}
                  onPress={
                    status ===
                    'Connected'
                      ? disconnectFromRing
                      : scanForRing
                  }
                >
                  <Text
                    style={
                      styles.primaryButtonText
                    }
                  >
                    {status ===
                    'Connected'
                      ? 'Disconnect'
                      : 'Connect to ring'}
                  </Text>
                </Pressable>
              </View>

              {/* BATTERY */}

              <View
                style={styles.card}
              >
                <View
                  style={
                    styles.cardHeader
                  }
                >
                  <Text
                    style={
                      styles.sectionLabel
                    }
                  >
                    BATTERY
                  </Text>

                  <Text
                    style={
                      styles.batteryValue
                    }
                  >
                    {battery !==
                    null
                      ? `${battery}%`
                      : '--'}
                  </Text>
                </View>

                <View
                  style={
                    styles.batteryTrack
                  }
                >
                  <View
                    style={[
                      styles.batteryFill,
                      {
                        width:
                          battery !==
                          null
                            ? `${Math.max(
                                0,
                                Math.min(
                                  100,
                                  battery
                                )
                              )}%`
                            : '0%',
                      },
                    ]}
                  />
                </View>

                <Text
                  style={
                    styles.mutedText
                  }
                >
                  {battery !==
                  null
                    ? 'Battery level'
                    : 'Waiting for telemetry'}
                </Text>
              </View>

              {/* LIVE HEART RATE */}

              <View
                style={styles.card}
              >
                <View
                  style={
                    styles.cardHeader
                  }
                >
                  <View>
                    <Text
                      style={
                        styles.sectionLabel
                      }
                    >
                      HEART RATE
                    </Text>

                    <Text
                      style={
                        styles.featureTitle
                      }
                    >
                      Live measurement
                    </Text>
                  </View>

                  <Text
                    style={
                      styles.heartIcon
                    }
                  >
                    ♥
                  </Text>
                </View>

                <View
                  style={
                    styles.liveHrRow
                  }
                >
                  <Text
                    style={
                      styles.liveHrValue
                    }
                  >
                    {heartRate !==
                    null
                      ? heartRate
                      : '--'}
                  </Text>

                  <Text
                    style={
                      styles.liveHrUnit
                    }
                  >
                    BPM
                  </Text>
                </View>

                <View
                  style={[
                    styles.homeHrStatus,
                    styles[
                      `statusBackground_${hrStatus.level}`
                    ],
                  ]}
                >
                  <View
                    style={[
                      styles.homeHrStatusDot,
                      styles[
                        `statusDot_${hrStatus.level}`
                      ],
                    ]}
                  />

                  <Text
                    style={[
                      styles.homeHrStatusText,
                      styles[
                        `statusText_${hrStatus.level}`
                      ],
                    ]}
                  >
                    {hrStatus.label}
                  </Text>
                </View>

                <Text
                  style={
                    styles.mutedText
                  }
                >
                  {heartRate !==
                  null
                    ? hrStatus.description
                    : 'Waiting for heart rate data'}
                </Text>

                <Pressable
                  style={
                    styles.secondaryButton
                  }
                  onPress={() =>
                    setActiveTab(
                      'heartRate'
                    )
                  }
                >
                  <Text
                    style={
                      styles.secondaryButtonText
                    }
                  >
                    View heart rate analysis
                  </Text>
                </Pressable>
              </View>

              {/* COMING ONLINE */}

              <Text
                style={
                  styles.sectionHeading
                }
              >
                COMING ONLINE
              </Text>

              <View
                style={
                  styles.featureCard
                }
              >
                <View
                  style={
                    styles.featureIconBox
                  }
                >
                  <Text
                    style={
                      styles.featureIcon
                    }
                  >
                    ♥
                  </Text>
                </View>

                <View
                  style={
                    styles.featureContent
                  }
                >
                  <Text
                    style={
                      styles.featureTitle
                    }
                  >
                    Heart rate
                  </Text>

                  <Text
                    style={
                      styles.featureDescription
                    }
                  >
                    Continuous heart rate
                    monitoring and analysis.
                  </Text>
                </View>

                <View
                  style={
                    styles.liveBadge
                  }
                >
                  <Text
                    style={
                      styles.liveBadgeText
                    }
                  >
                    LIVE
                  </Text>
                </View>
              </View>

              <View
                style={
                  styles.featureCard
                }
              >
                <View
                  style={
                    styles.featureIconBox
                  }
                >
                  <Text
                    style={
                      styles.featureIcon
                    }
                  >
                    ◷
                  </Text>
                </View>

                <View
                  style={
                    styles.featureContent
                  }
                >
                  <Text
                    style={
                      styles.featureTitle
                    }
                  >
                    Sleep timer
                  </Text>

                  <Text
                    style={
                      styles.featureDescription
                    }
                  >
                    Sleep tracking will be
                    available soon.
                  </Text>
                </View>

                <View
                  style={
                    styles.soonBadge
                  }
                >
                  <Text
                    style={
                      styles.soonBadgeText
                    }
                  >
                    SOON
                  </Text>
                </View>
              </View>

              {telemetry !==
                null && (
                <View
                  style={
                    styles.telemetryCard
                  }
                >
                  <Text
                    style={
                      styles.sectionLabel
                    }
                  >
                    RAW TELEMETRY
                  </Text>

                  <Text
                    style={
                      styles.telemetryValue
                    }
                  >
                    {telemetry}
                  </Text>
                </View>
              )}
            </>
          )}

          {/* ==================================================
              HEART RATE
              ================================================== */}

          {activeTab ===
            'heartRate' && (
            <>
              <View
                style={
                  styles.analysisHeader
                }
              >
                <Text
                  style={
                    styles.analysisTitle
                  }
                >
                  Heart rate analysis
                </Text>

                <Text
                  style={
                    styles.analysisSubtitle
                  }
                >
                  Live monitoring and range alerts
                </Text>
              </View>

              {/* CURRENT */}

              <View
                style={
                  styles.hrHeroCard
                }
              >
                <Text
                  style={
                    styles.sectionLabelDark
                  }
                >
                  CURRENT HEART RATE
                </Text>

                <View
                  style={
                    styles.hrHeroRow
                  }
                >
                  <Text
                    style={
                      styles.hrHeroValue
                    }
                  >
                    {heartRate !==
                    null
                      ? heartRate
                      : '--'}
                  </Text>

                  <Text
                    style={
                      styles.hrHeroUnit
                    }
                  >
                    BPM
                  </Text>
                </View>

                <View
                  style={[
                    styles.heroStatusPill,
                    styles[
                      `heroStatus_${hrStatus.level}`
                    ],
                  ]}
                >
                  <View
                    style={[
                      styles.heroStatusDot,
                      styles[
                        `statusDot_${hrStatus.level}`
                      ],
                    ]}
                  />

                  <Text
                    style={[
                      styles.heroStatusText,
                      styles[
                        `heroStatusText_${hrStatus.level}`
                      ],
                    ]}
                  >
                    {hrStatus.label}
                  </Text>
                </View>
              </View>

              {/* ALERT */}

              {heartRate !==
                null &&
                hrStatus.level !==
                  'normal' && (
                  <View
                    style={[
                      styles.alertCard,
                      styles[
                        `alertCard_${hrStatus.level}`
                      ],
                    ]}
                  >
                    <View
                      style={
                        styles.alertIconCircle
                      }
                    >
                      <Text
                        style={
                          styles.alertIcon
                        }
                      >
                        !
                      </Text>
                    </View>

                    <View
                      style={
                        styles.alertContent
                      }
                    >
                      <Text
                        style={
                          styles.alertTitle
                        }
                      >
                        {hrStatus.label}{' '}
                        heart rate
                      </Text>

                      <Text
                        style={
                          styles.alertDescription
                        }
                      >
                        {hrStatus.description}
                      </Text>
                    </View>
                  </View>
                )}

              {/* NORMAL */}

              {heartRate !==
                null &&
                hrStatus.level ===
                  'normal' && (
                  <View
                    style={
                      styles.normalCard
                    }
                  >
                    <View
                      style={
                        styles.normalIconCircle
                      }
                    >
                      <Text
                        style={
                          styles.normalIcon
                        }
                      >
                        ✓
                      </Text>
                    </View>

                    <View
                      style={
                        styles.alertContent
                      }
                    >
                      <Text
                        style={
                          styles.normalTitle
                        }
                      >
                        Heart rate looks normal
                      </Text>

                      <Text
                        style={
                          styles.normalDescription
                        }
                      >
                        Current reading is within
                        the typical resting range.
                      </Text>
                    </View>
                  </View>
                )}

              {/* STATS */}

              <View
                style={
                  styles.statsRow
                }
              >
                <View
                  style={
                    styles.statCard
                  }
                >
                  <Text
                    style={
                      styles.statLabel
                    }
                  >
                    AVG
                  </Text>

                  <Text
                    style={
                      styles.statValue
                    }
                  >
                    {heartRateStats.average ??
                      '--'}
                  </Text>

                  <Text
                    style={
                      styles.statUnit
                    }
                  >
                    BPM
                  </Text>
                </View>

                <View
                  style={
                    styles.statCard
                  }
                >
                  <Text
                    style={
                      styles.statLabel
                    }
                  >
                    MIN
                  </Text>

                  <Text
                    style={
                      styles.statValue
                    }
                  >
                    {heartRateStats.minimum ??
                      '--'}
                  </Text>

                  <Text
                    style={
                      styles.statUnit
                    }
                  >
                    BPM
                  </Text>
                </View>

                <View
                  style={
                    styles.statCard
                  }
                >
                  <Text
                    style={
                      styles.statLabel
                    }
                  >
                    MAX
                  </Text>

                  <Text
                    style={
                      styles.statValue
                    }
                  >
                    {heartRateStats.maximum ??
                      '--'}
                  </Text>

                  <Text
                    style={
                      styles.statUnit
                    }
                  >
                    BPM
                  </Text>
                </View>
              </View>

              {/* RANGE GUIDE */}

              <View
                style={
                  styles.rangeCard
                }
              >
                <Text
                  style={
                    styles.rangeTitle
                  }
                >
                  HEART RATE RANGE GUIDE
                </Text>

                <View
                  style={
                    styles.rangeRow
                  }
                >
                  <View
                    style={[
                      styles.rangeIndicator,
                      styles.rangeNormal,
                    ]}
                  />

                  <Text
                    style={
                      styles.rangeLabel
                    }
                  >
                    50–100 BPM
                  </Text>

                  <Text
                    style={
                      styles.rangeDescription
                    }
                  >
                    Normal resting
                  </Text>
                </View>

                <View
                  style={
                    styles.rangeRow
                  }
                >
                  <View
                    style={[
                      styles.rangeIndicator,
                      styles.rangeElevated,
                    ]}
                  />

                  <Text
                    style={
                      styles.rangeLabel
                    }
                  >
                    101–120
                  </Text>

                  <Text
                    style={
                      styles.rangeDescription
                    }
                  >
                    Elevated
                  </Text>
                </View>

                <View
                  style={
                    styles.rangeRow
                  }
                >
                  <View
                    style={[
                      styles.rangeIndicator,
                      styles.rangeHigh,
                    ]}
                  />

                  <Text
                    style={
                      styles.rangeLabel
                    }
                  >
                    121–150
                  </Text>

                  <Text
                    style={
                      styles.rangeDescription
                    }
                  >
                    High
                  </Text>
                </View>

                <View
                  style={
                    styles.rangeRow
                  }
                >
                  <View
                    style={[
                      styles.rangeIndicator,
                      styles.rangeVeryHigh,
                    ]}
                  />

                  <Text
                    style={
                      styles.rangeLabel
                    }
                  >
                    &gt;150
                  </Text>

                  <Text
                    style={
                      styles.rangeDescription
                    }
                  >
                    Very high
                  </Text>
                </View>

                <View
                  style={
                    styles.rangeRow
                  }
                >
                  <View
                    style={[
                      styles.rangeIndicator,
                      styles.rangeLow,
                    ]}
                  />

                  <Text
                    style={
                      styles.rangeLabel
                    }
                  >
                    &lt;50
                  </Text>

                  <Text
                    style={
                      styles.rangeDescription
                    }
                  >
                    Low
                  </Text>
                </View>
              </View>

              {/* GRAPH */}

              <View
                style={
                  styles.graphCard
                }
                onLayout={(event) => {
                  setGraphWidth(
                    event.nativeEvent
                      .layout.width
                  );
                }}
              >
                <View
                  style={
                    styles.graphHeader
                  }
                >
                  <View>
                    <Text
                      style={
                        styles.graphTitle
                      }
                    >
                      Heart rate trend
                    </Text>

                    <Text
                      style={
                        styles.graphSubtitle
                      }
                    >
                      Last 30 readings
                    </Text>
                  </View>

                  <Text
                    style={
                      styles.readingCount
                    }
                  >
                    {heartRateReadings.length}
                  </Text>
                </View>

                {graphData ? (
                  <View
                    style={[
                      styles.graphArea,
                      {
                        height:
                          graphData.chartHeight,
                      },
                    ]}
                  >
                    {/* GRID */}

                    <View
                      style={[
                        styles.gridLine,
                        {
                          top:
                            graphData.topPadding,
                          left:
                            graphData.leftPadding,
                          width:
                            graphData.plotWidth,
                        },
                      ]}
                    />

                    <View
                      style={[
                        styles.gridLine,
                        {
                          top:
                            graphData.topPadding +
                            graphData.plotHeight /
                              2,
                          left:
                            graphData.leftPadding,
                          width:
                            graphData.plotWidth,
                        },
                      ]}
                    />

                    <View
                      style={[
                        styles.gridLine,
                        {
                          top:
                            graphData.topPadding +
                            graphData.plotHeight,
                          left:
                            graphData.leftPadding,
                          width:
                            graphData.plotWidth,
                        },
                      ]}
                    />

                    {/* Y LABELS */}

                    <Text
                      style={[
                        styles.graphYLabel,
                        {
                          top:
                            graphData.topPadding -
                            7,
                        },
                      ]}
                    >
                      {Math.round(
                        graphData.maxValue
                      )}
                    </Text>

                    <Text
                      style={[
                        styles.graphYLabel,
                        {
                          top:
                            graphData.topPadding +
                            graphData.plotHeight /
                              2 -
                            7,
                        },
                      ]}
                    >
                      {Math.round(
                        (graphData.maxValue +
                          graphData.minValue) /
                          2
                      )}
                    </Text>

                    <Text
                      style={[
                        styles.graphYLabel,
                        {
                          top:
                            graphData.topPadding +
                            graphData.plotHeight -
                            7,
                        },
                      ]}
                    >
                      {Math.round(
                        graphData.minValue
                      )}
                    </Text>

                    {/* GRAPH LINES */}

                    {graphData.points.map(
                      (
                        point,
                        index
                      ) => {
                        if (
                          index ===
                          graphData.points
                            .length -
                            1
                        ) {
                          return null;
                        }

                        const next =
                          graphData.points[
                            index + 1
                          ];

                        const dx =
                          next.x -
                          point.x;

                        const dy =
                          next.y -
                          point.y;

                        const length =
                          Math.sqrt(
                            dx * dx +
                              dy * dy
                          );

                        const angle =
                          (Math.atan2(
                            dy,
                            dx
                          ) *
                            180) /
                          Math.PI;

                        return (
                          <View
                            key={`line-${index}`}
                            style={[
                              styles.graphSegment,
                              {
                                width:
                                  length,
                                left:
                                  (point.x +
                                    next.x) /
                                    2 -
                                  length /
                                    2,
                                top:
                                  (point.y +
                                    next.y) /
                                    2 -
                                  1.5,
                                transform:
                                  [
                                    {
                                      rotate: `${angle}deg`,
                                    },
                                  ],
                              },
                            ]}
                          />
                        );
                      }
                    )}

                    {/* GRAPH POINTS */}

                    {graphData.points.map(
                      (
                        point,
                        index
                      ) => {
                        const isLast =
                          index ===
                          graphData.points
                            .length -
                            1;

                        return (
                          <View
                            key={`point-${index}`}
                            style={[
                              styles.graphPoint,
                              isLast &&
                                styles.graphPointLast,
                              {
                                left:
                                  point.x -
                                  (isLast
                                    ? 6
                                    : 4),
                                top:
                                  point.y -
                                  (isLast
                                    ? 6
                                    : 4),
                              },
                            ]}
                          />
                        );
                      }
                    )}
                  </View>
                ) : (
                  <View
                    style={
                      styles.emptyGraph
                    }
                  >
                    <Text
                      style={
                        styles.emptyGraphTitle
                      }
                    >
                      No heart rate data yet
                    </Text>

                    <Text
                      style={
                        styles.emptyGraphText
                      }
                    >
                      Connect the ring and wait
                      for heart rate notifications.
                    </Text>
                  </View>
                )}
              </View>

              {/* RECENT READINGS */}

              <View
                style={
                  styles.recentCard
                }
              >
                <View
                  style={
                    styles.cardHeader
                  }
                >
                  <Text
                    style={
                      styles.sectionLabel
                    }
                  >
                    RECENT READINGS
                  </Text>

                  <Text
                    style={
                      styles.mutedText
                    }
                  >
                    Every notification
                  </Text>
                </View>

                {heartRateReadings.length ===
                0 ? (
                  <Text
                    style={
                      styles.noReadingsText
                    }
                  >
                    No readings recorded yet.
                  </Text>
                ) : (
                  heartRateReadings
                    .slice(-8)
                    .reverse()
                    .map(
                      (
                        reading,
                        index
                      ) => (
                        <View
                          key={`${reading.timestamp}-${index}`}
                          style={
                            styles.readingRow
                          }
                        >
                          <View
                            style={
                              styles.readingDot
                            }
                          />

                          <Text
                            style={
                              styles.readingTime
                            }
                          >
                            {formatTime(
                              reading.timestamp
                            )}
                          </Text>

                          <View
                            style={
                              styles.readingSpacer
                            }
                          />

                          <Text
                            style={
                              styles.readingValue
                            }
                          >
                            {reading.value}
                          </Text>

                          <Text
                            style={
                              styles.readingUnit
                            }
                          >
                            BPM
                          </Text>
                        </View>
                      )
                    )
                )}
              </View>

              {/* ONLY THIS BUTTON CLEARS HISTORY */}

              <Pressable
                style={
                  styles.secondaryButton
                }
                onPress={() => {
                  setHeartRateReadings(
                    []
                  );
                }}
              >
                <Text
                  style={
                    styles.secondaryButtonText
                  }
                >
                  Clear analysis history
                </Text>
              </Pressable>
            </>
          )}

          <Text
            style={styles.footer}
          >
            SR08 Companion · BLE telemetry
          </Text>
        </ScrollView>
      </View>
    </SafeAreaView>
  );
}

/*
 * ============================================================
 * LIGHT THEME
 * ============================================================
 */

const lightStyles = StyleSheet.create({
  safeArea: {
    flex: 1,
    backgroundColor: '#F5F7FA',
  },

  container: {
    flex: 1,
    backgroundColor: '#F5F7FA',
  },

  scrollContent: {
    paddingHorizontal: 20,
    paddingTop: 18,
    paddingBottom: 40,
  },

  header: {
    flexDirection: 'row',
    alignItems: 'flex-start',
    justifyContent: 'space-between',
    marginBottom: 22,
  },

  brand: {
    fontSize: 13,
    fontWeight: '800',
    letterSpacing: 1.8,
    color: '#0F172A',
  },

  tagline: {
    marginTop: 6,
    fontSize: 13,
    color: '#64748B',
  },

  themeButton: {
    width: 42,
    height: 42,
    borderRadius: 21,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#FFFFFF',
    borderWidth: 1,
    borderColor: '#E2E8F0',
  },

  themeButtonText: {
    fontSize: 20,
    color: '#0F172A',
  },

  tabContainer: {
    flexDirection: 'row',
    padding: 4,
    borderRadius: 14,
    backgroundColor: '#E8EDF3',
    marginBottom: 18,
  },

  tab: {
    flex: 1,
    paddingVertical: 11,
    alignItems: 'center',
    borderRadius: 11,
  },

  activeTab: {
    backgroundColor: '#FFFFFF',
  },

  tabText: {
    fontSize: 14,
    fontWeight: '700',
    color: '#64748B',
  },

  activeTabText: {
    color: '#0F172A',
  },

  card: {
    backgroundColor: '#FFFFFF',
    borderRadius: 18,
    padding: 18,
    marginBottom: 14,
    borderWidth: 1,
    borderColor: '#E5EAF0',
  },

  cardHeader: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
  },

  sectionLabel: {
    fontSize: 10,
    fontWeight: '800',
    letterSpacing: 1.4,
    color: '#64748B',
  },

  sectionLabelDark: {
    fontSize: 10,
    fontWeight: '800',
    letterSpacing: 1.4,
    color: '#94A3B8',
  },

  deviceName: {
    marginTop: 6,
    fontSize: 20,
    fontWeight: '800',
    color: '#0F172A',
  },

  statusDot: {
    width: 10,
    height: 10,
    borderRadius: 5,
    backgroundColor: '#CBD5E1',
  },

  statusDotConnected: {
    backgroundColor: '#14B8A6',
  },

  statusText: {
    marginTop: 8,
    fontSize: 13,
    color: '#64748B',
  },

  primaryButton: {
    marginTop: 18,
    height: 48,
    borderRadius: 13,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#0F172A',
  },

  disconnectButton: {
    backgroundColor: '#334155',
  },

  primaryButtonText: {
    color: '#FFFFFF',
    fontSize: 14,
    fontWeight: '800',
  },

  batteryValue: {
    fontSize: 26,
    fontWeight: '800',
    color: '#0F172A',
  },

  batteryTrack: {
    height: 8,
    marginTop: 18,
    borderRadius: 4,
    backgroundColor: '#E2E8F0',
    overflow: 'hidden',
  },

  batteryFill: {
    height: '100%',
    borderRadius: 4,
    backgroundColor: '#14B8A6',
  },

  mutedText: {
    marginTop: 8,
    fontSize: 12,
    color: '#64748B',
  },

  featureTitle: {
    marginTop: 5,
    fontSize: 17,
    fontWeight: '800',
    color: '#0F172A',
  },

  heartIcon: {
    fontSize: 25,
    color: '#0EA5E9',
  },

  liveHrRow: {
    flexDirection: 'row',
    alignItems: 'baseline',
    marginTop: 12,
  },

  liveHrValue: {
    fontSize: 52,
    fontWeight: '800',
    letterSpacing: -2,
    color: '#0F172A',
  },

  liveHrUnit: {
    marginLeft: 8,
    fontSize: 14,
    fontWeight: '800',
    color: '#64748B',
  },

  homeHrStatus: {
    alignSelf: 'flex-start',
    flexDirection: 'row',
    alignItems: 'center',
    marginTop: 10,
    paddingHorizontal: 10,
    paddingVertical: 6,
    borderRadius: 8,
  },

  homeHrStatusDot: {
    width: 7,
    height: 7,
    borderRadius: 4,
    marginRight: 6,
  },

  homeHrStatusText: {
    fontSize: 11,
    fontWeight: '800',
  },

  statusBackground_normal: {
    backgroundColor: '#DCFCE7',
  },

  statusBackground_elevated: {
    backgroundColor: '#FEF3C7',
  },

  statusBackground_high: {
    backgroundColor: '#FFEDD5',
  },

  statusBackground_veryHigh: {
    backgroundColor: '#FEE2E2',
  },

  statusBackground_low: {
    backgroundColor: '#FEF3C7',
  },

  statusBackground_veryLow: {
    backgroundColor: '#FEE2E2',
  },

  statusDot_normal: {
    backgroundColor: '#16A34A',
  },

  statusDot_elevated: {
    backgroundColor: '#D97706',
  },

  statusDot_high: {
    backgroundColor: '#EA580C',
  },

  statusDot_veryHigh: {
    backgroundColor: '#DC2626',
  },

  statusDot_low: {
    backgroundColor: '#D97706',
  },

  statusDot_veryLow: {
    backgroundColor: '#DC2626',
  },

  statusText_normal: {
    color: '#166534',
  },

  statusText_elevated: {
    color: '#92400E',
  },

  statusText_high: {
    color: '#9A3412',
  },

  statusText_veryHigh: {
    color: '#991B1B',
  },

  statusText_low: {
    color: '#92400E',
  },

  statusText_veryLow: {
    color: '#991B1B',
  },

  secondaryButton: {
    marginTop: 16,
    height: 44,
    borderRadius: 12,
    borderWidth: 1,
    borderColor: '#CBD5E1',
    alignItems: 'center',
    justifyContent: 'center',
  },

  secondaryButtonText: {
    fontSize: 13,
    fontWeight: '700',
    color: '#0F172A',
  },

  sectionHeading: {
    marginTop: 12,
    marginBottom: 10,
    fontSize: 11,
    fontWeight: '800',
    letterSpacing: 1.5,
    color: '#64748B',
  },

  featureCard: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: '#FFFFFF',
    borderRadius: 17,
    padding: 15,
    marginBottom: 10,
    borderWidth: 1,
    borderColor: '#E5EAF0',
  },

  featureIconBox: {
    width: 42,
    height: 42,
    borderRadius: 12,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#E0F2FE',
  },

  featureIcon: {
    fontSize: 20,
    color: '#0284C7',
  },

  featureContent: {
    flex: 1,
    marginLeft: 13,
  },

  featureDescription: {
    marginTop: 4,
    fontSize: 12,
    lineHeight: 18,
    color: '#64748B',
  },

  liveBadge: {
    paddingHorizontal: 9,
    paddingVertical: 5,
    borderRadius: 7,
    backgroundColor: '#CCFBF1',
  },

  liveBadgeText: {
    fontSize: 9,
    fontWeight: '900',
    color: '#0F766E',
  },

  soonBadge: {
    paddingHorizontal: 9,
    paddingVertical: 5,
    borderRadius: 7,
    backgroundColor: '#F1F5F9',
  },

  soonBadgeText: {
    fontSize: 9,
    fontWeight: '900',
    color: '#64748B',
  },

  telemetryCard: {
    marginTop: 8,
    padding: 15,
    borderRadius: 14,
    backgroundColor: '#E2E8F0',
  },

  telemetryValue: {
    marginTop: 7,
    fontSize: 12,
    color: '#334155',
  },

  analysisHeader: {
    marginBottom: 14,
  },

  analysisTitle: {
    fontSize: 27,
    fontWeight: '800',
    color: '#0F172A',
  },

  analysisSubtitle: {
    marginTop: 5,
    fontSize: 13,
    color: '#64748B',
  },

  hrHeroCard: {
    backgroundColor: '#0F172A',
    borderRadius: 20,
    padding: 20,
    marginBottom: 12,
  },

  hrHeroRow: {
    flexDirection: 'row',
    alignItems: 'baseline',
    marginTop: 10,
  },

  hrHeroValue: {
    fontSize: 58,
    fontWeight: '800',
    color: '#FFFFFF',
    letterSpacing: -2,
  },

  hrHeroUnit: {
    marginLeft: 9,
    fontSize: 14,
    fontWeight: '800',
    color: '#94A3B8',
  },

  heroStatusPill: {
    alignSelf: 'flex-start',
    flexDirection: 'row',
    alignItems: 'center',
    marginTop: 10,
    paddingHorizontal: 10,
    paddingVertical: 6,
    borderRadius: 8,
  },

  heroStatusText: {
    marginLeft: 6,
    fontSize: 11,
    fontWeight: '800',
  },

  heroStatus_normal: {
    backgroundColor: '#14532D',
  },

  heroStatus_elevated: {
    backgroundColor: '#713F12',
  },

  heroStatus_high: {
    backgroundColor: '#7C2D12',
  },

  heroStatus_veryHigh: {
    backgroundColor: '#7F1D1D',
  },

  heroStatus_low: {
    backgroundColor: '#713F12',
  },

  heroStatus_veryLow: {
    backgroundColor: '#7F1D1D',
  },

  heroStatusText_normal: {
    color: '#BBF7D0',
  },

  heroStatusText_elevated: {
    color: '#FDE68A',
  },

  heroStatusText_high: {
    color: '#FED7AA',
  },

  heroStatusText_veryHigh: {
    color: '#FECACA',
  },

  heroStatusText_low: {
    color: '#FDE68A',
  },

  heroStatusText_veryLow: {
    color: '#FECACA',
  },

  heroStatusDot: {
    width: 7,
    height: 7,
    borderRadius: 4,
  },

  alertCard: {
    flexDirection: 'row',
    alignItems: 'center',
    borderRadius: 16,
    padding: 15,
    marginBottom: 12,
    borderWidth: 1,
  },

  alertCard_elevated: {
    backgroundColor: '#FFFBEB',
    borderColor: '#FDE68A',
  },

  alertCard_high: {
    backgroundColor: '#FFF7ED',
    borderColor: '#FED7AA',
  },

  alertCard_veryHigh: {
    backgroundColor: '#FEF2F2',
    borderColor: '#FECACA',
  },

  alertCard_low: {
    backgroundColor: '#FFFBEB',
    borderColor: '#FDE68A',
  },

  alertCard_veryLow: {
    backgroundColor: '#FEF2F2',
    borderColor: '#FECACA',
  },

  alertIconCircle: {
    width: 34,
    height: 34,
    borderRadius: 17,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#DC2626',
  },

  alertIcon: {
    color: '#FFFFFF',
    fontSize: 17,
    fontWeight: '900',
  },

  alertContent: {
    flex: 1,
    marginLeft: 11,
  },

  alertTitle: {
    fontSize: 13,
    fontWeight: '800',
    color: '#0F172A',
  },

  alertDescription: {
    marginTop: 3,
    fontSize: 11,
    lineHeight: 16,
    color: '#64748B',
  },

  normalCard: {
    flexDirection: 'row',
    alignItems: 'center',
    borderRadius: 16,
    padding: 15,
    marginBottom: 12,
    backgroundColor: '#F0FDF4',
    borderWidth: 1,
    borderColor: '#BBF7D0',
  },

  normalIconCircle: {
    width: 34,
    height: 34,
    borderRadius: 17,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: '#16A34A',
  },

  normalIcon: {
    color: '#FFFFFF',
    fontSize: 16,
    fontWeight: '900',
  },

  normalTitle: {
    fontSize: 13,
    fontWeight: '800',
    color: '#166534',
  },

  normalDescription: {
    marginTop: 3,
    fontSize: 11,
    color: '#64748B',
  },

  statsRow: {
    flexDirection: 'row',
    gap: 9,
    marginBottom: 12,
  },

  statCard: {
    flex: 1,
    backgroundColor: '#FFFFFF',
    borderRadius: 15,
    padding: 13,
    borderWidth: 1,
    borderColor: '#E5EAF0',
  },

  statLabel: {
    fontSize: 9,
    fontWeight: '900',
    letterSpacing: 1,
    color: '#64748B',
  },

  statValue: {
    marginTop: 8,
    fontSize: 25,
    fontWeight: '800',
    color: '#0F172A',
  },

  statUnit: {
    marginTop: 2,
    fontSize: 9,
    fontWeight: '700',
    color: '#94A3B8',
  },

  rangeCard: {
    backgroundColor: '#FFFFFF',
    borderRadius: 18,
    padding: 16,
    marginBottom: 12,
    borderWidth: 1,
    borderColor: '#E5EAF0',
  },

  rangeTitle: {
    fontSize: 10,
    fontWeight: '900',
    letterSpacing: 1.2,
    color: '#64748B',
    marginBottom: 8,
  },

  rangeRow: {
    flexDirection: 'row',
    alignItems: 'center',
    paddingVertical: 7,
  },

  rangeIndicator: {
    width: 8,
    height: 8,
    borderRadius: 4,
    marginRight: 9,
  },

  rangeNormal: {
    backgroundColor: '#16A34A',
  },

  rangeElevated: {
    backgroundColor: '#D97706',
  },

  rangeHigh: {
    backgroundColor: '#EA580C',
  },

  rangeVeryHigh: {
    backgroundColor: '#DC2626',
  },

  rangeLow: {
    backgroundColor: '#D97706',
  },

  rangeLabel: {
    width: 75,
    fontSize: 11,
    fontWeight: '800',
    color: '#334155',
  },

  rangeDescription: {
    fontSize: 11,
    color: '#64748B',
  },

  graphCard: {
    backgroundColor: '#FFFFFF',
    borderRadius: 18,
    padding: 16,
    marginBottom: 12,
    borderWidth: 1,
    borderColor: '#E5EAF0',
    overflow: 'hidden',
  },

  graphHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },

  graphTitle: {
    fontSize: 16,
    fontWeight: '800',
    color: '#0F172A',
  },

  graphSubtitle: {
    marginTop: 3,
    fontSize: 11,
    color: '#94A3B8',
  },

  readingCount: {
    minWidth: 30,
    paddingHorizontal: 8,
    paddingVertical: 5,
    textAlign: 'center',
    borderRadius: 8,
    backgroundColor: '#E0F2FE',
    color: '#0369A1',
    fontSize: 10,
    fontWeight: '800',
  },

  graphArea: {
    marginTop: 12,
    position: 'relative',
    width: '100%',
  },

  gridLine: {
    position: 'absolute',
    height: 1,
    backgroundColor: '#E2E8F0',
  },

  graphYLabel: {
    position: 'absolute',
    left: 0,
    width: 31,
    fontSize: 9,
    color: '#94A3B8',
    textAlign: 'right',
  },

  graphSegment: {
    position: 'absolute',
    height: 3,
    borderRadius: 2,
    backgroundColor: '#0EA5E9',
  },

  graphPoint: {
    position: 'absolute',
    width: 8,
    height: 8,
    borderRadius: 4,
    backgroundColor: '#FFFFFF',
    borderWidth: 2,
    borderColor: '#0EA5E9',
  },

  graphPointLast: {
    width: 12,
    height: 12,
    borderRadius: 6,
    backgroundColor: '#0EA5E9',
    borderWidth: 3,
    borderColor: '#BAE6FD',
  },

  emptyGraph: {
    height: 190,
    alignItems: 'center',
    justifyContent: 'center',
    paddingHorizontal: 30,
  },

  emptyGraphTitle: {
    fontSize: 14,
    fontWeight: '800',
    color: '#334155',
  },

  emptyGraphText: {
    marginTop: 6,
    textAlign: 'center',
    fontSize: 12,
    lineHeight: 18,
    color: '#94A3B8',
  },

  recentCard: {
    backgroundColor: '#FFFFFF',
    borderRadius: 18,
    padding: 16,
    marginBottom: 12,
    borderWidth: 1,
    borderColor: '#E5EAF0',
  },

  noReadingsText: {
    marginTop: 15,
    fontSize: 13,
    color: '#94A3B8',
  },

  readingRow: {
    flexDirection: 'row',
    alignItems: 'center',
    paddingVertical: 11,
    borderBottomWidth: 1,
    borderBottomColor: '#F1F5F9',
  },

  readingDot: {
    width: 7,
    height: 7,
    borderRadius: 4,
    backgroundColor: '#0EA5E9',
  },

  readingTime: {
    marginLeft: 10,
    fontSize: 12,
    color: '#64748B',
  },

  readingSpacer: {
    flex: 1,
  },

  readingValue: {
    fontSize: 17,
    fontWeight: '800',
    color: '#0F172A',
  },

  readingUnit: {
    marginLeft: 5,
    fontSize: 9,
    fontWeight: '800',
    color: '#94A3B8',
  },

  footer: {
    marginTop: 24,
    textAlign: 'center',
    fontSize: 10,
    color: '#94A3B8',
  },
});

/*
 * ============================================================
 * DARK THEME
 * ============================================================
 */

const darkStyles = StyleSheet.create({
  ...lightStyles,

  safeArea: {
    flex: 1,
    backgroundColor: '#080D16',
  },

  container: {
    flex: 1,
    backgroundColor: '#080D16',
  },

  brand: {
    ...lightStyles.brand,
    color: '#F8FAFC',
  },

  tagline: {
    ...lightStyles.tagline,
    color: '#94A3B8',
  },

  themeButton: {
    ...lightStyles.themeButton,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  themeButtonText: {
    ...lightStyles.themeButtonText,
    color: '#F8FAFC',
  },

  tabContainer: {
    ...lightStyles.tabContainer,
    backgroundColor: '#111827',
  },

  activeTab: {
    backgroundColor: '#1E293B',
  },

  tabText: {
    ...lightStyles.tabText,
    color: '#94A3B8',
  },

  activeTabText: {
    color: '#F8FAFC',
  },

  card: {
    ...lightStyles.card,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  sectionLabel: {
    ...lightStyles.sectionLabel,
    color: '#94A3B8',
  },

  deviceName: {
    ...lightStyles.deviceName,
    color: '#F8FAFC',
  },

  statusText: {
    ...lightStyles.statusText,
    color: '#94A3B8',
  },

  batteryValue: {
    ...lightStyles.batteryValue,
    color: '#F8FAFC',
  },

  batteryTrack: {
    ...lightStyles.batteryTrack,
    backgroundColor: '#1E293B',
  },

  mutedText: {
    ...lightStyles.mutedText,
    color: '#94A3B8',
  },

  featureTitle: {
    ...lightStyles.featureTitle,
    color: '#F8FAFC',
  },

  liveHrValue: {
    ...lightStyles.liveHrValue,
    color: '#F8FAFC',
  },

  liveHrUnit: {
    ...lightStyles.liveHrUnit,
    color: '#94A3B8',
  },

  secondaryButton: {
    ...lightStyles.secondaryButton,
    borderColor: '#334155',
  },

  secondaryButtonText: {
    ...lightStyles.secondaryButtonText,
    color: '#F8FAFC',
  },

  sectionHeading: {
    ...lightStyles.sectionHeading,
    color: '#94A3B8',
  },

  featureCard: {
    ...lightStyles.featureCard,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  featureDescription: {
    ...lightStyles.featureDescription,
    color: '#94A3B8',
  },

  telemetryCard: {
    ...lightStyles.telemetryCard,
    backgroundColor: '#111827',
  },

  telemetryValue: {
    ...lightStyles.telemetryValue,
    color: '#CBD5E1',
  },

  analysisTitle: {
    ...lightStyles.analysisTitle,
    color: '#F8FAFC',
  },

  analysisSubtitle: {
    ...lightStyles.analysisSubtitle,
    color: '#94A3B8',
  },

  statsRow: {
    ...lightStyles.statsRow,
  },

  statCard: {
    ...lightStyles.statCard,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  statValue: {
    ...lightStyles.statValue,
    color: '#F8FAFC',
  },

  rangeCard: {
    ...lightStyles.rangeCard,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  rangeLabel: {
    ...lightStyles.rangeLabel,
    color: '#CBD5E1',
  },

  rangeDescription: {
    ...lightStyles.rangeDescription,
    color: '#94A3B8',
  },

  graphCard: {
    ...lightStyles.graphCard,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  graphTitle: {
    ...lightStyles.graphTitle,
    color: '#F8FAFC',
  },

  gridLine: {
    ...lightStyles.gridLine,
    backgroundColor: '#1E293B',
  },

  graphYLabel: {
    ...lightStyles.graphYLabel,
    color: '#64748B',
  },

  emptyGraphTitle: {
    ...lightStyles.emptyGraphTitle,
    color: '#CBD5E1',
  },

  emptyGraphText: {
    ...lightStyles.emptyGraphText,
    color: '#64748B',
  },

  recentCard: {
    ...lightStyles.recentCard,
    backgroundColor: '#111827',
    borderColor: '#1E293B',
  },

  readingRow: {
    ...lightStyles.readingRow,
    borderBottomColor: '#1E293B',
  },

  readingValue: {
    ...lightStyles.readingValue,
    color: '#F8FAFC',
  },

  readingTime: {
    ...lightStyles.readingTime,
    color: '#94A3B8',
  },

  footer: {
    ...lightStyles.footer,
    color: '#475569',
  },
});