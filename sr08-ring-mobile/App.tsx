
import { StatusBar } from 'expo-status-bar';
import { useEffect, useRef, useState } from 'react';
import {
  Alert,
  PermissionsAndroid,
  Platform,
  Pressable,
  SafeAreaView,
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  View,
} from 'react-native';
import { BleManager, Device, State } from 'react-native-ble-plx';

/*
 * ============================================================
 * SR08 COMPANION APP
 * ============================================================
 *
 * BLE DEVICE:
 *      DLG-PRPH
 *
 * RING MAC PREFIX:
 *      48:23:35
 *
 * BATTERY:
 *      UUID:
 *      15005991-b131-3396-014c-664c9867b917
 *
 * Battery values supported:
 *
 *      Raw byte:
 *          0x55 -> 85
 *
 *      ASCII:
 *          "85" -> 85
 *
 *      Base64:
 *          VQ== -> 0x55 -> 85
 *
 * ============================================================
 */


/*
 * ============================================================
 * BLE UUIDs
 * ============================================================
 */

const RING_NAME = 'DLG-PRPH';

const RING_MAC_PREFIX = '48:23:35';

const DATA_SERVICE =
  '18424398-7cbc-11e9-8f9e-2a86e4085a59';

const DIGIT_CHARACTERISTIC =
  '2d86686a-53dc-25b3-0c4a-f0e10c8dee20';

const LETTER_CHARACTERISTIC =
  '5a87b4ef-3bfa-76a8-e642-92933c31434f';

const TELEMETRY_CHARACTERISTIC =
  '15005991-b131-3396-014c-664c9867b917';


/*
 * ============================================================
 * BASE64 ENCODING
 * ============================================================
 */

function encode(value: string) {
  return btoa(value);
}


/*
 * ============================================================
 * BATTERY DECODER
 * ============================================================
 *
 * react-native-ble-plx gives characteristic.value as Base64.
 *
 * Example:
 *
 *      Firmware sends:
 *
 *          0x55
 *
 *      BLE gives:
 *
 *          VQ==
 *
 *      Base64 decoded:
 *
 *          0x55
 *
 *      Decimal:
 *
 *          85
 *
 * Therefore:
 *
 *      VQ== -> 85%
 *
 * The function also supports:
 *
 *      "85"
 *      "100"
 *      0x55
 *      0x64
 *
 * ============================================================
 */

function decodeBattery(value: string): number | null {

  if (!value) {
    return null;
  }

  try {

    /*
     * Convert Base64 into raw binary bytes.
     */
    const binaryString = atob(value);

    const bytes = new Uint8Array(
      binaryString.length
    );

    for (
      let i = 0;
      i < binaryString.length;
      i++
    ) {
      bytes[i] =
        binaryString.charCodeAt(i);
    }


    /*
     * --------------------------------------------------------
     * CASE 1
     * --------------------------------------------------------
     *
     * Firmware sends ASCII:
     *
     *      "85"
     *
     * BLE Base64:
     *
     *      OD U=   (depending on value)
     *
     * Decode to readable text.
     */

    let asciiText = '';

    let allAscii = true;

    for (const byte of bytes) {

      if (
        byte >= 32 &&
        byte <= 126
      ) {

        asciiText +=
          String.fromCharCode(byte);

      } else {

        allAscii = false;

      }
    }


    if (allAscii) {

      asciiText =
        asciiText.trim();

      if (
        /^[0-9]+$/.test(
          asciiText
        )
      ) {

        const number =
          parseInt(
            asciiText,
            10
          );

        if (
          number >= 0 &&
          number <= 100
        ) {

          console.log(
            'Battery format: ASCII'
          );

          console.log(
            'ASCII battery:',
            asciiText
          );

          return number;
        }
      }
    }


    /*
     * --------------------------------------------------------
     * CASE 2
     * --------------------------------------------------------
     *
     * Firmware sends one raw byte.
     *
     * Example:
     *
     *      0x55
     *
     *      decimal = 85
     */

    if (bytes.length > 0) {

      const rawValue =
        bytes[0];

      console.log(
        'Battery format: RAW BYTE'
      );

      console.log(
        'Raw byte decimal:',
        rawValue
      );

      console.log(
        'Raw byte hex:',
        '0x' +
        rawValue
          .toString(16)
          .padStart(2, '0')
          .toUpperCase()
      );


      if (
        rawValue >= 0 &&
        rawValue <= 100
      ) {

        return rawValue;

      }
    }

    return null;

  } catch (error) {

    console.log(
      'Battery decode error:',
      error
    );

    return null;
  }
}


