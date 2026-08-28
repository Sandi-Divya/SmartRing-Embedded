import { StatusBar } from 'expo-status-bar';
import { useEffect, useRef, useState } from 'react';
import { Alert, PermissionsAndroid, Platform, Pressable, SafeAreaView, ScrollView, StyleSheet, Text, TextInput, View } from 'react-native';
import { BleManager, Device, State } from 'react-native-ble-plx';

const RING_NAME = 'DLG-PRPH';
const DATA_SERVICE = '18424398-7cbc-11e9-8f9e-2a86e4085a59';
const DIGIT_CHARACTERISTIC = '2d86686a-53dc-25b3-0c4a-f0e10c8dee20';
const LETTER_CHARACTERISTIC = '5a87b4ef-3bfa-76a8-e642-92933c31434f';
const TELEMETRY_CHARACTERISTIC = '15005991-b131-3396-014c-664c9867b917';

function encode(value: string) {
  return btoa(value);
}

function decode(value: string) {
  if (!value) return '';
  try {
    const binaryString = atob(value);
    const bytes = new Uint8Array(binaryString.length);
    for (let i = 0; i < binaryString.length; i++) {
      bytes[i] = binaryString.charCodeAt(i);
    }
    // Check if it's already plain ASCII text (like numbers or strings) or raw byte data
    const textDecoder = new TextDecoder('utf-8');
    const decodedText = textDecoder.decode(bytes).trim();
    if (/^[\x20-\x7E]*$/.test(decodedText) && decodedText.length > 0) {
      return decodedText;
    }
    return bytes[0].toString();
  } catch (e) {
    return value;
  }
}

