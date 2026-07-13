from datetime import datetime

# Data Recorder 
# Sender : sender is the specific pathway/channel that the data is coming from 
# When ring sned a packet bleak detect it and wrap that data into 2 variables, sender and data.
async def notification_handler(sender, data):
    # Converts raw electronic signals into readable Hex
    hex_data = data.hex(" ")
    timestamp = datetime.now()

    print("\n===== NOTIFICATION =====")
    print("Time:", timestamp)
    print("Sender:", sender)
    print("Data:", hex_data)

    # Save every packet to a log file
    with open("logs/notifications.txt", "a") as file:
        file.write(f"{timestamp} | {hex_data}\n")

        

#Listner Function to subscribe to notifications from the BLE device
async def subscribe_notifications(client):
    ## The ring's output pathway
    notify_uuid = "000033f4-0000-1000-8000-00805f9b34fb"

    print("Subscribing to notifications...")

    # Look at that specific pathway and turn on "Listening Mode"
    await client.start_notify(
        notify_uuid,
        # When data is received, call the notification_handler function to process it
        notification_handler
    )

    print("Listening...")