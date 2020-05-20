SRC=eeprom.c
LIBS = -llibglv-eeprom

eeprom_tool: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LIBS)

install:
	mkdir -p $(DESTDIR)/usr/bin
	install eeprom_tool $(DESTDIR)/usr/bin

clean:
	rm -f eeprom_tool
