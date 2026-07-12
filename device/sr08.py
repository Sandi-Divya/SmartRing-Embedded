from ble.connection import connect_ring
from ble.notify import subscribe_notifications
from ble.writer import send_command


class SR08:

    def __init__(self):

        self.client = None

    async def connect(self):

        self.client = await connect_ring()

    async def listen(self):

        await subscribe_notifications(
            self.client
        )

    async def handshake(self):

        await send_command(
            self.client,
            bytes([0x5A])
        )