/*
 * ============================================================
 * GENERAL TELEMETRY DECODER
 * ============================================================
 *
 * Used for displaying LIVE TELEMETRY.
 *
 * Battery is handled separately by decodeBattery().
 *
 * ============================================================
 */

function decodeTelemetry(
  value: string
) {

  if (!value) {
    return '';
  }

  try {

    const binaryString =
      atob(value);

    const bytes =
      new Uint8Array(
        binaryString.length
      );

    for (
      let i = 0;
      i < binaryString.length;
      i++
    ) {

      bytes[i] =
        binaryString.charCodeAt(i);

    }


    /*
     * Try ASCII first.
     */

    let text = '';

    let validAscii = true;

    for (const byte of bytes) {

      if (
        byte >= 32 &&
        byte <= 126
      ) {

        text +=
          String.fromCharCode(byte);

      } else {

        validAscii = false;

      }
    }


    text = text.trim();


    if (
      validAscii &&
      text.length > 0
    ) {

      return text;

    }


    /*
     * Raw byte.
     */

    if (bytes.length > 0) {

      return bytes[0].toString();

    }

    return '';

  } catch (error) {

    console.log(
      'Telemetry decode error:',
      error
    );

    return value;
  }
}


/*
 * ============================================================
 * APP
 * ============================================================
 */

