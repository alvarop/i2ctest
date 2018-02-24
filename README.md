# I2C Bug Example

There seems to be a bug in the mynewt TWI code for the NRF52. When a device isn't present or otherwise NACKS the transaction, the NRF52 gets stuck pulling SCL low. This is a project to demonstrate that on an NRF52DK

## Test runs on the nrf52dk (PCA10040)

## Equipment required
* NRF52DK (PCA10040)
* Multimeter

## Steps to reproduce
1. Load on the nrf52 (newt run nrf52dk 0)
2. Measure pin p0.27 (SCL) with a multimeter, it should be pulled high
3. Connect over serial terminal and run the command 'i2c_scan 0'
4. Measure p0.27 again and it will be low, which it shouldn't (Note that there are no devices connected that could be pulling SCL low)
5. Now run the command '2c_fix' and measure p0.27 again (It should be high)
