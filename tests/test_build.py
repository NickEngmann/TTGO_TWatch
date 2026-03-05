import os
import sys

# Test script for verifying T-Watch 2020 v3 build
# This script verifies:
# 1. PlatformIO configuration is valid
# 2. PSRAM settings are correctly configured
# 3. AXP202 driver is included
# 4. LVGL v7.7.2 is configured

def test_platformio_config():
    """Test that platformio.ini exists and is valid"""
    pio_path = os.path.join(os.path.dirname(__file__), '..', 'platformio.ini')
    assert os.path.exists(pio_path), "platformio.ini not found"
    
    with open(pio_path, 'r') as f:
        content = f.read()
        
    # Verify PSRAM configuration
    assert 'PSRAM_SIZE=0x500000' in content, "PSRAM size not configured"
    assert 'PSRAM_TYPE=PSRAM_TYPE_OCTAL' in content, "PSRAM type not configured"
    
    # Verify Arduino framework
    assert 'framework = arduino' in content, "Arduino framework not configured"
    
    # Verify LVGL configuration
    assert 'lvgl/lvgl@^7.7.2' in content, "LVGL library not configured"
    
    # Verify AXP202 driver
    assert 'AXP202X_Library' in content, "AXP202 driver not configured"
    
    pass

def test_source_files():
    """Test that required source files exist"""
    src_path = os.path.join(os.path.dirname(__file__), '..', 'src')
    
    # Check for TTGO.h
    ttgo_h = os.path.join(src_path, 'TTGO.h')
    assert os.path.exists(ttgo_h), "TTGO.h not found"
    
    # Check for axp20x driver
    axp_path = os.path.join(src_path, 'drive', 'axp')
    assert os.path.exists(axp_path), "AXP202 driver directory not found"
    
    axp_cpp = os.path.join(axp_path, 'axp20x.cpp')
    assert os.path.exists(axp_cpp), "axp20x.cpp not found"
    
    axp_h = os.path.join(axp_path, 'axp20x.h')
    assert os.path.exists(axp_h), "axp20x.h not found"
    
    print("✓ Source files test passed")
    return True

def test_test_hardware_init():
    """Test that hardware initialization test file exists"""
    test_path = os.path.join(os.path.dirname(__file__), '..', 'test_hardware_init.cpp')
    assert os.path.exists(test_path), "test_hardware_init.cpp not found"
    
    with open(test_path, 'r') as f:
        content = f.read()
        
    # Verify test content
    assert 'ESP.getPsramSize()' in content, "PSRAM size check not in test"
    assert 'Wire.begin()' in content, "I2C scan not in test"
    assert 'AXP202' in content or '0x34' in content, "AXP202 address check not in test"
    
    print("✓ Hardware initialization test file test passed")
    return True

if __name__ == '__main__':
    try:
        test_platformio_config()
        test_source_files()
        test_test_hardware_init()
        print("\n=== All Tests Passed ===")
        sys.exit(0)
    except AssertionError as e:
        print(f"\n✗ Test Failed: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\n✗ Unexpected Error: {e}")
        sys.exit(1)