export default function App() {
  const manager = useRef(Platform.OS === 'web' ? null : new BleManager()).current;
  const isScanningRef = useRef(false);
  const [device, setDevice] = useState<Device | null>(null);
  const [status, setStatus] = useState('Ready to scan');
  const [battery, setBattery] = useState<number | null>(null);
  const [digit, setDigit] = useState('7');
  const [letter, setLetter] = useState('A');
  const [telemetry, setTelemetry] = useState('No sensor data yet');
  const [isDarkMode, setIsDarkMode] = useState(true);

  useEffect(() => {
    if (!manager) return;
    const subscription = manager.onStateChange((state) => {
      if (state !== State.PoweredOn) {
        setDevice(null);
        setStatus('Bluetooth is turned off');
        isScanningRef.current = false;
      }
    }, true);

    return () => {
      subscription.remove();
    };
  }, [manager]);

  const disconnectDevice = async () => {
    if (device) {
      try {
        await manager?.cancelDeviceConnection(device.id);
      } catch (e) {
        console.log('Disconnect error:', e);
      }
    }
    setDevice(null);
    setBattery(null);
    setStatus('Ready to scan');
    isScanningRef.current = false;
  };

  const scanAndConnect = async () => {
    if (device) {
      disconnectDevice();
      return;
    }

    if (!manager) {
      setStatus('BLE is available in the Android/iOS build');
      return;
    }

    if (isScanningRef.current) return;

    if (Platform.OS === 'android') {
      const apiLevel = Platform.Version;
      if (typeof apiLevel === 'number' && apiLevel >= 31) {
        const result = await PermissionsAndroid.requestMultiple([
          PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
          PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
          PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
        ]);

        const scanGranted = result['android.permission.BLUETOOTH_SCAN'] === PermissionsAndroid.RESULTS.GRANTED;
        const connectGranted = result['android.permission.BLUETOOTH_CONNECT'] === PermissionsAndroid.RESULTS.GRANTED;

        if (!scanGranted || !connectGranted) {
          setStatus('Bluetooth permissions denied');
          Alert.alert('Permission required', 'Please allow Bluetooth and location permissions to scan.');
          return;
        }
      } else {
        const granted = await PermissionsAndroid.request(
          PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION
        );
        if (granted !== PermissionsAndroid.RESULTS.GRANTED) {
          setStatus('Location permission denied');
          return;
        }
      }
    }

    isScanningRef.current = true;
    setStatus('Scanning for devices...');
    
    manager.startDeviceScan(null, { allowDuplicates: true }, async (error, scanned) => {
      if (error) { 
        setStatus(error.message); 
        manager.stopDeviceScan(); 
        isScanningRef.current = false;
        return; 
      }
      
      const localName = scanned?.name || scanned?.localName || '';
      const matchesTarget = 
        (localName && localName.toLowerCase().includes(RING_NAME.toLowerCase())) ||
        (scanned?.id && scanned.id.startsWith('48:23:35'));

      if (matchesTarget && isScanningRef.current) {
        isScanningRef.current = false;
        manager.stopDeviceScan();
        setStatus(`Connecting to ${localName || RING_NAME}...`);

        try {
          const connected = await scanned.connect();
          await connected.discoverAllServicesAndCharacteristics();
          setDevice(connected);
          setStatus(`Connected to ${connected.name ?? RING_NAME}`);

          connected.monitorCharacteristicForService(DATA_SERVICE, TELEMETRY_CHARACTERISTIC, (monitorError, characteristic) => {
            if (!monitorError && characteristic?.value) {
              const decodedVal = decode(characteristic.value);
              setTelemetry(decodedVal);
              
              const parsedNum = parseInt(decodedVal, 10);
              if (!isNaN(parsedNum) && parsedNum >= 0 && parsedNum <= 100) {
                setBattery(parsedNum);
              }
            }
          });
        } catch (connectionError) { 
          isScanningRef.current = false;
          setStatus(connectionError instanceof Error ? connectionError.message : 'Connection failed'); 
        }
      }
    });
  };

  const writeValue = async (characteristic: string, value: string) => {
    if (!device) return;
    try {
      await device.writeCharacteristicWithResponseForService(DATA_SERVICE, characteristic, encode(value));
      setStatus(`Sent ${value}`);
    } catch (error) { Alert.alert('Write failed', error instanceof Error ? error.message : 'The ring rejected the value.'); }
  };

  const currentTheme = isDarkMode ? darkStyles : lightStyles;

  return (
    <SafeAreaView style={[styles.safe, currentTheme.safe]}>
      <ScrollView contentContainerStyle={styles.container}>
        <View style={styles.header}>
          <Text style={[styles.kicker, currentTheme.kicker]}>SR08 / COMPANION</Text>
          <View style={styles.titleRow}>
            <Text style={[styles.title, currentTheme.title]}>Your ring, in rhythm.</Text>
            <Pressable 
              style={[styles.themeToggle, currentTheme.themeToggle]} 
              onPress={() => setIsDarkMode(!isDarkMode)}
            >
              <Text style={[styles.themeToggleText, currentTheme.themeToggleText]}>
                {isDarkMode ? '☀️ Light' : '🌙 Dark'}
              </Text>
            </Pressable>
          </View>
          <Text style={[styles.subtitle, currentTheme.subtitle]}>A quiet control surface for the hardware on your finger.</Text>
        </View>

        <View style={[styles.connectionCard, currentTheme.connectionCard]}>
          <View>
            <Text style={[styles.label, currentTheme.label]}>DEVICE</Text>
            <Text style={[styles.statusText, currentTheme.statusText]}>{device ? `Connected (${device.name || RING_NAME})` : status}</Text>
          </View>
          <Pressable style={[styles.primaryButton, currentTheme.primaryButton, device && (isDarkMode ? styles.disconnectButtonDark : styles.disconnectButtonLight)]} onPress={scanAndConnect}>
            <Text style={[styles.buttonText, currentTheme.buttonText]}>{device ? 'Disconnect' : 'Scan ring'}</Text>
          </Pressable>
        </View>

        <View style={[styles.batteryCard, currentTheme.batteryCard]}>
          <View style={styles.cardHeader}>
            <Text style={[styles.label, currentTheme.label]}>BATTERY</Text>
            <Text style={[styles.batteryValue, currentTheme.batteryValue]}>{battery === null ? '--' : battery}<Text style={[styles.percent, currentTheme.percent]}>%</Text></Text>
          </View>
          <View style={[styles.batteryBarBackground, currentTheme.batteryBarBackground]}>
            <View style={[styles.batteryFill, currentTheme.batteryFill, { width: `${battery ?? 0}%` }]} />
          </View>
        </View>

        <Text style={[styles.sectionTitle, currentTheme.sectionTitle]}>Ring controls</Text>
        
        <View style={[styles.controlCard, currentTheme.controlCard]}>
          <View style={styles.controlHeader}>
            <Text style={[styles.controlTitle, currentTheme.controlTitle]}>Send one digit</Text>
            <Text style={[styles.characterCount, currentTheme.characterCount]}>1 / 1</Text>
          </View>
          <View style={styles.inputRow}>
            <TextInput 
              value={digit} 
              onChangeText={(value) => setDigit(value.replace(/[^0-9]/g, '').slice(0, 1))} 
              keyboardType="number-pad" 
              maxLength={1} 
              style={[styles.input, currentTheme.input]} 
            />
            <Pressable style={[styles.secondaryButton, currentTheme.secondaryButton]} onPress={() => writeValue(DIGIT_CHARACTERISTIC, digit)}>
              <Text style={[styles.secondaryText, currentTheme.secondaryText]}>Send</Text>
            </Pressable>
          </View>
        </View>

        <View style={[styles.controlCard, currentTheme.controlCard]}>
          <View style={styles.controlHeader}>
            <Text style={[styles.controlTitle, currentTheme.controlTitle]}>Send one letter</Text>
            <Text style={[styles.characterCount, currentTheme.characterCount]}>1 / 1</Text>
          </View>
          <View style={styles.inputRow}>
            <TextInput 
              value={letter} 
              onChangeText={(value) => setLetter(value.replace(/[^a-z]/gi, '').slice(0, 1).toUpperCase())} 
              maxLength={1} 
              autoCapitalize="characters" 
              style={[styles.input, currentTheme.input]} 
            />
            <Pressable style={[styles.secondaryButton, currentTheme.secondaryButton]} onPress={() => writeValue(LETTER_CHARACTERISTIC, letter)}>
              <Text style={[styles.secondaryText, currentTheme.secondaryText]}>Send</Text>
            </Pressable>
          </View>
        </View>

        <Text style={[styles.sectionTitle, currentTheme.sectionTitle]}>Coming online</Text>
        
        <View style={[styles.futureRow, currentTheme.futureRow]}>
          <View>
            <Text style={[styles.futureTitle, currentTheme.futureTitle]}>Heart rate</Text>
            <Text style={[styles.futureText, currentTheme.futureText]}>Sensor stream will appear here</Text>
          </View>
          <Text style={[styles.futureValue, currentTheme.futureValue]}>-- BPM</Text>
        </View>

        <View style={[styles.futureRow, currentTheme.futureRow]}>
          <View>
            <Text style={[styles.futureTitle, currentTheme.futureTitle]}>Sleep timer</Text>
            <Text style={[styles.futureText, currentTheme.futureText]}>Overnight state tracking</Text>
          </View>
          <Text style={[styles.futureValue, currentTheme.futureValue]}>--:--</Text>
        </View>

        <Text style={[styles.telemetry, currentTheme.telemetry]}>LIVE TELEMETRY: {telemetry}</Text>
      </ScrollView>
      <StatusBar style={isDarkMode ? "light" : "dark"} />
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  safe: { flex: 1 },
  container: { padding: 24, paddingBottom: 48, gap: 16 },
  header: { paddingTop: 20, paddingBottom: 8 },
  titleRow: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start', marginTop: 6 },
  kicker: { fontSize: 12, fontWeight: '800', letterSpacing: 2 },
  title: { fontSize: 36, fontWeight: '800', flex: 1, marginRight: 10 },
  subtitle: { fontSize: 14, lineHeight: 20, marginTop: 6, maxWidth: 320 },
  
  themeToggle: {
    borderRadius: 8,
    paddingVertical: 8,
    paddingHorizontal: 12,
    borderWidth: 1,
  },
  themeToggleText: { fontSize: 12, fontWeight: '700' },

  connectionCard: { 
    borderRadius: 16, 
    padding: 20, 
    flexDirection: 'row', 
    alignItems: 'center', 
    justifyContent: 'space-between',
    borderWidth: 1,
  },
  label: { fontSize: 10, fontWeight: '800', letterSpacing: 1.5 },
  statusText: { fontSize: 13, marginTop: 6, maxWidth: 180 },
  primaryButton: { 
    borderRadius: 12, 
    paddingVertical: 12, 
    paddingHorizontal: 18 
  },
  buttonText: { fontWeight: '800', fontSize: 13 },
  
  batteryCard: { 
    borderRadius: 16, 
    padding: 20, 
    gap: 14,
    borderWidth: 1,
  },
  cardHeader: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center' },
  batteryValue: { fontSize: 32, fontWeight: '800' },
  percent: { fontSize: 18 },
  batteryBarBackground: { height: 8, borderRadius: 4, overflow: 'hidden' },
  batteryFill: { height: '100%' },
  
  sectionTitle: { fontSize: 18, fontWeight: '800', marginTop: 10 },
  
  controlCard: { 
    borderRadius: 16, 
    padding: 20,
    borderWidth: 1,
  },
  controlHeader: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center' },
  controlTitle: { fontSize: 15, fontWeight: '700' },
  characterCount: { fontSize: 12 },
  
  inputRow: { flexDirection: 'row', gap: 12, marginTop: 14 },
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
    paddingHorizontal: 22 
  },
  secondaryText: { fontWeight: '800', fontSize: 14 },
  
  futureRow: { 
    borderRadius: 16, 
    padding: 18, 
    flexDirection: 'row', 
    justifyContent: 'space-between', 
    alignItems: 'center',
    borderWidth: 1,
  },
  futureTitle: { fontWeight: '700', fontSize: 15 },
  futureText: { marginTop: 4, fontSize: 12 },
  futureValue: { fontWeight: '800', fontSize: 14 },
  
  telemetry: { fontSize: 10, letterSpacing: 1.2, marginTop: 10 },
});