export default function App() {

  /*
   * BLE manager.
   */

  const manager =
    useRef(
      Platform.OS === 'web'
        ? null
        : new BleManager()
    ).current;


  /*
   * Prevent duplicate scanning.
   */

  const isScanningRef =
    useRef(false);


  /*
   * Prevent duplicate connection attempts.
   */

  const isConnectingRef =
    useRef(false);


  /*
   * Remember devices already seen.
   *
   * This prevents console spam such as:
   *
   * DLG-PRPH
   * DLG-PRPH
   * DLG-PRPH
   * DLG-PRPH
   */

  const seenDevicesRef =
    useRef(
      new Set<string>()
    );


  /*
   * BLE notification subscription.
   */

  const telemetrySubscriptionRef =
    useRef<any>(null);


  /*
   * Device state.
   */

  const [device, setDevice] =
    useState<Device | null>(null);


  /*
   * Status.
   */

  const [status, setStatus] =
    useState(
      'Ready to scan'
    );


  /*
   * Battery.
   */

  const [battery, setBattery] =
    useState<number | null>(null);


  /*
   * Digit.
   */

  const [digit, setDigit] =
    useState('7');


  /*
   * Letter.
   */

  const [letter, setLetter] =
    useState('A');


  /*
   * Telemetry text.
   */

  const [telemetry, setTelemetry] =
    useState(
      'No sensor data yet'
    );


  /*
   * Theme.
   */

  const [isDarkMode, setIsDarkMode] =
    useState(true);


  /*
   * ==========================================================
   * BLUETOOTH STATE LISTENER
   * ==========================================================
   */

  useEffect(() => {

    if (!manager) {
      return;
    }


    const subscription =
      manager.onStateChange(
        (state) => {

          console.log(
            'Bluetooth state:',
            state
          );


          /*
           * Bluetooth OFF.
           */

          if (
            state !==
            State.PoweredOn
          ) {

            /*
             * Stop scanning.
             */

            manager.stopDeviceScan();

            isScanningRef.current =
              false;

            isConnectingRef.current =
              false;


            /*
             * Remove notification.
             */

            if (
              telemetrySubscriptionRef.current
            ) {

              telemetrySubscriptionRef
                .current
                .remove();

              telemetrySubscriptionRef
                .current = null;
            }


            setDevice(null);

            setBattery(null);

            setTelemetry(
              'No sensor data yet'
            );

            setStatus(
              'Bluetooth is turned off'
            );

          }

        },
        true
      );


    /*
     * Cleanup.
     */

    return () => {

      subscription.remove();

      manager.stopDeviceScan();

      if (
        telemetrySubscriptionRef.current
      ) {

        telemetrySubscriptionRef
          .current
          .remove();

        telemetrySubscriptionRef
          .current = null;

      }

    };

  }, [manager]);


  /*
   * ==========================================================
   * DISCONNECT
   * ==========================================================
   */

  const disconnectDevice =
    async () => {

      /*
       * Stop scanner.
       */

      manager?.stopDeviceScan();

      isScanningRef.current =
        false;

      isConnectingRef.current =
        false;


      /*
       * Remove telemetry monitor.
       */

      if (
        telemetrySubscriptionRef.current
      ) {

        telemetrySubscriptionRef
          .current
          .remove();

        telemetrySubscriptionRef
          .current = null;

      }


      /*
       * Disconnect BLE device.
       */

      if (device) {

        try {

          await manager?.cancelDeviceConnection(
            device.id
          );

        } catch (error) {

          console.log(
            'Disconnect error:',
            error
          );

        }

      }


      /*
       * Reset UI.
       */

      setDevice(null);

      setBattery(null);

      setTelemetry(
        'No sensor data yet'
      );

      setStatus(
        'Ready to scan'
      );


      /*
       * Clear duplicate device cache.
       */

      seenDevicesRef.current.clear();
    };


  /*
   * ==========================================================
   * CONNECT TO RING
   * ==========================================================
   */

  const connectToRing =
    async (
      scanned: Device
    ) => {

      /*
       * Prevent duplicate connection.
       */

      if (
        isConnectingRef.current
      ) {

        return;

      }


      isConnectingRef.current =
        true;


      const displayName =
        scanned.name ||
        scanned.localName ||
        RING_NAME;


      try {

        /*
         * Connect.
         */

        console.log(
          'Connecting to ring:',
          displayName,
          scanned.id
        );


        setStatus(
          `Connecting to ${displayName}...`
        );


        const connected =
          await scanned.connect();


        /*
         * Discover GATT.
         */

        await connected
          .discoverAllServicesAndCharacteristics();


        /*
         * Save device.
         */

        setDevice(
          connected
        );


        setStatus(
          `Connected to ${
            connected.name ||
            RING_NAME
          }`
        );


        console.log(
          'Connected device:',
          connected.id
        );


        /*
         * ====================================================
         * SERVICES
         * ====================================================
         */

        const services =
          await connected.services();


        console.log(
          'BLE SERVICES:',
          services.map(
            service =>
              service.uuid
          )
        );


        /*
         * ====================================================
         * CHARACTERISTICS
         * ====================================================
         */

        for (
          const service of services
        ) {

          try {

            const characteristics =
              await connected
                .characteristicsForService(
                  service.uuid
                );


            console.log(
              `CHARACTERISTICS FOR ${service.uuid}:`,
              characteristics.map(
                characteristic => ({
                  uuid:
                    characteristic.uuid,

                  isReadable:
                    characteristic.isReadable,

                  isWritableWithResponse:
                    characteristic
                      .isWritableWithResponse,

                  isWritableWithoutResponse:
                    characteristic
                      .isWritableWithoutResponse,

                  isNotifiable:
                    characteristic.isNotifiable,

                  isIndicatable:
                    characteristic.isIndicatable,
                })
              )
            );

          } catch (
            characteristicError
          ) {

            console.log(
              'Characteristic discovery error:',
              characteristicError
            );

          }

        }


        /*
         * ====================================================
         * BATTERY / TELEMETRY MONITOR
         * ====================================================
         */

        console.log(
          'Starting telemetry monitor:',
          TELEMETRY_CHARACTERISTIC
        );


        /*
         * Remove old monitor first.
         */

        if (
          telemetrySubscriptionRef.current
        ) {

          telemetrySubscriptionRef
            .current
            .remove();

          telemetrySubscriptionRef
            .current = null;

        }


        telemetrySubscriptionRef.current =
          connected.monitorCharacteristicForService(
            DATA_SERVICE,
            TELEMETRY_CHARACTERISTIC,

            (
              monitorError,
              characteristic
            ) => {

              /*
               * Notification error.
               */

              if (
                monitorError
              ) {

                console.log(
                  'Telemetry monitor error:',
                  monitorError
                );

                setTelemetry(
                  `ERROR: ${monitorError.message}`
                );

                return;
              }


              /*
               * Characteristic missing.
               */

              if (
                !characteristic
              ) {

                console.log(
                  'Telemetry notification returned no characteristic'
                );

                return;
              }


              /*
               * Value missing.
               */

              if (
                !characteristic.value
              ) {

                console.log(
                  'Telemetry characteristic has no value'
                );

                return;
              }


              /*
               * =================================================
               * RAW BLE DATA
               * =================================================
               */

              console.log(
                '================================'
              );


              console.log(
                'BLE TELEMETRY UUID:',
                characteristic.uuid
              );


              console.log(
                'BLE TELEMETRY BASE64:',
                characteristic.value
              );


              /*
               * =================================================
               * BATTERY CONVERSION
               * =================================================
               */

              const batteryValue =
                decodeBattery(
                  characteristic.value
                );


              console.log(
                'BLE BATTERY RESULT:',
                batteryValue
              );


              /*
               * Update battery UI.
               */

              if (
                batteryValue !== null
              ) {

                console.log(
                  'BATTERY VALUE:',
                  batteryValue,
                  '%'
                );


                setBattery(
                  batteryValue
                );

              } else {

                console.log(
                  'Could not interpret notification as battery.'
                );

              }


              /*
               * =================================================
               * TELEMETRY DISPLAY
               * =================================================
               */

              const decodedTelemetry =
                decodeTelemetry(
                  characteristic.value
                );


              console.log(
                'BLE TELEMETRY DECODED:',
                decodedTelemetry
              );


              setTelemetry(
                decodedTelemetry
              );


              console.log(
                '================================'
              );

            }
          );


      } catch (
        connectionError
      ) {

        console.log(
          'Connection error:',
          connectionError
        );


        setStatus(
          connectionError instanceof Error
            ? connectionError.message
            : 'Connection failed'
        );


        /*
         * If connection failed,
         * allow another attempt.
         */

        isConnectingRef.current =
          false;

      }

    };


  /*
   * ==========================================================
   * SCAN + CONNECT
   * ==========================================================
   */

  const scanAndConnect =
    async () => {

      /*
       * Already connected:
       * button becomes Disconnect.
       */

      if (device) {

        await disconnectDevice();

        return;
      }


      /*
       * BLE unavailable on web.
       */

      if (!manager) {

        setStatus(
          'BLE is available in the Android/iOS build'
        );

        return;

      }


      /*
       * Already scanning.
       */

      if (
        isScanningRef.current
      ) {

        console.log(
          'Scan already running.'
        );

        return;

      }


      /*
       * Already connecting.
       */

      if (
        isConnectingRef.current
      ) {

        console.log(
          'Connection already in progress.'
        );

        return;

      }


      /*
       * ======================================================
       * ANDROID PERMISSIONS
       * ======================================================
       */

      if (
        Platform.OS === 'android'
      ) {

        const apiLevel =
          Platform.Version;


        /*
         * Android 12+
         */

        if (
          typeof apiLevel === 'number' &&
          apiLevel >= 31
        ) {

          const result =
            await PermissionsAndroid
              .requestMultiple([
                PermissionsAndroid
                  .PERMISSIONS
                  .BLUETOOTH_SCAN,

                PermissionsAndroid
                  .PERMISSIONS
                  .BLUETOOTH_CONNECT,

                PermissionsAndroid
                  .PERMISSIONS
                  .ACCESS_FINE_LOCATION,
              ]);


          const scanGranted =
            result[
              'android.permission.BLUETOOTH_SCAN'
            ] ===
            PermissionsAndroid.RESULTS.GRANTED;


          const connectGranted =
            result[
              'android.permission.BLUETOOTH_CONNECT'
            ] ===
            PermissionsAndroid.RESULTS.GRANTED;


          if (
            !scanGranted ||
            !connectGranted
          ) {

            setStatus(
              'Bluetooth permissions denied'
            );


            Alert.alert(
              'Permission required',
              'Please allow Bluetooth permissions to scan for the ring.'
            );


            return;
          }


        } else {

          /*
           * Android < 12
           */

          const granted =
            await PermissionsAndroid
              .request(
                PermissionsAndroid
                  .PERMISSIONS
                  .ACCESS_FINE_LOCATION
              );


          if (
            granted !==
            PermissionsAndroid.RESULTS.GRANTED
          ) {

            setStatus(
              'Location permission denied'
            );

            return;

          }

        }

      }


      /*
       * ======================================================
       * RESET SCAN CACHE
       * ======================================================
       */

      seenDevicesRef.current.clear();


      /*
       * ======================================================
       * START SCAN
       * ======================================================
       */

      isScanningRef.current =
        true;


      setStatus(
        'Scanning for DLG-PRPH...'
      );


      console.log(
        '================================'
      );


      console.log(
        'Starting BLE scan...'
      );


      /*
       * IMPORTANT:
       *
       * allowDuplicates = false
       *
       * This prevents the same BLE device
       * from being reported repeatedly.
       */

      manager.startDeviceScan(
        null,
        {
          allowDuplicates: false,
        },

        async (
          error,
          scanned
        ) => {

          /*
           * Scan error.
           */

          if (error) {

            console.log(
              'BLE scan error:',
              error
            );


            manager.stopDeviceScan();


            isScanningRef.current =
              false;


            setStatus(
              error.message
            );


            return;
          }


          /*
           * No device.
           */

          if (!scanned) {
            return;
          }


          /*
           * ==================================================
           * DUPLICATE DEVICE FILTER
           * ==================================================
           */

          if (
            seenDevicesRef.current.has(
              scanned.id
            )
          ) {

            return;

          }


          seenDevicesRef.current.add(
            scanned.id
          );


          /*
           * Device name.
           */

          const localName =
            scanned.name ||
            scanned.localName ||
            '';


          /*
           * ==================================================
           * ONLY LOG DEVICES WE CARE ABOUT
           * ==================================================
           *
           * This means your console will not get spammed
           * with random Bluetooth devices.
           */

          const nameMatches =
            localName
              .toLowerCase()
              .includes(
                RING_NAME.toLowerCase()
              );


          const macMatches =
            scanned.id
              .toUpperCase()
              .startsWith(
                RING_MAC_PREFIX
              );


          /*
           * Ignore unrelated devices.
           *
           * Therefore:
           *
           * DEKSTOP-DION
           *
           * will not be printed.
           */

          if (
            !nameMatches &&
            !macMatches
          ) {

            return;

          }


          /*
           * ==================================================
           * RING FOUND
           * ==================================================
           */

          console.log(
            'FOUND SR08 RING:',
            localName || RING_NAME,
            scanned.id
          );


          /*
           * Stop scanning immediately.
           */

          if (
            isScanningRef.current
          ) {

            isScanningRef.current =
              false;

            manager.stopDeviceScan();

          }


          /*
           * Connect.
           */

          await connectToRing(
            scanned
          );

        }
      );

    };


  /*
   * ==========================================================
   * WRITE VALUE
   * ==========================================================
   */

  const writeValue =
    async (
      characteristic: string,
      value: string
    ) => {

      /*
       * Not connected.
       */

      if (!device) {

        Alert.alert(
          'Not connected',
          'Connect to the ring first.'
        );

        return;

      }


      /*
       * Empty value.
       */

      if (!value) {

        Alert.alert(
          'No value',
          'Enter a value first.'
        );

        return;

      }


      try {

        console.log(
          'Writing:',
          value
        );


        console.log(
          'Characteristic:',
          characteristic
        );


        await device
          .writeCharacteristicWithResponseForService(
            DATA_SERVICE,
            characteristic,
            encode(value)
          );


        console.log(
          'Write successful'
        );


        setStatus(
          `Sent ${value}`
        );


      } catch (error) {

        console.log(
          'Write failed:',
          error
        );


        Alert.alert(
          'Write failed',
          error instanceof Error
            ? error.message
            : 'The ring rejected the value.'
        );

      }

    };


  /*
   * ==========================================================
   * THEME
   * ==========================================================
   */

  const currentTheme =
    isDarkMode
      ? darkStyles
      : lightStyles;


  /*
   * ==========================================================
   * UI
   * ==========================================================
   */

  return (

    <SafeAreaView
      style={[
        styles.safe,
        currentTheme.safe
      ]}
    >

      <ScrollView
        contentContainerStyle={
          styles.container
        }
      >

        {/* ==================================================
            HEADER
        ================================================== */}

        <View
          style={styles.header}
        >

          <Text
            style={[
              styles.kicker,
              currentTheme.kicker
            ]}
          >
            SR08 / COMPANION
          </Text>


          <View
            style={styles.titleRow}
          >

            <Text
              style={[
                styles.title,
                currentTheme.title
              ]}
            >
              Your ring, in rhythm.
            </Text>


            <Pressable
              style={[
                styles.themeToggle,
                currentTheme.themeToggle
              ]}
              onPress={() =>
                setIsDarkMode(
                  !isDarkMode
                )
              }
            >

              <Text
                style={[
                  styles.themeToggleText,
                  currentTheme.themeToggleText
                ]}
              >
                {isDarkMode
                  ? '☀️ Light'
                  : '🌙 Dark'}
              </Text>

            </Pressable>

          </View>


          <Text
            style={[
              styles.subtitle,
              currentTheme.subtitle
            ]}
          >
            A quiet control surface for the
            hardware on your finger.
          </Text>

        </View>


        {/* ==================================================
            CONNECTION
        ================================================== */}

        <View
          style={[
            styles.connectionCard,
            currentTheme.connectionCard
          ]}
        >

          <View>

            <Text
              style={[
                styles.label,
                currentTheme.label
              ]}
            >
              DEVICE
            </Text>


            <Text
              style={[
                styles.statusText,
                currentTheme.statusText
              ]}
            >
              {
                device
                  ? `Connected (${
                      device.name ||
                      RING_NAME
                    })`
                  : status
              }
            </Text>

          </View>


          <Pressable
            style={[
              styles.primaryButton,
              currentTheme.primaryButton,

              device &&
              (
                isDarkMode
                  ? styles.disconnectButtonDark
                  : styles.disconnectButtonLight
              )
            ]}
            onPress={scanAndConnect}
          >

            <Text
              style={[
                styles.buttonText,
                currentTheme.buttonText
              ]}
            >
              {
                device
                  ? 'Disconnect'
                  : 'Scan ring'
              }
            </Text>

          </Pressable>

        </View>


        {/* ==================================================
            BATTERY
        ================================================== */}

        <View
          style={[
            styles.batteryCard,
            currentTheme.batteryCard
          ]}
        >

          <View
            style={styles.cardHeader}
          >

            <Text
              style={[
                styles.label,
                currentTheme.label
              ]}
            >
              BATTERY
            </Text>


            <Text
              style={[
                styles.batteryValue,
                currentTheme.batteryValue
              ]}
            >

              {
                battery === null
                  ? '--'
                  : battery
              }

              <Text
                style={[
                  styles.percent,
                  currentTheme.percent
                ]}
              >
                %
              </Text>

            </Text>

          </View>


          <View
            style={[
              styles.batteryBarBackground,
              currentTheme.batteryBarBackground
            ]}
          >

            <View
              style={[
                styles.batteryFill,
                currentTheme.batteryFill,
                {
                  width:
                    `${battery ?? 0}%`
                }
              ]}
            />

          </View>

        </View>


        {/* ==================================================
            RING CONTROLS
        ================================================== */}

        <Text
          style={[
            styles.sectionTitle,
            currentTheme.sectionTitle
          ]}
        >
          Ring controls
        </Text>


        {/* DIGIT */}

        <View
          style={[
            styles.controlCard,
            currentTheme.controlCard
          ]}
        >

          <View
            style={styles.controlHeader}
          >

            <Text
              style={[
                styles.controlTitle,
                currentTheme.controlTitle
              ]}
            >
              Send one digit
            </Text>


            <Text
              style={[
                styles.characterCount,
                currentTheme.characterCount
              ]}
            >
              1 / 1
            </Text>

          </View>


          <View
            style={styles.inputRow}
          >

            <TextInput
              value={digit}

              onChangeText={
                (value) =>
                  setDigit(
                    value
                      .replace(
                        /[^0-9]/g,
                        ''
                      )
                      .slice(
                        0,
                        1
                      )
                  )
              }

              keyboardType="number-pad"

              maxLength={1}

              style={[
                styles.input,
                currentTheme.input
              ]}
            />


            <Pressable
              style={[
                styles.secondaryButton,
                currentTheme.secondaryButton
              ]}
              onPress={() =>
                writeValue(
                  DIGIT_CHARACTERISTIC,
                  digit
                )
              }
            >

              <Text
                style={[
                  styles.secondaryText,
                  currentTheme.secondaryText
                ]}
              >
                Send
              </Text>

            </Pressable>

          </View>

        </View>


        {/* LETTER */}

        <View
          style={[
            styles.controlCard,
            currentTheme.controlCard
          ]}
        >

          <View
            style={styles.controlHeader}
          >

            <Text
              style={[
                styles.controlTitle,
                currentTheme.controlTitle
              ]}
            >
              Send one letter
            </Text>


            <Text
              style={[
                styles.characterCount,
                currentTheme.characterCount
              ]}
            >
              1 / 1
            </Text>

          </View>


          <View
            style={styles.inputRow}
          >

            <TextInput
              value={letter}

              onChangeText={
                (value) =>
                  setLetter(
                    value
                      .replace(
                        /[^a-z]/gi,
                        ''
                      )
                      .slice(
                        0,
                        1
                      )
                      .toUpperCase()
                  )
              }

              maxLength={1}

              autoCapitalize="characters"

              style={[
                styles.input,
                currentTheme.input
              ]}
            />


            <Pressable
              style={[
                styles.secondaryButton,
                currentTheme.secondaryButton
              ]}
              onPress={() =>
                writeValue(
                  LETTER_CHARACTERISTIC,
                  letter
                )
              }
            >

              <Text
                style={[
                  styles.secondaryText,
                  currentTheme.secondaryText
                ]}
              >
                Send
              </Text>

            </Pressable>

          </View>

        </View>


        {/* ==================================================
            COMING ONLINE
        ================================================== */}

        <Text
          style={[
            styles.sectionTitle,
            currentTheme.sectionTitle
          ]}
        >
          Coming online
        </Text>


        <View
          style={[
            styles.futureRow,
            currentTheme.futureRow
          ]}
        >

          <View>

            <Text
              style={[
                styles.futureTitle,
                currentTheme.futureTitle
              ]}
            >
              Heart rate
            </Text>


            <Text
              style={[
                styles.futureText,
                currentTheme.futureText
              ]}
            >
              Sensor stream will appear here
            </Text>

          </View>


          <Text
            style={[
              styles.futureValue,
              currentTheme.futureValue
            ]}
          >
            -- BPM
          </Text>

        </View>


        <View
          style={[
            styles.futureRow,
            currentTheme.futureRow
          ]}
        >

          <View>

            <Text
              style={[
                styles.futureTitle,
                currentTheme.futureTitle
              ]}
            >
              Sleep timer
            </Text>


            <Text
              style={[
                styles.futureText,
                currentTheme.futureText
              ]}
            >
              Overnight state tracking
            </Text>

          </View>


          <Text
            style={[
              styles.futureValue,
              currentTheme.futureValue
            ]}
          >
            --:--
          </Text>

        </View>


        {/* ==================================================
            LIVE TELEMETRY
        ================================================== */}

        <Text
          style={[
            styles.telemetry,
            currentTheme.telemetry
          ]}
        >
          LIVE TELEMETRY: {telemetry}
        </Text>

      </ScrollView>


      <StatusBar
        style={
          isDarkMode
            ? 'light'
            : 'dark'
        }
      />

    </SafeAreaView>
  );
}


