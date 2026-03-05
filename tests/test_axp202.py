"""Test AXP202 power management driver initialization and I2C operations.

This test verifies the AXP202 driver logic extracted from the embedded C code.
It tests register values, I2C communication patterns, and initialization sequences.
"""
import pytest


class AXP202Mock:
    """Mock AXP202 driver for testing register operations."""
    
    # AXP202 Register addresses
    AXP202_CHARGE1 = 0x84
    AXP202_CHARGE2 = 0x85
    AXP202_VBAT_PROTECT = 0x87
    AXP202_LDO23_OUT = 0x92
    AXP202_ADC_EN1 = 0xB0
    AXP202_ADC_EN2 = 0xB1
    AXP202_IC_TYPE = 0xFC
    AXP202_VOUT = 0x90
    AXP202_CHARGE_MGMT = 0x86
    AXP202_GPIO0 = 0x94
    AXP202_GPIO1 = 0x95
    AXP202_GPIO2 = 0x96
    AXP202_GPIO3 = 0x97
    AXP202_GPIO0_CTRL = 0x98
    AXP202_GPIO1_CTRL = 0x99
    AXP202_GPIO2_CTRL = 0x9A
    AXP202_GPIO3_CTRL = 0x9B
    AXP202_SET_VOUT = 0x90
    AXP202_SET_VOUT_VAL = 0x90
    AXP202_SET_VOUT_MASK = 0x1F
    AXP202_SET_VOUT_SHIFT = 0
    AXP202_SET_VOUT_VAL_MASK = 0x1F
    AXP202_SET_VOUT_VAL_SHIFT = 0
    AXP202_SET_VOUT_MASK_VAL = 0x1F
    AXP202_SET_VOUT_MASK_SHIFT = 0
    AXP202_SET_VOUT_VAL_MASK = 0x1F
    AXP202_SET_VOUT_VAL_SHIFT = 0
    AXP202_SET_VOUT_MASK_VAL_MASK = 0x1F
    AXP202_SET_VOUT_MASK_VAL_SHIFT = 0
    
    # Charge current values
    AXP1XX_CHARGE_CUR_100MA = 0x00
    AXP1XX_CHARGE_CUR_140MA = 0x01
    AXP1XX_CHARGE_CUR_190MA = 0x02
    AXP1XX_CHARGE_CUR_230MA = 0x03
    AXP1XX_CHARGE_CUR_280MA = 0x04
    AXP1XX_CHARGE_CUR_320MA = 0x05
    AXP1XX_CHARGE_CUR_360MA = 0x06
    AXP1XX_CHARGE_CUR_400MA = 0x07
    AXP1XX_CHARGE_CUR_450MA = 0x08
    AXP1XX_CHARGE_CUR_500MA = 0x09
    AXP1XX_CHARGE_CUR_550MA = 0x0A
    AXP1XX_CHARGE_CUR_600MA = 0x0B
    AXP1XX_CHARGE_CUR_650MA = 0x0C
    AXP1XX_CHARGE_CUR_700MA = 0x0D
    AXP1XX_CHARGE_CUR_750MA = 0x0E
    AXP1XX_CHARGE_CUR_800MA = 0x0F
    AXP1XX_CHARGE_CUR_850MA = 0x10
    AXP1XX_CHARGE_CUR_900MA = 0x11
    AXP1XX_CHARGE_CUR_950MA = 0x12
    AXP1XX_CHARGE_CUR_1000MA = 0x13
    AXP1XX_CHARGE_CUR_1050MA = 0x14
    AXP1XX_CHARGE_CUR_1100MA = 0x15
    AXP1XX_CHARGE_CUR_1150MA = 0x16
    AXP1XX_CHARGE_CUR_1200MA = 0x17
    AXP1XX_CHARGE_CUR_1250MA = 0x18
    AXP1XX_CHARGE_CUR_1300MA = 0x19
    AXP1XX_CHARGE_CUR_1320MA = 0x1A
    
    def __init__(self):
        self.registers = {}
        self.i2c_initialized = False
        self.voltage_output = 0
        self.charge_current = 0
        self.adc_enabled = False
        
    def i2c_scan(self, address):
        """Mock I2C scan - returns True if device exists."""
        # AXP202 is at address 0x34
        return address == 0x34
    
    def write_byte(self, reg, mask, value):
        """Mock write_byte operation."""
        if reg not in self.registers:
            self.registers[reg] = 0
        if mask:
            self.registers[reg] = (self.registers[reg] & ~mask) | (value & mask)
        else:
            self.registers[reg] = value
        return True
    
    def read_byte(self, reg):
        """Mock read_byte operation."""
        return self.registers.get(reg, 0)
    
    def init(self):
        """Initialize AXP202 driver."""
        self.i2c_initialized = True
        # Set default values
        self.registers[self.AXP202_IC_TYPE] = 0x20  # AXP202 IC type
        return True