const darkStyles = StyleSheet.create({
  safe: { backgroundColor: '#18161B' },
  kicker: { color: '#D4A373' },
  title: { color: '#F4F1EA' },
  subtitle: { color: '#9E98A0' },
  themeToggle: { backgroundColor: '#26222B', borderColor: '#342F3A' },
  themeToggleText: { color: '#F4F1EA' },
  connectionCard: { backgroundColor: '#26222B', borderColor: '#342F3A' },
  label: { color: '#9E98A0' },
  statusText: { color: '#F4F1EA' },
  primaryButton: { backgroundColor: '#8C4A5D' },
  disconnectButtonDark: { backgroundColor: '#3D2F36' },
  buttonText: { color: '#F4F1EA' },
  batteryCard: { backgroundColor: '#26222B', borderColor: '#342F3A' },
  batteryValue: { color: '#F4F1EA' },
  percent: { color: '#D4A373' },
  batteryBarBackground: { backgroundColor: '#18161B' },
  batteryFill: { backgroundColor: '#D4A373' },
  sectionTitle: { color: '#F4F1EA' },
  controlCard: { backgroundColor: '#26222B', borderColor: '#342F3A' },
  controlTitle: { color: '#F4F1EA' },
  characterCount: { color: '#7A747D' },
  input: { backgroundColor: '#18161B', color: '#F4F1EA', borderColor: '#342F3A' },
  secondaryButton: { backgroundColor: '#8C4A5D' },
  secondaryText: { color: '#F4F1EA' },
  futureRow: { backgroundColor: '#26222B', borderColor: '#342F3A' },
  futureTitle: { color: '#F4F1EA' },
  futureText: { color: '#7A747D' },
  futureValue: { color: '#7A747D' },
  telemetry: { color: '#7A747D' },
});