/*
 * ============================================================
 * GENERAL STYLES
 * ============================================================
 */

const styles = StyleSheet.create({

  safe: {
    flex: 1,
  },


  container: {
    padding: 24,
    paddingBottom: 48,
    gap: 16,
  },


  header: {
    paddingTop: 20,
    paddingBottom: 8,
  },


  titleRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'flex-start',
    marginTop: 6,
  },


  kicker: {
    fontSize: 12,
    fontWeight: '800',
    letterSpacing: 2,
  },


  title: {
    fontSize: 36,
    fontWeight: '800',
    flex: 1,
    marginRight: 10,
  },


  subtitle: {
    fontSize: 14,
    lineHeight: 20,
    marginTop: 6,
    maxWidth: 320,
  },


  themeToggle: {
    borderRadius: 8,
    paddingVertical: 8,
    paddingHorizontal: 12,
    borderWidth: 1,
  },


  themeToggleText: {
    fontSize: 12,
    fontWeight: '700',
  },


  connectionCard: {
    borderRadius: 16,
    padding: 20,
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    borderWidth: 1,
  },


  label: {
    fontSize: 10,
    fontWeight: '800',
    letterSpacing: 1.5,
  },


  statusText: {
    fontSize: 13,
    marginTop: 6,
    maxWidth: 180,
  },


  primaryButton: {
    borderRadius: 12,
    paddingVertical: 12,
    paddingHorizontal: 18,
  },


  buttonText: {
    fontWeight: '800',
    fontSize: 13,
  },


  batteryCard: {
    borderRadius: 16,
    padding: 20,
    gap: 14,
    borderWidth: 1,
  },


  cardHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },


  batteryValue: {
    fontSize: 32,
    fontWeight: '800',
  },


  percent: {
    fontSize: 18,
  },


  batteryBarBackground: {
    height: 8,
    borderRadius: 4,
    overflow: 'hidden',
  },


  batteryFill: {
    height: '100%',
  },


  sectionTitle: {
    fontSize: 18,
    fontWeight: '800',
    marginTop: 10,
  },


  controlCard: {
    borderRadius: 16,
    padding: 20,
    borderWidth: 1,
  },


  controlHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },


  controlTitle: {
    fontSize: 15,
    fontWeight: '700',
  },


  characterCount: {
    fontSize: 12,
  },


  inputRow: {
    flexDirection: 'row',
    gap: 12,
    marginTop: 14,
  },


  input: {
    borderRadius: 12,
    fontSize: 20,
    fontWeight: '700',
    paddingHorizontal: 16,
    height: 48,
    flex: 1,
    borderWidth: 1,
  },


  secondaryButton: {
    borderRadius: 12,
    justifyContent: 'center',
    paddingHorizontal: 22,
  },


  secondaryText: {
    fontWeight: '800',
    fontSize: 14,
  },


  futureRow: {
    borderRadius: 16,
    padding: 18,
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    borderWidth: 1,
  },


  futureTitle: {
    fontWeight: '700',
    fontSize: 15,
  },


  futureText: {
    marginTop: 4,
    fontSize: 12,
  },


  futureValue: {
    fontWeight: '800',
    fontSize: 14,
  },


  telemetry: {
    fontSize: 10,
    letterSpacing: 1.2,
    marginTop: 10,
  },

});


