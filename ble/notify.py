from datetime import datetime


async def notification_handler(sender, data):

    hex_data = data.hex(" ")
    timestamp = datetime.now()

    print("\n===== NOTIFICATION =====")
    print("Time:", timestamp)
    print("Sender:", sender)
    print("Data:", hex_data)


    # Save every packet to a log file
    with open(
        "logs/notifications.txt",
        "a"
    ) as file:

        file.write(
            f"{timestamp} | {hex_data}\n"
        )



async def subscribe_notifications(client):

    notify_uuid = "000033f4-0000-1000-8000-00805f9b34fb"

    print("Subscribing to notifications...")

    await client.start_notify(
        notify_uuid,
        notification_handler
    )

    print("Listening...")