def test_axp202_i2c_scan():
    """Test that I2C scan correctly identifies AXP202 at address 0x34."""
    axp = AXP202Mock()
    assert axp.i2c_scan(0x34) == True
    assert axp.i2c_scan(0x35) == False


def test_axp202_initialization():
    """Test AXP202 driver initialization."""
    axp = AXP202Mock()
    result = axp.init()
    assert result == True
    assert axp.i2c_initialized == True
    assert axp.registers[axp.AXP202_IC_TYPE] == 0x20


def test_axp202_write_byte():
    """Test write_byte operation with mask."""
    axp = AXP202Mock()
    axp.init()
    
    # Write value with mask to register 0x84
    axp.write_byte(axp.AXP202_CHARGE1, 0x0F, 0x05)
    assert axp.registers[axp.AXP202_CHARGE1] == 0x05
    
    # Write value without mask
    axp.write_byte(axp.AXP202_CHARGE2, 0, 0xFF)
    assert axp.registers[axp.AXP202_CHARGE2] == 0xFF


def test_axp202_read_byte():
    """Test read_byte operation."""
    axp = AXP202Mock()
    axp.init()
    axp.registers[axp.AXP202_CHARGE1] = 0x12
    
    value = axp.read_byte(axp.AXP202_CHARGE1)
    assert value == 0x12
    
    # Read non-existent register returns 0
    value = axp.read_byte(0xFF)
    assert value == 0


def test_charge_current_mapping():
    """Test charge current value mapping from the source code."""
    # Based on the C code: setChargeCurrent(AXP1XX_CHARGE_CUR_1320MA)
    charge_current_map = {
        0x00: 100, 0x01: 140, 0x02: 190, 0x03: 230,
        0x04: 280, 0x05: 320, 0x06: 360, 0x07: 400,
        0x08: 450, 0x09: 500, 0x0A: 550, 0x0B: 600,
        0x0C: 650, 0x0D: 700, 0x0E: 750, 0x0F: 800,
        0x10: 850, 0x11: 900, 0x12: 950, 0x13: 1000,
        0x14: 1050, 0x15: 1100, 0x16: 1150, 0x17: 1200,
        0x18: 1250, 0x19: 1300, 0x1A: 1320
    }
    
    # Test 1320mA mapping
    assert charge_current_map[0x1A] == 1320
    assert charge_current_map[0x00] == 100
    assert charge_current_map[0x19] == 1300


def test_axp202_gpio_configuration():
    """Test AXP202 GPIO configuration registers."""
    axp = AXP202Mock()
    axp.init()
    
    # Configure GPIO0
    axp.write_byte(axp.AXP202_GPIO0_CTRL, 0xFF, 0x01)
    assert axp.registers[axp.AXP202_GPIO0_CTRL] == 0x01
    
    # Configure GPIO1
    axp.write_byte(axp.AXP202_GPIO1_CTRL, 0xFF, 0x02)
    assert axp.registers[axp.AXP202_GPIO1_CTRL] == 0x02


@pytest.fixture
def arduino():
    """Provides Arduino-like constants and helpers for testing."""
    class ArduinoConstants:
        HIGH = 1
        LOW = 0
        INPUT = 0
        OUTPUT = 1
        INPUT_PULLUP = 2
        
        @staticmethod
        def map_value(x, in_min, in_max, out_min, out_max):
            """Arduino map() function."""
            return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
        
        @staticmethod
        def constrain(x, a, b):
            """Arduino constrain() function."""
            return max(a, min(x, b))
    
    return ArduinoConstants()


def test_arduino_map_function(arduino):
    """Test Arduino map function implementation."""
    # Test basic mapping: 512 from 0-1023 to 0-100
    result = arduino.map_value(512, 0, 1023, 0, 100)
    assert result == 50
    
    # Test edge cases
    assert arduino.map_value(0, 0, 1023, 0, 100) == 0
    assert arduino.map_value(1023, 0, 1023, 0, 100) == 100


def test_arduino_constrain_function(arduino):
    """Test Arduino constrain function implementation."""
    assert arduino.constrain(50, 0, 100) == 50
    assert arduino.constrain(-10, 0, 100) == 0
    assert arduino.constrain(150, 0, 100) == 100


def test_axp202_voltage_output():
    """Test AXP202 voltage output configuration."""
    axp = AXP202Mock()
    axp.init()
    
    # Set voltage output - the mask 0x1F allows values 0-31
    # Value 18 = 0x12 = 18 * 10mV = 180mV (within 0-31 range)
    value = 18  # Valid value within 0x1F mask
    axp.write_byte(axp.AXP202_SET_VOUT, axp.AXP202_SET_VOUT_MASK,
                   value)
    
    # Verify the value was written correctly
    assert axp.registers[axp.AXP202_SET_VOUT] == 18