/*
 * ============================================================
 * DARK THEME
 * ============================================================
 */

const darkStyles = StyleSheet.create({

  safe: {
    backgroundColor: '#18161B',
  },


  kicker: {
    color: '#D4A373',
  },


  title: {
    color: '#F4F1EA',
  },


  subtitle: {
    color: '#9E98A0',
  },


  themeToggle: {
    backgroundColor: '#26222B',
    borderColor: '#342F3A',
  },


  themeToggleText: {
    color: '#F4F1EA',
  },


  connectionCard: {
    backgroundColor: '#26222B',
    borderColor: '#342F3A',
  },


  label: {
    color: '#9E98A0',
  },


  statusText: {
    color: '#F4F1EA',
  },


  primaryButton: {
    backgroundColor: '#8C4A5D',
  },


  disconnectButtonDark: {
    backgroundColor: '#3D2F36',
  },


  buttonText: {
    color: '#F4F1EA',
  },


  batteryCard: {
    backgroundColor: '#26222B',
    borderColor: '#342F3A',
  },


  batteryValue: {
    color: '#F4F1EA',
  },


  percent: {
    color: '#D4A373',
  },


  batteryBarBackground: {
    backgroundColor: '#18161B',
  },


  batteryFill: {
    backgroundColor: '#D4A373',
  },


  sectionTitle: {
    color: '#F4F1EA',
  },


  controlCard: {
    backgroundColor: '#26222B',
    borderColor: '#342F3A',
  },


  controlTitle: {
    color: '#F4F1EA',
  },


  characterCount: {
    color: '#7A747D',
  },


  input: {
    backgroundColor: '#18161B',
    color: '#F4F1EA',
    borderColor: '#342F3A',
  },


  secondaryButton: {
    backgroundColor: '#8C4A5D',
  },


  secondaryText: {
    color: '#F4F1EA',
  },


  futureRow: {
    backgroundColor: '#26222B',
    borderColor: '#342F3A',
  },


  futureTitle: {
    color: '#F4F1EA',
  },


  futureText: {
    color: '#7A747D',
  },


  futureValue: {
    color: '#7A747D',
  },


  telemetry: {
    color: '#7A747D',
  },

});


