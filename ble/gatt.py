#Take an already connected device(client) and inspect its internal BLE database
async def discover_gatt(client):

    print("\n========== GATT TABLE ==========")

    #give all services and characteristics a chance to be discovered
    services = client.services

    for service in services:

        print("\nSERVICE:")
        #UUID identifies the service
        print(service.uuid)

        for char in service.characteristics:

            print("  CHARACTERISTIC:")
            print("   ", char.uuid)

            print(
                "    Properties:",
                char.properties
            )

    print("\n================================")