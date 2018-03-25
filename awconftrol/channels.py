# Channels
import collections

# gpio = gpio pin
# direction = whether it runs forward or backwards, +1 or -1
# neutral = neutral pulse length in ms.

ChannelDef = collections.namedtuple('ChannelDef', ['gpio', 'direction', 'neutral'] )

CHANNEL_DEFS = [
	ChannelDef(24, -1, 1450), # Left
	ChannelDef(23, 1, 1450), # Right
]


