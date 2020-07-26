
offset=1118208
size=1048576
esptool.py read_flash $offset $size storage.bin
MTOOLS_SKIP_CHECK=1 mdir -i storage.bin ::

