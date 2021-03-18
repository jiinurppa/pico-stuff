add_executable(
  blobs
  blobs.cpp
)

# Use bootrom float library
pico_set_float_implementation(blobs pico)

# Pull in pico libraries that we need
target_link_libraries(blobs pico_stdlib hardware_spi hardware_pwm hardware_dma pico_display)

# Create map/bin/hex file etc.
pico_add_extra_outputs(blobs)