const lightStyles = StyleSheet.create({
  safe: { backgroundColor: '#f4f1ea' },
  kicker: { color: '#c4512c' },
  title: { color: '#1c2522' },
  subtitle: { color: '#68716e' },
  themeToggle: { backgroundColor: '#fffaf3', borderColor: '#e6e0d7' },
  themeToggleText: { color: '#1c2522' },
  connectionCard: { backgroundColor: '#1c2522', borderColor: '#1c2522' },
  label: { color: '#95a19d' },
  statusText: { color: '#f4f1ea' },
  primaryButton: { backgroundColor: '#e4734b' },
  disconnectButtonLight: { backgroundColor: '#5c2c1c' },
  buttonText: { color: '#fffaf3' },
  batteryCard: { backgroundColor: '#fffaf3', borderColor: '#e6e0d7' },
  batteryValue: { color: '#1c2522' },
  percent: { color: '#e4734b' },
  batteryBarBackground: { backgroundColor: '#e6e0d7' },
  batteryFill: { backgroundColor: '#e4734b' },
  sectionTitle: { color: '#1c2522' },
  controlCard: { backgroundColor: '#fffaf3', borderColor: '#e6e0d7' },
  controlTitle: { color: '#1c2522' },
  characterCount: { color: '#9ca39f' },
  input: { backgroundColor: '#f0ebe2', color: '#1c2522', borderColor: '#e6e0d7' },
  secondaryButton: { backgroundColor: 'transparent', borderColor: '#c4512c', borderWidth: 1 },
  secondaryText: { color: '#c4512c' },
  futureRow: { backgroundColor: '#e7e5dc', borderColor: '#e7e5dc' },
  futureTitle: { color: '#1c2522' },
  futureText: { color: '#737c78' },
  futureValue: { color: '#7e8984' },
  telemetry: { color: '#9ca39f' },
});