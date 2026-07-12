from bleak import BleakClient


SR08_ADDRESS = "02:02:E3:FB:D6:C4"


async def connect_ring():

    #Create communication client with the SR08's MAC address
    client = BleakClient(SR08_ADDRESS)

    print("Connecting to SR08...")

    #Sends connection request to the SR08
    await client.connect()


    if client.is_connected:

        print("Connected successfully!")

    else:

        print("Connection failed")


    return client