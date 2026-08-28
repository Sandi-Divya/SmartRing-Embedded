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
  return atob(value);
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

  return (
    <SafeAreaView style={styles.safe}>
      <ScrollView contentContainerStyle={styles.container}>
        <View style={styles.header}>
          <Text style={styles.kicker}>SR08 / COMPANION</Text>
          <Text style={styles.title}>Your ring, in rhythm.</Text>
          <Text style={styles.subtitle}>A quiet control surface for the hardware on your finger.</Text>
        </View>
        <View style={styles.connectionRow}>
          <View>
            <Text style={styles.label}>DEVICE</Text>
            <Text style={styles.status}>{device ? `Connected (${device.name || RING_NAME})` : status}</Text>
          </View>
          <Pressable style={[styles.primaryButton, device && styles.disconnectButton]} onPress={scanAndConnect}>
            <Text style={styles.buttonText}>{device ? 'Disconnect' : 'Scan ring'}</Text>
          </Pressable>
        </View>
        <View style={styles.batteryCard}>
          <View><Text style={styles.label}>BATTERY</Text><Text style={styles.battery}>{battery === null ? '--' : battery}<Text style={styles.percent}>%</Text></Text></View>
          <View style={styles.batteryBar}><View style={[styles.batteryFill, { width: `${battery ?? 0}%` }]} /></View>
        </View>
        <Text style={styles.sectionTitle}>Ring controls</Text>
        <View style={styles.controlCard}>
          <View style={styles.controlHeader}><Text style={styles.controlTitle}>Send one digit</Text><Text style={styles.characterCount}>1 / 1</Text></View>
          <View style={styles.inputRow}><TextInput value={digit} onChangeText={(value) => setDigit(value.replace(/[^0-9]/g, '').slice(0, 1))} keyboardType="number-pad" maxLength={1} style={styles.input} /><Pressable style={styles.secondaryButton} onPress={() => writeValue(DIGIT_CHARACTERISTIC, digit)}><Text style={styles.secondaryText}>Send</Text></Pressable></View>
        </View>
        <View style={styles.controlCard}>
          <View style={styles.controlHeader}><Text style={styles.controlTitle}>Send one letter</Text><Text style={styles.characterCount}>1 / 1</Text></View>
          <View style={styles.inputRow}><TextInput value={letter} onChangeText={(value) => setLetter(value.replace(/[^a-z]/gi, '').slice(0, 1).toUpperCase())} maxLength={1} autoCapitalize="characters" style={styles.input} /><Pressable style={styles.secondaryButton} onPress={() => writeValue(LETTER_CHARACTERISTIC, letter)}><Text style={styles.secondaryText}>Send</Text></Pressable></View>
        </View>
        <Text style={styles.sectionTitle}>Coming online</Text>
        <View style={styles.futureRow}><View><Text style={styles.futureTitle}>Heart rate</Text><Text style={styles.futureText}>Sensor stream will appear here</Text></View><Text style={styles.futureValue}>-- BPM</Text></View>
        <View style={styles.futureRow}><View><Text style={styles.futureTitle}>Sleep timer</Text><Text style={styles.futureText}>Overnight state tracking</Text></View><Text style={styles.futureValue}>--:--</Text></View>
        <Text style={styles.telemetry}>LIVE TELEMETRY  {telemetry}</Text>
      </ScrollView>
      <StatusBar style="auto" />
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  safe: { flex: 1, backgroundColor: '#f4f1ea' },
  container: { padding: 24, paddingBottom: 48, gap: 16 },
  header: { paddingTop: 20, paddingBottom: 12 },
  kicker: { color: '#c4512c', fontSize: 12, fontWeight: '800', letterSpacing: 2 },
  title: { color: '#1c2522', fontSize: 36, fontWeight: '800', marginTop: 10 },
  subtitle: { color: '#68716e', fontSize: 15, lineHeight: 22, marginTop: 8, maxWidth: 300 },
  connectionRow: { backgroundColor: '#1c2522', borderRadius: 8, padding: 18, flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between' },
  label: { color: '#95a19d', fontSize: 10, fontWeight: '800', letterSpacing: 1.5 },
  status: { color: '#f4f1ea', fontSize: 14, marginTop: 7, maxWidth: 190 },
  primaryButton: { backgroundColor: '#e4734b', borderRadius: 5, paddingVertical: 12, paddingHorizontal: 15 },
  disconnectButton: { backgroundColor: '#5c2c1c' },
  buttonText: { color: '#fffaf3', fontWeight: '800' },
  batteryCard: { backgroundColor: '#fffaf3', borderRadius: 8, padding: 20, gap: 18 },
  battery: { color: '#1c2522', fontSize: 45, fontWeight: '800', marginTop: 3 },
  percent: { color: '#e4734b', fontSize: 20 },
  batteryBar: { height: 8, backgroundColor: '#e6e0d7', borderRadius: 4, overflow: 'hidden' },
  batteryFill: { height: '100%', backgroundColor: '#e4734b' },
  sectionTitle: { color: '#1c2522', fontSize: 19, fontWeight: '800', marginTop: 10 },
  controlCard: { backgroundColor: '#fffaf3', borderRadius: 8, padding: 18 },
  controlHeader: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center' },
  controlTitle: { color: '#1c2522', fontSize: 16, fontWeight: '700' },
  characterCount: { color: '#9ca39f', fontSize: 12 },
  inputRow: { flexDirection: 'row', gap: 10, marginTop: 14 },
  input: { backgroundColor: '#f0ebe2', borderRadius: 5, color: '#1c2522', fontSize: 22, fontWeight: '700', paddingHorizontal: 14, height: 50, flex: 1 },
  secondaryButton: { borderColor: '#c4512c', borderWidth: 1, borderRadius: 5, justifyContent: 'center', paddingHorizontal: 19 },
  secondaryText: { color: '#c4512c', fontWeight: '800' },
  futureRow: { backgroundColor: '#e7e5dc', borderRadius: 8, padding: 17, flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center' },
  futureTitle: { color: '#1c2522', fontWeight: '700', fontSize: 15 },
  futureText: { color: '#737c78', marginTop: 5, fontSize: 12 },
  futureValue: { color: '#7e8984', fontWeight: '800' },
  telemetry: { color: '#9ca39f', fontSize: 10, letterSpacing: 1.2, marginTop: 10 },
});