/*
 * ============================================================
 * LIGHT THEME
 * ============================================================
 */

const lightStyles = StyleSheet.create({

  safe: {
    backgroundColor: '#f4f1ea',
  },


  kicker: {
    color: '#c4512c',
  },


  title: {
    color: '#1c2522',
  },


  subtitle: {
    color: '#68716e',
  },


  themeToggle: {
    backgroundColor: '#fffaf3',
    borderColor: '#e6e0d7',
  },


  themeToggleText: {
    color: '#1c2522',
  },


  connectionCard: {
    backgroundColor: '#1c2522',
    borderColor: '#1c2522',
  },


  label: {
    color: '#95a19d',
  },


  statusText: {
    color: '#f4f1ea',
  },


  primaryButton: {
    backgroundColor: '#e4734b',
  },


  disconnectButtonLight: {
    backgroundColor: '#5c2c1c',
  },


  buttonText: {
    color: '#fffaf3',
  },


  batteryCard: {
    backgroundColor: '#fffaf3',
    borderColor: '#e6e0d7',
  },


  batteryValue: {
    color: '#1c2522',
  },


  percent: {
    color: '#e4734b',
  },


  batteryBarBackground: {
    backgroundColor: '#e6e0d7',
  },


  batteryFill: {
    backgroundColor: '#e4734b',
  },


  sectionTitle: {
    color: '#1c2522',
  },


  controlCard: {
    backgroundColor: '#fffaf3',
    borderColor: '#e6e0d7',
  },


  controlTitle: {
    color: '#1c2522',
  },


  characterCount: {
    color: '#9ca39f',
  },


  input: {
    backgroundColor: '#f0ebe2',
    color: '#1c2522',
    borderColor: '#e6e0d7',
  },


  secondaryButton: {
    backgroundColor: 'transparent',
    borderColor: '#c4512c',
    borderWidth: 1,
  },


  secondaryText: {
    color: '#c4512c',
  },


  futureRow: {
    backgroundColor: '#e7e5dc',
    borderColor: '#e7e5dc',
  },


  futureTitle: {
    color: '#1c2522',
  },


  futureText: {
    color: '#737c78',
  },


  futureValue: {
    color: '#7e8984',
  },


  telemetry: {
    color: '#9ca39f',
  },

});

