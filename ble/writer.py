WRITE_UUID = "000033f3-0000-1000-8000-00805f9b34fb"


async def send_command(client, command: bytes):

    print("\n========== SEND COMMAND ==========")
    print("HEX:", command.hex(" "))

    await client.write_gatt_char(
        WRITE_UUID,
        command
    )

    print("Command sent!")