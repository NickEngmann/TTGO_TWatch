"""Embedded C mock utilities for testing Arduino/ESP32 code.

Provides mock implementations of Arduino functions and embedded C utilities
that can be tested in Python.
"""


def arduino_map(x, in_min, in_max, out_min, out_max):
    """Arduino map() function - maps a value from one range to another."""
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


def arduino_constrain(x, a, b):
    """Arduino constrain() function - constrains a value between two limits."""
    return max(a, min(x, b))


class MockUART:
    """Mock UART for testing serial communication."""
    
    def __init__(self):
        self.buffer = []
        self.baud_rate = 115200
        
    def begin(self, baud):
        """Initialize UART with baud rate."""
        self.baud_rate = baud
        self.buffer = []
        return True
    
    def print(self, data):
        """Print data to UART buffer."""
        self.buffer.append(str(data))
        return len(str(data))
    
    def println(self, data=""):
        """Print data with newline."""
        self.buffer.append(str(data))
        return len(str(data)) + 1
    
    def available(self):
        """Check if data is available to read."""
        return len(self.buffer) > 0
    
    def read(self):
        """Read data from buffer."""
        if self.buffer:
            return self.buffer.pop(0)
        return None
    
    def flush(self):
        """Flush UART buffer."""
        self.buffer = []


class ArduinoConstants:
    """Arduino-like constants for testing."""
    
    HIGH = 1
    LOW = 0
    INPUT = 0
    OUTPUT = 1
    INPUT_PULLUP = 2
    INPUT_PULLDOWN = 3
    
    @staticmethod
    def map_value(x, in_min, in_max, out_min, out_max):
        """Arduino map() function."""
        return arduino_map(x, in_min, in_max, out_min, out_max)
    
    @staticmethod
    def constrain(x, a, b):
        """Arduino constrain() function."""
        return arduino_constrain(x, a, b)


def setup_arduino_constants():
    """Setup Arduino constants fixture."""
    return ArduinoConstants()


# Export for use in tests
__all__ = [
    'arduino_map',
    'arduino_constrain',
    'MockUART',
    'ArduinoConstants',
    'setup_arduino_constants'
]
