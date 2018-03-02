detect_sensor_type()
{
  local id1 id2 id3 id4

  devmem 0x200f0040 32 0x2	# I2C0_SCL
  devmem 0x200f0044 32 0x2	# I2C0_SDA
  devmem 0x2003002c 32 0xb4001	# clk 27M, VI 99MHz

  # detect SmartSens sensors
  id1=$(i2c_read 0 0x60 0x3107 0x3107 2 2 2)
  case "$id1" in
    0x1145)
      id3=$(i2c_read 0 0x60 0x3109 0x3109 2 1 1)
      id4=$(i2c_read 0 0x60 0x3c05 0x3c05 2 1 1)
      if [ "$id3$id4" = "0x10x0" ]; then
        echo "sc1145"
        return 0
      fi
      ;;
    0x2135)
      id3=$(i2c_read 0 0x60 0x3109 0x3109 2 1 1)
      id4=$(i2c_read 0 0x60 0x3c05 0x3c05 2 1 1)
      if [ "$id3$id4" = "0x20x8" ]; then
        echo "sc2135"
        return 0
      fi
      ;;
    0x0)
      id3=$(i2c_read 0 0x60 0x2148 0x2148 2 1 1)
      if [ "$id3" = "0x35" ]; then
        echo "sc1135"
        return 0
      fi
      ;;
  esac

  # fallback when detect failed
  #   get sensor type from u-boot environment
  fw_printenv -n sensor_type
}
