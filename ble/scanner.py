from bleak import BleakScanner


async def scan_devices():

    print("\nScanning BLE devices...\n")

    
    devices = await BleakScanner.discover(
        return_adv=True
    )

    for device, advertisement in devices.values():

        print("--------------------------------")
        
        print("Name:", device.name)
        print("Address:", device.address)

        print(
            "RSSI:",
            advertisement.rssi
        )

        print(
            "Services:",
            advertisement.service_uuids
        )

        print(
            "Manufacturer Data:",
            advertisement.manufacturer_data
        )