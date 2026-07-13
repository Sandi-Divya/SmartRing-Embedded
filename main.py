import asyncio

from ble.connection import connect_ring
from ble.notify import subscribe_notifications
from ble.writer import send_command


async def main():

    print("==============================")
    print(" SR08 BLE Reverse Engineering ")
    print(" Framework Demo")
    print("==============================\n")


    print("Connecting to SR08...")


    client = await connect_ring()


    if client is None:
        print("Connection failed!")
        return


    print("Connected successfully!\n")


    await subscribe_notifications(client)


    print("\nNotification channel ready!")
    print("Listening on 0x33F4\n")


    await asyncio.sleep(2)


    print("==============================")
    print(" SEND HANDSHAKE COMMAND ")
    print("==============================")

    print("TX: 5A\n")


    await send_command(
        client,
        bytes.fromhex("5A")
    )


    print("Command sent!")
    print("Waiting for SR08 response...\n")


    while True:
        await asyncio.sleep(1)



asyncio.run(main())